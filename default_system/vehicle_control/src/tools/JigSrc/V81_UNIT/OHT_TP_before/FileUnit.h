//---------------------------------------------------------------------------

#ifndef FileUnitH
#define FileUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFileForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *panFile;
   TPanel *Panel2;
   TComboBox *comboFileSelect;
   TPanel *panFileControl;
    TPageControl *pgcFileControl;
   TTabSheet *tabDownload;
   TButton *btnDownCancel;
   TTabSheet *tabUpload;
   TButton *btnUpload;
   TButton *btnUpCancel;
   TTabSheet *tabDelete;
   TButton *btnDelete;
   TButton *btnDelCancel;
    TOpenDialog *dlgOpenFile;
   TButton *btnDownload;
    TSaveDialog *dlgSaveFile;
    TGroupBox *grpSelectFiles;
    TCheckBox *chkMapDataFile;
    TCheckBox *chkAMCOutFile;
    TCheckBox *chkTeachingDataFile;
    TCheckBox *chkNodeDisableFile;
    TCheckBox *chkUserSelectFile;
    TEdit *edtUserSelectFileName;
    TTabSheet *tabAMCUpdate;
    TMemo *memoAMCUpdate;
    TCheckBox *chkParamDrivingFile;
    TRadioGroup *rgSelectUploadFile;
   TTabSheet *tabFileList;
   TButton *Button1;
   TRadioGroup *RadioGroup1;
   TEdit *Edit1;
    TButton *btnGetFileInfo;
    TTabSheet *tabAssistant;
   TButton *btnAssitantClose;
   TButton *btnAssistantUpload;
   TButton *btnAssistantExec;
    TCheckBox *chkStationDataFile;
    TCheckBox *chkParamTransFile;
	TLabel *Label1;
   void __fastcall comboFileSelectChange(TObject *Sender);
   void __fastcall btnDownCancelClick(TObject *Sender);
   void __fastcall btnUploadClick(TObject *Sender);
   void __fastcall btnDeleteClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall btnDownloadClick(TObject *Sender);
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall btnGetFileInfoClick(TObject *Sender);
   void __fastcall btnAssitantCloseClick(TObject *Sender);
   void __fastcall btnAssistantUploadClick(TObject *Sender);
   void __fastcall btnAssistantExecClick(TObject *Sender);
	void __fastcall Edit1Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
   BOOL bFileCancel;

   // by JYH 111027 : FileUploadAlarm창 강제 종료 여부 확인을 위한 플래그
   BOOL bFlag_ForceFileUpload;

   void FileControlInit();
   int FileControlStart(String);
   void FileControlClose();
   int NetWork_Connect(String TmpLocalName, String TmpRemoteName, String TmpPassWord, String TmpID);
   void DownLoadFile(String FileName, String FromPath, String ToPath);
   void UploadFile(int DataType, String FileName);
   void Delete_OHTFile(String FromPath);
   void GetLST_OHTFile(String);
   String ExtractComboString(TComboBox* tmpCombo);
   void NetWork_DisConnect(String TmpLocalName);
   void FileDownStart(int DataType, String FileName);
   int GetnetworkDrive_NotConnected(String);
   int GetOhtLogDrive_NotConnected(String);
   void GetLST_OHTFileInfo(String);

	// by JYH 111027 : 파일 수정 시간 비교를 위한 함수
	BOOL	IsNewFile_Older(String oldFileName, String newFileName);

   __fastcall TFileForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileForm *FileForm;
//---------------------------------------------------------------------------
#endif
