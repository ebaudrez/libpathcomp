#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libpathcomp], [0.2], [edward.baudrez@gmail.com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE
AM_SILENT_RULES
LT_PREREQ([2.2])
LT_INIT
AC_CONFIG_SRCDIR([include/pathcomp.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([config])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdlib.h string.h sys/param.h unistd.h])
AC_CHECK_HEADERS([glob.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove mkdir regcomp rmdir strchr strdup strerror strrchr])
AC_CHECK_FUNCS([glob globfree])

# Lua support: use Lua 5.1.4 shipped with this distribution
# must add -I$(top_builddir)/liblua/src because luaconf.h is generated there!
AC_SUBST([LIBLUACPPFLAGS], ['-I$(top_builddir)/liblua/src -I$(top_srcdir)/liblua/src'])
AC_SUBST([LIBLUALDFLAGS], [])
AC_SUBST([LIBLUA], ['$(top_builddir)/liblua/src/liblua.la'])

AC_CONFIG_SUBDIRS([liblua])
AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile
                 test/Makefile
                 libpathcomp.pc
                 Doxyfile])
AC_OUTPUT
