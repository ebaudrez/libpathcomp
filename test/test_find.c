/* test pathcomp_find() */

#include <config.h>
#include "tap.h"
#include "pathcomp.h"
#include "taputil.h"
#include "list.h"
#include <assert.h>

const char *config = "\
[test.find.0]\n\
    ; no root\n\
    compose = lua { return self.dir .. '/' .. self.file }\n\
\n\
[test.find.1]\n\
    ; one root\n\
    root    = lib/basic/storage\n\
    compose = lua { return self.dir .. '/' .. self.file }\n\
\n\
[test.find.2]\n\
    ; two roots\n\
    root    = lib/basic/cache\n\
    root    = lib/basic/storage\n\
    compose = lua { return self.dir .. '/' .. self.file }\n\
\n\
[test.find.42]\n\
    ;\n\
    root      = lib/basic/cache\n\
    root      = lib/basic/storage\n\
    root      = lib/basic/ftp\n\
    root      = lib/basic/remote\n\
    extension = .hdf\n\
    extension = .hdf.gz\n\
    compose   = lua { return self.dir .. '/' .. self.file .. self.extension }\n\
\n\
[test.find.empty]\n\
";

static void
test_find()
{
    pathcomp_t *c = NULL;
    char *s;
    list_t *got = NULL, *expected = NULL;

    ok(c = pathcomp_new("test.find.0"));
    pathcomp_set(c, "dir", "G1");
    pathcomp_set(c, "file", "abc");
    is(s = pathcomp_yield(c), "G1/abc");
    path_not_exists_ok(s);
    free(s);
    is(s = pathcomp_find(c), NULL);
    is(s = pathcomp_find(c), NULL, "second find() on nonexisting file doesn't crash");
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.1"));
    pathcomp_set(c, "dir", "G1");
    pathcomp_set(c, "file", "abc");
    is(s = pathcomp_yield(c), "lib/basic/storage/G1/abc");
    path_exists_ok(s);
    free(s);
    is(s = pathcomp_find(c), "lib/basic/storage/G1/abc");
    path_exists_ok(s);
    free(s);
    is(s = pathcomp_yield(c), "lib/basic/storage/G1/abc");
    free(s);
    is(s = pathcomp_find(c), NULL);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.1"));
    pathcomp_set(c, "dir", "G1");
    pathcomp_set(c, "file", "def");
    is(s = pathcomp_yield(c), "lib/basic/storage/G1/def");
    path_not_exists_ok(s);
    free(s);
    is(s = pathcomp_find(c), NULL);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.2"));
    pathcomp_set(c, "dir", "G1");
    pathcomp_set(c, "file", "abc");
    is(s = pathcomp_yield(c), "lib/basic/cache/G1/abc");
    path_exists_ok(s);
    free(s);
    while ((s = pathcomp_find(c))) {
        char *tmp;
        path_exists_ok(s);
        got = list_push(got, s);
        tmp = pathcomp_yield(c);
        is(tmp, s, "yield() after find() yields same result");
        free(tmp);
    }
    expected = list_from("lib/basic/cache/G1/abc",
        "lib/basic/storage/G1/abc",
        NULL);
    cmp_bag(got, expected);
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    got = NULL;
    list_free(expected);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.2"));
    pathcomp_set(c, "dir", "G2");
    pathcomp_set(c, "file", "def");
    is(s = pathcomp_yield(c), "lib/basic/cache/G2/def");
    path_not_exists_ok(s);
    free(s);
    while ((s = pathcomp_find(c))) {
        char *tmp;
        path_exists_ok(s);
        got = list_push(got, s);
        tmp = pathcomp_yield(c);
        is(tmp, s, "yield() after find() yields same result");
        free(tmp);
    }
    expected = list_from("lib/basic/storage/G2/def", NULL);
    cmp_bag(got, expected);
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    got = NULL;
    list_free(expected);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.2"));
    pathcomp_set(c, "dir", "G3");
    pathcomp_set(c, "file", "ghi");
    is(s = pathcomp_yield(c), "lib/basic/cache/G3/ghi");
    path_not_exists_ok(s);
    free(s);
    is(s = pathcomp_find(c), NULL);
    pathcomp_free(c);

    ok(c = pathcomp_new("test.find.42"));
    pathcomp_set(c, "dir", "G5");
    pathcomp_set(c, "file", "one");
    is(s = pathcomp_yield(c), "lib/basic/cache/G5/one.hdf");
    path_exists_ok(s);
    free(s);
    while ((s = pathcomp_find(c))) {
        char *tmp;
        path_exists_ok(s);
        got = list_push(got, s);
        tmp = pathcomp_yield(c);
        is(tmp, s, "yield() after find() yields same result");
        free(tmp);
    }
    expected = list_from("lib/basic/cache/G5/one.hdf",
        "lib/basic/storage/G5/one.hdf.gz",
        "lib/basic/ftp/G5/one.hdf.gz",
        "lib/basic/remote/G5/one.hdf.gz",
        NULL);
    cmp_bag(got, expected);
    list_foreach(got, (list_traversal_t *) free, NULL);
    list_free(got);
    got = NULL;
    list_free(expected);
    pathcomp_free(c);
}

static void
test_find_empty(void)
{
    pathcomp_t *c;
    c = pathcomp_new("test.find.empty");
    ok(!pathcomp_find(c));
    pathcomp_free(c);
}

int
main(void)
{
    plan(NO_PLAN);
    pathcomp_add_config_from_string(config);
    test_find();
    test_find_empty();
    pathcomp_cleanup();
    done_testing();
}
