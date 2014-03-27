/**
 * \file
 * \brief Path name composition (implementation)
 */

#include "config.h"
#include "pathcomp.h"
#include "list.h"
#include "cf.h"
#include "log.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct {
    char *name;
    char *value;
} att_t;

/**
 * \brief Main data type
 */
struct pathcomp_t {
    char   *name;
    list_t *attributes;
};

static cf_t *config;

static att_t *
attribute_new( char *name, char *value )
{
    att_t *att;
    att = malloc( sizeof *att );
    if( !att ) return att;
    att->name = name;
    att->value = value;
    return att;
}

/* in order to match the type of list_traversal_t, attribute_free() takes a pointer to pointer */
static int
attribute_free( void **p, void *userdata )
{
    att_t *att = *p;
    if( !att ) return 0;
    free( att->name );
    free( att->value );
    free( att );
    *p = NULL;
    return 0;
}

void
pathcomp_use_config_from( const char *string )
{
    assert( string );
    config = cf_new_from_string( string );
}

void
pathcomp_cleanup( void )
{
    cf_free( config );
    config = NULL;
}

static int
find_section_with_name( void **p, void *userdata )
{
    cf_section_t *sec = *p;
    char *name = userdata;
    return !strcmp( sec->name, name );
}

static void
pathcomp_add_attribute( pathcomp_t *composer, cf_kv_t *kv )
{
    att_t *att;
    assert( kv );
    att = attribute_new( kv->key, kv->value );
    if( composer->attributes ) list_push( composer->attributes, att );
    else                       composer->attributes = list_new( att );
}

static void
pathcomp_make_from_config( pathcomp_t *composer )
{
    list_t *psec;
    log_t *log;
    assert( composer );
    assert( config );
    log = log_get_logger( "pathcomp" );
    psec = config->sections;
    while( (psec = list_find_first(psec, find_section_with_name, composer->name)) ) {
        cf_section_t *sec = psec->value;
        list_t       *pkv = sec->entries;
        log_debug( log, "found section with name '%s'", sec->name );
        while( pkv ) {
            cf_kv_t *kv = pkv->value;
            log_debug( log, "found key-value pair with name '%s'", kv->key );
            pathcomp_add_attribute( composer, kv );
            pkv = pkv->next;
        }
        psec = psec->next;
    }
}

pathcomp_t *
pathcomp_new( const char *name )
{
    pathcomp_t *composer = NULL;
    assert( name );
    composer = malloc( sizeof *composer );
    composer->name = strdup( name );
    composer->attributes = NULL;
    pathcomp_make_from_config( composer );
    return composer;
}

void
pathcomp_free( pathcomp_t *composer )
{
    if( !composer ) return;
    free( composer->name );
    list_map( composer->attributes, attribute_free, NULL );
    list_free( composer->attributes );
    free( composer );
}

static int
find_attribute_with_name( void **p, void *userdata )
{
    att_t *att = *p;
    const char *name = userdata;
    return !strcmp( att->name, name );
}

const char *
pathcomp_eval( pathcomp_t *composer, const char *name )
{
    list_t *p;
    att_t *att;
    assert( composer );
    p = list_find_first( composer->attributes, find_attribute_with_name, (void *) name );
    if( !p ) return NULL;
    att = p->value;
    return att->value;
}

void
pathcomp_set( pathcomp_t *composer, const char *name, const void *value )
{
    assert( composer );
    assert( name );
    /* TODO */
}
