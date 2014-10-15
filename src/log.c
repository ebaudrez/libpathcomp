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
#include <string.h>
#include <stdlib.h>

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

static FILE *log_stream_global = NULL;
static char *log_prefix_global = NULL;

/**
 * \note We do absolutely nothing with the category right now.
 */
log_t *
log_get_logger(char *category)
{
    if (!log_stream_global) log_stream_global = stderr;
    if (!log_prefix_global) log_prefix_global = strdup("");
    return &global_logger;
}

void
log_cleanup(void)
{
    free(log_prefix_global);
}

void
log_set_stream_global(void *new)
{
    assert(new);
    log_stream_global = new;
}

void
log_set_prefix_global(const char *s)
{
    assert(s);
    log_prefix_global = strdup(s);
}

static void
log_emit(log_t *log, int level, char *fmt, va_list ap)
{
    fprintf(log_stream_global, "%s[%s] ", log_prefix_global, log_level_string[level]);
    vfprintf(log_stream_global, fmt, ap);
    fputc('\n', log_stream_global);
}

void
log_debug(log_t *log, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_emit(log, LOG_LEVEL_DEBUG, fmt, ap);
    va_end(ap);
}

void
log_error(log_t *log, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_emit(log, LOG_LEVEL_ERROR, fmt, ap);
    va_end(ap);
}
