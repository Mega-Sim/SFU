object CommDisplayForm: TCommDisplayForm
  Left = 601
  Top = 154
  Width = 679
  Height = 354
  Caption = 'Communication Display'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object mmoDisplay: TListBox
    Left = 0
    Top = 0
    Width = 671
    Height = 320
    Align = alClient
    Color = 16744576
    Font.Charset = ANSI_CHARSET
    Font.Color = clWhite
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = []
    ImeName = '한국어(한글) (MS-IME98)'
    ItemHeight = 13
    MultiSelect = True
    ParentFont = False
    TabOrder = 0
    OnDblClick = mmoDisplayDblClick
  end
end
