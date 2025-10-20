//---------------------------------------------------------------------------

#ifndef MonitoringUtilH
#define MonitoringUtilH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include "../Common/Def_DataType.h"
#include "ErrorCode.h"
#include "Utility.h"
#define OHT_MAIN_FILE_NAME 		L"OHT.exe"
#define	FBWF_CURRENT_STATUS		"fbwfmgr"
#define FBWF_SET_ENABLE			"fbwfmgr /enable"
#define FBWF_SET_DISABLE		"fbwfmgr /disable"

#define	FBWF_ENABLE				0x01
#define	FBWF_DISABLE			0x02
#define	FBWF_WILL_BE_ENABLED	0x03
#define	FBWF_WILL_BE_DISABLED	0x04
#define	FBWF_ERROR				0x07
//---------------------------------------------------------------------------
class MonitoringUtil
{
private:
	TSearchRec sr; //LogDeleter 존재여부때 사용
	int status;
	String strResult;
	void Init();
    OHTLogFile *m_Log;     ///< Log 저장을 위한 클래스
	static MonitoringUtil* instance;
	MonitoringUtil();
public:
	static MonitoringUtil& getInstance();
	bool isFileExist(String strFilePath);
	DISK_INFO* GetDiskInfo(int drive);
	String GetResultString();
	int GetStatus();
	int ExecuteDosCmd(String strDocCmd);
//	void ConvertDiskFormattedSize(UINT64 size, TCHAR* formattedSize);
	void SetCurrentTime(String strTime);
	OHT_VERSION* GetOHTMainProgramVersion();
	void MonitoringOHT();
};

MonitoringUtil* MonitoringUtil::instance = NULL;

#endif
