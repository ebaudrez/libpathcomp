#include <config.h>
#include "pathcomp/log.h"
#include <stdarg.h>
#include <stdio.h>

void
pathcomp_log_debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("# ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
}

void
pathcomp_log_warning(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("# ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
}

void
pathcomp_log_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("# ", stdout);
    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
}
