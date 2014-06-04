/**
 * \file
 * \brief Linked lists (header)
 */

#ifndef LIST_INCLUDED
#define LIST_INCLUDED

#include "config.h"

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

#endif /* LIST_INCLUDED */
