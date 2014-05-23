#include "config.h"
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

int
main(void)
{
    plan(NO_PLAN);
    test_literal();
    test_lua();
    interpreter_cleanup();
    done_testing();
}
