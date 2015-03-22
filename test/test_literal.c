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

/* test literals in config */

#include <config.h>
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
    pathcomp_add_config_from_string(config);
    test_literal1();
    test_literal2();
    pathcomp_cleanup();
    done_testing();
}
