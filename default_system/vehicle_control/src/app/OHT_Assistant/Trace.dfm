object frmWork: TfrmWork
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = 'Assistance Trace'
  ClientHeight = 441
  ClientWidth = 403
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 8
    Top = -103
    Width = 385
    Height = 498
    ImeName = 'Microsoft IME 2010'
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object btnClear: TButton
    Left = 275
    Top = 401
    Width = 121
    Height = 28
    Caption = 'Clear'
    TabOrder = 1
    OnClick = btnClearClick
  end
end
