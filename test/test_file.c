/* test config from file */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

static void
test_file(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.file"));
    pathcomp_set(c, "slot", "200403011130");
    is(s = pathcomp_yield(c), "G2_SEV1_L20_BARG_SOL_M15_R50_20040301_113000_V003.hdf.gz");
    free(s);
    ok(!pathcomp_eval_nocopy(c, "to_be_created"));
    pathcomp_free(c);
}

static void
test_file_and_string(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.file"));
    pathcomp_set(c, "slot", "200403011130");
    is(s = pathcomp_yield(c), "G2_SEV1_L20_BARG_SOL_M15_R50_20040301_113000_V003.hdf.gz");
    free(s);
    is(pathcomp_eval_nocopy(c, "to_be_created"), "ABC");
    pathcomp_free(c);
}

static void
test_backslash(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.backslash"));
    is(s = pathcomp_eval(c, "value"), "some             value on several lines             which should be concatenated", "backslash continuation");
    free(s);
    is(s = pathcomp_eval(c, "other"), "something from whatever", "backslash continuation in Lua function");
    free(s);
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_file(SRCDIR "/.pathcomprc");
    test_file();
    pathcomp_add_config_from_string("[test.file]\nto_be_created = ABC");
    test_file_and_string();
    test_backslash();
    pathcomp_cleanup();
    done_testing();
}
