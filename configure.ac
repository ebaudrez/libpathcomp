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
AC_CONFIG_SRCDIR([src/pathcomp.h])
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
lua_dir="$srcdir/liblua"
abs_lua_dir=`(cd "$lua_dir"; pwd)`
AC_SUBST([LIBLUACPPFLAGS], ["-I$abs_lua_dir/src"])
AC_SUBST([LIBLUALDFLAGS], ["-L$abs_lua_dir/src"])
AC_SUBST([LIBLUA], ["-llua"])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])
AC_CONFIG_SUBDIRS([liblua])
AC_OUTPUT
