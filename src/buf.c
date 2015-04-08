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
 *
 * This implementation is derived from strbuf.c, which is a part of Git
 * (http://git-scm.com). That code is covered by the following copyright and
 * permission notice:
 *
 *      Copyright (C) 2005-2015 Git contributors (Linus Torvalds, Junio Hamano,
 *      and many others, see https://github.com/git/git/contributors).
 *
 *      Git is free software; you can redistribute it and/or modify it under
 *      the terms of the GNU General Public License, version 2, as published by
 *      the Free Software Foundation.
 *
 *      Git is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *      GNU General Public License for more details.
 */

#include <config.h>
#include "buf.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

static char buf_initial[1] = { '\0' };

#if 0
buf_t *
buf_new(size_t hint)
{
    buf_t *buf = NULL;
    buf = malloc(sizeof *buf);
    if (!buf) return buf;
    buf_init(buf, hint);
    return buf;
}

void
buf_free(buf_t *buf)
{
    if (!buf) return;
    buf_release(buf);
    free(buf);
}
#endif

void
buf_init(buf_t *buf, size_t hint)
{
    assert(buf);
    buf->buf   = buf_initial;
    buf->alloc = 0;
    buf->len   = 0;
    buf->pos   = 0;
    if (hint) buf_grow(buf, hint);
}

void
buf_release(buf_t *buf)
{
    assert(buf);
    if (buf->alloc) {
        free(buf->buf);
        buf_init(buf, 0);
    }
}

/*
 * \param len Pointer to store length of buffer (terminating null not included)
 */
char *
buf_detach(buf_t *buf, size_t *len)
{
    assert(buf);
    char *s;
    buf_grow(buf, 0);   /* resize to actual length; buf->buf may be reallocated */
    s = buf->buf;
    if (len) *len = buf->len;
    buf_init(buf, 0);
    return s;
}

/*
 * \param n Number of elements currently allocated
 */
#define GROW_N(n) (((n) + 16)*3/2)

/*
 * \param p    Pointer to allocated variable
 * \param want Number of elements desired
 * \param cur  Number of elements currently allocated (must be an lvalue)
 */
#define GROW(p, want, cur) \
    do { \
        if ((want) > (cur)) { \
            if (GROW_N(cur) < (want)) (cur) = (want); \
            else                      (cur) = GROW_N(cur); \
            (p) = realloc((p), (cur) * sizeof(*(p))); \
        } \
    } while (0)

void
buf_grow(buf_t *buf, size_t extra)
{
    assert(buf);
    int new_buf = !buf->alloc;
    if (new_buf) buf->buf = NULL;
    GROW(buf->buf, buf->len + extra + 1, buf->alloc);
    if (new_buf) buf->buf[0] = '\0';
}

void
buf_setlen(buf_t *buf, size_t len)
{
    assert(buf);
    assert(buf->alloc ? len < buf->alloc : !len);
    buf->len = len;
    buf->buf[len] = '\0';
    if (buf->pos > buf->len) buf->pos = buf->len;
}

size_t
buf_avail(buf_t *buf)
{
    assert(buf);
    return buf->alloc ? buf->alloc - buf->len - 1 : 0;
}

void
buf_addch(buf_t *buf, int c)
{
    assert(buf);
    buf_grow(buf, 1);
    buf->buf[buf->len++] = c;
    buf->buf[buf->len] = '\0';
}

void
buf_add(buf_t *buf, const void *data, size_t len)
{
    assert(buf);
    if (!data) return;
    buf_grow(buf, len);
    memcpy(buf->buf + buf->len, data, len);
    buf_setlen(buf, buf->len + len);
}

void
buf_addstr(buf_t *buf, const char *s)
{
    buf_add(buf, s, strlen(s));
}

static void
buf_vaddf(buf_t *buf, const char *fmt, va_list ap)
{
    int n;
    va_list copy;
    if (!buf_avail(buf)) buf_grow(buf, 64); /* magic number taken from Git sources */
    va_copy(copy, ap);
    n = vsnprintf(buf->buf + buf->len, buf->alloc - buf->len, fmt, copy);
    va_end(copy);
    assert(n >= 0);
    if (n > buf_avail(buf)) {
        buf_grow(buf, n);
        n = vsnprintf(buf->buf + buf->len, buf->alloc - buf->len, fmt, ap);
        assert(n <= buf_avail(buf));
    }
    buf_setlen(buf, buf->len + n);
}

void
buf_addf(buf_t *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    buf_vaddf(buf, fmt, ap);
    va_end(ap);
}

size_t
buf_fread(buf_t *buf, size_t size, FILE *f)
{
    size_t n;
    size_t oldalloc = buf->alloc;
    assert(buf);
    buf_grow(buf, size);
    n = fread(buf->buf + buf->len, 1, size, f);
    if (n > 0) {
        buf_setlen(buf, buf->len + n);
    }
    else if (!oldalloc) {
        buf_release(buf);
    }
    return n;
}

static ssize_t
try_read(int fd, void *buf, size_t count)
{
    ssize_t n;
    for (;;) {
        n = read(fd, buf, count);
        if ((n < 0) && (errno == EAGAIN || errno == EINTR)) continue;
        return n;
    }
}

int
buf_read(buf_t *buf, int fd, size_t hint)
{
    size_t oldalloc = buf->alloc;
    size_t oldlen   = buf->len;
    assert(buf);
    buf_grow(buf, hint ? hint : 8192);
    for (;;) {
        ssize_t n;
        n = try_read(fd, buf->buf + buf->len, buf_avail(buf));
        if (!n) break;
        if (n == -1) {
            /* must restore to state before call to buf_read() */
            if (oldalloc) buf_setlen(buf, oldlen);
            else          buf_release(buf);
            return -1;
        }
        buf->len += n;
        buf_grow(buf, 8192);
    }
    buf->buf[buf->len] = '\0';
    return buf->len - oldlen;
}

/*
 * \bug close(2) may overwrite \a errno if it was previously set by read(2).
 */
int
buf_read_file(buf_t *buf, const char *path, size_t hint)
{
    int fd, rc;
    ssize_t len;
    assert(buf);
    assert(path);
    fd = open(path, O_RDONLY);
    if (fd == -1) return -1;
    len = buf_read(buf, fd, hint);
    rc = close(fd);
    if (rc == -1) return -1;
    return len;
}

void
buf_ltrim(buf_t *buf)
{
    char *b, *end;
    assert(buf);
    b = buf->buf;
    end = buf->buf + buf->len;
    while (b < end && isspace(*b)) ++b;
    if (buf->pos < b - buf->buf) buf->pos = 0;
    else buf->pos -= b - buf->buf;
    memmove(buf->buf, b, end - b);
    buf_setlen(buf, end - b);
}

void
buf_rtrim(buf_t *buf)
{
    size_t len;
    assert(buf);
    len = buf->len;
    while (len > 0 && isspace(buf->buf[len-1])) --len;
    buf_setlen(buf, len);
}

/*
 * \note The value of buf->pos is unspecified after calling this function!
 */
void
buf_splice(buf_t *buf, int off, size_t len, const void *data, size_t data_len)
{
    int delta = data_len - len;
    assert(buf);
    if (off < 0) off = buf->len + off;
    assert(off >= 0 && off <= buf->len);
    assert(off + len <= buf->len);
    assert(!data_len || data);
    if (delta > 0) buf_grow(buf, delta);
    if (delta) memmove(buf->buf + off + data_len, buf->buf + off + len, buf->len - off - len);
    if (data_len) memcpy(buf->buf + off, data, data_len);
    buf_setlen(buf, buf->len + delta);
}

/*
 * \note The value of buf->pos is unspecified after calling this function!
 */
void
buf_splicestr(buf_t *buf, int off, size_t len, const char *s)
{
    buf_splice(buf, off, len, s, s ? strlen(s) : 0);
}

int
buf_fgetc(buf_t *buf)
{
    assert(buf);
    if (buf->pos < buf->len) return buf->buf[buf->pos++];
    return EOF;
}

int
buf_ungetc(buf_t *buf, int c)
{
    assert(buf);
    if (buf->pos > 0) return buf->buf[--buf->pos] = c;
    return EOF;
}

size_t
buf_ftell(buf_t *buf)
{
    assert(buf);
    return buf->pos;
}

void
buf_rewind(buf_t *buf)
{
    assert(buf);
    buf->pos = 0;
}
