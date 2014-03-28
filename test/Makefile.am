check_PROGRAMS = util_buf util_cf util_list literal
TESTS = $(check_PROGRAMS)
AM_CPPFLAGS = -I$(top_srcdir)/src
add_libraries = ../src/libpathcomp.la ../src/libutil.la tap.o
util_buf_LDADD = $(add_libraries)
util_cf_LDADD = $(add_libraries)
util_list_LDADD = $(add_libraries)
literal_LDADD = $(add_libraries)
## prove cannot just depend on check, because that rule will run the tests!
## therefore, we have to imitate it
prove-TESTS: $(TESTS)
	@srcdir=$(srcdir); export srcdir; \
	test "$(PROVE)" && PATH=".:$$PATH" $(PROVE) -e '' $(TESTS)
prove-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS) prove-TESTS
prove: prove-am
