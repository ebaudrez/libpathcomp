#include "config.h"
#include "tap.h"
#include "value.h"
#include <string.h>

static void
test_literal(void)
{
    value_t *val;

    ok(val = value_new("abc"));
    is(value_eval(val, NULL, NULL), "abc");
    value_free(val);
}

int
main(void)
{
    plan(NO_PLAN);
    test_literal();
    /* can't test value_lua_t without a composer object and a metatable, but
     * this functionality is tested in lua.c */
    done_testing();
}
