/*
 * Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
 * This file is part of Libpathcomp.
 *
 * Libpathcomp is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Libpathcomp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
 */

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

#define PATHCOMP_ACTION_ADD 0
#define PATHCOMP_ACTION_REPLACE 1
#define PATHCOMP_ACTION_ADD_IF 2
#define PATHCOMP_ORIGIN_RUNTIME NULL
#define PATHCOMP_ATT_ROOT "root"
#define PATHCOMP_ATT_COMPOSE "compose"
#define PATHCOMP_ATT_COPY "copy-from"

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
pathcomp_add_or_replace(pathcomp_t *composer, const char *name, const char *value, const char *origin, int action)
{
    list_t *patt;
    assert(name);
    assert(value);
    patt = list_find_first(composer->attributes, (list_traversal_t *) att_name_equal_to, (void *) name);
    if (!patt) {
        att_t *new;
        new = att_new(name, value, origin);
        composer->attributes = list_push(composer->attributes, new);
        return;
    }
    /* there happens to be an attribute with this name already */
    if (action == PATHCOMP_ACTION_ADD_IF) {
        const char *old_origin;
        int old_inherited, new_inherited;
        old_origin = att_get_origin(patt->el);
        old_inherited = old_origin && (strcmp(old_origin, composer->name) != 0);
        new_inherited = origin && (strcmp(origin, composer->name) != 0);
        if (new_inherited && !old_inherited) return;
        else if (!new_inherited && old_inherited) action = PATHCOMP_ACTION_REPLACE;
        else action = PATHCOMP_ACTION_ADD;
    }
    if (action == PATHCOMP_ACTION_REPLACE) att_replace_value(patt->el, value, origin);
    else if (action == PATHCOMP_ACTION_ADD) att_add_value(patt->el, value);
    else assert(0);
}

static void pathcomp_add_atts_from_sections(pathcomp_t *composer, char *section_name);

static void
pathcomp_add_atts_from_entries(pathcomp_t *composer, char *section_name, list_t *entry)
{
    assert(composer);
    assert(section_name);
    while (entry) {
        cf_kv_t *kv = entry->el;
        assert(kv);
        pathcomp_add_or_replace(composer, kv->key, kv->value, section_name, PATHCOMP_ACTION_ADD_IF);
        if (strcmp(kv->key, PATHCOMP_ATT_COPY) == 0) {
            char *parent_section_name = kv->value;
            pathcomp_add_atts_from_sections(composer, parent_section_name);
        }
        entry = entry->next;
    }
    /* after setting or adding attributes, must rewind all attributes */
    pathcomp_rewind(composer);
}

static void
pathcomp_add_atts_from_sections(pathcomp_t *composer, char *section_name)
{
    list_t *psec;
    assert(composer);
    assert(section_name);
    if (!config) return;
    psec = config->sections;
    while ((psec = list_find_first(psec, (list_traversal_t *) find_section_with_name, section_name))) {
        cf_section_t *section = psec->el;
        pathcomp_add_atts_from_entries(composer, section->name, section->entries);
        psec = psec->next;
    }
}

static void
pathcomp_make_from_config(pathcomp_t *composer)
{
    assert(composer);
    pathcomp_add_atts_from_sections(composer, composer->name);
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

pathcomp_t *
pathcomp_clone(pathcomp_t *composer)
{
    pathcomp_t *clone;
    assert(composer);
    clone = malloc(sizeof *clone);
    if (!clone) return clone;
    clone->name = strdup(composer->name);
    clone->attributes = list_transform(composer->attributes, (list_transform_t *) att_clone, NULL);
    clone->metatable = strdup(composer->metatable);
    clone->done = composer->done;
    clone->started = composer->started;
    return clone;
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
    const char *s;
    s = pathcomp_eval_nocopy(composer, name);
    return s ? strdup(s) : NULL;
}

char *
pathcomp_yield(pathcomp_t *composer)
{
    buf_t path;
    const char *root, *compose;
    assert(composer);
    buf_init(&path, 0);
    root = pathcomp_eval_nocopy(composer, PATHCOMP_ATT_ROOT);
    compose = pathcomp_eval_nocopy(composer, PATHCOMP_ATT_COMPOSE);
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
    pathcomp_add_or_replace(composer, name, value, PATHCOMP_ORIGIN_RUNTIME, PATHCOMP_ACTION_REPLACE);
    pathcomp_rewind(composer);
}

void
pathcomp_add(pathcomp_t *composer, const char *name, const char *value)
{
    assert(composer);
    assert(name);
    assert(value);
    pathcomp_add_or_replace(composer, name, value, PATHCOMP_ORIGIN_RUNTIME, PATHCOMP_ACTION_ADD);
    pathcomp_rewind(composer);
}

void
pathcomp_rewind(pathcomp_t *composer)
{
    list_t *p;
    assert(composer);
    for (p = composer->attributes; p; p = p->next) att_rewind(p->el);
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
        /* alternative has wrapped around: rewind and cycle next attribute */
        att_rewind(p->el);
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

char *
pathcomp_dump(pathcomp_t *composer)
{
    buf_t buf;
    buf_init(&buf, 0);
    buf_addf(&buf, "composer object at 0x%x\n", composer);
    buf_addf(&buf, "  class: %s\n", composer->name);
    buf_addf(&buf, "  metatable: %s\n", composer->metatable);
    buf_addf(&buf, "  done: %d\n", composer->done);
    buf_addf(&buf, "  started: %d\n", composer->started);
    buf_addf(&buf, "  attributes:\n");
    list_foreach(composer->attributes, (list_traversal_t *) att_dump, &buf);
    return buf_detach(&buf, NULL);
}
