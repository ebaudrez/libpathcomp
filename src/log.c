/**
 * \file
 * \brief Logging (implementation)
 */

#include <config.h>
#include "log.h"
#include <stdio.h>
#include <stdarg.h>

static void
log_emit(const char *fmt, va_list ap)
{
    fprintf(stderr, "pathcomp: ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

void
log_debug(const char *fmt, ...)
{
}

void
log_warning(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_emit(fmt, ap);
    va_end(ap);
}

void
log_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_emit(fmt, ap);
    va_end(ap);
}
