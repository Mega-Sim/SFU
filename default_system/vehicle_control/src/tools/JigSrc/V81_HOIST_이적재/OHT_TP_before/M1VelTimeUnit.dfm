object M1VelForm: TM1VelForm
  Left = 58
  Top = 153
  Width = 248
  Height = 125
  Caption = 'M1VelForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 8
    Width = 216
    Height = 13
    Caption = 'Motor1 Vel Time Setting....'
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlue
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 112
    Top = 28
    Width = 35
    Height = 13
    Caption = '[Min]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = []
    ParentFont = False
  end
  object Edit1: TEdit
    Left = 16
    Top = 24
    Width = 89
    Height = 21
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = []
    ImeName = '한국어 입력 시스템 (IME 2000)'
    ParentFont = False
    TabOrder = 0
    Text = '1'
  end
  object BitBtn1: TBitBtn
    Left = 16
    Top = 56
    Width = 75
    Height = 25
    TabOrder = 1
    OnClick = BitBtn1Click
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 104
    Top = 56
    Width = 75
    Height = 25
    TabOrder = 2
    Kind = bkCancel
  end
end
