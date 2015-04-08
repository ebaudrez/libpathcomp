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

#ifndef VALUE_INCLUDED
#define VALUE_INCLUDED

#include "list.h"

typedef enum { VALUE_LITERAL, VALUE_LUA, VALUE_ALT } value_type_t;

typedef struct {
    value_type_t  type;
    char         *literal;
} value_t;

typedef struct {
    value_type_t  type;
    char         *source;
    char         *result;
} value_lua_t;

typedef struct {
    value_type_t  type;
    list_t       *alternatives;
    list_t       *current;
} value_alt_t;

extern value_t    *value_new(const char *);
extern value_t    *value_clone(value_t *);
extern void        value_free(value_t *);
extern const char *value_eval(value_t *, void *, const char *);
extern void        value_add(value_t **, value_t *);
extern void        value_rewind(value_t *);
extern int         value_next(value_t *);

#endif /* VALUE_INCLUDED */
