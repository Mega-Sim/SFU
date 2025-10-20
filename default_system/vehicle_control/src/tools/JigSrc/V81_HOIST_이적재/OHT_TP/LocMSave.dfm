object MoveForm: TMoveForm
  Left = 444
  Top = 311
  Caption = '#2 Loc + Move Loc Save'
  ClientHeight = 108
  ClientWidth = 267
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblAxis: TLabel
    Left = 64
    Top = 16
    Width = 128
    Height = 13
    Caption = '#2 Move Loc Save'
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlue
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblWho: TLabel
    Left = 16
    Top = 16
    Width = 40
    Height = 13
    Caption = '[STB]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clFuchsia
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Edit1: TEdit
    Left = 72
    Top = 32
    Width = 121
    Height = 21
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
    ParentFont = False
    TabOrder = 0
    Text = '0'
  end
  object BitBtn1: TBitBtn
    Left = 48
    Top = 72
    Width = 75
    Height = 25
    DoubleBuffered = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    Kind = bkOK
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 1
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 152
    Top = 72
    Width = 75
    Height = 25
    DoubleBuffered = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    Kind = bkCancel
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 2
  end
end
