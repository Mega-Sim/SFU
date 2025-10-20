object SocketForm: TSocketForm
  Left = 758
  Top = 263
  Caption = 'SocketForm'
  ClientHeight = 155
  ClientWidth = 251
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object OHTSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnecting = OHTSocketConnecting
    OnConnect = OHTSocketConnect
    OnDisconnect = OHTSocketDisconnect
    OnRead = OHTSocketRead
    OnError = OHTSocketError
    Left = 24
    Top = 20
  end
  object AssistantSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnecting = AssistantSocketConnecting
    OnConnect = AssistantSocketConnect
    OnDisconnect = AssistantSocketDisconnect
    OnRead = AssistantSocketRead
    OnError = AssistantSocketError
    Left = 24
    Top = 76
  end
  object AutoTeachingSocket: TClientSocket
    Active = False
    Address = '192.168.100.62'
    ClientType = ctNonBlocking
    Port = 1234
    OnConnecting = AutoTeachingSocketConnecting
    OnConnect = AutoTeachingSocketConnect
    OnDisconnect = AutoTeachingSocketDisconnect
    OnRead = AutoTeachingSocketRead
    OnError = AutoTeachingSocketError
    Left = 104
    Top = 16
  end
  object tmrOhtTypeCheck: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrOhtTypeCheckTimer
    Left = 144
    Top = 80
  end
end
