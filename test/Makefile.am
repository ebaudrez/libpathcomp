check_PROGRAMS = test_buf test_cf test_list test_value test_literal test_lua \
                 test_yield test_set test_find test_mkdir test_empty test_file
TESTS = $(check_PROGRAMS) test_standalone.pl
AM_CPPFLAGS = -I$(top_srcdir)/src $(LIBLUACPPFLAGS)
add_libraries = ../src/libpathcomp.la tap.o taputil.o
test_buf_LDADD = $(add_libraries)
test_cf_LDADD = $(add_libraries)
test_list_LDADD = $(add_libraries)
test_value_LDADD = $(add_libraries)
test_literal_LDADD = $(add_libraries)
test_lua_LDADD = $(add_libraries)
test_yield_LDADD = $(add_libraries)
test_set_LDADD = $(add_libraries)
test_find_LDADD = $(add_libraries)
test_mkdir_LDADD = $(add_libraries)
test_empty_LDADD = $(add_libraries)
test_file_LDADD = $(add_libraries)
## prove cannot just depend on check, because that rule will run the tests!
## therefore, we have to imitate it
prove-TESTS: $(TESTS)
	@srcdir=$(srcdir); export srcdir; \
	test "$(PROVE)" && PATH=".:$$PATH" $(PROVE) -e '' $(TESTS)
prove-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS) prove-TESTS
prove: prove-am
