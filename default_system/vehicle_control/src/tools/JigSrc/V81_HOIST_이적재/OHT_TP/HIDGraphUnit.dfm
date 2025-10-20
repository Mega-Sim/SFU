object HIDGraphForm: THIDGraphForm
  Left = 700
  Top = 0
  Caption = 'HIDGraphForm'
  ClientHeight = 687
  ClientWidth = 1070
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 0
    Width = 1070
    Height = 3
    Cursor = crVSplit
    Align = alTop
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 668
    Width = 1070
    Height = 19
    Panels = <
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 120
      end
      item
        Width = 100
      end
      item
        Width = 50
      end>
    ExplicitTop = 648
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer1Timer
    Left = 208
    Top = 24
  end
  object MainMenu1: TMainMenu
    Left = 88
    Top = 64
    object Online1: TMenuItem
      Caption = 'Online'
      object popStart: TMenuItem
        Caption = '&Start'
        OnClick = popStartClick
      end
      object popEnd: TMenuItem
        Caption = '&End'
        OnClick = popEndClick
      end
    end
    object Offline1: TMenuItem
      Caption = 'Offline'
      object FileOpen1: TMenuItem
        Caption = '&File Open'
        OnClick = FileOpen1Click
      end
    end
    object SaveImage1: TMenuItem
      Caption = 'Save Image'
      OnClick = SaveImage1Click
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 120
    Top = 96
  end
  object SPicture: TSavePictureDialog
    Left = 216
    Top = 88
  end
end
