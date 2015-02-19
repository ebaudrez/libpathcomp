#include <config.h>
#include "att.h"
#include "value.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct att_t {
    char    *name;
    value_t *value;
};

att_t *
att_new(const char *name, const char *value)
{
    att_t *att;
    att = malloc(sizeof *att);
    if (!att) return att;
    att->name = strdup(name);
    att->value = value_new(value);
    return att;
}

void
att_replace_value(att_t *att, const char *value)
{
    assert(att);
    value_free(att->value);
    att->value = value_new(value);
}

void
att_add_value(att_t *att, const char *value)
{
    assert(att);
    value_add(&att->value, value_new(value));
}

void
att_free(att_t *att)
{
    if (!att) return;
    free(att->name);
    value_free(att->value);
    free(att);
}

int
att_name_equal_to(att_t *att, char *name)
{
    assert(att);
    assert(name);
    return !strcmp(att->name, name);
}

/*
 * This function returns a pointer to internal storage. There are no guarantees
 * on the lifetime of the object pointed to by the return value of this
 * function.
 */
const char *
att_eval(att_t *att, void *composer, const char *metatable)
{
    assert(att);
    return value_eval(att->value, composer, metatable);
}

void
att_reset(att_t *att)
{
    assert(att);
    value_reset(att->value);
}

int
att_next(att_t *att)
{
    assert(att);
    return value_next(att->value);
}
