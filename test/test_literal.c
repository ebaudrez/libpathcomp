/* test literals in config */

#include "config.h"
#include "tap.h"
#include "pathcomp.h"

const char *config = "\
[test.literal1]\n\
    key = value\n\
\n\
[test.literal2]\n\
    instrument = G2\n\
    imager     = SEV1\n\
    level      = 20\n\
    resolution = HR\n\
    product    = SOL_TH\n\
";

static void
test_literal1()
{
    pathcomp_t *composer = NULL;
    ok(composer = pathcomp_new("test.literal1"));
    const char *val = pathcomp_eval_nocopy(composer, "key");
    is(val, "value");
    char *s = pathcomp_eval(composer, "key");
    is(s, val);
    free(s);
    pathcomp_free(composer);
}

static void
test_literal2()
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.literal2"));
    is(pathcomp_eval_nocopy(c, "instrument"), "G2");
    is(pathcomp_eval_nocopy(c, "imager"), "SEV1");
    is(pathcomp_eval_nocopy(c, "level"), "20");   /* everything evaluates as strings */
    is(pathcomp_eval_nocopy(c, "resolution"), "HR");
    is(pathcomp_eval_nocopy(c, "product"), "SOL_TH");
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_use_config_from(config);
    test_literal1();
    test_literal2();
    pathcomp_cleanup();
    done_testing();
}
