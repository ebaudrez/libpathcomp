AM_CPPFLAGS = $(LIBLUACPPFLAGS)
AM_LDFLAGS = $(LIBLUALDFLAGS)
noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = buf.c buf.h cf.c cf.h interpreter.c interpreter.h list.c \
                     list.h log.c log.h value.c value.h
libutil_la_LIBADD = $(LIBLUA)
include_HEADERS = pathcomp.h
lib_LTLIBRARIES = libpathcomp.la
libpathcomp_la_SOURCES = pathcomp.c
libpathcomp_la_LIBADD = libutil.la $(LIBLUA)
prove: check
