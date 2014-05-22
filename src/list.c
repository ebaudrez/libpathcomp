/**
 * \file
 * \brief Linked lists (implementation)
 */

#include "config.h"
#include "list.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

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

void
list_push(list_t *list, void *el)
{
    list_t **p = &list;
    assert(list);
    while (*p) {
        p = & (*p)->next;
    }
    *p = malloc(sizeof **p);
    if (!(*p)) return;
    (*p)->next = NULL;
    (*p)->el = el;
}

void
list_map(list_t *list, list_traversal_t *f, void *userdata)
{
    assert(f);
    while (list) {
        f(&list->el, userdata);
        list = list->next;
    }
}

list_t *
list_find_first(list_t *list, list_traversal_t *f, void *userdata)
{
    assert(f);
    while (list) {
        if (f(&list->el, userdata)) return list;
        list = list->next;
    }
    return NULL;
}
