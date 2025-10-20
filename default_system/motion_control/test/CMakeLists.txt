
include_directories(. gRPC ../src/control ../src/ecmaster /usr/include/kdl )

if(!ARCH_ARMV6)
    add_subdirectory(unit_test)
endif()

set(mr_grpc_srcs gRPC/myrpc.grpc.pb.cc gRPC/myrpc.pb.cc )
set(grpc_flags grpc++ grpc grpc++_reflection protobuf dl)

set(LOG_SOURCE_FILES logger_test.cpp   )
set(SVR_SOURCE_FILES ipcsvr/svrmain.cpp )
set(CLI_SOURCE_FILES ipccli/climain.cpp ../include/event_host.h)

#add_executable(logtest ${LOG_SOURCE_FILES})
#add_executable(ipcsvr ${SVR_SOURCE_FILES}  ${mr_grpc_srcs})
#add_executable(ipccli ${CLI_SOURCE_FILES}  ${mr_grpc_srcs})
#add_executable(evtest event_host_test.cpp )
#add_executable(menu_test cli_menu_test.cpp)
#add_executable(socktest socktest.cpp)
#add_executable(rmkeys ipc_remove.cpp)
#add_executable(yaml_test yaml_test.cpp)
#add_executable(mapload map/load.cpp)

#target_link_libraries(logtest ${sep_libs})
#target_link_libraries(ipcsvr ${sep_libs} ${grpc_flags})
#target_link_libraries(ipccli ${sep_libs} ${grpc_flags})
#target_link_libraries(evtest ${sep_libs})
#target_link_libraries(menu_test ${sep_libs})
#target_link_libraries(socktest ${sep_libs})
#target_link_libraries(rmkeys ${sep_libs})
#target_link_libraries(yaml_test yaml-cpp)
