check_PROGRAMS = buf cf list literal
TESTS = $(check_PROGRAMS)
AM_CPPFLAGS = -I$(top_srcdir)/src
add_libraries = ../src/libpathcomp.la ../src/libutil.la tap.o
buf_LDADD = $(add_libraries)
cf_LDADD = $(add_libraries)
list_LDADD = $(add_libraries)
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
