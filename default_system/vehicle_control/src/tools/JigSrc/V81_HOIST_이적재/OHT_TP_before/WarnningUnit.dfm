object WarnningForm: TWarnningForm
  Left = 404
  Top = 209
  BorderStyle = bsDialog
  Caption = 'Warnning !!!!!!!!!!!!!!!!!!! '
  ClientHeight = 107
  ClientWidth = 402
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object DisplayMemo: TMemo
    Left = 0
    Top = 0
    Width = 402
    Height = 107
    Align = alClient
    Color = clYellow
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '바탕체'
    Font.Style = [fsBold]
    ImeName = '한국어(한글) (MS-IME98)'
    Lines.Strings = (
      'Warnning !!!!!!!!!!!!!')
    ParentFont = False
    TabOrder = 0
  end
  object ControlTimer: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = ControlTimerTimer
    Left = 160
    Top = 64
  end
end
