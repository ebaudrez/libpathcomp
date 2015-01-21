/**
 * \file
 * \brief String buffers
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
