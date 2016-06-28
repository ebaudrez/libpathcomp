/*
 * Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>
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
#include "value.h"
#include "interpreter.h"
#include "pathcomp/log.h"
#include "buf.h"
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lua.h>
#include <lauxlib.h>

/*
 * \name Routines specific to string values
 * \{
 */

static value_t *
value_new_string(const char *text)
{
    value_t *val;
    assert(text);
    val = malloc(sizeof *val);
    if (!val) return val;
    val->type = VALUE_STRING;
    val->u.string = strdup(text);
    return val;
}

static value_t *
value_clone_string(value_t *val)
{
    value_t *clone;
    assert(val);
    clone = malloc(sizeof *clone);
    if (!clone) return clone;
    clone->type = VALUE_STRING;
    clone->u.string = strdup(val->u.string);
    return clone;
}

/*
 * \}
 * \name Routines specific to Lua values
 * \{
 */

static char *
is_lua_code(const char *text)
{
    buf_t buf;
    char *p;
    const char *keyword  = "lua",
               *preamble = "local self = ...; ";
    assert(text);
    buf_init(&buf, strlen(text) + strlen(preamble));
    buf_addstr(&buf, text);
    p = buf.buf;
    if (strlen(p) < strlen(keyword)) goto fail;
    if (strncmp(p, keyword, strlen(keyword)) != 0) goto fail;
    p += strlen(keyword);
    while (*p && isspace(*p)) ++p;
    if (*p == '\0' || *p != '{') goto fail;
    ++p;
    buf_splicestr(&buf, 0, p - buf.buf, preamble);
    p = strrchr(buf.buf, '}');
    if (!p) goto fail;
    buf_setlen(&buf, p - buf.buf);
    return buf_detach(&buf, NULL);
fail:
    buf_release(&buf);
    return NULL;
}

static value_t *
value_new_lua(const char *source)
{
    value_t *val;
    assert(source);
    val = malloc(sizeof *val);
    if (!val) return val;
    val->type = VALUE_LUA;
    val->u.lua.source = strdup(source);
    val->u.lua.result = NULL;
    return val;
}

static value_t *
value_clone_lua(value_t *val)
{
    value_t *clone;
    assert(val);
    clone = malloc(sizeof *clone);
    if (!clone) return clone;
    clone->type = VALUE_LUA;
    clone->u.lua.source = strdup(val->u.lua.source);
    clone->u.lua.result = val->u.lua.result ? strdup(val->u.lua.result) : NULL;
    return clone;
}

/*
 * \param composer Pointer to composer object
 * \param metatable Name of the Lua metatable
 *
 * \a composer and \a metatable may be null if the Lua code to be evaluated
 * does not need access to other attributes in the composer object via 'self'.
 * This is mainly useful for testing. Otherwise, it is probably a mistake.
 * Fortunately, the user does not normally have access to value_eval() and
 * value_eval_lua(). It will be called for him by pathcomp_eval(), and the
 * composer and metatable arguments will be properly set. Hence, the user will
 * always have access to 'self' in the Lua code.
 */
static const char *
value_eval_lua(value_t *val, void *composer, const char *metatable)
{
    lua_State  *L = interpreter_get_state();
    void      **p;
    int         nargs = 0;
    const char *s;
    assert(val);
    if (luaL_loadstring(L, val->u.lua.source) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        pathcomp_log_error("cannot parse Lua code: %s", error);
        lua_pop(L, 1);
        free(val->u.lua.result);
        return val->u.lua.result = NULL;
    }
    if (composer && metatable) {
        p = lua_newuserdata(L, sizeof(*p));
        *p = composer;
        luaL_getmetatable(L, metatable);
        lua_setmetatable(L, -2);
        nargs = 1;
    }
    if (lua_pcall(L, nargs, 1, 0) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        pathcomp_log_error("cannot execute Lua code: %s", error);
        lua_pop(L, 1);
        free(val->u.lua.result);
        return val->u.lua.result = NULL;
    }
    free(val->u.lua.result);
    s = lua_tostring(L, -1);
    val->u.lua.result = s ? strdup(s) : NULL;
    lua_pop(L, 1);
    return val->u.lua.result;
}

/*
 * \}
 * \name Generic \a value_t routines
 * \{
 */

value_t *
value_new(const char *text)
{
    char *source;
    assert(text);
    if ((source = is_lua_code(text))) {
        value_t *val = value_new_lua(source);
        free(source);
        return val;
    }
    else {
        return value_new_string(text);
    }
}

value_t *
value_clone(value_t *val)
{
    assert(val);
    switch (val->type) {
        case VALUE_STRING:
            return value_clone_string(val);
        case VALUE_LUA:
            return value_clone_lua(val);
        default:
            assert(0);
    }
}

void
value_free(value_t *val)
{
    if (!val) return;
    switch (val->type) {
        case VALUE_STRING:
            free(val->u.string);
            free(val);
            break;
        case VALUE_LUA:
            free(val->u.lua.source);
            free(val->u.lua.result);
            free(val);
            break;
        default:
            assert(0);
    }
}

/*
 * \a composer and \a metatable may be null if the Lua code to be evaluated
 * does not need access to other attributes in the composer object via 'self'.
 *
 * This function returns a pointer to internal storage. There are no guarantees
 * on the lifetime of the object pointed to by the return value of this
 * function.
 */
const char *
value_eval(value_t *val, void *composer, const char *metatable)
{
    assert(val);
    switch (val->type) {
        case VALUE_STRING:
            return val->u.string;
        case VALUE_LUA:
            return value_eval_lua(val, composer, metatable);
        default:
            assert(0);
    }
}

void
value_dump(value_t *val, value_dump_info_t *info)
{
    buf_t *buf;
    char marker[] = " ";
    assert(val);
    assert(info);
    buf = info->buf;
    if (val == info->current) strncpy(marker, "*", (sizeof marker) - 1);
    switch (val->type) {
        case VALUE_STRING:
            buf_addf(buf, "       %sstring(0x%x) | %s\n", marker, val, val->u.string);
            break;
        case VALUE_LUA:
            buf_addf(buf, "       %slua(0x%x)     | %s | (source:) %s\n", marker, val, val->u.lua.result ? val->u.lua.result : "(null)", val->u.lua.source);
            break;
        default:
            assert(0);
    }
}

/*
 * \}
 */
