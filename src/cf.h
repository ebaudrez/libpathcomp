/**
 * \file
 * \brief Configuration file parsing
 */

#ifndef CF_INCLUDED
#define CF_INCLUDED

#include "list.h"

/**
 * \brief Key-value pair
 */
typedef struct cf_kv_t {
    char *key;
    char *value;
} cf_kv_t;

typedef struct cf_section_t {
    char   *name;
    list_t *entries;
} cf_section_t;

typedef struct cf_t {
    list_t *sections;
} cf_t;

extern cf_t *cf_new(void);
extern void  cf_free(cf_t *);
extern int   cf_add_from_string(cf_t *, const char *);
extern int   cf_add_from_file(cf_t *, const char *);

#endif /* CF_INCLUDED */
