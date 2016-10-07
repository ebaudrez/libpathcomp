/*
 * Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>
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

/* test Lua callbacks in config */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"
#include <string.h>
#include <limits.h>
#include "buf.h"

const char *config = "\
[test.basic]\n\
    sum = lua { return 2 + 3 + 4 + 5 + 6 + 7 }\n\
\n\
[test.callbacks]\n\
    instrument = G2\n\
    imager     = SEV1\n\
    level      = 20\n\
    resolution = BARG\n\
    product    = SOL_M15_R50\n\
    version    = V003\n\
    extension  = .hdf.gz\n\
    slot       = 200403011130\n\
    yyyy       = lua { return string.sub(self.slot,  1,  4) }\n\
    mmdd       = lua { return string.sub(self.slot,  5,  8) }\n\
    hhmm       = lua { return string.sub(self.slot,  9, 12) }\n\
    ss         = lua { local ss = string.sub(self.slot, 13, 14); return #ss > 0 and ss or '00' }\n\
    hhmmss     = lua { return self.hhmm .. self.ss }\n\
    prefix     = lua { return string.format('%s_%s_L%s_%s_%s', self.instrument, self.imager, self.level, self.resolution, self.product) }\n\
    filename   = lua { return self.prefix .. '_' .. self.yyyy .. self.mmdd .. '_' .. self.hhmmss .. '_' .. self.version .. self.extension }\n\
\n\
[test.int]\n\
    plus4 = lua { return self.val + 4 }\n\
    concat = lua { return 'concat' .. self.val }\n\
\n\
[test.args]\n\
    sum    = lua { ? }\n\
    number = lua { return self.sum(1, -5, -3, 2) } \n\
";

static void
test_basic(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.basic"));
    is(pathcomp_eval_nocopy(c, "sum"), "27");
    pathcomp_free(c);
}

static void
test_callbacks(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.callbacks"));
    is(pathcomp_eval_nocopy(c, "instrument"), "G2");
    is(pathcomp_eval_nocopy(c, "imager"), "SEV1");
    is(pathcomp_eval_nocopy(c, "level"), "20");   /* everything evaluates as strings */
    is(pathcomp_eval_nocopy(c, "resolution"), "BARG");
    is(pathcomp_eval_nocopy(c, "product"), "SOL_M15_R50");
    is(pathcomp_eval_nocopy(c, "version"), "V003");
    is(pathcomp_eval_nocopy(c, "extension"), ".hdf.gz");
    is(pathcomp_eval_nocopy(c, "slot"), "200403011130");
    is(pathcomp_eval_nocopy(c, "yyyy"), "2004");
    is(pathcomp_eval_nocopy(c, "mmdd"), "0301");   /* leading 0 not stripped! */
    is(pathcomp_eval_nocopy(c, "hhmm"), "1130");
    is(pathcomp_eval_nocopy(c, "ss"), "00");
    is(pathcomp_eval_nocopy(c, "hhmmss"), "113000");
    is(pathcomp_eval_nocopy(c, "prefix"), "G2_SEV1_L20_BARG_SOL_M15_R50");
    is(pathcomp_eval_nocopy(c, "filename"), "G2_SEV1_L20_BARG_SOL_M15_R50_20040301_113000_V003.hdf.gz");
    pathcomp_free(c);
}

#define STRINGIFY_HELPER(n) #n
#define STRINGIFY(n) STRINGIFY_HELPER(n)
static void
test_int(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.int"));
    pathcomp_set_int(c, "val", 15);
    is(pathcomp_eval_nocopy(c, "plus4"), "19");
    is(pathcomp_eval_nocopy(c, "concat"), "concat15");
    pathcomp_set_int(c, "val", -15);
    is(pathcomp_eval_nocopy(c, "plus4"), "-11");
    is(pathcomp_eval_nocopy(c, "concat"), "concat-15");
    pathcomp_set_int(c, "val", INT_MAX);
    is(pathcomp_eval_nocopy(c, "concat"), "concat" STRINGIFY(INT_MAX));
    pathcomp_set_int(c, "val", INT_MIN);
    /* cannot stringify INT_MIN as it's an expression */
    buf_t buf;
    buf_init(&buf, 0);
    buf_addf(&buf, "concat%d", INT_MIN);
    is(pathcomp_eval_nocopy(c, "concat"), buf.buf);
    buf_release(&buf);
    pathcomp_free(c);
}

static void
test_args(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.args"));
    todo("Lua function args not implemented");
    is(pathcomp_eval_nocopy(c, "number"), "-5");
    end_todo;
    pathcomp_free(c);
}

static void
test_env(void)
{
    pathcomp_t *c = NULL;
    char *expected;
    ok(c = pathcomp_new("test.env"));
    pathcomp_set(c, "home", "lua { return os.getenv('HOME') }");
    expected = strdup(getenv("HOME"));
    is(pathcomp_eval_nocopy(c, "home"), expected, "matches what is retrieved with getenv()");
    free(expected);
    pathcomp_free(c);
}

static void
test_incomplete(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.incomplete"));
    pathcomp_set(c, "abc", "lua { return self.def }");
    is(pathcomp_eval_nocopy(c, "abc"), NULL, "eval() of incomplete composer returns NULL (but does not crash)");
    pathcomp_free(c);
}

static void
test_failing_parse(void)
{
    pathcomp_t *c = NULL;
    ok(c = pathcomp_new("test.incomplete"));
    pathcomp_set(c, "abc", "lua { return 'abc' ");
    todo("bad Lua function definitions pass through as-is but should yield NULL");
    is(pathcomp_eval_nocopy(c, "abc"), NULL, "test bad syntax: missing closing brace");
    end_todo;
    pathcomp_set(c, "def", "lua { return 123 + }");
    is(pathcomp_eval_nocopy(c, "def"), NULL, "test failing luaL_loadstring()");
    pathcomp_set(c, "ghi", "lua { return 'ghi' }");
    is(pathcomp_eval_nocopy(c, "ghi"), "ghi");
    pathcomp_set(c, "ghi", "lua { return ' }");
    is(pathcomp_eval_nocopy(c, "ghi"), NULL, "test failing parse after successful evaluation");
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_string(config);
    test_basic();
    test_callbacks();
    test_int();
    test_args();
    test_env();
    test_incomplete();
    test_failing_parse();
    pathcomp_cleanup();
    done_testing();
}
