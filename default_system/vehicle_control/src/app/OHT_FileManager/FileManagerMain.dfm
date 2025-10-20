object FileManagerForm: TFileManagerForm
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'FileManager'
  ClientHeight = 558
  ClientWidth = 409
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 385
    Height = 498
    ImeName = 'Microsoft IME 2010'
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object btnClear: TButton
    Left = 270
    Top = 512
    Width = 121
    Height = 28
    Caption = 'Clear'
    TabOrder = 1
    OnClick = btnClearClick
  end
  object Button1: TButton
    Left = 8
    Top = 512
    Width = 65
    Height = 33
    Caption = 'TEST'
    TabOrder = 2
    Visible = False
    OnClick = Button1Click
  end
  object btnShow: TButton
    Left = 105
    Top = 512
    Width = 73
    Height = 28
    Caption = 'Dbg. Show'
    TabOrder = 3
    OnClick = btnShowClick
  end
  object btnHide: TButton
    Left = 184
    Top = 512
    Width = 73
    Height = 28
    Caption = 'Dgb. Hide'
    TabOrder = 4
    OnClick = btnHideClick
  end
  object MainTimer: TTimer
    Interval = 50
    OnTimer = MainTimerTimer
    Left = 576
    Top = 344
  end
  object IdFTP1: TIdFTP
    IPVersion = Id_IPv4
    ProxySettings.ProxyType = fpcmNone
    ProxySettings.Port = 0
    Left = 576
    Top = 296
  end
  object TrayIconFileManager: TTrayIcon
    PopupMenu = popupTrayIcon
    Left = 576
    Top = 248
  end
  object popupTrayIcon: TPopupMenu
    Left = 576
    Top = 200
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = Exit1Click
    end
  end
  object IdTCPClient1: TIdTCPClient
    ConnectTimeout = 0
    IPVersion = Id_IPv4
    Port = 0
    ReadTimeout = -1
    Left = 24
    Top = 448
  end
  object IdFTP2: TIdFTP
    IPVersion = Id_IPv4
    ProxySettings.ProxyType = fpcmNone
    ProxySettings.Port = 0
    Left = 104
    Top = 448
  end
  object trmMinimize: TTimer
    Interval = 100
    OnTimer = trmMinimizeTimer
    Left = 256
    Top = 352
  end
end
