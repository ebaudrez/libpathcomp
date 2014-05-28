/**
 * \file
 * \brief Configuration file parsing
 */

#include "config.h"
#include "cf.h"
#include "list.h"
#include "buf.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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

static int
cf_kv_free(void **p, void *userdata)
{
    cf_kv_t *kv = *p;
    if (!kv) return 0;
    free(kv->key);
    free(kv->value);
    free(kv);
    *p = NULL;
    return 0;
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

static int
cf_section_free(void **p, void *userdata)
{
    cf_section_t *section = *p;
    if (!section) return 0;
    free(section->name);
    list_map(section->entries, cf_kv_free, NULL);
    list_free(section->entries);
    free(section);
    *p = NULL;
    return 0;
}

cf_t *
cf_new_from_string(const char *string)
{
    cf_t *cf;
    buf_t text;
    cf = malloc(sizeof *cf);
    if (!cf) return cf;
    cf->sections = NULL;
    buf_init(&text, 0);
    buf_addstr(&text, string);
    if (!cf_parse_text(cf, &text)) {
        buf_release(&text);
        cf_free(cf);
        return NULL;
    }
    buf_release(&text);
    return cf;
}

void
cf_free(cf_t *cf)
{
    if (!cf) return;
    list_map(cf->sections, cf_section_free, NULL);
    list_free(cf->sections);
    free(cf);
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
    for (;;) {
        int ch = buf_fgetc(text);
        if (ch < 0) break;
        if (isspace(ch)) continue;
        if (ch == '[') {
            buf_t name;

            buf_init(&name, 0);
            /* any existing section must be pushed onto the list now */
            if (sec) {
                if (cf->sections) list_push(cf->sections, sec);
                else              cf->sections = list_new(sec);
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
                cf_kv_free((void **) &kv, NULL);
                continue;
            }
            if (sec->entries) list_push(sec->entries, kv);
            else              sec->entries = list_new(kv);
        }
    }
    if (sec) {
        if (cf->sections) list_push(cf->sections, sec);
        else              cf->sections = list_new(sec);
    }
    return 1;
}
