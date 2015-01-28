AM_CPPFLAGS = $(LIBLUACPPFLAGS)
AM_LDFLAGS = $(LIBLUALDFLAGS)
noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = att.c att.h buf.c buf.h cf.c cf.h interpreter.c interpreter.h \
                     list.c list.h log.c log.h value.c value.h
include_HEADERS = pathcomp.h
lib_LTLIBRARIES = libpathcomp.la
libpathcomp_la_SOURCES = pathcomp.c
libpathcomp_la_LIBADD = libutil.la $(LIBLUA)
libpathcomp_la_LDFLAGS = $(LIBLUALDFLAGS) -version-info 0:0:0
bin_PROGRAMS = pathcomp
pathcomp_SOURCES = standalone.c
pathcomp_LDADD = libpathcomp.la
