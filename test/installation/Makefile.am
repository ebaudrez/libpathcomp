EXTRA_DIST =  \
	test_c_app/Makefile.am \
	test_c_app/configure.ac \
	test_c_app/main.c \
	test_c_app/main.conf \
	test_c_app/test_main.sh \
	test_cxx_app/Makefile.am \
	test_cxx_app/configure.ac \
	test_cxx_app/main.cc \
	test_cxx_app/main.conf \
	test_cxx_app/test_main.sh \
	test_util/Makefile.am \
	test_util/configure.ac \
	test_util/main.c \
	test_util/main.sh.in \
	test_util/test_main.sh \
	test_util/your-config-file

## This rule tries and builds small example programs against the installed
## library, to verify that it works well, as well as the other infrastructure
## parts, like the pkg-config file.
installcheck-local:
	for dir in $(srcdir)/test_*; do \
	    echo "running installation check for case $$dir ..."; \
	    abs_dir=`cd $$dir && pwd` || exit 1; \
	    tmp_path=`mktemp -d` || exit 1; \
	    ({ cd "$$tmp_path" \
	        && cp "$$abs_dir"/* . \
	        && (test -e configure || autoreconf -i) \
	        && chmod +x ./configure \
	        && export PATH="$(bindir):$$PATH" \
	        && export LD_LIBRARY_PATH="$(libdir):$$LD_LIBRARY_PATH" \
	        && ./configure PKG_CONFIG_PATH=$(libdir)/pkgconfig \
	        && make \
	        && make check; } \
	      || { test -r test-suite.log && cat test-suite.log; \
	           rm -rf "$$tmp_path"; exit 1; }) \
	    && rm -rf "$$tmp_path" \
	    || exit 1; \
	done

# vim:set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab:
