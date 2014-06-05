#include "config.h"
#include "taputil.h"
#include "tap.h"
#include "list.h"
#include <assert.h>
#include <string.h>

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
