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

/* test Libpathcomp with glob(3) */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"
#include "taputil.h"
#include "list.h"
#include <assert.h>
#include <string.h>
#if HAVE_GLOB_H
#include <glob.h>
#endif

static void
test_glob()
{
    pathcomp_t *c = NULL;
    char *pattern;
    char **p;
    list_t *got = NULL, *expected = NULL;
#if HAVE_GLOB_H
    glob_t buf;
#endif

    pathcomp_add_config_from_string("[class]\n\
            root       = " SRCDIR "/lib/glob\n\
            instrument = *\n\
            imager     = SEV?\n\
            prefix     = lua { return string.format('%s_%s_L20_HR_SOL_TH', self.instrument, self.imager) }\n\
            compose    = lua { return string.format('%s/%s/%s/README', self.instrument, self.imager, self.prefix) }\n\
            ");
    ok(c = pathcomp_new("class"));
    is(pathcomp_find(c), NULL, "regular find should not find anything");
    is(pattern = pathcomp_yield(c), SRCDIR "/lib/glob/*/SEV?/*_SEV?_L20_HR_SOL_TH/README", "wildcard characters correctly substitute for instrument and imager");
#if HAVE_GLOB
    cmp_ok(glob(pattern, 0, NULL, &buf), "==", 0, "glob()");
    cmp_ok(buf.gl_pathc, "==", 4, "returns 4 results");
    ok(buf.gl_pathv, "results in .gl_pathv");
    for (p = buf.gl_pathv; *p; p++) {
        got = list_push(got, strdup(*p));
    }
    globfree(&buf);
    expected = list_from(
        SRCDIR "/lib/glob/G2/SEV1/G2_SEV1_L20_HR_SOL_TH/README",
        SRCDIR "/lib/glob/G3/SEV3/G3_SEV3_L20_HR_SOL_TH/README",
        SRCDIR "/lib/glob/G1/SEV3/G1_SEV3_L20_HR_SOL_TH/README",
        SRCDIR "/lib/glob/G1/SEV2/G1_SEV2_L20_HR_SOL_TH/README",
        NULL);
    cmp_bag(got, expected, "glob() returns all expected files");
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    list_free(expected);
#endif
    free(pattern);
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    test_glob();
    pathcomp_cleanup();
    done_testing();
}
