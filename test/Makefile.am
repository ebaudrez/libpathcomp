check_PROGRAMS = test_buf test_cf test_list test_value test_literal test_lua \
                 test_yield test_set test_find test_mkdir test_empty \
                 test_file test_glob
TESTS = $(check_PROGRAMS) test_standalone.pl
## need to include src/ because we are also testing internals
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(LIBLUACPPFLAGS) -DSRCDIR='"$(srcdir)"'
AM_LDFLAGS = $(LIBLUALDFLAGS)
## since we are testing internals of the library, we need to add in libutil and
## the Lua library manually
LDADD = routelog.o ../src/libpathcomp.la ../src/libutil.la $(LIBLUA) tap.o taputil.o
EXTRA_DIST = routelog.c tap.c tap.h taputil.c taputil.h
## prove cannot just depend on check, because that rule will run the tests!
## therefore, we have to imitate it
prove-TESTS: $(TESTS)
	@srcdir=$(srcdir); export srcdir; \
	test "$(PROVE)" && PATH=".:$$PATH" $(PROVE) -e '' $(TESTS)
prove-local: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS) prove-TESTS
