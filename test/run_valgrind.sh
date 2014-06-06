#!/bin/sh
files="$*"
test -z "$files" && files=.libs/test_*
> valgrind.log
for f in $files
do
    echo "--$f"
    LD_LIBRARY_PATH=../src/.libs/ valgrind --leak-check=full --show-reachable=yes $f 2>&1 | tee -a valgrind.log | grep 'no leaks are possible'
done
