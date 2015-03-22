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

#ifndef CF_INCLUDED
#define CF_INCLUDED

#include "list.h"

typedef struct cf_kv_t {
    char *key;
    char *value;
} cf_kv_t;

typedef struct cf_section_t {
    char   *name;
    list_t *entries;
} cf_section_t;

typedef struct cf_t {
    list_t *sections;
} cf_t;

extern cf_t *cf_new(void);
extern void  cf_free(cf_t *);
extern int   cf_add_from_string(cf_t *, const char *);
extern int   cf_add_from_file(cf_t *, const char *);

#endif /* CF_INCLUDED */
