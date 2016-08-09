/*
 * Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>
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
#include "list.h"
#include "value.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct att_t {
    char   *name;
    list_t *alternatives;
    list_t *current;
    char   *origin; /* not used by att_*() functions */
};

/**
 * \note att_new() assumes ownership of \a value. Callers must never free the
 * value they pass into att_new().
 */
att_t *
att_new(const char *name, value_t *value, const char *origin)
{
    att_t *att;
    assert(name);
    assert(value);
    att = malloc(sizeof *att);
    if (!att) return att;
    att->name = strdup(name);
    att->alternatives = list_new(value);
    att->current = att->alternatives;
    att->origin = origin ? strdup(origin) : NULL;
    return att;
}

att_t *
att_clone(att_t *att)
{
    att_t *clone;
    list_t *p;
    assert(att);
    clone = malloc(sizeof *clone);
    if (!clone) return clone;
    clone->name = strdup(att->name);
    clone->alternatives = list_transform(att->alternatives, (list_transform_t *) value_clone, NULL);
    clone->current = clone->alternatives;
    /* att->current points to the n-th alternative; make clone->current point
     * to the n-th alternative simply by counting */
    for (p = att->alternatives; (p != att->current) && p; p = p->next) {
        assert(clone->current);
        clone->current = clone->current->next;
    }
    assert(p == att->current);
    clone->origin = att->origin ? strdup(att->origin) : NULL;
    return clone;
}

/**
 * \note att_replace_value() assumes ownership of \a value. Callers must never
 * free the value they pass into att_replace_value().
 */
void
att_replace_value(att_t *att, value_t *value, const char *origin)
{
    assert(att);
    assert(value);
    list_foreach(att->alternatives, (list_traversal_t *) value_free, NULL);
    list_free(att->alternatives);
    att->alternatives = list_new(value);
    att->current = att->alternatives;
    free(att->origin);
    att->origin = origin ? strdup(origin) : NULL;
}

/**
 * \note att_add_value() assumes ownership of \a value. Callers must never free
 * the value they pass into att_add_value().
 */
void
att_add_value(att_t *att, value_t *value)
{
    assert(att);
    assert(value);
    list_push(att->alternatives, value);
}

void
att_free(att_t *att)
{
    if (!att) return;
    free(att->name);
    list_foreach(att->alternatives, (list_traversal_t *) value_free, NULL);
    list_free(att->alternatives);
    free(att->origin);
    free(att);
}

int
att_name_equal_to(att_t *att, char *name)
{
    assert(att);
    assert(name);
    return !strcmp(att->name, name);
}

const char *
att_get_origin(att_t *att)
{
    assert(att);
    return att->origin;
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
    return att->current ? value_eval(att->current->el, composer, metatable) : NULL;
}

void
att_rewind(att_t *att)
{
    assert(att);
    att->current = att->alternatives;
}

int
att_next(att_t *att)
{
    assert(att);
    if (!att->current) return 0;
    att->current = att->current->next;
    return att->current != NULL;
}

int
att_push(att_t *att, void *composer, const char *metatable)
{
    assert(att);
    return att->current ? value_push(att->current->el, composer, metatable) : 0;
}

void
att_dump(att_t *att, buf_t *buf)
{
    assert(att);
    assert(buf);
    buf_addf(buf, "    attribute at 0x%x\n", att);
    buf_addf(buf, "      name: %s\n", att->name);
    buf_addf(buf, "      origin: %s\n", att->origin ? att->origin : "(null)");
    buf_addf(buf, "      values:\n");
    value_dump_info_t info = { buf, att->current ? att->current->el : NULL };
    list_foreach(att->alternatives, (list_traversal_t *) value_dump, &info);
}
