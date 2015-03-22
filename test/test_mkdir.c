/*
 * Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
 * This file is part of Libpathcomp.
 *
 * Libpathcomp is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Libpathcomp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
 */

/* test pathcomp_mkdir() */

#include <config.h>
#include "tap.h"
#include "taputil.h"
#include "pathcomp.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

/* test pathname ending in a slash - is assumed to represent a directory and not a file */
static void
test_basic_dir_only(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.mkdir"));
    pathcomp_set(c, "compose", "lib/scratch/G2/SEV1/");
    is(s = pathcomp_yield(c), "lib/scratch/G2/SEV1/");
    free(s);
    path_not_exists_ok("lib/scratch");
    path_not_exists_ok("lib/scratch/G2");
    path_not_exists_ok("lib/scratch/G2/SEV1");
    if (!ok(pathcomp_mkdir(c), "call pathcomp_mkdir()")) diag("pathcomp_mkdir: %s", strerror(errno));
    dir_exists_ok("lib");
    dir_exists_ok("lib/scratch");
    dir_exists_ok("lib/scratch/G2");
    dir_exists_ok("lib/scratch/G2/SEV1");
    if (rmdir("lib/scratch/G2/SEV1") == -1) diag("rmdir: %s", strerror(errno));
    if (rmdir("lib/scratch/G2") == -1) diag("rmdir: %s", strerror(errno));
    if (rmdir("lib/scratch") == -1) diag("rmdir: %s", strerror(errno));
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    test_basic();
    test_basic_dir_only();
    pathcomp_cleanup();
    done_testing();
}
