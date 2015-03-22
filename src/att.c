/*
 * Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
 * This file is part of Libpathcomp.
 *
 * Libpathcomp is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Libpathcomp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
 */

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
