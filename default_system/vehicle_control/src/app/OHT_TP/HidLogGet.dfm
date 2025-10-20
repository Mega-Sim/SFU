object frmHidLogGet: TfrmHidLogGet
  Left = 0
  Top = 0
  Caption = 'HID '#47196#44536' '#51200#51109
  ClientHeight = 253
  ClientWidth = 386
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 369
    Height = 97
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 0
    object Label1: TLabel
      Left = 24
      Top = 16
      Width = 323
      Height = 13
      Caption = #44592#45733#51032' '#49892#54665' '#50756#47308#44620#51648' 2'#48516' '#44032#47049' '#49548#50836#46104#48064#47196', '#51452#51032#54644' '#51452#49901#49884#50724'.'
    end
    object Label2: TLabel
      Left = 24
      Top = 43
      Width = 281
      Height = 13
      Caption = #48376' '#44592#45733#51008' OHT'#44032' Manual '#49345#53468#50640#49436#47564' '#49324#50857' '#44032#45733#54633#45768#45796'.'
    end
    object Label3: TLabel
      Left = 22
      Top = 70
      Width = 322
      Height = 13
      Caption = #47196#44536#47484' '#51200#51109#54616#44256' '#51080#45716' '#46041#50504#51008' OHT'#51032' '#51312#51089#51012' '#49340#44032#54644' '#51452#49901#49884#50724'.'
    end
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 138
    Width = 257
    Height = 57
    Caption = #49892#54665#49345#53468
    TabOrder = 1
    object panCmd_Response: TPanel
      Left = 96
      Top = 19
      Width = 66
      Height = 23
      Caption = #51025#45813#49688#49888
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 0
    end
    object panCmd_Complete: TPanel
      Left = 181
      Top = 19
      Width = 64
      Height = 23
      Caption = #50756#47308#52376#47532
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 1
    end
    object panCmd_Start: TPanel
      Left = 9
      Top = 19
      Width = 72
      Height = 23
      Caption = #47749#47161#51204#49569
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 2
    end
  end
  object btnStart: TButton
    Left = 273
    Top = 148
    Width = 105
    Height = 43
    Caption = 'HID '#47196#44536#52712#46301
    TabOrder = 2
    OnClick = btnStartClick
  end
  object edtHelp: TEdit
    Left = 8
    Top = 111
    Width = 370
    Height = 21
    ReadOnly = True
    TabOrder = 3
  end
  object btnClose: TButton
    Left = 273
    Top = 208
    Width = 105
    Height = 36
    Caption = 'Close'
    TabOrder = 4
    OnClick = btnCloseClick
  end
end
