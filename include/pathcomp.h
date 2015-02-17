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

extern void pathcomp_add_config_from_string(const char *string);
extern void pathcomp_add_config_from_file(const char *filename);
extern void pathcomp_cleanup(void);
extern pathcomp_t *pathcomp_new(const char *name);
extern void pathcomp_free(pathcomp_t *composer);

/**
 * \note The string returned by this function must not be deallocated by the user.
 * \note Beware: this function may return a pointer to internal storage. There
 * are no guarantees on the lifetime of the object pointed to by the return
 * value of this function. Therefore, this function should probably only be
 * used for debugging. Prefer pathcomp_eval().
 * \seealso pathcomp_eval()
 */
extern const char *pathcomp_eval_nocopy(pathcomp_t *composer, const char *name);

/**
 * \note The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_eval(pathcomp_t *composer, const char *name);

/**
 * \note The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_yield(pathcomp_t *composer);

/**
 * pathcomp_set() will reset all alternatives to their first value. This is
 * necessary because alternatives are tried in order, and there is no guarantee
 * that a matching combination of alternatives comes later in the list of
 * possible combinations.
 *
 * Imagine the situation where you have successfully located a GERB archive
 * file with the following characteristics:
 *
 *    -# instrument = G2
 *    -# extension  = .hdf
 *    -# extension  = .hdf.gz
 *    -# etc.
 *
 * After executing pathcomp_find(), the \e extension alternative will point to
 * the .hdf.gz value, because G2 files are gzip-compressed. If you now call
 *
 *   pathcomp_set(composer, "instrument", "GL")
 *
 * pathcomp_find() will fail, because GERB-like files are not compressed
 * externally, and the alternative .hdf comes before .hdf.gz in the list of
 * alternatives, and pathcomp_find() will not rewind it! Therefore,
 * pathcomp_reset() must be called to rewind all the alternatives.
 */
extern void pathcomp_set(pathcomp_t *composer, const char *name, const char *value);

/**
 * All alternatives are rewound after calling this function, so that a failed
 * search with pathcomp_find() can be retried using the newly added
 * alternative.
 */
extern void pathcomp_add(pathcomp_t *composer, const char *name, const char *value);
extern void pathcomp_reset(pathcomp_t *composer);
extern int pathcomp_done(pathcomp_t *composer);
extern int pathcomp_next(pathcomp_t *composer);

/**
 * \note The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_find(pathcomp_t *composer);
extern int pathcomp_mkdir(pathcomp_t *composer);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_INCLUDED */
