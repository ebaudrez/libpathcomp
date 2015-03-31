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

#ifndef ATT_INCLUDED
#define ATT_INCLUDED

typedef struct att_t att_t;

extern att_t      *att_new(const char *, const char *, const char *);
extern void        att_replace_value(att_t *, const char *, const char *);
extern void        att_add_value(att_t *, const char *);
extern void        att_free(att_t *);
extern int         att_name_equal_to(att_t *, char *);
extern const char *att_get_origin(att_t *);
extern const char *att_eval(att_t *, void *, const char *);
extern void        att_reset(att_t *);
extern int         att_next(att_t *);

#endif /* ATT_INCLUDED */
