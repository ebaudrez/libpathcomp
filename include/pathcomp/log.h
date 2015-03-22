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

/**
 * \file
 * \brief Logging routines for \a libpathcomp
 *
 * The \a libpathcomp library uses these routines to log its debug, warning,
 * and error messages. Default implementations are provided, but these may be
 * overridden by the user. The default implementations log to \a stderr.
 *
 * If you intend to override these functions, this header file should be
 * included to provide the proper declarations. If the default implementations
 * of these functions are acceptable, there is no need to include this header.
 */

#ifndef PATHCOMP_LOG_INCLUDED
#define PATHCOMP_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Log a message at debug level
 * \param fmt <em>sprintf()</em>-style format string
 * \param ... Additional arguments according to format string
 *
 * The default implementation of this function silently discards debug
 * messages.
 */
extern void pathcomp_log_debug(const char *fmt, ...);

/**
 * Log a message at warning level
 * \param fmt <em>sprintf()</em>-style format string
 * \param ... Additional arguments according to format string
 *
 * The default implementation of this function prints warnings messages to \a
 * stderr, preceded by the string <tt>"pathcomp: "</tt>, and followed by a
 * newline.
 */
extern void pathcomp_log_warning(const char *fmt, ...);

/**
 * Log a message at error level
 * \param fmt <em>sprintf()</em>-style format string
 * \param ... Additional arguments according to format string
 *
 * The default implementation of this function prints error messages to \a
 * stderr, preceded by the string <tt>"pathcomp: "</tt>, and followed by a
 * newline.
 */
extern void pathcomp_log_error(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_LOG_INCLUDED */
