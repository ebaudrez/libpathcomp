/**
 * \file
 * \brief Path name composition (implementation)
 */

#include "config.h"
#include "pathcomp.h"
#include "list.h"
#include "cf.h"
#include "value.h"
#include "log.h"
#include "interpreter.h"
#include "buf.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <sys/stat.h>

typedef struct {
    char    *name;
    value_t *value;
} att_t;

/**
 * \brief Main data type
 */
struct pathcomp_t {
    char   *name;
    list_t *attributes;
    char   *metatable; /**< \brief Name of the Lua metatable */
    int     done;      /**< \brief Iterator state */
};

static cf_t *config;

static att_t *
attribute_new(const char *name, const char *value)
{
    att_t *att;
    att = malloc(sizeof *att);
    if (!att) return att;
    att->name = strdup(name);
    att->value = value_new(value);
    return att;
}

static void
attribute_replace_value(att_t *att, const char *value)
{
    assert(att);
    value_free(att->value);
    att->value = value_new(value);
}

static void
attribute_add_value(att_t *att, const char *value)
{
    assert(att);
    value_add(&att->value, value_new(value));
}

static void
attribute_free(att_t *att)
{
    if (!att) return;
    free(att->name);
    value_free(att->value);
    free(att);
}

void
pathcomp_use_config_from(const char *string)
{
    assert(string);
    config = cf_new_from_string(string);
}

void
pathcomp_cleanup(void)
{
    cf_free(config);
    config = NULL;
    interpreter_cleanup();
    log_cleanup();
}

static int
find_section_with_name(cf_section_t *sec, char *name)
{
    return !strcmp(sec->name, name);
}

static int
find_attribute_with_name(att_t *att, char *name)
{
    return !strcmp(att->name, name);
}

static void
pathcomp_add_or_replace(pathcomp_t *composer, const char *name, const char *value, int replace)
{
    list_t *patt;
    assert(name);
    assert(value);
    patt = list_find_first(composer->attributes, (list_traversal_t *) find_attribute_with_name, (void *) name);
    if (!patt) {
        att_t *new;
        new = attribute_new(name, value);
        composer->attributes = list_push(composer->attributes, new);
        return;
    }
    /* there happens to be an attribute with this name already */
    if (replace) attribute_replace_value(patt->el, value);
    else attribute_add_value(patt->el, value);
}

static void
pathcomp_make_from_config(pathcomp_t *composer)
{
    list_t *psec;
    log_t *log;
    assert(composer);
    assert(config);
    log = log_get_logger("pathcomp");
    psec = config->sections;
    while ((psec = list_find_first(psec, (list_traversal_t *) find_section_with_name, composer->name))) {
        cf_section_t *sec = psec->el;
        list_t       *pkv = sec->entries;
        log_debug(log, "found section with name '%s'", sec->name);
        while (pkv) {
            cf_kv_t *kv = pkv->el;
            log_debug(log, "found key-value pair with name '%s'", kv->key);
            pathcomp_add(composer, kv->key, kv->value);
            pkv = pkv->next;
        }
        psec = psec->next;
    }
}

static int
pathcomp_eval_callback(lua_State *L)
{
    pathcomp_t *composer;
    const char *name, *value;
    assert(lua_isuserdata(L, -2));
    composer = *((pathcomp_t **) lua_touserdata(L, -2));
    assert(lua_isstring(L, -1));
    name = lua_tostring(L, -1);
    value = pathcomp_eval(composer, name);
    lua_pushstring(L, value);
    return 1;
}

pathcomp_t *
pathcomp_new(const char *name)
{
    buf_t       buf;
    lua_State  *L = interpreter_get_state();
    const char *metatable_prefix = "libpathcomp::";
    pathcomp_t *composer = NULL;
    assert(name);
    composer = malloc(sizeof *composer);
    if (!composer) return composer;
    composer->name = strdup(name);
    composer->attributes = NULL;
    pathcomp_make_from_config(composer);
    buf_init(&buf, 0);
    buf_addstr(&buf, metatable_prefix);
    buf_addstr(&buf, name);
    composer->metatable = buf_detach(&buf, NULL);
    luaL_newmetatable(L, composer->metatable);
    lua_pushcfunction(L, pathcomp_eval_callback);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    composer->done = 0;
    return composer;
}

void
pathcomp_free(pathcomp_t *composer)
{
    if (!composer) return;
    free(composer->name);
    list_foreach(composer->attributes, (list_traversal_t *) attribute_free, NULL);
    list_free(composer->attributes);
    free(composer->metatable);
    free(composer);
}

/**
 * \note The string returned by this function must not be deallocated by the user.
 */
const char *
pathcomp_eval(pathcomp_t *composer, const char *name)
{
    list_t *p;
    att_t *att;
    assert(composer);
    p = list_find_first(composer->attributes, (list_traversal_t *) find_attribute_with_name, (void *) name);
    if (!p) return NULL;
    att = p->el;
    return value_eval(att->value, composer, composer->metatable);
}

/**
 * \note The string returned by this function must be deallocated by the user.
 */
char *
pathcomp_yield(pathcomp_t *composer)
{
    buf_t path;
    const char *root, *compose;
    assert(composer);
    buf_init(&path, 0);
    root = pathcomp_eval(composer, "root");
    compose = pathcomp_eval(composer, "compose");
    if (root && strlen(root)) {
        buf_addstr(&path, root);
        buf_addch(&path, '/');
    }
    if (compose && strlen(compose)) {
        buf_addstr(&path, compose);
    }
    if (path.len) return buf_detach(&path, NULL);
    buf_release(&path);
    return NULL;
}

/**
 * pathcomp_set() will reset all alternatives to their first value. This is
 * necessary because alternatives are tried in order, and there is no guarantee
 * that a matching combination of alternatives comes later in the list of
 * possible combinations.
 *
 * Imagine the situation where you have successfully located a GERB archive
 * file with the following characteristics:
 *
 *    -# instrument = G2
 *    -# extension  = .hdf
 *    -# extension  = .hdf.gz
 *    -# etc.
 *
 * After executing pathcomp_find(), the \e extension alternative will point to
 * the .hdf.gz value, because G2 files are gzip-compressed. If you now call
 *
 *   pathcomp_set(composer, "instrument", "GL")
 *
 * pathcomp_find() will fail, because GERB-like files are not compressed
 * externally, and the alternative .hdf comes before .hdf.gz in the list of
 * alternatives, and pathcomp_find() will not rewind it! Therefore,
 * pathcomp_reset() must be called to rewind all the alternatives.
 */
void
pathcomp_set(pathcomp_t *composer, const char *name, const char *value)
{
    assert(composer);
    assert(name);
    assert(value);
    pathcomp_add_or_replace(composer, name, value, 1);
    pathcomp_reset(composer);
}

/**
 * All alternatives are rewound after calling this function, so that a failed
 * search with pathcomp_find() can be retried using the newly added
 * alternative.
 */
void
pathcomp_add(pathcomp_t *composer, const char *name, const char *value)
{
    assert(composer);
    assert(name);
    assert(value);
    pathcomp_add_or_replace(composer, name, value, 0);
    pathcomp_reset(composer);
}

void
pathcomp_reset(pathcomp_t *composer)
{
    list_t *p;
    assert(composer);
    for (p = composer->attributes; p; p = p->next) {
        att_t *att = p->el;
        value_t *val = att->value;
        if (val->type != VALUE_ALT) continue;
        value_alt_t *alt = (value_alt_t *) val;
        alt->current = alt->alternatives;
    }
    composer->done = 0;
}

int
pathcomp_done(pathcomp_t *composer)
{
    assert(composer);
    return composer->done;
}

void
pathcomp_next(pathcomp_t *composer)
{
    list_t *p;
    assert(composer);
    if (composer->done) return;
    for (p = composer->attributes; p; p = p->next) {
        att_t *att = p->el;
        value_t *val = att->value;
        if (val->type != VALUE_ALT) continue;
        value_alt_t *alt = (value_alt_t *) val;
        assert(alt->current);
        alt->current = alt->current->next;
        if (alt->current) return;
        /* an alternative has wrapped around: reset and cycle next alternative */
        alt->current = alt->alternatives;
    }
    composer->done = 1;
}

static int
path_exists(const char *path)
{
    struct stat statbuf;
    assert(path);
    return stat(path, &statbuf) == 0;
}

/**
 * \note The string returned by this function must be deallocated by the user.
 *
 * Do not mix calls to pathcomp_yield() and pathcomp_find() on the same object.
 * pathcomp_find() will leave the object pointing to the \e next alternative!
 */
char *
pathcomp_find(pathcomp_t *composer)
{
    char *path = NULL;
    assert(composer);
    while (!pathcomp_done(composer)) {
        path = pathcomp_yield(composer);
        pathcomp_next(composer);
        if (path_exists(path)) break;
        free(path);
        path = NULL;
    }
    return path;
}
