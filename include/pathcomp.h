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

/** Perform cleanup of the globals */
extern void pathcomp_cleanup(void);

/**
 * Allocate and return a new composer object for class \a name; initialize
 * attributes from sections in configuration with same name
 */
extern pathcomp_t *pathcomp_new(const char *name);

/** Clone an existing composer object */
extern pathcomp_t *pathcomp_clone(pathcomp_t *composer);

/** Free a composer object */
extern void pathcomp_free(pathcomp_t *composer);

/**
 * Set the value of attribute \a name to \a value, replacing its former
 * value(s), if any
 *
 * This function will rewind all alternatives. See the user manual for more
 * information.
 */
extern void pathcomp_set(pathcomp_t *composer, const char *name, const char *value);

/**
 * Add value \a value to the values already present for attribute \a name,
 * instead of replacing them
 *
 * This function will rewind all alternatives. See the user manual for more
 * information.
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
 * testing. Prefer pathcomp_eval().
 *
 * \see pathcomp_eval()
 */
extern const char *pathcomp_eval_nocopy(pathcomp_t *composer, const char *name);

/**
 * Return a textual representation of the state of composer object
 *
 * The string returned by this function must be deallocated by the user. Format
 * and contents of the string are subject to change. This function is to be
 * used for debugging or testing only.
 */
extern char *pathcomp_dump(pathcomp_t *composer);

/**
 * Advance to next combination of alternatives
 *
 * \return A true value if there is a next combination; a false value otherwise
 */
extern int pathcomp_next(pathcomp_t *composer);

/** Return whether all combinations of alternatives have been visited */
extern int pathcomp_done(pathcomp_t *composer);

/** Rewind all alternatives */
extern void pathcomp_rewind(pathcomp_t *composer);

/**
 * Evaluate and return the pathname represented by the composer object
 *
 * The pathname represented by the current combination of alternatives is
 * constructed by evaluating the attributes \a root and \a compose, and joining
 * them with an intervening directory separator. Note that the directory
 * separator is hardcoded to a slash (<tt>/</tt>).
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
 * Step through combinations of alternatives until an existing pathname is
 * found, and return this pathname
 *
 * This function will call pathcomp_yield() and pathcomp_next() repeatedly,
 * until a combination of alternatives is found that happens to exist on the
 * file system. This pathname is returned to the caller. Subsequent calls to
 * pathcomp_find() will start from the \e next combination. If no existing
 * pathname is found, \null is returned.
 *
 * The string returned by this function must be deallocated by the user.
 */
extern char *pathcomp_find(pathcomp_t *composer);

/**
 * Recursively create directories up to the last directory separator of the
 * pathname represented by the composer object
 *
 * The pathname represented by the current combination of alternatives, as
 * returned by pathcomp_yield(), is scanned for directory separators. The path
 * <em>up to the last directory separator</em> is created by calling
 * <tt>mkdir(2)</tt> recursively. This function operates like the command
 * <tt>mkdir -p</tt>. Notably, it is not an error if a directory component
 * exists already. Note that the directory separator is hardcoded to a slash
 * (<tt>/</tt>).
 *
 * \return A true value if the directory has been made successfully; a false
 * value otherwise (\a errno will be set)
 */
extern int pathcomp_mkdir(pathcomp_t *composer);

#ifdef __cplusplus
}
#endif

#endif /* PATHCOMP_INCLUDED */
