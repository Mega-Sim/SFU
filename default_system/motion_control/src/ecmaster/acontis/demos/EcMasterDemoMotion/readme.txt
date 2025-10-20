EcMasterDemoMotion application
==============================

This demo is intended to be used to do some positioning with an DS402 based EtherCAT drive.
A detailed description about CiA402 with EtherCAT is available in the document ETG6010_V1i0i0_D_D_CiA402_ImplDirective.pdf

Features:
---------
- Up to 4 axis are supported. Adjust DEMO_MAX_NUM_OF_AXIS to support more.
- The demo configuration is defined in the parameter file, e. g., DemoConfig.xml
- The application requires DCM bus shift synchronization. Please check the EtherCAT Master Class A 
  user manual how to configure DCM bus shift in the configuration tool.
- In MotionControl.cpp some motion control function blocks (MCFBs) are implemented. 
  There is just basic error handling implemented.
- Works only with link layer in polling mode.
- Default cycle time: 1 ms 
- Supported operation modes, Object 0x6060
  1 = PP (profiled position mode)
  6 = Homing
  8 = CSP (cyclic synchronous position mode)
  9 = CSV (cyclic synchronous velocity mode)


PDO Configuration
-----------------
The example works with various PDO layouts. The offset of the required objects are evaluated dynamically.
Further details in myAppSetup().

In CSP mode these values have to mapped:
Receive  PDO:   0x6040 Control word         UINT    (2 Bytes)
                0x607A Target Position      DINT    (4 Bytes)
Transmit PDO:   0x6041 Status word          UINT    (2 Bytes)
                0x6064 Position actual value DINT   (4 Bytes)

In CSV mode these values have to mapped:
Receive  PDO:   0x6040 Control word         UINT    (2 Bytes)
                0x60FF Target Velocity      DINT    (4 Bytes)
Transmit PDO:   0x6041 Status word          UINT    (2 Bytes)
                0x6064 Position actual value DINT   (4 Bytes)

In PP mode these values have to mapped:
Receive  PDO:   0x6040 Control word         UINT    (2 Bytes)
                0x607A Target Position      DINT    (4 Bytes)
                0x6081 Profile velocity     DINT    (4 Bytes)
                0x6083 Profile acceration   DINT    (4 Bytes)
                0x6084 Profile deceration   DINT    (4 Bytes)
Transmit PDO:   0x6041 Status word          UINT    (2 Bytes)
                0x6064 Position actual value DINT   (4 Bytes)

In PP mode these values have to mapped:
Receive  PDO:   0x6040 Control word         UINT    (2 Bytes)
                0x607A Target Position      DINT    (4 Bytes)
Transmit PDO:   0x6041 Status word          UINT    (2 Bytes)
                0x6064 Position actual value DINT   (4 Bytes)
                0x6061 Operation mode display UINT   (2 Bytes)
               
                
Motion Control Function Blocks
------------------------------

MC_Power:                       Power On Drive
MC_Stop:                        Controlled motion stop
MC_Halt:                        Controlled motion stop

MC_MoveAbsolute:                Move to an absolute position
MC_MoveRelative:                Move relative

MC_MoveVelocity:                Endless motion at the specified speed

MC_ReadParameter:               Read parameter, e. g., Commanded position
MC_ReadBoolParameter:           Read bool parameter, e. g., Enable limit switch
MC_WriteParameter:              Write parameter, e. g., Limit switch position
MC_WriteBoolParameter:          Write bool parameter, e. g., Enable limit switch

MC_ReadActualPosition:          Returns the current position of an axis
MC_ReadActualVelocity:          Returns the current velocity of an axis

MC_ReadMotionState              Read motion state
MC_ReadAxisError:               Read axis error
MC_Reset                        Transition from the state ErrorStop to StandStill

MC_SetModeOfOperation:          Set Mode of Operation
MC_GetModesOfOperationDisplay: 	Get Modes of Operation Display
MC_GetSupportedDriveModes:      Get Modes of Operation

MC_Home                         Homing

MC_ReadDigitalInputs            Read digital inputs
MC_ReadDigitalOutputs           Read digital outputs
MC_WriteDigitalOutputs          Write digital outputs


More information about MCFB is available here: http://www.plcopen.org/pages/tc2_motion_control/part_1_2/index.htm

Date: 21 January 2016
acontis technologies GmbH, Weingarten, Germany
