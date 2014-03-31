/**
 * \file
 * \brief ???
 */

#include "config.h"
#include "value.h"
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

value_t *
value_new( const char *text )
{
    value_t *val;
    val = malloc( sizeof *val );
    if( !val ) return val;
    val->literal = strdup( text );
    return val;
}

void
value_free( value_t *val )
{
    if( !val ) return;
    free( val->literal );
    free( val );
}

const char *
value_eval( value_t *val )
{
    assert( val );
    return val->literal;
}
