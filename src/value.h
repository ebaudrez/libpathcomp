#ifndef VALUE_INCLUDED
#define VALUE_INCLUDED

#include "list.h"

typedef enum { VALUE_LITERAL, VALUE_LUA, VALUE_ALT } value_type_t;

typedef struct {
    value_type_t  type;
    char         *literal;
} value_t;

typedef struct {
    value_type_t  type;
    char         *source;
    char         *result;
} value_lua_t;

typedef struct {
    value_type_t  type;
    list_t       *alternatives;
    list_t       *current;
} value_alt_t;

extern value_t    *value_new(const char *);
extern void        value_free(value_t *);
extern const char *value_eval(value_t *, void *, const char *);
extern void        value_add(value_t **, value_t *);
extern void        value_reset(value_t *);
extern int         value_next(value_t *);

#endif /* VALUE_INCLUDED */
