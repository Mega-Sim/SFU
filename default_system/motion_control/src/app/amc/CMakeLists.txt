FIND_LIBRARY ( SUSILIB SUSI-4.00 /usr/lib )
find_package ( ALSA REQUIRED)

add_executable(amc_oht amc_oht_main.cpp amc_dpram.cpp amc_emulator.cpp amc_oht.cpp amc_oht_anticollision.cpp max11609.cpp ALSADevice.cpp amc_sound.cpp amc_log.cpp amc_iomap.cpp patchlib.cpp)
target_link_libraries(amc_oht oht_control ${sep_control_libs} SUSI-4.00 ${ALSA_LIBRARIES} boost_filesystem boost_system)
