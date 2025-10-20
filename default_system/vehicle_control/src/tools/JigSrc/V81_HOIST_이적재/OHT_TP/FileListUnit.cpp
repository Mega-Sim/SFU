//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FileListUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileListForm *FileListForm;
//---------------------------------------------------------------------------
__fastcall TFileListForm::TFileListForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
int TFileListForm::DataAdd(String strName, String strDate)
{
  TListItem  *ListItem;
  ListItem = ListView1->Items->Add();
  ListItem->SubItems->Add(strName);
  ListItem->SubItems->Add(strDate);

  return 0;
}
//---------------------------------------------------------------------------
int TFileListForm::DataAdd(String strName)
{
  TListItem  *ListItem;
  ListItem = ListView1->Items->Add();
  ListItem->SubItems->Add(strName);
  return 0;
}
//---------------------------------------------------------------------------
int TFileListForm::DataClear(void)
{
   ListView1->Items->Clear();
   return 0;
}
//---------------------------------------------------------------------------
