/*
 * Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>
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

#include "buf.h"

typedef struct {
    enum { VALUE_STRING, VALUE_LUA } type;
    union {
        char *string;
        struct {
            char *source;
            char *result;
        } lua;
    } u;
} value_t;

typedef struct {
    buf_t   *buf;
    value_t *current;
} value_dump_info_t;

extern value_t    *value_new(const char *);
extern value_t    *value_clone(value_t *);
extern void        value_free(value_t *);
extern const char *value_eval(value_t *, void *, const char *);
extern void        value_dump(value_t *, value_dump_info_t *);

#endif /* VALUE_INCLUDED */
