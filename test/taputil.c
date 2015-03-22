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

#include <config.h>
#include "taputil.h"
#include "tap.h"
#include "list.h"
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

static void
list_diag_el(char *p)
{
    assert(p);
    diag("           %s", p);
}

static void
list_diag(list_t *list)
{
    if (!list) {
        diag("           (empty)");
        return;
    }
    list_foreach(list, (list_traversal_t *) list_diag_el, NULL);
}

static void
list_add_el_to(char *el, list_t **list)
{
    assert(el);
    assert(list);
    *list = list_push(*list, strdup(el));
}

static list_t *
list_copy(list_t *list)
{
    list_t *new = NULL;
    list_foreach(list, (list_traversal_t *) list_add_el_to, &new);
    return new;
}

static int
compare(char *a, char *b)
{
    return !strcmp(a, b);
}

/* works only for lists of (dynamically allocated) strings */
int
cmp_bag_at_loc(const char *file, int line, list_t *got, list_t *expected, const char *fmt, ...)
{
    va_list ap;
    int test;

    va_start(ap, fmt);
    got = list_copy(got);
    expected = list_copy(expected);
    if (!got && !expected) { test = 1; }
    else if (!got)         { test = 0; }
    else if (!expected)    { test = 0; }
    else {
        test = 1;
        while (list_length(expected)) {
            list_t *p, *q = expected;
            p = list_find_first(got, (list_traversal_t *) compare, q->el);
            if (!p) {
                test = 0;
                break;
            }
            got = list_remove(got, p);
            free(p->el);
            list_free(p);
            expected = list_remove(expected, q);
            free(q->el);
            list_free(q);
        }
        if (list_length(got)) {
            test = 0;
        }
    }
    vok_at_loc(file, line, test, fmt, ap);
    va_end(ap);
    if (!test) {
        diag("  lists differ (showing only elements that are not common)");
        diag("         got:");
        list_diag(got);
        diag("    expected:");
        list_diag(expected);
    }
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    list_foreach(expected, (list_traversal_t *) free, NULL);
    list_free(expected);
    return test;
}

int
path_exists_ok_at_loc(const char *file, int line, const char *path, const char *fmt, ...)
{
    int test;
    FILE *f;
    va_list ap;

    va_start(ap, fmt);
    assert(path);
    f = fopen(path, "r");
    test = (f != NULL);
    if (fmt) vok_at_loc(file, line, test, fmt, ap);
    else {
        const char *def = "path %s exists";
        char *s;
        s = malloc((strlen(def) - 2 + strlen(path) + 1)*sizeof *s);
        sprintf(s, def, path);
        vok_at_loc(file, line, test, s, ap);
        free(s);
    }
    if (f) fclose(f);
    va_end(ap);
    return test;
}

int
path_not_exists_ok_at_loc(const char *file, int line, const char *path, const char *fmt, ...)
{
    int test;
    FILE *f;
    va_list ap;

    va_start(ap, fmt);
    assert(path);
    f = fopen(path, "r");
    test = (f == NULL);
    if (fmt) vok_at_loc(file, line, test, fmt, ap);
    else {
        const char *def = "path %s does not exist";
        char *s;
        s = malloc((strlen(def) - 2 + strlen(path) + 1)*sizeof *s);
        sprintf(s, def, path);
        vok_at_loc(file, line, test, s, ap);
        free(s);
    }
    if (f) fclose(f);
    va_end(ap);
    return test;
}

int
dir_exists_ok_at_loc(const char *file, int line, const char *path, const char *fmt, ...)
{
    int test;
    va_list ap;
    struct stat buf;

    va_start(ap, fmt);
    assert(path);
    if (stat(path, &buf) == -1) test = 0;
    else test = S_ISDIR(buf.st_mode);
    if (fmt) vok_at_loc(file, line, test, fmt, ap);
    else {
        const char *def = "path %s exists and is a directory";
        char *s;
        s = malloc((strlen(def) - 2 + strlen(path) + 1)*sizeof *s);
        sprintf(s, def, path);
        vok_at_loc(file, line, test, s, ap);
        free(s);
    }
    va_end(ap);
    return test;
}
