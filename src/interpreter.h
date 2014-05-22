#ifndef INTERPRETER_INCLUDED
#define INTERPRETER_INCLUDED

#include "config.h"
#include <lua.h>

extern lua_State *interpreter_get_state(void);
extern void       interpreter_cleanup(void);

#endif /* INTERPRETER_INCLUDED */
