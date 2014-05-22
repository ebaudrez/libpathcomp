/**
 * \file
 * \brief Configuration file parsing
 */

#ifndef CF_INCLUDED
#define CF_INCLUDED

#include "config.h"
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

extern cf_t *cf_new_from_string(const char *);
extern void  cf_free(cf_t *);

#endif /* CF_INCLUDED */
