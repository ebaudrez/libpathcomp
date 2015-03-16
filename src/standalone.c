#include <config.h>
#include "pathcomp.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "pathcomp/log.h"

static void
print_usage(void)
{
    puts("Usage\n"
         "    pathcomp -c class [ -f config -aehm -x att ] key=value key=value key+=value ...\n"
         "\n"
         "Mandatory command-line arguments\n"
         "    -c class: use this locator class\n"
         "\n"
         "Options\n"
         "    -a: print all pathnames (default: print first)\n"
         "    -e: print only existing pathnames (default: print any pathname)\n"
         "    -f config: use config file 'config' (default: .pathcomprc)\n"
         "    -h: display this information\n"
         "    -m: create parent directory recursively\n"
         "    -x att: evaluate and print attribute 'att' instead of pathname\n"
         "\n"
         "Attributes\n"
         "    key=value: set attribute 'key' to 'value'\n"
         "    key+=value: add value 'value' to attribute 'key'\n");
}

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
    char *config_file;
    int do_mkdir;
    char *eval_att;
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
    options->config_file = strdup(".pathcomprc");
    options->do_mkdir = 0;
    options->eval_att = NULL;
    opterr = 0; /* prevent getopt() from printing error messages */
    while ((opt = getopt(argc, argv, ":ac:ef:hmx:")) != -1) {
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

            case 'f':
                free(options->config_file);
                options->config_file = strdup(optarg);
                break;

            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
                break;

            case 'm':
                options->do_mkdir = 1;
                break;

            case 'x':
                options->eval_att = strdup(optarg);
                break;

            case '?':
                pathcomp_log_error("invalid option '%c'", optopt);
                print_usage();
                exit(EXIT_FAILURE);

            case ':':
                pathcomp_log_error("missing argument for option '%c'", optopt);
                print_usage();
                exit(EXIT_FAILURE);

            default:
                assert(0);
        }
    }
    if (!options->class) {
        pathcomp_log_error("class name (argument to option '-c') is mandatory");
        print_usage();
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
    free(options->config_file);
    free(options->eval_att);
    free(options);
}

int
main(int argc, char **argv)
{
    opt_t *options;
    char *path, *att;
    pathcomp_t *composer;

    options = opt_new(argc, argv);
    pathcomp_add_config_from_file(options->config_file);
    assert(composer = pathcomp_new(options->class));
    list_foreach(options->attributes, (list_traversal_t *) kv_add_to_composer, composer);
    for (;;) {
        if (pathcomp_done(composer)) break;
        if (options->only_existing) path = pathcomp_find(composer);
        else path = pathcomp_yield(composer);
        if (path) {
            if (options->do_mkdir) pathcomp_mkdir(composer);
            if (options->eval_att) {
                if ((att = pathcomp_eval(composer, options->eval_att))) {
                    puts(att);
                    free(att);
                }
            }
            else puts(path);
            free(path);
        }
        if (!options->print_all) break;
        /* pathcomp_find() automatically advances to the next alternative when
         * called repeatedly */
        if (!options->only_existing) pathcomp_next(composer);
    }
    pathcomp_free(composer);
    pathcomp_cleanup();
    opt_free(options);
    return EXIT_SUCCESS;
}
