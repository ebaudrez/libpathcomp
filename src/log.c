/**
 * \file
 * \brief Logging (implementation)
 */

#include "config.h"
#include "log.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

struct log_t {
};

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR 2
static const char *log_level_string[] = {
    "debug",
    "warning",
    "error"
};

static log_t global_logger;

/**
 * \note We do absolutely nothing with the category right now.
 */
log_t *
log_get_logger( char *category )
{
    return &global_logger;
}

void
log_cleanup( void )
{
}

static void
log_emit( log_t *log, int level, char *fmt, va_list ap )
{
    fprintf( stdout, "# [%s] ", log_level_string[level] );
    vfprintf( stdout, fmt, ap );
    fputc( '\n', stdout );
}

void
log_debug( log_t *log, char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    log_emit( log, LOG_LEVEL_DEBUG, fmt, ap );
    va_end( ap );
}

void
log_error( log_t *log, char *fmt, ... )
{
    va_list ap;
    va_start( ap, fmt );
    log_emit( log, LOG_LEVEL_ERROR, fmt, ap );
    va_end( ap );
}
