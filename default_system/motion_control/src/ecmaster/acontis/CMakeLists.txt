set(demo_motion_srcs
        demos/EcMasterDemoMotion/at_xml_parser.cpp
        demos/EcMasterDemoMotion/ecatDemoCommon.cpp
        demos/EcMasterDemoMotion/ecatNotification.cpp
        demos/EcMasterDemoMotion/Logging.cpp
        demos/EcMasterDemoMotion/MotionDemo.cpp
        demos/EcMasterDemoMotion/MotionDemoMain.cpp
        demos/EcMasterDemoMotion/selectLinkLayer.cpp
        )

#add_executable(demo_motion ${demo_motion_srcs})
#target_link_libraries(demo_motion  AtemRasSrv EcMaster ECMotionControl dl pthread rt)


set(demo_stokerio_srcs
        demos/DemoStokerIO/ATEMDemo2.cpp
        demos/DemoStokerIO/ATEMDemoMain.cpp
        demos/DemoStokerIO/ecatDemoCommon.cpp
        demos/DemoStokerIO/ecatNotification.cpp
        demos/DemoStokerIO/Logging.cpp
        demos/DemoStokerIO/selectLinkLayer.cpp
        )

#add_executable(demo_stokerio ${demo_stokerio_srcs})
#target_link_libraries(demo_stokerio  AtemRasSrv EcMaster ECMotionControl dl pthread rt)
