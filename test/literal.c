#include "config.h"
#include "tap.h"
#include "pathcomp.h"

const char *config = "\
[test.literal]\n\
    key = value\
";

static void
test_literal( const char *class_name )
{
    pathcomp_t *composer = NULL;
    ok( composer = pathcomp_new(class_name) );
    const char *val = pathcomp_eval( composer, "key" );
    is( val, "value" );
    pathcomp_free( composer );
}

int
main( void )
{
    plan( NO_PLAN );
    pathcomp_use_config_from( config );
    test_literal( "test.literal" );
    pathcomp_cleanup();
    done_testing();
}
