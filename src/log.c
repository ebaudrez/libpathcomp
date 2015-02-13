#include <config.h>
#include "pathcomp/log.h"
#include <stdio.h>
#include <stdarg.h>

static void
pathcomp_log_emit(const char *fmt, va_list ap)
{
    fprintf(stderr, "pathcomp: ");
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
}

void
pathcomp_log_debug(const char *fmt, ...)
{
}

void
pathcomp_log_warning(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    pathcomp_log_emit(fmt, ap);
    va_end(ap);
}

void
pathcomp_log_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    pathcomp_log_emit(fmt, ap);
    va_end(ap);
}
