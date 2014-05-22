#ifndef VALUE_INCLUDED
#define VALUE_INCLUDED

#include "config.h"

typedef enum { VALUE_LITERAL, VALUE_LUA } value_type_t;

typedef struct {
    value_type_t  type;
    char         *literal;
} value_t;

typedef struct {
    value_type_t  type;
    char         *source;
} value_lua_t;

extern value_t    *value_new(const char *);
extern void        value_free(value_t *);
extern const char *value_eval(value_t *, void *, const char *);

#endif /* VALUE_INCLUDED */
