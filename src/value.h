/**
 * \file
 * \brief ???
 */

#ifndef VALUE_INCLUDED
#define VALUE_INCLUDED

#include "config.h"

typedef struct {
    char *literal;
} value_t;

extern value_t    *value_new( const char * );
extern void        value_free( value_t * );
extern const char *value_eval( value_t * );

#endif /* VALUE_INCLUDED */
