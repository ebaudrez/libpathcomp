/**
 * \file
 * \brief Logging (header)
 */

#ifndef LOG_INCLUDED
#define LOG_INCLUDED

extern void log_debug(const char *fmt, ...);
extern void log_warning(const char *fmt, ...);
extern void log_error(const char *fmt, ...);

#endif /* LOG_INCLUDED */
