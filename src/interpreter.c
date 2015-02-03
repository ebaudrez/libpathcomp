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
