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
 * This implementation is derived from list.c, which is a part of the source
 * code accompanying the book "C Interfaces and Implementations: Techniques for
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

#include <config.h>
#include "list.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

list_t *
list_new(void *el)
{
    list_t *list;
    list = malloc(sizeof *list);
    if (!list) return list;
    list->next = NULL;
    list->el = el;
    return list;
}

void
list_free(list_t *list)
{
    while (list) {
        list_t *next = list->next;
        free(list);
        list = next;
    }
}

int
list_length(list_t *list)
{
    int n = 0;
    while (list) {
        ++n;
        list = list->next;
    }
    return n;
}

list_t *
list_push(list_t *list, void *el)
{
    list_t *new, *p = list;
    new = malloc(sizeof *new);
    if (!new) return NULL;
    new->next = NULL;
    new->el = el;
    if (!list) return new;
    while (p->next) p = p->next;
    p->next = new;
    return list;
}

list_t *
list_from(void *el, ...)
{
    list_t *list = NULL;
    va_list ap;
    va_start(ap, el);
    while (el) {
        list = list_push(list, el);
        el = va_arg(ap, void *);
    }
    va_end(ap);
    return list;
}

void
list_foreach_byval(list_t *list, list_traversal_t *f, void *userdata)
{
    assert(f);
    while (list) {
        f(list->el, userdata);
        list = list->next;
    }
}

list_t *
list_find_first(list_t *list, list_traversal_t *f, void *userdata)
{
    assert(f);
    while (list) {
        if (f(list->el, userdata)) return list;
        list = list->next;
    }
    return NULL;
}

list_t *
list_remove(list_t *list, list_t *link)
{
    list_t *p = list, *prev = NULL;
    while (p) {
        if (p == link) {
            if (prev) prev->next = link->next;
            else      list = link->next;
            link->next = NULL;
            break;
        }
        prev = p;
        p = p->next;
    }
    return list;
}
