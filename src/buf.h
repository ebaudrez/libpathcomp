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
 * This interface is derived from strbuf.h, which is a part of Git
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

#ifndef BUF_INCLUDED
#define BUF_INCLUDED

#include <stddef.h>
#include <stdio.h>

typedef struct buf_t {
    char   *buf;
    size_t  alloc;
    size_t  len;
    size_t  pos;
} buf_t;

extern void    buf_init(buf_t *, size_t);
extern void    buf_release(buf_t *);
extern char   *buf_detach(buf_t *, size_t *);
extern void    buf_grow(buf_t *, size_t);
extern void    buf_setlen(buf_t *, size_t);
extern size_t  buf_avail(buf_t *);
extern void    buf_addch(buf_t *, int);
extern void    buf_add(buf_t *, const void *, size_t);
extern void    buf_addstr(buf_t *, const char *);
extern void    buf_addf(buf_t *, const char *, ...);
extern size_t  buf_fread(buf_t *, size_t, FILE *);
extern int     buf_read(buf_t *, int, size_t);
extern int     buf_read_file(buf_t *, const char *, size_t);
extern void    buf_ltrim(buf_t *);
extern void    buf_rtrim(buf_t *);
extern void    buf_splice(buf_t *, int, size_t, const void *, size_t);
extern void    buf_splicestr(buf_t *, int, size_t, const char *);
extern int     buf_fgetc(buf_t *);
extern int     buf_ungetc(buf_t *, int);
extern size_t  buf_ftell(buf_t *);
extern void    buf_rewind(buf_t *);

#endif /* BUF_INCLUDED */
