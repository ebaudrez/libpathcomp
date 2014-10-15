#!/bin/bash

files="$*"
test -z "$files" && files=.libs/test_*
> valgrind.log
for f in $files
do
    echo -n "--$f"
    if ! test -x "$f"; then
        echo " (not executable)"
        continue
    else
        echo
    fi
    output=`LD_LIBRARY_PATH=../src/.libs/ valgrind --leak-check=full --show-reachable=yes "$f" 2>&1 | tee -a valgrind.log | grep 'no leaks are possible'`
    if test "$output"; then
        echo "$output"
    else
        echo -e '    ** \e[0;31;1mWarning!\e[0m    No output!'
    fi
done
