#include "config.h"
#include "value.h"
#include "interpreter.h"
#include "log.h"
#include "buf.h"
#include "pathcomp.h"
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lua.h>
#include <lauxlib.h>

static value_t *
value_new_literal( const char *text )
{
    value_t *val;
    assert( text );
    val = malloc( sizeof *val );
    if( !val ) return val;
    val->type = VALUE_LITERAL;
    val->literal = strdup( text );
    return val;
}

static char *
is_lua_code( const char *text )
{
    buf_t buf;
    char *p;
    const char *keyword  = "lua",
               *preamble = "local self = ...; ";
    assert( text );
    buf_init( &buf, strlen(text) + strlen(preamble) );
    buf_addstr( &buf, text );
    p = buf.buf;
    if( strlen(p) < strlen(keyword) ) goto fail;
    if( strncmp(p, keyword, strlen(keyword)) != 0 ) goto fail;
    p += strlen( keyword );
    while( *p && isspace(*p) ) ++p;
    if( *p == '\0' || *p != '{' ) goto fail;
    ++p;
    buf_splicestr( &buf, 0, p - buf.buf, preamble );
    p = strrchr( buf.buf, '}' );
    buf_setlen( &buf, p - buf.buf );
    return buf_detach( &buf, NULL );
fail:
    buf_release( &buf );
    return NULL;
}

static value_t *
value_new_lua( const char *source )
{
    value_lua_t *val;
    assert( source );
    val = malloc( sizeof *val );
    if( !val ) return (value_t *) val;
    val->type = VALUE_LUA;
    val->source = strdup( source );
    return (value_t *) val;
}

value_t *
value_new( const char *text )
{
    char *source;
    assert( text );
    if( (source = is_lua_code(text)) ) {
        value_t *val = value_new_lua( source );
        free( source );
        return val;
    }
    else {
        return value_new_literal( text );
    }
}

static void
value_free_lua( value_lua_t *val )
{
    assert( val );
    free( val->source );
}

void
value_free( value_t *val )
{
    if( !val ) return;
    switch( val->type ) {
        case VALUE_LITERAL:
            free( val->literal );
            break;
        case VALUE_LUA:
            value_free_lua( (value_lua_t *) val );
            break;
        default:
            assert( 0 );
    }
    free( val );
}

static const char *
value_eval_lua( value_lua_t *val, pathcomp_t *composer )
{
    log_t       *log;
    lua_State   *L = interpreter_get_state();
    pathcomp_t **p;
    assert( val );
    log = log_get_logger( "pathcomp" );
    if( luaL_loadstring( L, val->source ) != LUA_OK ) {
        const char *error = lua_tostring( L, -1 );
        log_error( log, "cannot parse Lua code: %s", error );
        return NULL;
    }
    p = lua_newuserdata( L, sizeof(*p) );
    *p = composer;
    luaL_getmetatable( L, pathcomp_metatable(composer) );
    lua_setmetatable( L, -2 );
    if( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
        const char *error = lua_tostring( L, -1 );
        log_error( log, "cannot execute Lua code: %s", error );
        return NULL;
    }
    return lua_tostring( L, -1 );
}

const char *
value_eval( value_t *val, pathcomp_t *composer )
{
    assert( val );
    switch( val->type ) {
        case VALUE_LITERAL:
            return val->literal;
        case VALUE_LUA:
            return value_eval_lua( (value_lua_t *) val, composer );
        default:
            assert( 0 );
    }
}
