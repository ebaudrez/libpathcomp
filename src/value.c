#include "config.h"
#include "value.h"
#include "interpreter.h"
#include "log.h"
#include "buf.h"
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <lua.h>
#include <lauxlib.h>

/**
 * \name Routines specific to literal values
 * \{
 */

static value_t *
value_literal_new(const char *text)
{
    value_t *val;
    assert(text);
    val = malloc(sizeof *val);
    if (!val) return val;
    val->type = VALUE_LITERAL;
    val->literal = strdup(text);
    return val;
}

/**
 * \}
 * \name Routines specific to Lua values
 * \{
 */

static char *
is_lua_code(const char *text)
{
    buf_t buf;
    char *p;
    const char *keyword  = "lua",
               *preamble = "local self = ...; ";
    assert(text);
    buf_init(&buf, strlen(text) + strlen(preamble));
    buf_addstr(&buf, text);
    p = buf.buf;
    if (strlen(p) < strlen(keyword)) goto fail;
    if (strncmp(p, keyword, strlen(keyword)) != 0) goto fail;
    p += strlen(keyword);
    while (*p && isspace(*p)) ++p;
    if (*p == '\0' || *p != '{') goto fail;
    ++p;
    buf_splicestr(&buf, 0, p - buf.buf, preamble);
    p = strrchr(buf.buf, '}');
    buf_setlen(&buf, p - buf.buf);
    return buf_detach(&buf, NULL);
fail:
    buf_release(&buf);
    return NULL;
}

static value_t *
value_lua_new(const char *source)
{
    value_lua_t *val;
    assert(source);
    val = malloc(sizeof *val);
    if (!val) return (value_t *) val;
    val->type = VALUE_LUA;
    val->source = strdup(source);
    val->result = NULL;
    return (value_t *) val;
}

static void
value_lua_free(value_lua_t *val)
{
    assert(val);
    free(val->source);
    free(val->result);
    free(val);
}

/**
 * \param composer Pointer to composer object
 * \param metatable Name of the Lua metatable
 *
 * \a composer and \a metatable may be null if the Lua code to be evaluated
 * does not need access to other attributes in the composer object via 'self'.
 * This is mainly useful for testing. Otherwise, it is probably a mistake.
 * Fortunately, the user does not normally have access to value_eval() and
 * value_lua_eval(). It will be called for him by pathcomp_eval(), and the
 * composer and metatable arguments will be properly set. Hence, the user will
 * always have access to 'self' in the Lua code.
 */
static const char *
value_lua_eval(value_lua_t *val, void *composer, const char *metatable)
{
    log_t      *log;
    lua_State  *L = interpreter_get_state();
    void      **p;
    int         nargs = 0;
    assert(val);
    log = log_get_logger("pathcomp");
    if (luaL_loadstring(L, val->source) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        log_error(log, "cannot parse Lua code: %s", error);
        return NULL;
    }
    if (composer && metatable) {
        p = lua_newuserdata(L, sizeof(*p));
        *p = composer;
        luaL_getmetatable(L, metatable);
        lua_setmetatable(L, -2);
        nargs = 1;
    }
    if (lua_pcall(L, nargs, 1, 0) != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        log_error(log, "cannot execute Lua code: %s", error);
        return NULL;
    }
    free(val->result);
    return val->result = strdup(lua_tostring(L, -1));
}

/**
 * \}
 * \name Routines specific to alternatives
 * \{
 */

static value_t *
value_alt_new(value_t *orig)
{
    value_alt_t *val;
    assert(orig);
    val = malloc(sizeof *val);
    if (!val) return (value_t *) val;
    val->type = VALUE_ALT;
    val->alternatives = list_new(orig);
    val->current = val->alternatives;
    return (value_t *) val;
}

static void
value_alt_free(value_alt_t *val)
{
    assert(val);
    list_foreach(val->alternatives, (list_traversal_t *) value_free, NULL);
    list_free(val->alternatives);
    free(val);
}

/**
 * \}
 * \name Generic \a value_t routines
 * \{
 */

value_t *
value_new(const char *text)
{
    char *source;
    assert(text);
    if ((source = is_lua_code(text))) {
        value_t *val = value_lua_new(source);
        free(source);
        return val;
    }
    else {
        return value_literal_new(text);
    }
}

void
value_free(value_t *val)
{
    if (!val) return;
    switch (val->type) {
        case VALUE_LITERAL:
            free(val->literal);
            free(val);
            break;
        case VALUE_LUA:
            value_lua_free((value_lua_t *) val);
            break;
        case VALUE_ALT:
            value_alt_free((value_alt_t *) val);
            break;
        default:
            assert(0);
    }
}

/**
 * \a composer and \a metatable may be null if the Lua code to be evaluated
 * does not need access to other attributes in the composer object via 'self'.
 */
const char *
value_eval(value_t *val, void *composer, const char *metatable)
{
    assert(val);
    switch (val->type) {
        case VALUE_LITERAL:
            return val->literal;
        case VALUE_LUA:
            return value_lua_eval((value_lua_t *) val, composer, metatable);
        case VALUE_ALT:
            return value_eval(((value_alt_t *) val)->current->el, composer, metatable);
        default:
            assert(0);
    }
}

/**
 * \brief Upgrade any \a value_t to \a value_alt_t in place
 */
static void
value_upgrade(value_t **pval)
{
    assert(pval && *pval);
    if ((*pval)->type == VALUE_ALT) return;
    *pval = value_alt_new(*pval);
}

/* add a value to another value, converting the latter into a \a value_alt_t if necessary */
void
value_add(value_t **pdst, value_t *src)
{
    assert(pdst);
    assert(src);
    value_upgrade(pdst);
    list_push(((value_alt_t *) *pdst)->alternatives, src);
}

/**
 * \}
 */
