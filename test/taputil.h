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

#ifndef TAPUTIL_INCLUDED
#define TAPUTIL_INCLUDED

#include "list.h"

extern int cmp_bag_at_loc(const char *, int, list_t *, list_t *, const char *, ...);
extern int path_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
extern int path_not_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
extern int dir_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
#define cmp_bag(...) cmp_bag_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define path_exists_ok(...) path_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define path_not_exists_ok(...) path_not_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define dir_exists_ok(...) dir_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)

#endif /* TAPUTIL_INCLUDED */
