object FBWFManagerForm: TFBWFManagerForm
  Left = 0
  Top = 0
  Caption = 'FBWFManagerForm'
  ClientHeight = 298
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object mmoLog: TMemo
    Left = 0
    Top = 55
    Width = 360
    Height = 225
    ImeName = 'Microsoft IME 2010'
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object btGetStatus: TButton
    Left = 280
    Top = 8
    Width = 75
    Height = 41
    Caption = 'Get Status'
    TabOrder = 1
    OnClick = btGetStatusClick
  end
  object rgFBWF: TRadioGroup
    Left = 8
    Top = 8
    Width = 185
    Height = 41
    Caption = 'Status'
    Columns = 2
    ItemIndex = 0
    Items.Strings = (
      'Enable'
      'Disable')
    TabOrder = 2
  end
  object btSetStatus: TButton
    Left = 199
    Top = 8
    Width = 75
    Height = 41
    Caption = 'Set Status'
    TabOrder = 3
    OnClick = btSetStatusClick
  end
  object statFBWF: TStatusBar
    Left = 0
    Top = 279
    Width = 360
    Height = 19
    Panels = <>
    SimplePanel = True
  end
end
