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
#include "value.c"
#include "interpreter.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

static void
test_block(void)
{
    char *s;
    s = value_match_and_extract_block("ident { contents }", "ident");
    is(s, " contents ");
    free(s);
    s = value_match_and_extract_block("ident{contents }", "ident");
    is(s, "contents ");
    free(s);
    s = value_match_and_extract_block("ident            {contents }  ", "ident");
    is(s, "contents ");
    free(s);
    s = value_match_and_extract_block("ident contents }", "ident");
    is(s, NULL);
    s = value_match_and_extract_block("ident { contents }", "keyword");
    is(s, NULL);
}

static void
test_string(void)
{
    value_t *val;

    ok(val = value_new_string("abc"));
    cmp_ok(val->type, "==", VALUE_STRING);
    is(value_eval(val, NULL, NULL), "abc");
    value_free(val);
}

static void
test_lua(void)
{
    value_t *val;

    ok(val = value_new_lua("return 1+2"));
    cmp_ok(val->type, "==", VALUE_LUA);
    is(value_eval(val, NULL, NULL), "3");
    value_free(val);
    ok(val = value_new_lua("return some_weird_name + 2"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "use of undefined variables raises an error");
    value_free(val);
    ok(val = value_new_lua("return self.slot"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "inadvertent use of value_eval() with NULL composer raises an error");
    value_free(val);
    /* can't test "Lua values" more thoroughly without a composer object and a
     * metatable, but this functionality is tested in test_lua.c */
}

#define STRINGIFY_HELPER(n) #n
#define STRINGIFY(n) STRINGIFY_HELPER(n)
static void
test_int(void)
{
    value_t *val;
    ok(val = value_new_int(123));
    cmp_ok(val->type, "==", VALUE_INT);
    is(value_eval(val, NULL, NULL), "123");
    value_free(val);
    ok(val = value_new_int(-789));
    cmp_ok(val->type, "==", VALUE_INT);
    is(value_eval(val, NULL, NULL), "-789");
    value_free(val);
    ok(val = value_new_int(INT_MAX));
    cmp_ok(val->type, "==", VALUE_INT);
    cmp_ok(val->source.integer, "==", INT_MAX);
    is(value_eval(val, NULL, NULL), STRINGIFY(INT_MAX));
    value_free(val);
    ok(val = value_new_int(INT_MIN));
    cmp_ok(val->type, "==", VALUE_INT);
    cmp_ok(val->source.integer, "==", INT_MIN);
    /* cannot stringify INT_MIN as it is not a literal */
    int converted = atoi(value_eval(val, NULL, NULL));
    cmp_ok(converted, "==", INT_MIN);
    value_free(val);
    ok(val = value_new_int(5001));
    is(value_eval(val, NULL, NULL), "5001");
    is(value_eval(val, NULL, NULL), "5001", "force two consecutive evaluations; should not leak memory");
    value_free(val);
}

static void
test_auto(void)
{
    value_t *val;
    ok(val = value_new_auto(" some string contents"));
    cmp_ok(val->type, "==", VALUE_STRING);
    is(value_eval(val, NULL, NULL), " some string contents");
    value_free(val);
    ok(val = value_new_auto("lua { return 1+2 }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    is(value_eval(val, NULL, NULL), "3");
    value_free(val);
    ok(val = value_new_auto("lua [ return 1+2+3 ]"));
    cmp_ok(val->type, "==", VALUE_STRING);
    is(value_eval(val, NULL, NULL), "lua [ return 1+2+3 ]", "invalid Lua function syntax leads to interpretation as string");
    value_free(val);
    ok(val = value_new_auto("lua { return 1+2+3"));
    cmp_ok(val->type, "==", VALUE_STRING);
    is(value_eval(val, NULL, NULL), "lua { return 1+2+3", "missing closing brace leads to interpretation as string");
    value_free(val);
    ok(val = value_new_auto("123"));
    cmp_ok(val->type, "==", VALUE_STRING, "value_new_auto() doesn't recognize integers");
    is(value_eval(val, NULL, NULL), "123");
    value_free(val);
    ok(val = value_new_auto("-987"));
    cmp_ok(val->type, "==", VALUE_STRING, "value_new_auto() doesn't recognize integers");
    is(value_eval(val, NULL, NULL), "-987");
    value_free(val);
    ok(val = value_new_auto("3.141592"));
    cmp_ok(val->type, "==", VALUE_STRING, "value_new_auto() doesn't recognize reals");
    is(value_eval(val, NULL, NULL), "3.141592");
    value_free(val);
    ok(val = value_new_auto("-1."));
    cmp_ok(val->type, "==", VALUE_STRING, "value_new_auto() doesn't recognize reals");
    is(value_eval(val, NULL, NULL), "-1.");
    value_free(val);
}

int
main(void)
{
    plan(NO_PLAN);
    test_block();
    test_string();
    test_lua();
    test_int();
    test_auto();
    interpreter_cleanup();
    done_testing();
}
