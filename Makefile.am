ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src test
## I would really like to include prove-recursive in RECURSIVE_TARGETS, so I
## wouldn't need to write the following rule, but somehow I can't get that to
## work
prove-recursive:
	cd src && $(MAKE) $(AM_MAKEFLAGS) prove
	cd test && $(MAKE) $(AM_MAKEFLAGS) prove
prove: prove-recursive
