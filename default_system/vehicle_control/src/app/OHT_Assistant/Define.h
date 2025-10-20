// ---------------------------------------------------------------------------
#ifndef DefineH
#define DefineH
// ---------------------------------------------------------------------------
#define FIXE_DISK					   0
#define REMOVABLE_DISK				1
#define NOT_CFCARD_TYPE				2
#define NOT_EXIST_C_DRIVE			3

#define DELETE_DATE					7
#define DELETE_3HOUR	  			0.125 //3HR
#define DELETE_6HOUR	  			0.25 //6HR
/*********************************************
 *  Content : Communication Definition       *
 *********************************************/

#define chST						   '$'
#define chTP						   's'
#define	chCR						   0x0D
#define	chLF						   0x0A
#define chSTX						   0x02
#define chETX						   0x03
#define chRST						   0x1F

#define NAME_PARAFILE				"OHTPARAM.ini"
#define STR_LOGFILE_PATH			"\\Log\\"

#define LOGMODE_NOTSAVE          "0"  //저장하지 않음
#define LOGMODE_LOCAL            "1"  //실행파일 아래 Log폴더안에 저장
#define LOGMODE_DRIVE            "2"  //다른 Drive 지정 가능

/*******************************************************************************
 *  Content : Log Buffer                             *
/******************************************************************************/
#define BASE_PORT                5100
#define LOG_MAXSIZE              50

#endif
