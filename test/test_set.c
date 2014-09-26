/* test pathcomp_set() and pathcomp_add() */

#include "config.h"
#include "tap.h"
#include "pathcomp.h"

const char *config = "\
[test.empty]";

static void
test_set(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.empty"));
    is(pathcomp_eval_nocopy(c, "att"), NULL);
    pathcomp_set(c, "att", "123");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_add(c, "att", "456");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_add(c, "att", "789");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_set(c, "att", "abc");
    is(pathcomp_eval_nocopy(c, "att"), "abc");
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_string(config);
    test_set();
    pathcomp_cleanup();
    done_testing();
}
