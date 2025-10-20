//---------------------------------------------------------------------------

#ifndef LogFileUnitH
#define LogFileUnitH
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
class TLogFileForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *panFile;
   TPanel *Panel2;
   TComboBox *comboFileSelect;
   TPanel *panFileControl;
   TPageControl *PageControl3;
   TTabSheet *tabDownload;
   TButton *btnDownCancel;
   TOpenDialog *OpenDialog1;
   TButton *btnDownload;
   TSaveDialog *SaveDialog1;
   TCheckBox *UserLckbox;
   TEdit *Logedt;
   TTabSheet *tabGetFileList;
   TRadioGroup *RadioGroup1;
   TButton *Button1;
   TEdit *Edit1;
   TPanel *Panel1;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *lblPCTime;
   TLabel *Label3;
   TLabel *lblOHTTime;
   void __fastcall comboFileSelectChange(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall btnDownloadClick(TObject *Sender);
   void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   BOOL bFileCancel;
   void FileControlInit();
   int FileControlStart(String);
   void FileControlClose();
   int NetWork_Connect(String TmpLocalName, String TmpRemoteName, String TmpPassWord, String TmpID);
   void DownloadLogFile(int, String, String, int);
   String ExtractComboString(TComboBox* tmpCombo);
   void NetWork_DisConnect(String TmpLocalName);
   void FileDownStart(int,String);
   int GetnetworkDrive_NotConnected(String);
   void GetLST_OHTFile(String);
   __fastcall TLogFileForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLogFileForm *LogFileForm;
//---------------------------------------------------------------------------
#endif
