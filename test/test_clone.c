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

/* test pathcomp_clone() */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

static void
test_clone1(void)
{
    pathcomp_t *orig = NULL, *clone = NULL;
    pathcomp_add_config_from_string(
            "[test.clone]\n"
            "attribute = 123\n"
            "attribute = lua { return '456' }\n"
            "attribute = 789\n"
            "other     = nothing");
    ok(orig = pathcomp_new("test.clone"));
    is(pathcomp_eval_nocopy(orig, "attribute"), "123");
    is(pathcomp_eval_nocopy(orig, "other"), "nothing");
    ok(pathcomp_next(orig));
    is(pathcomp_eval_nocopy(orig, "attribute"), "456");
    is(pathcomp_eval_nocopy(orig, "other"), "nothing");
    ok(!pathcomp_done(orig));
    ok(clone = pathcomp_clone(orig), "create clone");
    is(pathcomp_eval_nocopy(clone, "attribute"), "456", "clone starts out with copy of state");
    is(pathcomp_eval_nocopy(clone, "other"), "nothing");
    ok(!pathcomp_done(clone));
    ok(pathcomp_next(clone), "advance to next combination");
    is(pathcomp_eval_nocopy(clone, "attribute"), "789", "clone has independent state");
    ok(!pathcomp_next(clone));
    ok(pathcomp_done(clone));
    is(pathcomp_eval_nocopy(orig, "attribute"), "456", "original unaffected");
    ok(!pathcomp_done(orig));
    pathcomp_reset(clone);
    is(pathcomp_eval_nocopy(clone, "attribute"), "123", "reset clone");
    is(pathcomp_eval_nocopy(orig, "attribute"), "456", "original unaffected");
    pathcomp_set(orig, "attribute", "AAA---");
    pathcomp_set(orig, "other", "---ZZZ");
    is(pathcomp_eval_nocopy(orig, "attribute"), "AAA---", "change original");
    is(pathcomp_eval_nocopy(orig, "other"), "---ZZZ", "change original");
    ok(!pathcomp_next(orig), "original no longer has alternatives");
    is(pathcomp_eval_nocopy(clone, "attribute"), "123", "clone unaffected");
    is(pathcomp_eval_nocopy(clone, "other"), "nothing", "clone unaffected");
    ok(pathcomp_next(clone), "but clone still has alternatives");
    is(pathcomp_eval_nocopy(clone, "attribute"), "456");
    is(pathcomp_eval_nocopy(clone, "other"), "nothing");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "attribute"), "789");
    is(pathcomp_eval_nocopy(clone, "other"), "nothing");
    ok(!pathcomp_next(clone));
    pathcomp_free(orig);
    pathcomp_free(clone);
    pathcomp_cleanup();
}

/* test whether clone can live independently when original is destroyed (no
 * references to data managed by original); also test whether ->current member
 * points to right alternative when cloned */
static void
test_clone2(void)
{
    pathcomp_t *orig = NULL, *clone = NULL;
    pathcomp_add_config_from_string(
            "[test.clone]\n"
            "attribute = lua { return '456' }\n"
            "other     = lua { return 101112 }\n"
            "other     = lua { return 131415 }\n"
            "other     = lua { return 161718 }\n"
            "other     = 192021\n"
            "other     = lua { return 222324 }\n"
            "other     = 252627\n"
            "other     = 282930\n"
            );
    ok(orig = pathcomp_new("test.clone"));
    is(pathcomp_eval_nocopy(orig, "attribute"), "456");
    is(pathcomp_eval_nocopy(orig, "other"), "101112");
    ok(pathcomp_next(orig));
    is(pathcomp_eval_nocopy(orig, "attribute"), "456");
    is(pathcomp_eval_nocopy(orig, "other"), "131415");
    ok(pathcomp_next(orig));
    is(pathcomp_eval_nocopy(orig, "other"), "161718");
    ok(pathcomp_next(orig));
    is(pathcomp_eval_nocopy(orig, "other"), "192021");
    ok(pathcomp_next(orig));
    is(pathcomp_eval_nocopy(orig, "other"), "222324", "orig: ->current points to fifth alternative");
    ok(clone = pathcomp_clone(orig), "create clone");
    pathcomp_free(orig);
    orig = NULL;
    note("original destroyed");
    is(pathcomp_eval_nocopy(clone, "attribute"), "456", "clone starts out with copy of state");
    is(pathcomp_eval_nocopy(clone, "other"), "222324", "clone: ->current points to fifth alternative");
    ok(!pathcomp_done(clone));
    ok(pathcomp_next(clone), "advance to next combination");
    is(pathcomp_eval_nocopy(clone, "attribute"), "456");
    is(pathcomp_eval_nocopy(clone, "other"), "252627");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "other"), "282930");
    ok(!pathcomp_next(clone));
    ok(pathcomp_done(clone));
    pathcomp_reset(clone);
    is(pathcomp_eval_nocopy(clone, "attribute"), "456", "reset clone");
    is(pathcomp_eval_nocopy(clone, "other"), "101112");
    pathcomp_free(clone);
    pathcomp_cleanup();
}

/* test whether cloning alternatives works correctly, even if original ceases
 * to have alternatives; the ->current member of alternatives is tricky to get
 * right */
static void
test_clone3(void)
{
    pathcomp_t *orig, *clone;
    pathcomp_add_config_from_string(
            "[test.clone]\n"
            "att = 1\n"
            "att = 2\n"
            );
    ok(orig = pathcomp_new("test.clone"));
    ok(clone = pathcomp_clone(orig));
    is(pathcomp_eval_nocopy(clone, "att"), "1");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "2");
    ok(!pathcomp_next(clone));
    pathcomp_reset(clone);
    pathcomp_set(orig, "att", "0");
    is(pathcomp_eval_nocopy(orig, "att"), "0", "attribute of original set to single-value attribute");
    ok(!pathcomp_next(orig));
    is(pathcomp_eval_nocopy(clone, "att"), "1", "but attribute still has original value");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "2", "... and it's still a multi-valued attribute");
    ok(!pathcomp_next(clone));
    pathcomp_reset(orig);
    pathcomp_reset(clone);
    pathcomp_add(clone, "att", "3");
    pathcomp_add(clone, "att", "4");
    pathcomp_add(clone, "att", "5");
    is(pathcomp_eval_nocopy(clone, "att"), "1", "first value still ok");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "2", "second value still ok");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "3", "clone now has a third value");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "4", "clone now has a fourth value");
    ok(pathcomp_next(clone));
    is(pathcomp_eval_nocopy(clone, "att"), "5", "clone now has a fifth value");
    ok(!pathcomp_next(clone));
    is(pathcomp_eval_nocopy(orig, "att"), "0", "but original unaffected");
    ok(!pathcomp_next(orig), "... and still a single-valued attribute");
    pathcomp_free(orig);
    pathcomp_free(clone);
    pathcomp_cleanup();
}

int
main(void)
{
    plan(NO_PLAN);
    test_clone1();
    test_clone2();
    test_clone3();
    done_testing();
}
