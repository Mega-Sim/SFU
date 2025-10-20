cmake_minimum_required(VERSION 3.6)
project(sepmaster)

set(CMAKE_CXX_STANDARD 14)

#set(ECMASTER etherlab) # possible value is acontis, etherlab
set(ECMASTER acontis)
#set(BOARD_TYPE MASTER_ARM_BOARD) # pissible value is MASTER_ARM_BOARD, MASTER_X64_BOARD, MASTER_PC
set(BOARD_TYPE MASTER_PC)
add_definitions(-D${BOARD_TYPE})

set(AMC_CLIENT_MODE 0) #0 : stand-alone(demo), 1: used for mark updater
add_definitions(-DAMC_CLIENT_MODE=${AMC_CLIENT_MODE})

set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIB_ROOT "${PROJECT_ROOT}/lib")
set(BUILD_ROOT "${PROJECT_ROOT}/build")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_ROOT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_ROOT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_ROOT})

link_directories(lib/x64)
include_directories(include)

set(sep_libs sep_util PocoUtil PocoNet  PocoXML PocoFoundation pthread rt dl yaml-cpp)
set(grpc_flags grpc++ grpc grpc++_reflection protobuf dl)
set(sep_control_libs sepcontrol ${sep_libs} ${grpc_flags} orocos-kdl boost_system boost_thread)

add_subdirectory(src)

#add_subdirectory(test)
