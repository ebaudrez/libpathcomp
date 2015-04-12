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

static void
test_no_rewind_after_set(void)
{
    pathcomp_t *c;
    pathcomp_add_config_from_string(
            "[class]\n"
            "root = cache\n"
            "root = ftp\n"
            "root = remote\n"
            "root = storage\n"
            "irrelevant = 0\n"
            );
    ok(c = pathcomp_new("class"));
    is(pathcomp_eval_nocopy(c, "root"), "cache");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "ftp");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "remote");
    is(pathcomp_eval_nocopy(c, "irrelevant"), "0");
    pathcomp_set(c, "irrelevant", "1");
    is(pathcomp_eval_nocopy(c, "irrelevant"), "1");
    is(pathcomp_eval_nocopy(c, "root"), "remote", "do not rewind attributes after set()");
    pathcomp_set(c, "root", "nodef");
    is(pathcomp_eval_nocopy(c, "root"), "nodef", "changing multi-valued attribute to single-valued");
    ok(!pathcomp_next(c));
    pathcomp_free(c);
    pathcomp_cleanup();
}

static void
test_no_rewind_after_add(void)
{
    pathcomp_t *c;
    pathcomp_add_config_from_string(
            "[class]\n"
            "root = cache\n"
            "root = ftp\n"
            "root = remote\n"
            "root = storage\n"
            "irrelevant = 0\n"
            );
    ok(c = pathcomp_new("class"));
    is(pathcomp_eval_nocopy(c, "root"), "cache");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "ftp");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "remote");
    pathcomp_add(c, "root", "nodef");
    is(pathcomp_eval_nocopy(c, "root"), "remote", "do not rewind attribute after add() to same attribute");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "storage");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "nodef");
    ok(!pathcomp_next(c));
    pathcomp_rewind(c);
    is(pathcomp_eval_nocopy(c, "root"), "cache");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "ftp");
    ok(pathcomp_next(c));
    is(pathcomp_eval_nocopy(c, "root"), "remote");
    is(pathcomp_eval_nocopy(c, "irrelevant"), "0");
    pathcomp_add(c, "irrelevant", "1");
    is(pathcomp_eval_nocopy(c, "irrelevant"), "0");
    is(pathcomp_eval_nocopy(c, "root"), "remote", "do not rewind attribute after add() to another attribute");
    pathcomp_free(c);
    pathcomp_cleanup();
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_string(config);
    test_set();
    pathcomp_cleanup();
    test_no_rewind_after_set();
    test_no_rewind_after_add();
    done_testing();
}
