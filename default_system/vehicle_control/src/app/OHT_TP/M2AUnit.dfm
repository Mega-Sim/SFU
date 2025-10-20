object M2AForm: TM2AForm
  Left = 499
  Top = 278
  BorderStyle = bsDialog
  Caption = 'Mode : Manual -> Auto'
  ClientHeight = 358
  ClientWidth = 241
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = #48148#53461#52404
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 241
    Height = 297
    Align = alTop
    Caption = 'Panel1'
    TabOrder = 0
    object CheckONOFFGrid: TStringGrid
      Left = 1
      Top = 1
      Width = 239
      Height = 295
      Align = alClient
      ColCount = 3
      DefaultColWidth = 20
      DefaultRowHeight = 15
      RowCount = 18
      FixedRows = 0
      TabOrder = 0
      OnDrawCell = CheckONOFFGridDrawCell
      ColWidths = (
        20
        155
        55)
    end
  end
  object btnAutoOK: TBitBtn
    Left = 32
    Top = 328
    Width = 75
    Height = 25
    DoubleBuffered = True
    Kind = bkOK
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = btnAutoOKClick
  end
  object BitBtn2: TBitBtn
    Left = 136
    Top = 328
    Width = 75
    Height = 25
    DoubleBuffered = True
    Kind = bkCancel
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 2
    OnClick = BitBtn2Click
  end
  object Panel2: TPanel
    Left = 0
    Top = 297
    Width = 241
    Height = 24
    Align = alTop
    Caption = 'Mode Change : Manual -> Auto'
    Color = clBlue
    Font.Charset = ANSI_CHARSET
    Font.Color = clYellow
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    TabOrder = 3
  end
end
