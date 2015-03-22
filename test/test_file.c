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

/* test config from file */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

static void
test_file(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.file"));
    pathcomp_set(c, "slot", "200403011130");
    is(s = pathcomp_yield(c), "G2_SEV1_L20_BARG_SOL_M15_R50_20040301_113000_V003.hdf.gz");
    free(s);
    ok(!pathcomp_eval_nocopy(c, "to_be_created"));
    pathcomp_free(c);
}

static void
test_file_and_string(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.file"));
    pathcomp_set(c, "slot", "200403011130");
    is(s = pathcomp_yield(c), "G2_SEV1_L20_BARG_SOL_M15_R50_20040301_113000_V003.hdf.gz");
    free(s);
    is(pathcomp_eval_nocopy(c, "to_be_created"), "ABC");
    pathcomp_free(c);
}

static void
test_backslash(void)
{
    pathcomp_t *c = NULL;
    char *s;

    ok(c = pathcomp_new("test.backslash"));
    is(s = pathcomp_eval(c, "value"), "some             value on several lines             which should be concatenated", "backslash continuation");
    free(s);
    is(s = pathcomp_eval(c, "other"), "something from whatever", "backslash continuation in Lua function");
    free(s);
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_file(SRCDIR "/.pathcomprc");
    test_file();
    pathcomp_add_config_from_string("[test.file]\nto_be_created = ABC");
    test_file_and_string();
    test_backslash();
    pathcomp_cleanup();
    done_testing();
}
