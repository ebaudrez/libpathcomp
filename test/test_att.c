/*
 * Copyright (C) 2016 Edward Baudrez <edward.baudrez@gmail.com>
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

/* test att.h */

#include <config.h>
#include "tap.h"
#include "value.h"
#include "att.h"
#include "interpreter.h"
#include <string.h>

static void
test_1element(void)
{
    att_t *att;
    ok(att = att_new("key", value_new_string("value"), "origin"), "att_new()");
    is(att_eval(att, NULL, NULL), "value", "att_eval()");
    ok(att_name_equal_to(att, "key"), "att_name_equal_to()");
    ok(!att_name_equal_to(att, "clef"));
    is(att_get_origin(att), "origin", "att_get_origin()");
    ok(!att_next(att), "att_next() on single element");
    att_rewind(att);
    ok(!att_next(att), "rewind single element");
    att_replace_value(att, value_new_string("some_other"), "somewhere_else");
    is(att_eval(att, NULL, NULL), "some_other", "replace value");
    is(att_get_origin(att), "somewhere_else");
    att_free(att);
}

static void
test_2elements(void)
{
    att_t *att;
    ok(att = att_new("key1", value_new_string("value1"), NULL));
    is(att_eval(att, NULL, NULL), "value1");
    att_add_value(att, value_new_string("value2"));
    is(att_eval(att, NULL, NULL), "value1", "add_value() doesn't advance current alternative");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "value2", "att_next()");
    ok(!att_next(att), "end of alternatives");
    ok(!att_next(att), "att_next() doesn't recycle");
    att_rewind(att);
    is(att_eval(att, NULL, NULL), "value1", "rewind");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "value2");
    ok(!att_next(att));
    att_free(att);
}

static void
test_4elements(void)
{
    att_t *att;
    ok(att = att_new("key", value_new_string("value1"), "Orig"));
    att_add_value(att, value_new_string("value2"));
    att_add_value(att, value_new_lua("return 'value' .. 3"));
    att_add_value(att, value_new_string("value4"));
    is(att_eval(att, NULL, NULL), "value1");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "value2");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "value3");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "value4");
    ok(!att_next(att));
    ok(!att_next(att));
    is(att_get_origin(att), "Orig");
    att_replace_value(att, value_new_string("value_99"), "Air");
    is(att_eval(att, NULL, NULL), "value_99");
    ok(!att_next(att), "at end of list (all alternatives exhausted)");
    ok(!att_next(att));
    is(att_get_origin(att), "Air");
    att_add_value(att, value_new_string("XYZ"));
    ok(!att_next(att), "att_add_value() doesn't rewind");
    is(att_eval(att, NULL, NULL), NULL);
    att_rewind(att);
    is(att_eval(att, NULL, NULL), "value_99");
    ok(att_next(att));
    is(att_eval(att, NULL, NULL), "XYZ", "need to rewind to access newly added alternative");
    ok(!att_next(att));
    att_free(att);
}

int
main(void)
{
    plan(NO_PLAN);
    test_1element();
    test_2elements();
    test_4elements();
    interpreter_cleanup();
    done_testing();
}
