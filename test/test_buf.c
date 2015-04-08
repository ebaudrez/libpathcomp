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

/* test buf.c */

#include <config.h>
#include "tap.h"
#include "buf.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

const char *string = "This is a test string.\n\
                      Nothing spectacular here.\n\
                      Expect more.";

char testdata[8192];

static void
test_buf(void)
{
    buf_t buf;
    char *copy, *filename;
    FILE *f;
    int rc;
    note("basic functionality");
    /* initialization & invariants */
    buf_init(&buf, 0);
    ok(buf.buf, "buf member always initialized");
    cmp_ok(buf.len, "==", 0);
    cmp_ok(buf_avail(&buf), "==", 0);
    /* adding data */
    buf_grow(&buf, 1024);
    cmp_ok(buf.len, "==", 0);
    cmp_ok(buf_avail(&buf), ">=", 1024, "buf_grow()");
    buf_addstr(&buf, string);
    cmp_ok(buf.len, "==", strlen(string), "buf_addstr()");
    cmp_ok(buf_avail(&buf), ">=", 1024 - strlen(string));
    is(buf.buf, string, "stored data equals argument (and is null-terminated)");
    cmp_ok(buf.buf[buf.len], "==", '\0');
    /* detach */
    copy = buf_detach(&buf, NULL);
    cmp_ok(strlen(copy), "==", strlen(string));
    is(copy, string, "buf_detach()");
    ok(buf.buf);
    cmp_ok(buf.len, "==", 0);
    cmp_ok(buf_avail(&buf), "==", 0);
    free(copy);
    /* adding data to initially empty buf */
    buf_release(&buf);
    cmp_ok(buf.len, "==", 0);
    cmp_ok(buf_avail(&buf), "==", 0);
    buf_add(&buf, testdata, sizeof testdata);
    cmp_ok(buf.len, "==", sizeof testdata, "buf_add()");
    buf_addch(&buf, 'a');
    cmp_ok(buf.len, "==", 1+sizeof testdata);
    cmp_ok(buf.buf[buf.len-1], "==", 'a', "buf_addch()");
    /* reading */
    filename = strdup(SRCDIR "/data/test_buf");
    ok(filename, "got a filename for data/test_buf");
    buf_release(&buf);
    cmp_ok(buf.len, "==", 0, "buf empty");
    cmp_ok(buf_avail(&buf), "==", 0, "no space in buf");
    f = fopen(filename, "r");
    ok(f, "opened test file %s", filename);
    do {
        if ((rc=buf_fread(&buf, 8192, f)) < 0) break;
    } while (!feof(f));
    fclose(f);
    cmp_ok(rc, "==", 829, "length of buffer");
    cmp_ok(buf.len, "==", 829, "buf_fread()");
    copy = buf_detach(&buf, NULL);
    cmp_ok(buf.len, "==", 0, "buf empty");
    cmp_ok(buf_avail(&buf), "==", 0);
    rc = buf_read_file(&buf, filename, 0);
    cmp_ok(rc, "==", 829, "buf_read_file(): length of file");
    cmp_ok(buf.len, "==", 829, "length of buffer");
    is(copy, buf.buf, "equal to result of buf_fread()");
    free(filename);
    free(copy);
    /* destruction */
    buf_release(&buf);
    cmp_ok(buf.len, "==", 0);
    cmp_ok(buf_avail(&buf), "==", 0);
}

static void
test_trim(void)
{
    const char *s1 = "to be trimmed     ";
    const char *s2 = "     to be trimmed";
    const char *s3 = "  to be trimmed   ";
    const char *s4 = "  ";
    const char *s5 = "";
    int n;
    buf_t buf;

    note("test whitespace trimming");

    n = strlen(s1);
    buf_init(&buf, 0);
    buf_addstr(&buf, s1);
    buf_ltrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n);
    is(buf.buf, "to be trimmed     ");
    buf_rtrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n-5);
    is(buf.buf, "to be trimmed");
    buf_release(&buf);

    n = strlen(s2);
    buf_init(&buf, 0);
    buf_addstr(&buf, s2);
    buf_ltrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n-5);
    is(buf.buf, "to be trimmed");
    buf_rtrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n-5);
    is(buf.buf, "to be trimmed");
    buf_release(&buf);

    n = strlen(s3);
    buf_init(&buf, 0);
    buf_addstr(&buf, s3);
    buf_ltrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n-2);
    is(buf.buf, "to be trimmed   ");
    buf_rtrim(&buf);
    cmp_ok(strlen(buf.buf), "==", n-5);
    is(buf.buf, "to be trimmed");
    buf_release(&buf);

    n = strlen(s4);
    buf_init(&buf, 0);
    buf_addstr(&buf, s4);
    buf_ltrim(&buf);
    cmp_ok(strlen(buf.buf), "==", 0);
    is(buf.buf, "");
    buf_rtrim(&buf);
    cmp_ok(strlen(buf.buf), "==", 0);
    is(buf.buf, "");
    buf_release(&buf);

    n = strlen(s5);
    buf_init(&buf, 0);
    buf_addstr(&buf, s5);
    buf_ltrim(&buf);
    cmp_ok(strlen(buf.buf), "==", 0);
    is(buf.buf, "");
    buf_rtrim(&buf);
    cmp_ok(strlen(buf.buf), "==", 0);
    is(buf.buf, "");
    buf_release(&buf);

    /* test buf.pos */
    n = strlen(s2);
    buf_init(&buf, 0);
    buf_addstr(&buf, s2);
    buf.pos = 2;
    buf_ltrim(&buf);
    cmp_ok(buf.pos, "==", 0);
    buf_release(&buf);

    n = strlen(s2);
    buf_init(&buf, 0);
    buf_addstr(&buf, s2);
    buf.pos = 5;
    buf_ltrim(&buf);
    cmp_ok(buf.pos, "==", 0);
    buf_release(&buf);

    n = strlen(s2);
    buf_init(&buf, 0);
    buf_addstr(&buf, s2);
    buf.pos = 6;
    buf_ltrim(&buf);
    cmp_ok(buf.pos, "==", 1);
    buf_release(&buf);
}

static void
test_stream(void)
{
    buf_t buf;
    int ch;
    note("test stream-like interface");
    buf_init(&buf, 0);
    buf_addstr(&buf, string);
    is(buf.buf, string);
    cmp_ok(buf.alloc, ">=", strlen(string));
    cmp_ok(buf.len, "==", strlen(string));
    cmp_ok(buf.pos, "==", 0);
    cmp_ok(buf_ftell(&buf), "==", 0);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'T');
    cmp_ok(buf_ftell(&buf), "==", 1);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'h');
    cmp_ok(buf_ftell(&buf), "==", 2);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'i');
    cmp_ok(buf_ftell(&buf), "==", 3);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 's');
    cmp_ok(buf_ftell(&buf), "==", 4);
    ch = buf_ungetc(&buf, 'n');
    cmp_ok(ch, "==", 'n');
    cmp_ok(buf_ftell(&buf), "==", 3);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'n');
    cmp_ok(buf_ftell(&buf), "==", 4);
    buf_rewind(&buf);
    cmp_ok(buf_ftell(&buf), "==", 0);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'T');
    cmp_ok(buf_ftell(&buf), "==", 1);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'h');
    cmp_ok(buf_ftell(&buf), "==", 2);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'i');
    cmp_ok(buf_ftell(&buf), "==", 3);
    ch = buf_fgetc(&buf);
    cmp_ok(ch, "==", 'n');
    cmp_ok(buf_ftell(&buf), "==", 4);
    buf_release(&buf);
}

static void
test_splice(void)
{
    const char *s = "abfgZYXhijWVl";
    buf_t buf;
    buf_init(&buf, 0);
    buf_addstr(&buf, s);
    buf_splicestr(&buf, 2, 0, "cde");
    is(buf.buf, "abcdefgZYXhijWVl");
    buf_splicestr(&buf, 7, 3, NULL);
    is(buf.buf, "abcdefghijWVl");
    buf_splicestr(&buf, -3, 2, "k");
    is(buf.buf, "abcdefghijkl");
    buf_splicestr(&buf, buf.len, 0, "mnopqrstuvwxyz");
    is(buf.buf, "abcdefghijklmnopqrstuvwxyz");
    buf_splicestr(&buf, 0, 5, "");
    is(buf.buf, "fghijklmnopqrstuvwxyz");
    buf_splicestr(&buf, 0, 0, "ABCDE");
    is(buf.buf, "ABCDEfghijklmnopqrstuvwxyz");
    buf_splicestr(&buf, -1, 1, NULL);
    is(buf.buf, "ABCDEfghijklmnopqrstuvwxy");
    buf_splicestr(&buf, -1, 1, NULL);
    is(buf.buf, "ABCDEfghijklmnopqrstuvwx");
    buf_splicestr(&buf, 0, 1, NULL);
    is(buf.buf, "BCDEfghijklmnopqrstuvwx");
    buf_splicestr(&buf, -2, 1, NULL);
    is(buf.buf, "BCDEfghijklmnopqrstuvx");
    buf_release(&buf);
}

static void
test_addf(void)
{
    buf_t buf;
    buf_init(&buf, 0);
    buf_addf(&buf, "this is a %s", "test");
    is(buf.buf, "this is a test");
    buf_release(&buf);
    buf_init(&buf, 0);
    buf_addf(&buf, "twelve (%.1f) eggs in a box", 12.);
    is(buf.buf, "twelve (12.0) eggs in a box");
    buf_release(&buf);
    buf_init(&buf, 100);
    buf_addf(&buf, "there are %d letters in the string `%s`", (int) strlen("FOUR"), "FOUR");
    is(buf.buf, "there are 4 letters in the string `FOUR`");
    buf_release(&buf);
}

int
main(void)
{
    plan(NO_PLAN);
    test_buf();
    test_trim();
    test_stream();
    test_splice();
    test_addf();
    done_testing();
}
