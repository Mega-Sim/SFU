object ShutterOriginForm: TShutterOriginForm
  Left = 508
  Top = 304
  Caption = 'Shutter Origin '#54869#51064
  ClientHeight = 192
  ClientWidth = 291
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
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 291
    Height = 129
    Align = alTop
    Caption = ' '#52404#53356' '#54637#47785' '
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlue
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 299
    object CheckBox1: TCheckBox
      Left = 16
      Top = 24
      Width = 233
      Height = 17
      Caption = #48152#49569#52404' '#51316#51116', '#51060#49345' '#50668#48512' '#50977#50504' '#54869#51064
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = CheckBox1Click
    end
    object CheckBox2: TCheckBox
      Left = 16
      Top = 48
      Width = 233
      Height = 17
      Caption = 'Hand Close '#50668#48512' '#54869#51064'('#49468#49436'==ON)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      OnClick = CheckBox1Click
    end
    object CheckBox3: TCheckBox
      Left = 16
      Top = 72
      Width = 233
      Height = 17
      Caption = 'Hand Gripper '#49468#49436' '#44048#51648' '#50668#48512'(2EA=ON)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      OnClick = CheckBox1Click
    end
    object CheckBox4: TCheckBox
      Left = 16
      Top = 96
      Width = 265
      Height = 17
      Caption = 'Hand Alarm==OFF '#48143' InPosition==ON '#50668#48512' '
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = CheckBox1Click
    end
  end
  object btnOK: TBitBtn
    Left = 24
    Top = 136
    Width = 113
    Height = 49
    DoubleBuffered = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    Kind = bkOK
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 1
    OnClick = btnOKClick
  end
  object btnCancel: TBitBtn
    Left = 168
    Top = 136
    Width = 113
    Height = 49
    DoubleBuffered = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    Kind = bkCancel
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 2
    OnClick = btnCancelClick
  end
end
