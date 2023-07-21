# llvm-passes

Collection of my LLVM Passes.

## How to build all the Passes

```
cd path/to/llvm-passes
cmake -B ./build
cmake --build ./build
```

Pass list:

- [ConstFold](./ConstFold)
- [FenceBeforeJC](./FenceBeforeJC)
- [X86LFenceBeforeJC](./X86LFenceBeforeJC)

## Misc

How to build llvm-project:

```
cd path/to/llvm-project
git checkout llvmorg-15.0.6
mkdir build
cmake -G "Ninja" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DBUILD_SHARED_LIBS=ON \
    -DLLVM_USE_SPLIT_DWARF=ON \
    -DLLVM_OPTIMIZED_TABLEGEN=ON \
    -DLLVM_ENABLE_PROJECTS="clang;compiler-rt;lld" \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
    ../llvm
cmake --build build
```

Tested on:

| Name         | Version                                                         |
| ------------ | --------------------------------------------------------------- |
| llvm-project | `088f33605d8a61ff519c580a71b1dd57d16a03f8` (= `llvmorg-15.0.6`) |
| cmake        | 3.16.3                                                          |
