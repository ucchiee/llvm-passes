# X86LFenceBeforeJC

How to use:

1. Build (Ref. [here](../README.md)), or:
   ````
   cmake --build ../build
   ```
1. Load this Pass using `clang`:
   ```
   clang -fpass-plugin=../build/X86LFenceBeforeJC/X86LFenceBeforeJCPass.so -S -o ./test/test.s ./test/test.c
   ````
