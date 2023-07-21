# ConstFold

How to use:

1. Build (Ref. [here](../README.md)), or:
   ```
   cmake --build ../build
   ```
1. Load this Pass using `clang`:
   ```
   clang -O0 -S -emit-llvm -o ./test/test.ll ./test/test.c
   opt -load-pass-plugin ../build/ConstFold/LLVMConstFoldPass.so -passes="const-fold" -S -o ./test/mod.ll ./test/test.ll
   ````
