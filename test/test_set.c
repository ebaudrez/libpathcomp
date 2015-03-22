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

/* test pathcomp_set() and pathcomp_add() */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

const char *config = "\
[test.empty]";

static void
test_set(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.empty"));
    is(pathcomp_eval_nocopy(c, "att"), NULL);
    pathcomp_set(c, "att", "123");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_add(c, "att", "456");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_add(c, "att", "789");
    is(pathcomp_eval_nocopy(c, "att"), "123");
    pathcomp_set(c, "att", "abc");
    is(pathcomp_eval_nocopy(c, "att"), "abc");
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_string(config);
    test_set();
    pathcomp_cleanup();
    done_testing();
}
