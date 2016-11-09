#!/bin/bash
cat > output.expected << EOF
found: Makefile.am
found: configure.ac
found: main.c
EOF
sort < output.expected > tmp && mv tmp output.expected
./main | sort > output.got
diff output.got output.expected
