#!/bin/sh
set -eu

mkdir -p build

if gcc -std=c89 -Wall -Wextra -pedantic main.c \
    -o build/bua_step121 2> build/compiler_warnings.txt
then
    if test ! -s build/compiler_warnings.txt
    then
        printf '%s\n' 'No compiler warnings.' > build/compiler_warnings.txt
    fi
else
    printf '%s\n' 'Build failed. See build/compiler_warnings.txt.' >&2
    exit 1
fi

./build/bua_step121 > build/bua_step121_output.txt
printf '%s\n' 'Build and run completed.'
cat build/compiler_warnings.txt
