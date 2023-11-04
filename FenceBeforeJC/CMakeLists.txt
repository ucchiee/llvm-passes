add_llvm_library(FenceBeforeJCPass MODULE
  ./FenceBeforeJC.cpp
)
# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(FenceBeforeJCPass
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
