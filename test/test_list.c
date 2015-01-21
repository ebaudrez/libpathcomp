/* test list.c */

#include <config.h>
#include "tap.h"
#include "list.h"
#include <string.h>

static int
strarray_len(const char **strings)
{
    const char **p = strings;
    while (*p) p++;
    return (int) (p - strings);
}

static list_t *
test_construction(const char **strings)
{
    list_t *list;
    const char **p = strings;
    list = list_new((void *) *p++);
    ok(list, "list_new()");
    while (*p) {
        list_push(list, (void *) *p++);
    }
    cmp_ok(list_length(list), "==", strarray_len(strings), "list_length()");
    return list;
}

static void
compute_len(const char *s, int **dst)
{
    **dst = strlen(s);
    ++(*dst); /* advance pointer in destination array */
}

static void
test_foreach(list_t *list, const char **strings)
{
    int *len = malloc(strarray_len(strings) * sizeof(*len)), *tmp = len;
    list_foreach(list, (list_traversal_t *) compute_len, &len);
    len = tmp; /* compute_len() advances len! */
    while (*strings) {
        cmp_ok(strlen(*strings++), "==", *len++);
    }
    free(tmp);
}

static const char *test_data[] = {
    "abc",
    "defg",
    "hijk",
    "lmnop",
    "long string indeed",
    "and another long string that should surpass the previous one in length",
    NULL
};

static int
el_equal_to(void *p, void *userdata)
{
    const char *el = p, *str = userdata;
    return !strcmp(el, str);
}

static void
test_find_first(list_t *list)
{
    list_t *p, *q;
    p = list_find_first(list, el_equal_to, "abc");
    ok(p, "list_find_first() on first element");
    ok(p == list, "... equal to head of list");
    is(p->el, "abc", "correct element found");
    q = list_find_first(list, el_equal_to, "abc");
    ok(p == q);
    q = list_find_first(list, el_equal_to, "defg");
    ok(p != q);
    is(q->el, "defg", "correct element found");
    p = list_find_first(list, el_equal_to, "a string that should not appear in the list");
    ok(!p, "list_find_first() on element that can not be found");
    p = list_find_first(list, el_equal_to, "defg");
    q = list_find_first(p, el_equal_to, "defg");
    ok(p == q);
    q = list_find_first(p, el_equal_to, "abc");
    ok(!q, "'abc' cannot be found after 'defg'");
    p = list_find_first(list, el_equal_to, "defg");
    p = list_find_first(p, el_equal_to, "lmnop");
    p = list_find_first(p, el_equal_to, "long string indeed");
    ok(p, "chained calls");
    is(p->el, "long string indeed", "correct element found");
    /* check for more occurrences of string */
    p = p->next;
    p = list_find_first(p, el_equal_to, "long string indeed");
    ok(!p, "no more occurrences of string");
    p = list_find_first(NULL, el_equal_to, "01234");
    ok(!p, "no matches on NULL list (and does not die)");
    /* Valgrind will report a 'still reachable' memory leak for the following test */
    dies_ok(p = list_find_first(list, NULL, NULL); , "assert fails on NULL function");
}

static void
test_push2(void)
{
    list_t *list = NULL;
    ok(!list);
    cmp_ok(list_length(list), "==", 0);
    list = list_push(list, "abc");
    ok(list);
    cmp_ok(list_length(list), "==", 1);
    list = list_push(list, "def");
    ok(list);
    cmp_ok(list_length(list), "==", 2);
    list_free(list);
}

static void
test_from(void)
{
    list_t *list, *p;
    list = list_from("abc", "def", "ghi", "jkl", "mno", NULL);
    ok(list);
    cmp_ok(list_length(list), "==", 5);
    p = list;
    is(p->el, "abc");
    ok(p = p->next);
    is(p->el, "def");
    ok(p = p->next);
    is(p->el, "ghi");
    ok(p = p->next);
    is(p->el, "jkl");
    ok(p = p->next);
    is(p->el, "mno");
    ok(!p->next);
    list_free(list);
}

static void
test_remove(void)
{
    list_t *list, *p;
    list = list_from("abc", "def", "ghi", "jkl", "mno", NULL);
    cmp_ok(list_length(list), "==", 5);

    p = list_find_first(list, el_equal_to, "ghi");
    list = list_remove(list, p);
    cmp_ok(list_length(list), "==", 4);
    is(list->el, "abc");
    is(p->el, "ghi");
    ok(!p->next);
    list_free(p);

    p = list_find_first(list, el_equal_to, "jkl");
    list = list_remove(list, p);
    cmp_ok(list_length(list), "==", 3);
    is(list->el, "abc");
    is(p->el, "jkl");
    ok(!p->next);
    list_free(p);

    p = list_find_first(list, el_equal_to, "jkl");
    ok(!p);
    list = list_remove(list, p);
    cmp_ok(list_length(list), "==", 3);
    is(list->el, "abc");

    p = list_find_first(list, el_equal_to, "abc");
    list = list_remove(list, p);
    cmp_ok(list_length(list), "==", 2);
    is(list->el, "def");
    is(p->el, "abc");
    ok(!p->next);
    list_free(p);

    p = list_find_first(list, el_equal_to, "mno");
    list = list_remove(list, p);
    cmp_ok(list_length(list), "==", 1);
    is(list->el, "def");
    is(p->el, "mno");
    ok(!p->next);
    list_free(p);

    p = list_find_first(list, el_equal_to, "def");
    list = list_remove(list, p);
    ok(!list);
    cmp_ok(list_length(list), "==", 0);
    is(p->el, "def");
    ok(!p->next);
    list_free(p);

    list_free(list);
}

int
main(void)
{
    list_t *list;
    plan(NO_PLAN);
    list = test_construction(test_data);
    test_foreach(list, test_data);
    test_find_first(list);
    /* no need to deallocate the entries of list, as they are pointers to string literals */
    list_free(list);
    test_push2();
    test_from();
    test_remove();
    done_testing();
}
