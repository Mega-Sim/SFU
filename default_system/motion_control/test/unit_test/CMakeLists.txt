find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} )

add_executable(runTests gtest_main.cpp control/test_velocity.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} ${sep_control_libs})