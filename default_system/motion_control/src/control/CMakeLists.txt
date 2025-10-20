

include_directories(model)


set(ecmaster_rpc_sources
        ../ecmaster/rpc/ecmaster.grpc.pb.cc
        ../ecmaster/rpc/ecmaster.pb.cc
        )
set(cia402_test_sources
        servo_motor.cpp
        ec_servo_motor.cpp
        ec_servo_emulator.cpp)

set(robotics_lib_sources
        servo_motor.cpp
        ec_servo_motor.cpp
        ec_servo_emulator.cpp
        servo_motion.cpp
        robot_motion.cpp
        velocity_profile.cpp
        robotic_module.cpp
        axes_module.cpp
        multi_axes_module.cpp
        ${ecmaster_rpc_sources}
        io_board.cpp
        xr17v352_rt.cpp
        gpio.cpp
        serial_port.cpp
        ../app/oht/oht_onboard_io.cpp
        ../app/oht/oht_io_port_map.cpp
        )

add_library(sepcontrol ${robotics_lib_sources})

