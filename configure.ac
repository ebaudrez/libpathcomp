#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libpathcomp], [0.1], [edward.baudrez@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AM_SILENT_RULES
AM_EXTRA_RECURSIVE_TARGETS([prove])
LT_PREREQ([2.2])
LT_INIT
AC_CONFIG_SRCDIR([include/pathcomp.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

#
AC_PATH_PROG([PROVE], [prove])

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([regcomp])

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
                 libpathcomp.pc])
AC_OUTPUT
