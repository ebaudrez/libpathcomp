/*
 * Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
 * This file is part of Libpathcomp.
 *
 * Libpathcomp is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Libpathcomp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
 */

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
