
#set (CMAKE_CXX_FLAGS "-m32")
#set (CMAKE_EXE_LINKER_FLAGS "-m32")
#link_directories(acontis/lib/x86 /home/master/srcs/poco-1.7.8/lib/Linux/i386)

if(${ECMASTER} MATCHES "etherlab")
    include_directories(etherlab /opt/etherlab/include)
    link_directories(/opt/etherlab/lib)
    set(ecmasterlib ethercat)
    set(ecmasterflag  "-DUSE_ETHERLAB")
    set(ecmasterarchflag "-DARCH_X64")
    set(stack_dependent_sources
            etherlab/command_line_parser.cpp
            etherlab/master_control.cpp
            etherlab/cia402_servo_pdos.cpp
            etherlab/finetec_slaves_pdo.cpp
            etherlab/slave_pdos.cpp
            )
else(${ECMASTER} MATCHES "acontis")
    include_directories(acontis acontis/include)
    if(ARCH_ARMV6)
        link_directories(acontis/lib/armv6)
        set(ecmasterarchflag "-DARCH_ARM")
    else()
        link_directories(acontis/lib/x64)
        add_subdirectory(acontis)
        set(ecmasterarchflag "-DARCH_X64")
    endif()
    set(ecmasterlib EcMaster)
    set(ecmasterflag  "-DUSE_ACONTIS")
    set(stack_dependent_sources
            acontis/at_xml_parser.cpp
            acontis/select_link_layer.cpp
            acontis/ecat_notification.cpp
            acontis/ecat_common_info.cpp
            acontis/command_line_parser.cpp
            acontis/master_control.cpp
            )
endif()

set(ecmaster_sources
        ecmaster_task.cpp
        ecmaster_app.cpp
        ecmaster_service.cpp
        ecat_cia402_servo_test.cpp
        ecmaster_servo_test.cpp
        ecmaster_message_handler.cpp
        ecmaster_admin.cpp
        ecat_serial.cpp
        ecat_serial_rfc2217.cpp
        telnet_protocol_filter.cpp
        rpc/ecmaster.pb.cc
        rpc/ecmaster.grpc.pb.cc
        ecmaster_emulator.cpp
        emul/ecat_emulator.cpp
        emul/ecat_slave.cpp
        emul/yaskawa_servo_model.cpp
        ../control/velocity_profile.cpp
        ../control/oht_hoist_unit_conversion.cpp
        emul/stocker_io_model.cpp
        emul/oht_io_model.cpp
        emul/onboard_io_emulator.cpp
        emul/ecat_emulator_cli.cpp
        emul/oht/oht_simulator.cpp
        emul/oht/oht_info.cpp
        emul/oht/oht_map.cpp
        emul/oht/oht_cid.cpp
        emul/oht/oht_obstacle.cpp
        emul/oht/oht_pio.cpp
        emul/oht/oht_actor.cpp
        emul/oht/oht_station.cpp
        emul/oht/oht_sensors.cpp
        emul/simulation_model.cpp
        emul/oht/oht_serial_sensors.cpp
        serial/message.cpp serial/message.h serial/serial.cpp serial/serial.h serial/serial_params.cpp serial/serial_params.h serial/message_handler.h
        servopack_config.cpp)

add_executable(ecmaster ${ecmaster_sources} ${stack_dependent_sources})
target_link_libraries(ecmaster ${ecmasterlib} ${sep_libs} ${grpc_flags} yaml-cpp orocos-kdl boost_filesystem boost_system)
target_compile_definitions(ecmaster PRIVATE ${ecmasterflag} ${ecmasterarchflag})


if(${ECMASTER} MATCHES "etherlab")
    add_executable(etherlab_test etherlab/etherlab_test.c )
    target_link_libraries(etherlab_test  ${ecmasterlib} )
endif()
