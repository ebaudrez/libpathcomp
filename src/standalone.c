#include <config.h>
#include "pathcomp.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "pathcomp/log.h"

/* usage:
 *
 *   pathcomp  -c class  [ -aem -x attr ]  key=value key=value key+=value ...
 *
 * mandatory command-line arguments:
 *
 *   -c class: use this locator class
 *
 * options:
 *
 *   -a: print all matching paths (by default prints first path)
 *   -e: print only existing paths [[perform _find()]]
 *   -m: create parent directory [[perform _mkdir()]] (doesn't make sense with -e and will be ignored)
 *   -x attr: evaluate attribute 'attr' [[perform _eval()]] (not yet implemented)
 */

typedef struct {
    char *key;
    char *value;
    int add;
} kv_t;

typedef struct {
    char *class;
    list_t *attributes;
    int print_all;
    int only_existing;
    int do_mkdir;
} opt_t;

static kv_t *
kv_new(const char *text)
{
    kv_t *kv;
    char *key, *val, *p;

    assert(text);
    kv = malloc(sizeof *kv);
    if (!kv) return kv;
    assert(key = strdup(text));
    assert(val = strchr(key, '='));
    *val++ = '\0';
    if (strlen(key) < 1) {
        pathcomp_log_error("empty key specified on command line");
        free(key);
        free(kv);
        return NULL;
    }
    kv->key = strdup(key);
    kv->value = strdup(val);
    kv->add = 0;
    p = kv->key + strlen(kv->key) - 1;
    if (*p == '+') {
        *p = '\0';
        kv->add = 1;
    }
    free(key);
    return kv;
}

static void
kv_free(kv_t *kv)
{
    if (!kv) return;
    free(kv->key);
    free(kv->value);
    free(kv);
}

static void
kv_add_to_composer(kv_t *kv, pathcomp_t *composer)
{
    assert(kv);
    assert(composer);
    if (kv->add) pathcomp_add(composer, kv->key, kv->value);
    else pathcomp_set(composer, kv->key, kv->value);
}

static opt_t *
opt_new(int argc, char **argv)
{
    int opt;
    opt_t *options;

    options = malloc(sizeof *options);
    if (!options) return options;
    options->class = NULL;
    options->attributes = NULL;
    options->print_all = 0;
    options->only_existing = 0;
    options->do_mkdir = 0;
    while ((opt = getopt(argc, argv, "ac:em")) != -1) {
        switch (opt) {
            case 'a':
                options->print_all = 1;
                break;

            case 'c':
                options->class = strdup(optarg);
                break;

            case 'e':
                options->only_existing = 1;
                break;

            case 'm':
                options->do_mkdir = 1;
                break;

            default:
                pathcomp_log_error("invalid usage");
                exit(EXIT_FAILURE);
        }
    }
    if (!options->class) {
        pathcomp_log_error("class name (argument -c) is mandatory");
        exit(EXIT_FAILURE);
    }
    while (optind < argc) {
        kv_t *kv;
        assert(kv = kv_new(argv[optind++]));
        options->attributes = list_push(options->attributes, kv);
    }
    return options;
}

static void
opt_free(opt_t *options)
{
    if (!options) return;
    free(options->class);
    list_foreach(options->attributes, (list_traversal_t *) kv_free, NULL);
    list_free(options->attributes);
    free(options);
}

int
main(int argc, char **argv)
{
    opt_t *options;
    char *path;
    pathcomp_t *composer;

    options = opt_new(argc, argv);
    pathcomp_add_config_from_file(".pathcomprc");
    assert(composer = pathcomp_new(options->class));
    list_foreach(options->attributes, (list_traversal_t *) kv_add_to_composer, composer);
    for (;;) {
        if (pathcomp_done(composer)) break;
        if (options->only_existing) {
            path = pathcomp_find(composer);
            /* pathcomp_find() automatically advances to the next alternative
             * when called repeatedly */
        }
        else {
            path = pathcomp_yield(composer);
            if (options->do_mkdir) pathcomp_mkdir(composer);
            pathcomp_next(composer);
        }
        if (path) {
            puts(path);
            free(path);
        }
    }
    pathcomp_free(composer);
    pathcomp_cleanup();
    opt_free(options);
    return EXIT_SUCCESS;
}
