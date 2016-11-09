AC_INIT([test_util], [0.1], [nobody@example.org])
AC_CONFIG_SRCDIR([main.c])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PATH_PROG([PATHCOMP_BIN], [pathcomp])
PKG_CHECK_MODULES([PATHCOMP], [libpathcomp])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([main.sh], [chmod +x main.sh])
AC_OUTPUT
