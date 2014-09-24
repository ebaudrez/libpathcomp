/* test empty config */

#include "config.h"
#include "tap.h"
#include "pathcomp.h"

static void
test_empty(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.new"));
    pathcomp_set(c, "root", "lib/scratch/G2/SEV1");
    pathcomp_set(c, "compose", "my_filename");
    is(s = pathcomp_yield(c), "lib/scratch/G2/SEV1/my_filename");
    free(s);
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    test_empty();
    pathcomp_cleanup();
    done_testing();
}
