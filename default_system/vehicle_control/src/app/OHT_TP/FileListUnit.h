//---------------------------------------------------------------------------

#ifndef FileListUnitH
#define FileListUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFileListForm : public TForm
{
__published:	// IDE-managed Components
   TListView *ListView1;
private:	// User declarations
public:		// User declarations
   __fastcall TFileListForm(TComponent* Owner);
   int DataAdd(String, String);
   int DataAdd(String);
   int DataClear(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileListForm *FileListForm;
//---------------------------------------------------------------------------
#endif
