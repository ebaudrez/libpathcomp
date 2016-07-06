#!/bin/bash
#
# Copyright (C) 2015, 2016 Edward Baudrez <edward.baudrez@gmail.com>
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

# by default, test all executables with a name matching between the current
# directory and Libtool's .libs directory (unless the user passes in test
# executables as arguments)
unit_tests="$*"
if test -z "$unit_tests"; then
    for candidate in .libs/*; do
        candidate=${candidate#.libs/}
        test -x "$candidate" && unit_tests="$unit_tests $candidate"
    done
fi
echo "checking for memory leaks: $unit_tests"

has_leaks=
for unit_test in $unit_tests; do
    log_file="valgrind_${unit_test}.log"
    libtool --mode=execute valgrind --leak-check=full --show-reachable=yes \
        --log-file="$log_file" "$unit_test" > /dev/null 2>&1 || exit
    num_leaks=`grep 'are definitely lost' "$log_file" | wc -l`
    if test $num_leaks -gt 0; then
        echo "$unit_test: $num_leaks (potential) memory leak(s); check $log_file"
        has_leaks=yes
    else
        echo "$unit_test: no memory leaks"
    fi
done

if test -n "$has_leaks"; then
    exit 1
fi
