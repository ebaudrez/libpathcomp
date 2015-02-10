/**
 * \file
 * \brief Path name composition (header)
 */

#ifndef PATHCOMP_INCLUDED
#define PATHCOMP_INCLUDED

/**
 * \brief Main structure (abstract data type)
 */
typedef struct pathcomp_t pathcomp_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void        pathcomp_add_config_from_string(const char *);
extern void        pathcomp_add_config_from_file(const char *);
extern void        pathcomp_cleanup(void);
extern pathcomp_t *pathcomp_new(const char *);
extern void        pathcomp_free(pathcomp_t *);
extern const char *pathcomp_eval_nocopy(pathcomp_t *, const char *);
extern char       *pathcomp_eval(pathcomp_t *, const char *);
extern char       *pathcomp_yield(pathcomp_t *);
extern void        pathcomp_set(pathcomp_t *, const char *, const char *);
extern void        pathcomp_add(pathcomp_t *, const char *, const char *);
extern void        pathcomp_reset(pathcomp_t *);
extern int         pathcomp_done(pathcomp_t *);
extern int         pathcomp_next(pathcomp_t *);
extern char       *pathcomp_find(pathcomp_t *);
extern int         pathcomp_mkdir(pathcomp_t *);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_INCLUDED */