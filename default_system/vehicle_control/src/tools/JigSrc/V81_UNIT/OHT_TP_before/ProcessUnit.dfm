object ProcessForm: TProcessForm
  Left = 344
  Top = 459
  BorderStyle = bsNone
  Caption = 'ProcessForm'
  ClientHeight = 100
  ClientWidth = 357
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 357
    Height = 100
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 0
    object CGauge1: TCGauge
      Left = 2
      Top = 2
      Width = 353
      Height = 23
      Align = alTop
      BackColor = clYellow
      ExplicitLeft = 4
      ExplicitTop = 11
    end
    object Label1: TLabel
      Left = 8
      Top = 40
      Width = 42
      Height = 13
      Caption = 'Label1'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 8
      Top = 60
      Width = 48
      Height = 13
      Caption = 'From :'
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlue
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 8
      Top = 76
      Width = 48
      Height = 13
      Caption = 'To   :'
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlue
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblFrom: TLabel
      Left = 60
      Top = 60
      Width = 49
      Height = 13
      Caption = 'lblFrom'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentFont = False
    end
    object lblTo: TLabel
      Left = 60
      Top = 76
      Width = 35
      Height = 13
      Caption = 'lblTo'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentFont = False
    end
  end
end
