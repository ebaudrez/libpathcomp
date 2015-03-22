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
#include "interpreter.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>

static lua_State *state = NULL;

static void
interpreter_initialize(lua_State *L)
{
    assert(L);
    luaL_openlibs(L);
}

lua_State *
interpreter_get_state(void)
{
    if (state) return state;
    state = luaL_newstate();
    interpreter_initialize(state);
    return state;
}

void
interpreter_cleanup(void)
{
    if (!state) return;
    assert(!lua_gettop(state));
    lua_close(state);
    state = NULL;
}
