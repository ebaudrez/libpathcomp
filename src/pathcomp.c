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
};

static cf_t *config;

static att_t *
attribute_new(char *name, char *value)
{
    att_t *att;
    att = malloc(sizeof *att);
    if (!att) return att;
    att->name = strdup(name);
    att->value = value_new(value);
    return att;
}

/* in order to match the type of list_traversal_t, attribute_free() takes a pointer to pointer */
static int
attribute_free(void **p, void *userdata)
{
    att_t *att = *p;
    if (!att) return 0;
    free(att->name);
    value_free(att->value);
    free(att);
    *p = NULL;
    return 0;
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
find_section_with_name(void **p, void *userdata)
{
    cf_section_t *sec = *p;
    char *name = userdata;
    return !strcmp(sec->name, name);
}

static void
pathcomp_add_attribute(pathcomp_t *composer, cf_kv_t *kv)
{
    att_t *att;
    assert(kv);
    att = attribute_new(kv->key, kv->value);
    if (composer->attributes) list_push(composer->attributes, att);
    else                      composer->attributes = list_new(att);
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
    while ((psec = list_find_first(psec, find_section_with_name, composer->name))) {
        cf_section_t *sec = psec->el;
        list_t       *pkv = sec->entries;
        log_debug(log, "found section with name '%s'", sec->name);
        while (pkv) {
            cf_kv_t *kv = pkv->el;
            log_debug(log, "found key-value pair with name '%s'", kv->key);
            pathcomp_add_attribute(composer, kv);
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
    return composer;
}

void
pathcomp_free(pathcomp_t *composer)
{
    if (!composer) return;
    free(composer->name);
    list_map(composer->attributes, attribute_free, NULL);
    list_free(composer->attributes);
    free(composer->metatable);
    free(composer);
}

static int
find_attribute_with_name(void **p, void *userdata)
{
    att_t *att = *p;
    const char *name = userdata;
    return !strcmp(att->name, name);
}

const char *
pathcomp_eval(pathcomp_t *composer, const char *name)
{
    list_t *p;
    att_t *att;
    assert(composer);
    p = list_find_first(composer->attributes, find_attribute_with_name, (void *) name);
    if (!p) return NULL;
    att = p->el;
    return value_eval(att->value, composer, composer->metatable);
}

void
pathcomp_set(pathcomp_t *composer, const char *name, const void *value)
{
    assert(composer);
    assert(name);
    /* TODO */
}
