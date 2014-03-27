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

/**
 * \note The callback is guaranteed to receive a non-null pointer as the first
 * argument.
 */
typedef int list_traversal_t( void **, void * );

extern list_t *list_new( void * );
extern void    list_free( list_t * );
extern int     list_length( list_t * );
extern void    list_push( list_t *, void * );
extern void    list_map( list_t *, list_traversal_t *, void * );
extern list_t *list_find_first( list_t *list, list_traversal_t *, void * );

#endif /* LIST_INCLUDED */
