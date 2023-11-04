add_llvm_library(LLVMConstFoldPass MODULE
  ConstFold.cpp
)
# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(LLVMConstFoldPass
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
