/* test pathcomp_mkdir() */

#include "config.h"
#include "tap.h"
#include "taputil.h"
#include "pathcomp.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>

const char *config = "\
[test.mkdir]\n\
";

static void
test_basic(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.mkdir"));
    pathcomp_set(c, "root", "lib/scratch/G2/SEV1");
    pathcomp_set(c, "compose", "my_filename");
    is(s = pathcomp_yield(c), "lib/scratch/G2/SEV1/my_filename");
    free(s);
    if (!dir_exists_ok("lib")) {
        bail_out(0, "cannot find directory 'lib' - are you testing in subdirectory 'test'?");
    }
    path_not_exists_ok("lib/scratch");
    path_not_exists_ok("lib/scratch/G2");
    path_not_exists_ok("lib/scratch/G2/SEV1");
    path_not_exists_ok("lib/scratch/G2/SEV1/my_filename");
    if (!ok(pathcomp_mkdir(c))) diag("pathcomp_mkdir: %s", strerror(errno));
    dir_exists_ok("lib");
    dir_exists_ok("lib/scratch");
    dir_exists_ok("lib/scratch/G2");
    dir_exists_ok("lib/scratch/G2/SEV1");
    if (!path_not_exists_ok("lib/scratch/G2/SEV1/my_filename")) {
        if (unlink("lib/scratch/G2/SEV1/my_filename") == -1) diag("unlink: %s", strerror(errno));
    }
    if (rmdir("lib/scratch/G2/SEV1") == -1) diag("rmdir: %s", strerror(errno));
    if (rmdir("lib/scratch/G2") == -1) diag("rmdir: %s", strerror(errno));
    if (rmdir("lib/scratch") == -1) diag("rmdir: %s", strerror(errno));
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
