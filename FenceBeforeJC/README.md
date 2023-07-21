# FenceBeforeJC

How to use:

1. Build (Ref. [here](../README.md)), or:
   ```
   cmake --build ../build
   ```
1. Load this Pass using `clang`:
   ```
   clang -fpass-plugin=../build/FenceBeforeJC/FenceBeforeJCPass.so -S -o ./test/test.s ./test/test.c
   ```
