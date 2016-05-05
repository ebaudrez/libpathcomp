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

/* test value.h */

#include <config.h>
#include "tap.h"
#include "value.h"
#include "interpreter.h"
#include <string.h>

static void
test_literal(void)
{
    value_t *val;

    ok(val = value_new("abc"));
    cmp_ok(val->type, "==", VALUE_LITERAL);
    is(value_eval(val, NULL, NULL), "abc");
    value_free(val);
}

static void
test_lua(void)
{
    value_t *val;

    ok(val = value_new("lua { return 1+2 }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    is(value_eval(val, NULL, NULL), "3");
    value_free(val);
    ok(val = value_new("lua [ return 1+2+3 ]"));
    cmp_ok(val->type, "==", VALUE_LITERAL);
    is(value_eval(val, NULL, NULL), "lua [ return 1+2+3 ]", "invalid Lua function syntax leads to interpretation as literal string");
    value_free(val);
    ok(val = value_new("lua { return 1+2+3"));
    cmp_ok(val->type, "==", VALUE_LITERAL);
    is(value_eval(val, NULL, NULL), "lua { return 1+2+3", "missing closing brace leads to interpretation as literal string");
    value_free(val);
    ok(val = value_new("lua{ return 1+2+3+4 }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    is(value_eval(val, NULL, NULL), "10", "allows open brace immediately following Lua keyword");
    value_free(val);
    ok(val = value_new("lua { return some_weird_name + 2 }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "use of undefined variables raises an error");
    value_free(val);
    ok(val = value_new("lua { return self.slot }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "inadvertent use of value_eval() with NULL composer raises an error");
    value_free(val);
    /* can't test value_lua_t more thoroughly without a composer object and a
     * metatable, but this functionality is tested in test_lua.c */
}

int
main(void)
{
    plan(NO_PLAN);
    test_literal();
    test_lua();
    interpreter_cleanup();
    done_testing();
}
