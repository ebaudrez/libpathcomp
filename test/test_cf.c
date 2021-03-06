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

/* test config reading */

#include <config.h>
#include "tap.h"
#include "cf.h"
#include <string.h>

static void
test1(void)
{
    cf_t         *cf;
    cf_section_t *section;
    cf_kv_t      *kv;
    const char   *text = "\
[my.class]\n\
    key = value";

    note("simple test - one entry");
    cf = cf_new();
    ok(cf, "cf object returned");
    ok(cf_add_from_string(cf, text), "parses ok");
    ok(cf->sections, "sections are present");
    ok(!cf->sections->next, "only one section");
    section = cf->sections->el;
    is(section->name, "my.class");
    ok(section->entries, "entries are present");
    ok(!section->entries->next, "only one entry");
    ok(kv = section->entries->el);
    ok(kv->key);
    is(kv->key, "key");
    ok(kv->value);
    is(kv->value, "value");
    cf_free(cf);
}

static void
test2(void)
{
    cf_t         *cf;
    cf_section_t *sec;
    cf_kv_t      *kv;
    const char   *text = "\n\
stray_entry = some_stuff\n\
[test.archive]\n\
    instrument = G2\n\
    imager     = SEV1\n\
";

    note("simple test - stray entry + two entries in section");
    cf = cf_new();
    ok(cf);
    ok(cf_add_from_string(cf, text), "parses ok");
    ok(cf->sections);
    ok(!cf->sections->next);
    ok(sec = cf->sections->el);
    is(sec->name, "test.archive");
    ok(sec->entries);
    ok(kv = sec->entries->el);
    ok(kv->key);
    is(kv->key, "instrument");
    ok(kv->value);
    is(kv->value, "G2");
    ok(sec->entries->next);
    kv = sec->entries->next->el;
    is(kv->key, "imager");
    is(kv->value, "SEV1");
    cf_free(cf);
}

static void
test3(void)
{
    cf_t         *cf;
    cf_section_t *sec;
    cf_kv_t      *kv;
    const char   *text = "\
[test.backslash]\n\
    my_entry = some long\\\n\
               entry that was\\\n\
               split over several lines\n\
    string = abc\\def\\ghi\n\
";

    note("testing backslash continuation");
    cf = cf_new();
    ok(cf);
    ok(cf_add_from_string(cf, text), "parses ok");
    ok(cf->sections);
    ok(!cf->sections->next, "only one section");
    ok(sec = cf->sections->el);
    is(sec->name, "test.backslash");
    ok(kv = sec->entries->el);
    is(kv->key, "my_entry");
    is(kv->value, "some long               entry that was               split over several lines", "backslash continuation");
    ok(sec->entries->next);
    kv = sec->entries->next->el;
    is(kv->key, "string");
    is(kv->value, "abc\\def\\ghi", "no escape sequences except newline");
    cf_free(cf);
}

static void
test4(void)
{
    cf_t         *cf;
    list_t       *psec, *pkv;
    cf_section_t *sec;
    cf_kv_t      *kv;
    const char   *text = "\
\n\
and another stray entry = goes there\n\
\n\
\n\
[test.class1]\n\
    key1 = some value\n\
    key2 = whatever\n\
\n\
[test.other]\n\
    key1            = some other value\n\
    test            = yes\n\
    long_entry_name = 1\n\
\n\
; just a comment\n\
[MYSECTION   ]\n\
whatever = goes up\n\
must come= down,\\\n\
           albeit in a slightly\\\n\
           different form maybe\n\
      ; just another silly comment over here ...\n\
      ; and another one ...\n\
do we allow non-identifier characters?           =  definitely!\n\
     and this?=as \\well...             \n\
         \n\
\n\
";

    note("more testing - 3 sections & some odd formatting");
    ok(cf = cf_new());
    ok(cf_add_from_string(cf, text), "parses ok");
    ok(psec = cf->sections);
    ok(sec  = psec->el);
    is(sec->name, "test.class1");
    ok(pkv = sec->entries);
    ok(kv  = pkv->el);
    is(kv->key, "key1");
    is(kv->value, "some value");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "key2");
    is(kv->value, "whatever");
    ok(!pkv->next);
    ok(psec = psec->next);
    ok(sec  = psec->el);
    is(sec->name, "test.other");
    ok(pkv = sec->entries);
    ok(kv  = pkv->el);
    is(kv->key, "key1");
    is(kv->value, "some other value");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "test");
    is(kv->value, "yes");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "long_entry_name");
    is(kv->value, "1");
    ok(!pkv->next);
    ok(psec = psec->next);
    ok(sec  = psec->el);
    is(sec->name, "MYSECTION   ");
    ok(pkv = sec->entries);
    ok(kv  = pkv->el);
    is(kv->key, "whatever");
    is(kv->value, "goes up");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "must come");
    is(kv->value, "down,           albeit in a slightly           different form maybe");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "do we allow non-identifier characters?");
    is(kv->value, "definitely!");
    ok(pkv = pkv->next);
    ok(kv  = pkv->el);
    is(kv->key, "and this?");
    is(kv->value, "as \\well...");
    ok(!pkv->next);
    ok(!psec->next);
    cf_free(cf);
}

int
main(void)
{
    plan(NO_PLAN);
    test1();
    test2();
    test3();
    test4();
    done_testing();
}
