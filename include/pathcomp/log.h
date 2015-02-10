/**
 * \file
 * \brief Logging (header)
 */

#ifndef PATHCOMP_LOG_INCLUDED
#define PATHCOMP_LOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern void pathcomp_log_debug(const char *fmt, ...);
extern void pathcomp_log_warning(const char *fmt, ...);
extern void pathcomp_log_error(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_LOG_INCLUDED */
