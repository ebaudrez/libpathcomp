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

/* test complete usage examples */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"
#include "taputil.h"
#include <string.h>

static void
test_clone_log_file(void)
{
    pathcomp_t *hdf, *log;
    char *first_combination, *matching_combination, *path;
    pathcomp_add_config_from_string(
            "[hdf]\n"
            "root      = " SRCDIR "/lib/find/cache\n"
            "root      = " SRCDIR "/lib/find/storage\n"
            "root      = " SRCDIR "/lib/find/ftp\n"
            "root      = " SRCDIR "/lib/find/remote\n"
            "dir       = G5\n"
            "basename  = two\n"
            "extension = .hdf\n"
            "extension = .hdf.gz\n"
            "compose   = lua { return string.format('%s/%s%s', self.dir, self.basename, self.extension) }\n"
            );
    ok(hdf = pathcomp_new("hdf"));
    is(first_combination = pathcomp_yield(hdf), SRCDIR "/lib/find/cache/G5/two.hdf");
    path_not_exists_ok(first_combination, "first of the combinations is not in cache");
    is(matching_combination = pathcomp_find(hdf), SRCDIR "/lib/find/storage/G5/two.hdf", "find G5/two.hdf in storage");
    path_exists_ok(matching_combination, "basic find() sanity!");
    isnt(matching_combination, first_combination, "make sure the test makes sense by ensuring matching != first");
    ok(log = pathcomp_clone(hdf));
    is(path = pathcomp_yield(log), SRCDIR "/lib/find/storage/G5/two.hdf", "clone starts out with identical state");
    path_exists_ok(path, "must exist by construction");
    free(path);
    pathcomp_set(log, "extension", ".log");
    is(path = pathcomp_yield(log), SRCDIR "/lib/find/storage/G5/two.log", "change extension to .log and yield() pathname");
    path_exists_ok(path, "must yield an existing pathname");
    isnt(pathcomp_eval_nocopy(log, "root"), SRCDIR "/lib/find/cache", "don't rewind after set() - it makes clone() less useful than it could be");
    /* change extension in first_combination and matching_combination */
    strcpy(strstr(first_combination, ".hdf"), ".log");
    strcpy(strstr(matching_combination, ".hdf"), ".log");
    isnt(path, first_combination, "it's not the log file associated to the _first_ combination of alternatives (i.e., don't rewind!)");
    is(path, matching_combination, "but the log file associated to the matching combination!");
    free(first_combination);
    free(matching_combination);
    free(path);
    pathcomp_free(hdf);
    pathcomp_free(log);
    pathcomp_cleanup();
}

int
main(void)
{
    plan(NO_PLAN);
    test_clone_log_file();
    done_testing();
}
