
include_directories(ecmaster control /usr/include/kdl /usr/local/include/eigen3 /usr/include/eigen3)
link_directories(/usr/local/lib)
add_subdirectory(util)
add_subdirectory(ecmaster)
add_subdirectory(control)
add_subdirectory(app)
set(sep_libs sep_util dl PocoUtil PocoNet  PocoXML PocoFoundation pthread rt )