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
 *
 * This interface is derived from list.h, which is a part of the source code
 * accompanying the book "C Interfaces and Implementations: Techniques for
 * Creating Reusable Software" by David R. Hanson, and published by
 * Addison-Wesley in 1997. That code is covered by the following copyright and
 * permission notice:
 *
 *      Copyright (c) 1994,1995,1996,1997 by David R. Hanson.
 *
 *      Permission is hereby granted, free of charge, to any person obtaining a
 *      copy of this software and associated documentation files (the
 *      "Software"), to deal in the Software without restriction, including
 *      without limitation the rights to use, copy, modify, merge, publish,
 *      distribute, sublicense, and/or sell copies of the Software, and to
 *      permit persons to whom the Software is furnished to do so, subject to
 *      the following conditions:
 *
 *      The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 *      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LIST_INCLUDED
#define LIST_INCLUDED

typedef struct list_t {
    struct list_t *next;
    void          *el;
} list_t;

typedef int list_traversal_t(void *, void *);
/*typedef int list_traversal_ref_t(void **, void *);*/

extern list_t *list_new(void *);
extern void    list_free(list_t *);
extern int     list_length(list_t *);
extern list_t *list_push(list_t *, void *);
extern list_t *list_from(void *, ...);
#define list_foreach list_foreach_byval
extern void    list_foreach_byval(list_t *, list_traversal_t *, void *);
/*extern void    list_foreach_byref(list_t *, list_traversal_ref_t *, void *);*/
extern list_t *list_find_first(list_t *list, list_traversal_t *, void *);
extern list_t *list_remove(list_t *, list_t *);

#endif /* LIST_INCLUDED */
