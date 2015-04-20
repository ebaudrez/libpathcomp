AM_CPPFLAGS = -I$(top_srcdir)/include $(LIBLUACPPFLAGS)
AM_LDFLAGS = $(LIBLUALDFLAGS)
noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = att.c att.h buf.c buf.h cf.c cf.h interpreter.c interpreter.h \
                     list.c list.h value.c value.h
lib_LTLIBRARIES = libpathcomp.la
libpathcomp_la_SOURCES = pathcomp.c log.c
libpathcomp_la_LIBADD = libutil.la $(LIBLUA)
libpathcomp_la_LDFLAGS = $(LIBLUALDFLAGS) -version-info 1:0:0 -export-symbols $(srcdir)/export.sym
bin_PROGRAMS = pathcomp
pathcomp_SOURCES = standalone.c
pathcomp_LDADD = libpathcomp.la libutil.la
EXTRA_DIST = export.sym
