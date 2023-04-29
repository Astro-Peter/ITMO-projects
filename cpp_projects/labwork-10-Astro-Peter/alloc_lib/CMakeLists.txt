add_library(alloc TreeAllocator.h Pool.h MapAlloc.h Node.h)

set_target_properties(alloc PROPERTIES LINKER_LANGUAGE CXX)