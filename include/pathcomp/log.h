/**
 * \file
 * \brief Logging routines for \a libpathcomp
 *
 * The \a libpathcomp library uses these routines to log its debug, warning,
 * and error messages. Default implementations are provided, but these may be
 * overridden by the user. This header file should be included to provide the
 * proper declarations.
 */

#ifndef PATHCOMP_LOG_INCLUDED
#define PATHCOMP_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Log a message at debug level
 * \param fmt <em>sprintf()</em>-style format string
 */
extern void pathcomp_log_debug(const char *fmt, ...);

/**
 * Log a message at warning level
 * \param fmt <em>sprintf()</em>-style format string
 */
extern void pathcomp_log_warning(const char *fmt, ...);

/**
 * Log a message at error level
 * \param fmt <em>sprintf()</em>-style format string
 */
extern void pathcomp_log_error(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_LOG_INCLUDED */
