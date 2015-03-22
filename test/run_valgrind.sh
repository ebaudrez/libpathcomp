#!/bin/bash
#
# Copyright (C) 2015 Edward Baudrez <edward.baudrez@gmail.com>
# This file is part of Libpathcomp.
#
# Libpathcomp is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Libpathcomp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with Libpathcomp; if not, see <http://www.gnu.org/licenses/>.
#
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
