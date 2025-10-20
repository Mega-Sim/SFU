set(OHT_CONTROL_SOURCES
        oht_behaviors.cpp
        oht_controller.cpp
        oht_localization.cpp
        oht_io_port_map.cpp
        ../../ecmaster/emul/oht/oht_map.cpp
        ../../control/oht_hoist_unit_conversion.cpp
        oht_serial_devices.cpp oht_serial_devices.h oht_amc_axis.cpp oht_amc_axis.h oht_mark_detector.cpp oht_mark_detector.h oht_io_config.h)
add_library(oht_control ${OHT_CONTROL_SOURCES})
