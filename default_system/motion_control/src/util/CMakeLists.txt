set(UTIL_SOURCES logger.cpp sephi_ipc.cpp equation_solver.cpp)
add_library(sep_util ${UTIL_SOURCES})

#add_library(sep_util32 ${UTIL_SOURCES})
#target_compile_options(sep_util32 PRIVATE "-m32")
