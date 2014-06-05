/* test pathcomp_yield() */

#include "config.h"
#include "tap.h"
#include "taputil.h"
#include "pathcomp.h"
#include "list.h"

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
    list_t *got = NULL, *expected;

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
    for (;;) {
        got = list_push(got, pathcomp_yield(c));
        if (!pathcomp_next(c)) break;
    }
    expected = list_from("def",
        "nosync/cache/def",
        "abc",
        "nosync/cache/abc",
        NULL);
    cmp_bag(got, expected, "pathcomp_yield() yields all alternatives");
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    list_free(expected);
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
