#include <config.h>
#include "pathcomp.h"
#include "list.h"
#include "cf.h"
#include "att.h"
#include "pathcomp/log.h"
#include "interpreter.h"
#include "buf.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

struct pathcomp_t {
    char   *name;
    list_t *attributes;
    char   *metatable;  /* Name of the Lua metatable */
    int     done;       /* Iterator state */
    int     started;    /* pathcomp_find() has been called at least once */
};

static cf_t *config;

void
pathcomp_add_config_from_string(const char *string)
{
    assert(string);
    if (!config) config = cf_new();
    cf_add_from_string(config, string);
}

void
pathcomp_add_config_from_file(const char *filename)
{
    assert(filename);
    if (!config) config = cf_new();
    cf_add_from_file(config, filename);
}

void
pathcomp_cleanup(void)
{
    cf_free(config);
    config = NULL;
    interpreter_cleanup();
}

static int
find_section_with_name(cf_section_t *sec, char *name)
{
    return !strcmp(sec->name, name);
}

static void
pathcomp_add_or_replace(pathcomp_t *composer, const char *name, const char *value, int replace)
{
    list_t *patt;
    assert(name);
    assert(value);
    patt = list_find_first(composer->attributes, (list_traversal_t *) att_name_equal_to, (void *) name);
    if (!patt) {
        att_t *new;
        new = att_new(name, value);
        composer->attributes = list_push(composer->attributes, new);
        return;
    }
    /* there happens to be an attribute with this name already */
    if (replace) att_replace_value(patt->el, value);
    else att_add_value(patt->el, value);
}

static void
pathcomp_make_from_config(pathcomp_t *composer)
{
    list_t *psec;
    assert(composer);
    if (!config) return;
    psec = config->sections;
    while ((psec = list_find_first(psec, (list_traversal_t *) find_section_with_name, composer->name))) {
        cf_section_t *sec = psec->el;
        list_t       *pkv = sec->entries;
        /*pathcomp_log_debug("found section with name '%s'", sec->name);*/
        while (pkv) {
            cf_kv_t *kv = pkv->el;
            /*pathcomp_log_debug("found key-value pair with name '%s'", kv->key);*/
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
    value = pathcomp_eval_nocopy(composer, name); /* lua_pushstring() creates a copy */
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
    composer->started = 0;
    return composer;
}

void
pathcomp_free(pathcomp_t *composer)
{
    if (!composer) return;
    free(composer->name);
    list_foreach(composer->attributes, (list_traversal_t *) att_free, NULL);
    list_free(composer->attributes);
    free(composer->metatable);
    free(composer);
}

const char *
pathcomp_eval_nocopy(pathcomp_t *composer, const char *name)
{
    list_t *p;
    att_t *att;
    assert(composer);
    p = list_find_first(composer->attributes, (list_traversal_t *) att_name_equal_to, (void *) name);
    if (!p) return NULL;
    att = p->el;
    return att_eval(att, composer, composer->metatable);
}

char *
pathcomp_eval(pathcomp_t *composer, const char *name)
{
    return strdup(pathcomp_eval_nocopy(composer, name));
}

char *
pathcomp_yield(pathcomp_t *composer)
{
    buf_t path;
    const char *root, *compose;
    assert(composer);
    buf_init(&path, 0);
    root = pathcomp_eval_nocopy(composer, "root");
    compose = pathcomp_eval_nocopy(composer, "compose");
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

void
pathcomp_set(pathcomp_t *composer, const char *name, const char *value)
{
    assert(composer);
    assert(name);
    assert(value);
    pathcomp_add_or_replace(composer, name, value, 1);
    pathcomp_reset(composer);
}

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
    for (p = composer->attributes; p; p = p->next) att_reset(p->el);
    composer->done = 0;
    composer->started = 0;
}

int
pathcomp_done(pathcomp_t *composer)
{
    assert(composer);
    return composer->done;
}

int
pathcomp_next(pathcomp_t *composer)
{
    list_t *p;
    assert(composer);
    if (composer->done) return 0;
    for (p = composer->attributes; p; p = p->next) {
        if (att_next(p->el)) return 1;
        /* alternative has wrapped around: reset and cycle next attribute */
        att_reset(p->el);
    }
    composer->done = 1;
    return 0;
}

static int
path_exists(const char *path)
{
    struct stat statbuf;
    assert(path);
    return stat(path, &statbuf) == 0;
}

char *
pathcomp_find(pathcomp_t *composer)
{
    char *path;
    assert(composer);
    for (;;) {
        if (composer->started) pathcomp_next(composer);
        composer->started = 1;
        if (pathcomp_done(composer)) break;
        path = pathcomp_yield(composer);
        if (path && path_exists(path)) return path;
        free(path);
    }
    return NULL;
}

int
pathcomp_mkdir(pathcomp_t *composer)
{
    char *path, *p;
    int success = 1;
    assert(composer);
    path = pathcomp_yield(composer);
    p = path;
    for (;;) {
        while (*p && *p == '/') ++p; /* skip initial slashes */
        while (*p && *p != '/') ++p; /* then seek to slash after first component */
        if (!*p) break;
        *p = '\0';
        if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            int sv = errno;
            if (sv != EEXIST) {
                pathcomp_log_error("mkdir '%s': %s", path, strerror(sv));
                success = 0;
            }
        }
        *p++ = '/';
    }
    free(path);
    return success;
}
