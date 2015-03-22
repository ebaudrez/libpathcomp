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

/* test empty config */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

static void
test_empty(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.new"));
    is(pathcomp_eval(c, "abcdef"), NULL, "returns NULL but doesn't crash when evaluating missing attribute");
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
