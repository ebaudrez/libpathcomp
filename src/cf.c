/**
 * \file
 * \brief Configuration file parsing
 */

#include "config.h"
#include "cf.h"
#include "list.h"
#include "buf.h"
#include "log.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

/* forward declaration */
static int cf_parse_text(cf_t *, buf_t *);

static cf_kv_t *
cf_kv_new(const char *key, const char *value)
{
    cf_kv_t *kv;
    kv = malloc(sizeof *kv);
    if (!kv) return kv;
    kv->key = strdup(key);
    kv->value = strdup(value);
    return kv;
}

static void
cf_kv_free(cf_kv_t *kv)
{
    if (!kv) return;
    free(kv->key);
    free(kv->value);
    free(kv);
}

static cf_section_t *
cf_section_new(const char *name)
{
    cf_section_t *section;
    section = malloc(sizeof *section);
    if (!section) return section;
    section->name = strdup(name);
    section->entries = NULL;
    return section;
}

static void
cf_section_free(cf_section_t *section)
{
    if (!section) return;
    free(section->name);
    list_foreach(section->entries, (list_traversal_t *) cf_kv_free, NULL);
    list_free(section->entries);
    free(section);
}

cf_t *
cf_new(void)
{
    cf_t *cf;
    cf = malloc(sizeof *cf);
    if (!cf) return cf;
    cf->sections = NULL;
    return cf;
}

void
cf_free(cf_t *cf)
{
    if (!cf) return;
    list_foreach(cf->sections, (list_traversal_t *) cf_section_free, NULL);
    list_free(cf->sections);
    free(cf);
}

int
cf_add_from_string(cf_t *cf, const char *string)
{
    buf_t text;
    int rc;
    assert(cf);
    buf_init(&text, 0);
    buf_addstr(&text, string);
    rc = cf_parse_text(cf, &text);
    buf_release(&text);
    return rc;
}

int
cf_add_from_file(cf_t *cf, const char *filename)
{
    buf_t text;
    log_t *log;
    int rc;
    assert(cf);
    log = log_get_logger("cf");
    buf_init(&text, 0);
    rc = buf_read_file(&text, filename, 0);
    if (rc == -1) {
        int sv = errno;
        log_error(log, "cannot read file %s: %s", filename, strerror(sv));
        return 0;
    }
    rc = cf_parse_text(cf, &text);
    buf_release(&text);
    return rc;
}

static int
cf_parse_section_name(buf_t *text, buf_t *name)
{
    for (;;) {
        int ch = buf_fgetc(text);
        if (ch < 0) break;
        if (ch == '\n') break;
        if (ch == ']') return 1;
        buf_addch(name, ch);
    }
    return 0;
}

static int
cf_parse_value(buf_t *text, buf_t *value)
{
    /* skip initial spaces */
    for (;;) {
        int ch = buf_fgetc(text);
        if (!isspace(ch) || ch == '\n') {
            buf_ungetc(text, ch);
            break;
        }
    }
    /* parse value */
    for (;;) {
        int ch = buf_fgetc(text);
        if (ch < 0) break;
        if (ch == '\\') {
            ch = buf_fgetc(text);
            switch (ch) {
                case '\n':
                    /* escape newlines */
                    continue;
#if 0
                case '\\':
                    /* insert literal backslash */
                    break;
#endif
                default  :
                    /* let unknown escape sequences pass through */
                    buf_addch(value, '\\');
                    break;
            }
        }
        if (ch == '\n') return 1;
        buf_addch(value, ch);
    }
    /* reaching end-of-file is not a syntax error! */
    return 1;
}

static int
cf_parse_key_value_pair(buf_t *text, buf_t *key, buf_t *value)
{
    for (;;) {
        int ch = buf_fgetc(text);
        if (ch < 0) break;
        if (ch == '\n') break;
        if (ch == '[' || ch == ']') break;
        if (ch == '=') {
            buf_rtrim(key);
            return cf_parse_value(text, value);
        }
        buf_addch(key, ch);
    }
    return 0;
}

static int
cf_parse_text(cf_t *cf, buf_t *text)
{
    cf_section_t *sec = NULL;
    int comment = 0;
    for (;;) {
        int ch = buf_fgetc(text);
        if (ch < 0) break;
        if (ch == '\n') {
            comment = 0;
            continue;
        }
        if (comment || isspace(ch)) continue;
        if (ch == ';' || ch == '#' ) {
            comment = 1;
            continue;
        }
        if (ch == '[') {
            buf_t name;

            buf_init(&name, 0);
            /* any existing section must be pushed onto the list now */
            if (sec) {
                cf->sections = list_push(cf->sections, sec);
            }
            if (!cf_parse_section_name(text, &name)) return 0;
            sec = cf_section_new(name.buf);
            buf_release(&name);
            continue;
        }
        if (ch == '=') return 0;
        else {
            buf_t key, value;
            cf_kv_t *kv;

            buf_ungetc(text, ch);
            buf_init(&key, 0);
            buf_init(&value, 0);
            if (!cf_parse_key_value_pair(text, &key, &value)) return 0;
            buf_rtrim(&value);
            kv = cf_kv_new(key.buf, value.buf);
            buf_release(&key);
            buf_release(&value);
            if (!sec) {
                /* there is no section to attach this entry to */
                cf_kv_free(kv);
                continue;
            }
            sec->entries = list_push(sec->entries, kv);
        }
    }
    if (sec) {
        cf->sections = list_push(cf->sections, sec);
    }
    return 1;
}
