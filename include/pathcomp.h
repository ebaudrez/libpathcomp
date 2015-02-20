/**
 * \file
 * \brief A library for pathname composition
 */

#ifndef PATHCOMP_INCLUDED
#define PATHCOMP_INCLUDED

/** Abstract data type for pathname composer object */
typedef struct pathcomp_t pathcomp_t;

#ifdef __cplusplus
extern "C" {
#endif

/** Read config from \a string and add to the global configuration */
extern void pathcomp_add_config_from_string(const char *string);

/** Read config from file \a filename and add to the global configuration */
extern void pathcomp_add_config_from_file(const char *filename);

/**
 * Perform cleanup of the globals
 *
 * There are still some globals in the implementation: for the configuration,
 * and for the Lua interpreter. This function should be called at or near the
 * end of the main program to free the globals, lest a memory leak occurs.
 */
extern void pathcomp_cleanup(void);

/** Allocate and return a new composer object for class \a name */
extern pathcomp_t *pathcomp_new(const char *name);

/** Free a composer object */
extern void pathcomp_free(pathcomp_t *composer);

/**
 * Set the value of attribute \a name to \a value, replacing its former
 * value(s), if any
 *
 * pathcomp_set() will rewind all alternatives. This is necessary because
 * alternatives are tried in order, and there is no guarantee that a matching
 * combination of alternatives comes later in the list of possible
 * combinations.
 *
 * Imagine the situation where you have successfully located a GERB archive
 * file (identified with instrument G2) with the following characteristics:
 *
 *    -# instrument = G2
 *    -# extension  = .hdf
 *    -# extension  = .hdf.gz
 *    -# etc.
 *
 * After executing pathcomp_find(), the \a extension attribute will point to
 * the <em>.hdf.gz</em> value, because G2 files are gzip-compressed. If you now
 * call
 * \code
 * pathcomp_set(composer, "instrument", "GL")
 * \endcode
 * pathcomp_find() would fail if it didn't rewind all alternatives, because
 * 'GL' files are not gzip-compressed, and the alternative <em>.hdf</em> comes
 * before <em>.hdf.gz</em> in the list of alternatives. Therefore, all
 * alternatives are rewound when calling pathcomp_set().
 */
extern void pathcomp_set(pathcomp_t *composer, const char *name, const char *value);

/**
 * Add value \a value to the values already present for attribute \a name,
 * instead of replacing them
 *
 * All alternatives are rewound after calling this function, so that a search
 * with pathcomp_find() can be retried using the newly added alternative. See
 * the documentation of pathcomp_set() for more information.
 */
extern void pathcomp_add(pathcomp_t *composer, const char *name, const char *value);

/**
 * Evaluate attribute \a name and return its value
 *
 * The string returned by this function must be deallocated by the user. This
 * function returns \null on error, or when the attribute does not exist.
 */
extern char *pathcomp_eval(pathcomp_t *composer, const char *name);

/**
 * Evaluate attribute \a name and return a pointer to internal storage holding
 * the value of the attribute
 *
 * The string returned by this function must not be modified by the user;
 * neither must it be deallocated. This function returns \null on error, or
 * when the attribute does not exist.
 *
 * \note This function returns a pointer to internal storage. There are no
 * guarantees on the lifetime of the object pointed to by the return value of
 * this function. Therefore, this function should probably only be used for
 * debugging. Prefer pathcomp_eval().
 *
 * \see pathcomp_eval()
 */
extern const char *pathcomp_eval_nocopy(pathcomp_t *composer, const char *name);

/**
 * Advance to next alternative
 *
 * \return A true value if there is a next alternative; a false value otherwise
 */
extern int pathcomp_next(pathcomp_t *composer);

/** Return whether all alternatives have been visited */
extern int pathcomp_done(pathcomp_t *composer);

/** Rewind all alternatives */
extern void pathcomp_reset(pathcomp_t *composer);

/**
 * Evaluate and return the pathname represented by the current alternative
 *
 * The pathname represented by the current alternative is constructed by
 * evaluating the attributes \a root and \a compose, and joining them with an
 * intervening directory separator. Note that the directory separator is
 * hardcoded to a slash (<tt>/</tt>).
 *
 * If the attribute \a root cannot be evaluated, the value of the \a compose
 * attribute is returned. If the attribute \a compose cannot be evaluated, the
 * value of the \a root attribute is returned, followed by a directory
 * separator. If neither attribute can be found, \null is returned.
 *
 * The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_yield(pathcomp_t *composer);

/**
 * Advance alternatives until an existing pathname is found, and return this
 * pathname
 *
 * This function will call pathcomp_yield() and pathcomp_next() repeatedly,
 * until an alternative is found that happens to exist on the file system. This
 * pathname is returned to the caller. Subsequent calls to pathcomp_find() will
 * start from the \e next alternative. If no existing pathname is found, \null
 * is returned.
 *
 * The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_find(pathcomp_t *composer);

/**
 * Recursively create directories up to the last directory separator of the
 * pathname represented by the current alternative
 *
 * This function operates like <tt>mkdir -p</tt>. Notably, it is not an error
 * if a directory component exists already. Note that the directory separator
 * is hardcoded to a slash (<tt>/</tt>).
 *
 * \return A true value if the directory has been made successfully; a false
 * value otherwise (\a errno will be set)
 */
extern int pathcomp_mkdir(pathcomp_t *composer);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_INCLUDED */
