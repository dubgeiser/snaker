#!/usr/bin/env bash

if [ ! -d build ]; then
    mkdir build
fi

# Blind copy-paste from building Raylib's core/core_basic_window example.
gcc -o build/snaker snaker.c -Wall -std=c99 \
    -I. -I/home/per/work/raylib/src -I/home/per/work/raylib/src/external -I/usr/local/include \
    -L. -L/home/per/work/raylib/src -L/usr/local/lib \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic \
    -DPLATFORM_DESKTOP -DPLATFORM_DESKTOP_GLF

exit $?
