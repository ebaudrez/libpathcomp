/* test value.c */

#include "config.h"
#include "tap.h"
#include "value.h"
#include "interpreter.h"
#include <string.h>
#include "log.h"

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
    ok(val = value_new("lua { return some_weird_name + 2 }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "use of undefined variables raises an error");
    value_free(val);
    ok(val = value_new("lua { return self.slot }"));
    cmp_ok(val->type, "==", VALUE_LUA);
    ok(!value_eval(val, NULL, NULL), "inadvertent use of value_eval() with NULL composer raises an error");
    value_free(val);
    /* can't test value_lua_t more thoroughly without a composer object and a
     * metatable, but this functionality is tested in lua.c */
}

static void
test_alt_2elements(void)
{
    value_t *val, *val2;

    ok(val = value_new("abc"));
    cmp_ok(val->type, "==", VALUE_LITERAL);
    is(value_eval(val, NULL, NULL), "abc");
    ok(val2 = value_new("def"));
    cmp_ok(val2->type, "==", VALUE_LITERAL);
    value_add(&val, val2);
    cmp_ok(val->type, "==", VALUE_ALT);

    is(value_eval(val, NULL, NULL), "abc");
    ok(value_next(val));
    is(value_eval(val, NULL, NULL), "def");
    ok(!value_next(val));
    ok(!value_next(val), "value_next() doesn't recycle");
    value_free(val);
}

static void
test_alt_4elements(void)
{
    value_t *val, *val2;

    ok(val = value_new("abc"));
    cmp_ok(val->type, "==", VALUE_LITERAL);
    is(value_eval(val, NULL, NULL), "abc");
    ok(val2 = value_new("def"));
    cmp_ok(val2->type, "==", VALUE_LITERAL);
    value_add(&val, val2);
    cmp_ok(val->type, "==", VALUE_ALT);
    ok(val2 = value_new("lua { return 'g' .. 'h' .. 'i' }"));
    cmp_ok(val2->type, "==", VALUE_LUA);
    value_add(&val, val2);
    cmp_ok(val->type, "==", VALUE_ALT);
    ok(val2 = value_new("jkl"));
    cmp_ok(val2->type, "==", VALUE_LITERAL);
    value_add(&val, val2);
    cmp_ok(val->type, "==", VALUE_ALT);

    is(value_eval(val, NULL, NULL), "abc");
    ok(value_next(val));
    is(value_eval(val, NULL, NULL), "def");
    ok(value_next(val));
    is(value_eval(val, NULL, NULL), "ghi");
    ok(value_next(val));
    is(value_eval(val, NULL, NULL), "jkl");
    ok(!value_next(val));
    ok(!value_next(val), "value_next() doesn't recycle");
    value_free(val);
}

int
main(void)
{
    plan(NO_PLAN);
    log_set_stream_global(stdout);
    log_set_prefix_global("# ");
    test_literal();
    test_lua();
    test_alt_2elements();
    test_alt_4elements();
    interpreter_cleanup();
    log_cleanup();
    done_testing();
}
