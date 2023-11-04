add_llvm_library(X86LFenceBeforeJCPass MODULE
  ./X86LFenceBeforeJC.cpp
)
# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(X86LFenceBeforeJCPass
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
