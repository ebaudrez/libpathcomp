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

/* test reading of multiple sections */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"

static void
test_duplicate_sections(void)
{
    pathcomp_t *c = NULL;

    pathcomp_add_config_from_string(
        "[class]\n"
        "att = value1\n"
        "att = value2\n"
        "something = else\n"
        "att = value3\n"
        "multi = 1\n");
    pathcomp_add_config_from_string(
        "[class]\n"
        "is = this\n"
        "att = value4\n"
        "att = value5\n"
        "multi = 2\n"
        );
    ok(c = pathcomp_new("class"));
    is(pathcomp_eval_nocopy(c, "something"), "else", "contains attributes from first section ...");
    is(pathcomp_eval_nocopy(c, "is"), "this", "... and second section");
    is(pathcomp_eval_nocopy(c, "att"), "value1", "repeated section leads to accumulation of multivalues");
    is(pathcomp_eval_nocopy(c, "multi"), "1", "repeated section leads to accumulation of multivalues");
    pathcomp_free(c);
}

static void
test_copy1(void)
{
    pathcomp_t *parent = NULL, *child = NULL;

    pathcomp_add_config_from_string(
        "[parent]\n"
        "att = value1\n"
        "att = value2\n"
        "something = else\n"
        "att = value3\n"
        "multi = 1\n"
        "another = A\n"
        );
    pathcomp_add_config_from_string(
        "[child]\n"
        "another = B\n"
        "[child]\n"
        "multi = 2\n"
        "copy-from = parent\n"
        "is = this\n"
        "att = value4\n"
        "att = value5\n"
        );
    ok(parent = pathcomp_new("parent"));
    is(pathcomp_eval_nocopy(parent, "att"), "value1");
    is(pathcomp_eval_nocopy(parent, "something"), "else");
    is(pathcomp_eval_nocopy(parent, "multi"), "1");
    is(pathcomp_eval_nocopy(parent, "another"), "A");
    ok(pathcomp_next(parent));
    is(pathcomp_eval_nocopy(parent, "att"), "value2");
    is(pathcomp_eval_nocopy(parent, "something"), "else");
    is(pathcomp_eval_nocopy(parent, "multi"), "1");
    is(pathcomp_eval_nocopy(parent, "another"), "A");
    ok(pathcomp_next(parent));
    is(pathcomp_eval_nocopy(parent, "att"), "value3");
    ok(!pathcomp_next(parent));
    ok(child = pathcomp_new("child"));
    is(pathcomp_eval_nocopy(child, "another"), "B", "attribute inherited but overridden in child class (in another section!)");
    is(pathcomp_eval_nocopy(child, "multi"), "2", "attribute inherited but overridden in child class, and positioned before copy-from");
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive itself rendered verbatim");
    is(pathcomp_eval_nocopy(child, "something"), "else", "inherited attribute");
    is(pathcomp_eval_nocopy(child, "is"), "this", "own attribute of child class");
    is(pathcomp_eval_nocopy(child, "att"), "value4", "first alternative of overridden attribute");
    ok(pathcomp_next(child));
    is(pathcomp_eval_nocopy(child, "another"), "B");
    is(pathcomp_eval_nocopy(child, "multi"), "2");
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive itself rendered verbatim");
    is(pathcomp_eval_nocopy(child, "something"), "else");
    is(pathcomp_eval_nocopy(child, "is"), "this");
    is(pathcomp_eval_nocopy(child, "att"), "value5", "second alternative of overridden attribute");
    ok(!pathcomp_next(child));
    pathcomp_free(parent);
    pathcomp_free(child);
    pathcomp_cleanup();
}

static void
test_copy2(void)
{
    pathcomp_t *parent = NULL, *child = NULL;

    pathcomp_add_config_from_string(
        "[parent]\n"
        "att = value1\n"
        "att = value2\n"
        "something = else\n"
        "att = value3\n"
        "multi = 1\n"
        "another = A\n"
        );
    pathcomp_add_config_from_string(
        "[child]\n"
        "copy-from = parent\n"
        "[child]\n"
        "another = B\n"
        "multi = 2\n"
        "is = this\n"
        "att = value4\n"
        "att = value5\n"
        );
    ok(parent = pathcomp_new("parent"));
    is(pathcomp_eval_nocopy(parent, "att"), "value1");
    is(pathcomp_eval_nocopy(parent, "something"), "else");
    is(pathcomp_eval_nocopy(parent, "multi"), "1");
    is(pathcomp_eval_nocopy(parent, "another"), "A");
    ok(pathcomp_next(parent));
    is(pathcomp_eval_nocopy(parent, "att"), "value2");
    is(pathcomp_eval_nocopy(parent, "something"), "else");
    is(pathcomp_eval_nocopy(parent, "multi"), "1");
    is(pathcomp_eval_nocopy(parent, "another"), "A");
    ok(pathcomp_next(parent));
    is(pathcomp_eval_nocopy(parent, "att"), "value3");
    ok(!pathcomp_next(parent));
    ok(child = pathcomp_new("child"));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive");
    is(pathcomp_eval_nocopy(child, "att"), "value4", "attribute declared in other section than copy-from");
    is(pathcomp_eval_nocopy(child, "something"), "else", "inherited attribute");
    is(pathcomp_eval_nocopy(child, "multi"), "2", "attribute declared in other section than copy-from");
    is(pathcomp_eval_nocopy(child, "another"), "B", "attribute declared in other section than copy-from");
    is(pathcomp_eval_nocopy(child, "is"), "this", "attribute declared in other section than copy-from");
    ok(pathcomp_next(child));
    pathcomp_free(parent);
    pathcomp_free(child);
    pathcomp_cleanup();
}

static void
test_copy3(void)
{
    pathcomp_t *parent = NULL, *child = NULL;

    pathcomp_add_config_from_string(
        "[parent]\n"
        "att = value1\n"
        );
    pathcomp_add_config_from_string(
        "[child]\n"
        "copy-from = parent\n"
        );
    ok(parent = pathcomp_new("parent"));
    is(pathcomp_eval_nocopy(parent, "att"), "value1");
    ok(!pathcomp_next(parent));
    ok(child = pathcomp_new("child"));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive");
    is(pathcomp_eval_nocopy(child, "att"), "value1", "inherited");
    ok(!pathcomp_next(child));
    pathcomp_add(child, "att", "value2");
    is(pathcomp_eval_nocopy(child, "att"), "value1", "explicitly added to inherited attribute at run time");
    ok(pathcomp_next(child));
    is(pathcomp_eval_nocopy(child, "att"), "value2", "explicitly added to inherited attribute at run time");
    ok(!pathcomp_next(child));
    pathcomp_free(parent);
    pathcomp_free(child);
    pathcomp_cleanup();
}

static void
test_copy4(void)
{
    pathcomp_t *parent = NULL, *child = NULL;

    pathcomp_add_config_from_string(
        "[parent]\n"
        "att = value1\n"
        "att = value2\n"
        );
    pathcomp_add_config_from_string(
        "[child]\n"
        "copy-from = parent\n"
        );
    ok(parent = pathcomp_new("parent"));
    is(pathcomp_eval_nocopy(parent, "att"), "value1");
    ok(pathcomp_next(parent));
    is(pathcomp_eval_nocopy(parent, "att"), "value2");
    ok(!pathcomp_next(parent));
    ok(child = pathcomp_new("child"));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive itself rendered verbatim");
    is(pathcomp_eval_nocopy(child, "att"), "value1", "inherits attributes but not state, so alternatives start at first (even though parent is at second)");
    ok(pathcomp_next(child));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent", "directive itself rendered verbatim");
    is(pathcomp_eval_nocopy(child, "att"), "value2");
    ok(!pathcomp_next(child));
    pathcomp_free(parent);
    pathcomp_free(child);
    pathcomp_cleanup();
}

static void
test_recursive_copy(void)
{
    pathcomp_t *level0, *level1, *level2;

    pathcomp_add_config_from_string(
        "[level0]\n"
        "att0 = value0\n"
        );
    pathcomp_add_config_from_string(
        "[level1]\n"
        "copy-from = level0\n"
        "att1 = value1\n"
        );
    pathcomp_add_config_from_string(
        "[level2]\n"
        "copy-from = level1\n"
        "att2 = value2\n"
        );
    ok(level0 = pathcomp_new("level0"));
    is(pathcomp_eval_nocopy(level0, "att0"), "value0");
    /*ok(!pathcomp_has(level0, "att1");*/
    is(pathcomp_eval_nocopy(level0, "att1"), NULL);
    is(pathcomp_eval_nocopy(level0, "att2"), NULL);
    ok(level1 = pathcomp_new("level1"));
    is(pathcomp_eval_nocopy(level1, "copy-from"), "level0");
    is(pathcomp_eval_nocopy(level1, "att0"), "value0", "attribute copied from parent");
    is(pathcomp_eval_nocopy(level1, "att1"), "value1");
    is(pathcomp_eval_nocopy(level1, "att2"), NULL);
    ok(level2 = pathcomp_new("level2"));
    is(pathcomp_eval_nocopy(level2, "copy-from"), "level1");
    is(pathcomp_eval_nocopy(level2, "att0"), "value0", "attribute recursively copied from parent of parent");
    is(pathcomp_eval_nocopy(level1, "att1"), "value1");
    is(pathcomp_eval_nocopy(level2, "att2"), "value2");
    pathcomp_free(level0);
    pathcomp_free(level1);
    pathcomp_free(level2);
    pathcomp_cleanup();
}

static void
test_multi_inherit(void)
{
    pathcomp_t *parent1 = NULL, *parent2 = NULL, *child = NULL;

    pathcomp_add_config_from_string(
        "[parent1]\n"
        "att1 = value1\n"
        );
    pathcomp_add_config_from_string(
        "[parent2]\n"
        "att2 = value2\n"
        );
    pathcomp_add_config_from_string(
        "[child]\n"
        "copy-from = parent1\n"
        "copy-from = parent2\n"
        );
    ok(parent1 = pathcomp_new("parent1"));
    is(pathcomp_eval_nocopy(parent1, "att1"), "value1");
    is(pathcomp_eval_nocopy(parent1, "att2"), NULL);
    ok(!pathcomp_next(parent1));
    ok(parent2 = pathcomp_new("parent2"));
    is(pathcomp_eval_nocopy(parent2, "att1"), NULL);
    is(pathcomp_eval_nocopy(parent2, "att2"), "value2");
    ok(!pathcomp_next(parent2));
    ok(child = pathcomp_new("child"));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent1", "multiple inheritance");
    is(pathcomp_eval_nocopy(child, "att1"), "value1");
    is(pathcomp_eval_nocopy(child, "att2"), "value2");
    ok(pathcomp_next(child));
    is(pathcomp_eval_nocopy(child, "copy-from"), "parent2", "multiple inheritance");
    is(pathcomp_eval_nocopy(child, "att1"), "value1");
    is(pathcomp_eval_nocopy(child, "att2"), "value2");
    ok(!pathcomp_next(child));
    pathcomp_free(parent1);
    pathcomp_free(parent2);
    pathcomp_free(child);
    pathcomp_cleanup();
}

int
main(void)
{
    plan(NO_PLAN);
    test_duplicate_sections();
    test_copy1();
    test_copy2();
    test_copy3();
    test_copy4();
    test_recursive_copy();
    test_multi_inherit();
    done_testing();
}
