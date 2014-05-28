#include "config.h"
#include "tap.h"
#include "pathcomp.h"

const char *config = "\
[test.basic.1]\n\
    compose = my_filename\n\
\n\
[test.basic.2]\n\
    root = whatever\n\
\n\
[test.basic.3]\n\
    root    = /mnt/archive\n\
    compose = G2/SEV1/\n\
\n\
[test.basic.4]\n\
[test.basic.5]\n\
    att = 123\n\
[test.basic.6]\n\
    root = \n\
    compose = abc\n\
[test.basic.7]\n\
    root = \n\
    root = nosync/cache\n\
    compose = lua { return 'def' }\n\
    compose = abc\n\
";

static void
test_basic(void)
{
    pathcomp_t *c = NULL;
    char *s;
    ok(c = pathcomp_new("test.basic.1"));
    is(s = pathcomp_yield(c), "my_filename");
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.2"));
    is(s = pathcomp_yield(c), "whatever/");
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.3"));
    is(s = pathcomp_yield(c), "/mnt/archive/G2/SEV1/");
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.4"));
    is(s = pathcomp_yield(c), NULL);
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.5"));
    is(s = pathcomp_yield(c), NULL);
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.6"));
    is(s = pathcomp_yield(c), "abc");
    free(s);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.basic.7"));
    is(s = pathcomp_yield(c), "def");
    free(s);
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_use_config_from(config);
    test_basic();
    pathcomp_cleanup();
    done_testing();
}
