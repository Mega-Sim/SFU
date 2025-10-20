object RebootForm: TRebootForm
  Left = 397
  Top = 245
  Width = 208
  Height = 124
  Caption = 'RebootForm'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 119
    Height = 13
    Caption = 'Enter Password...'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = []
    ParentFont = False
  end
  object edtPassword: TEdit
    Left = 8
    Top = 24
    Width = 185
    Height = 21
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = []
    ImeName = '한국어 입력 시스템 (IME 2000)'
    ParentFont = False
    TabOrder = 0
  end
  object btnOK: TButton
    Left = 16
    Top = 56
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = btnOKClick
  end
  object btnCancel: TButton
    Left = 112
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = btnCancelClick
  end
end
