//---------------------------------------------------------------------------
#ifndef DefineH
#define DefineH
//---------------------------------------------------------------------------

#define MODE_OPERATOR        1     //반도체 라인 근무자
#define MODE_SETUP           2     //개발자



/** Foup OHT ver 4.0 ********************************************************/
#define WD2_DIST_1ROTATE_MM     65.68    //[mm]
/** Reticle OHT *************************************************************/
#define ROHT_DIST_1ROTATE_MM    69.15    //[mm]
/** Foup OHT ver 4륜 구동 ***************************************************/
#define WD4_DIST_1ROTATE_MM     72.257   //[mm]

/** Foup OHT ver 5.0 ********************************************************/
#define DIST_1ROTATE_MM         72.257   //[mm]
#define DIST_1ROTATE_CM         7.2257   //[cm]
#define DISTSHIFT_1ROTATE_MM    9.60    //[mm]

/** Foup OHT ver 6.0 ********************************************************/
#define DISTV6_1ROTATE_MM       62.832   //[mm]
#define DISTV6_1ROTATE_CM       6.2832   //[cm]
#define DISTSHIFTV6_1ROTATE_MM  27.00    //[mm]

/** Foup OHT ver 6.5 ********************************************************/
#define DISTV65_1ROTATE_MM       72.22   //[mm]
#define DISTV65_1ROTATE_CM       7.222   //[cm]

/** Reticle OHT ver 2.0 ********************************************************/
#define DISTSHIFTV2_1ROTATE_MM  2.00    //[mm]

#define COUNT_1ROTATE           8192.0   //[count]

#define JOG_MAX_SPEED_MODELEACHING   1.0      //[m/s]
#define JOG_MAX_SPEED_MODE_SETUP      3.5      //[m/s]

/*********************************************
 *  Content : Error Definition               *
 *********************************************/
#define NO_ERR               0
#define ERR_FILE_NO_EXIST    1
#define ERR_FILE_NAME        2
#define ERR_FILE_OPEN        3

#define PIOTYPE_E84          0
#define PIOTYPE_E23          1
/*********************************************
 *  Content : Trans LongRun Mode             *
*********************************************/
#define TRMODE_INIT          0
#define TRMODE_EQ_P0_R0      1
#define TRMODE_EQ_P0_R1      2
#define TRMODE_EQ_P1_R0      3
#define TRMODE_EQ_P1_R1      4
#define TRMODE_STB_R0        5
#define TRMODE_STB_R1        6
#define TRMODE_SCAN_STB      7
#define TRMODE_SCAN_UTB      8
#define TRMODE_UTB_R0        9
#define TRMODE_UTB_R1        10
#define TRMODE_STB_L_R0      11
#define TRMODE_STB_L_R1      12
#define TRMODE_SCAN_STB_L    13
#define TRMODE_STB_R_R0      14
#define TRMODE_STB_R_R1      15
#define TRMODE_SCAN_STB_R    16
#define TRMODE_LOADER_P0_R0  17
#define TRMODE_LOADER_P0_R1  18
#define TRMODE_LOADER_P1_R0  19
#define TRMODE_LOADER_P1_R1  20
#define TRMODE_EQ_X_P0_R0    21
#define TRMODE_EQ_X_P1_R0    22
#define MOVE_INIT            0
#define MOVE_STB             1
#define MOVE_EQ              2
#define MOVE_UTB             3
#define MOVE_EQ_X            4


/*********************************************
 *  Content : USER IO CMD Definition          *
 *********************************************/

#define SOUND1               1
#define SOUND2               2
#define SOUND3               3
#define SOUND4               4

/*********************************************
 *  Content : File Control                  *
 *********************************************/
#define TP_TAG               0x59100000
#define CHECK_CMDTYPE        0x0000F000


#endif
