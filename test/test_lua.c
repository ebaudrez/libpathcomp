/* test Lua callbacks in config */

#include "config.h"
#include "tap.h"
#include "pathcomp.h"
#include "log.h"
#include <string.h>

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

int
main(void)
{
    plan(NO_PLAN);
    log_set_stream_global(stdout);
    log_set_prefix_global("# ");
    pathcomp_add_config_from_string(config);
    test_basic();
    test_callbacks();
    test_args();
    test_env();
    pathcomp_cleanup();
    done_testing();
}
