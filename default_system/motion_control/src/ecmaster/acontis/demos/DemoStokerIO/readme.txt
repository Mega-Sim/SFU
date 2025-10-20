This demo is intended to be used alternatively to the standard AtemDemo demo application.

File information compared with AtemDemo files:
----------------------------------------------
ATEMDemo.h    --> simplified version
ATEMDemo2.cpp --> core demo application, please make your changes here! 
                  exchanges files el2004.cpp, el1004.cpp etc.


How to adjust ATEMDemo2.cpp
-----------------------

1. Adjust the bus configuration accordingly
- - - - - - - - - - - - - - - - - - - - - - 
The demo assumes the following bus configuration: 
EK1100 - EL2004 - EL2004 - EL1004 - EL1004

/* demo bus configuration */
#define EC_SLAVE_EK1100     0       /* 1. slave = bus coupler */
#define EC_SLAVE_EL2004_1   1       /* 2. slave = EL2004 (digital output) */
    :     :     :     :
    :     :     :     :
#define EC_NUM_SLAVES       5       /* number of slaves */



2. Adjust the physical addresses of the slaves
- - - - - - - - - - - - - - - - - - - - - -  -  
Take a look into the ET9000 configuration and adjust the following lines accordingly:

/* physical addresses: by default, ET9000 starts with physical address 1001 */
#define EC_ADDR_SLAVE_EK1100     1001
#define EC_ADDR_SLAVE_EL2004_1   1002
    :     :     :     :
    :     :     :     :
 

 
3. Adjust the physical addresses assignments in myAppPrepare()
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  -  
/* insert physical addresses into slave list entries */
S_aSlaveList[EC_SLAVE_EK1100].wFixedAddress     = EC_ADDR_SLAVE_EK1100;
S_aSlaveList[EC_SLAVE_EL2004_1].wFixedAddress   = EC_ADDR_SLAVE_EL2004_1;
    :     :     :     :
    :     :     :     :



4. Adjust the I/O processing
- - - - - - - - - - - - -  -  
Function myAppWorkpd() is called cyclically.
The code that handles input and output data should be placed herein.
