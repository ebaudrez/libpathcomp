/**
 * \file
 * \brief Logging (header)
 */

#ifndef LOG_INCLUDED
#define LOG_INCLUDED

typedef struct log_t log_t;

extern log_t *log_get_logger(char *);
extern void   log_cleanup(void);
extern void   log_set_stream_global(void *);
extern void   log_set_prefix_global(const char *);
extern void   log_debug(log_t *, char *fmt, ...);
extern void   log_warning(log_t *, char *fmt, ...);
extern void   log_error(log_t *, char *fmt, ...);

#endif /* LOG_INCLUDED */
