check_PROGRAMS = buf cf list literal
TESTS = $(check_PROGRAMS)
AM_CPPFLAGS = -I$(top_srcdir)/src
add_libraries = ../src/libpathcomp.la ../src/libutil.la tap.o
buf_LDADD = $(add_libraries)
cf_LDADD = $(add_libraries)
list_LDADD = $(add_libraries)
literal_LDADD = $(add_libraries)

prove:
	test "$(PROVE)" && PATH=".:$$PATH" $(PROVE) -e '' $(TESTS)
