object StatusForm: TStatusForm
  Left = 576
  Top = 223
  Caption = 'StatusForm'
  ClientHeight = 354
  ClientWidth = 221
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  ScreenSnap = True
  SnapBuffer = 40
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel13: TPanel
    Left = 0
    Top = 0
    Width = 221
    Height = 354
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 2
      Top = 2
      Width = 217
      Height = 350
      Align = alClient
      Color = 16769023
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentColor = False
      ParentFont = False
      TabOrder = 0
      object lbCommandType: TLabel
        Left = 4
        Top = 9
        Width = 87
        Height = 15
        Alignment = taCenter
        Caption = 'Command Type'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbMode: TLabel
        Left = 42
        Top = 30
        Width = 31
        Height = 15
        Alignment = taCenter
        Caption = 'Mode'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbStatus: TLabel
        Left = 39
        Top = 51
        Width = 37
        Height = 15
        Alignment = taCenter
        Caption = 'Status'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbCurrentNodeID: TLabel
        Left = 13
        Top = 93
        Width = 89
        Height = 15
        Alignment = taCenter
        Caption = 'Current Node ID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbStopNodeID: TLabel
        Left = 21
        Top = 114
        Width = 72
        Height = 15
        Alignment = taCenter
        Caption = 'Stop Node ID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbCurrentStationID: TLabel
        Left = 7
        Top = 135
        Width = 100
        Height = 15
        Alignment = taCenter
        Caption = 'Current Station ID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbTargetStationID: TLabel
        Left = 11
        Top = 157
        Width = 93
        Height = 15
        Alignment = taCenter
        Caption = 'Target Station ID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbErrorCode: TLabel
        Left = 27
        Top = 178
        Width = 61
        Height = 15
        Alignment = taCenter
        Caption = 'Error Code'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbCmdNumber: TLabel
        Left = 22
        Top = 199
        Width = 71
        Height = 15
        Alignment = taCenter
        Caption = 'CmdNumber'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbSndCmdID: TLabel
        Left = 27
        Top = 220
        Width = 62
        Height = 15
        Alignment = taCenter
        Caption = 'Snd CmdID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbRcvCmdID: TLabel
        Left = 27
        Top = 241
        Width = 61
        Height = 15
        Alignment = taCenter
        Caption = 'Rcv CmdID'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbRcvRespon: TLabel
        Left = 24
        Top = 262
        Width = 67
        Height = 15
        Alignment = taCenter
        Caption = 'Rcv Respon'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbTPStatus: TLabel
        Left = 33
        Top = 71
        Width = 52
        Height = 15
        Alignment = taCenter
        Caption = 'TPStatus'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object panCMDType: TPanel
        Left = 116
        Top = 8
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 0
      end
      object panMode: TPanel
        Left = 116
        Top = 29
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 1
      end
      object panStatus: TPanel
        Left = 116
        Top = 50
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 2
      end
      object panTPSt: TPanel
        Left = 116
        Top = 71
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 3
      end
      object panCurrentNodeID: TPanel
        Left = 116
        Top = 92
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 4
      end
      object panErrCode: TPanel
        Left = 116
        Top = 177
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 5
      end
      object panCmdNumber: TPanel
        Left = 116
        Top = 198
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 6
      end
      object panRcvID: TPanel
        Left = 116
        Top = 240
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 7
      end
      object panResponse: TPanel
        Left = 116
        Top = 261
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 8
      end
      object panSndCmdID: TPanel
        Left = 116
        Top = 219
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 9
      end
      object panTargetNodeID: TPanel
        Left = 116
        Top = 113
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 10
      end
      object panOHTVer: TPanel
        Left = 2
        Top = 280
        Width = 213
        Height = 68
        Align = alBottom
        BevelInner = bvLowered
        Caption = 'Ver 5.0'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clLime
        Font.Height = -19
        Font.Name = 'Arial Black'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 11
        object memoStatus: TMemo
          Left = 2
          Top = 2
          Width = 209
          Height = 64
          Align = alClient
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
          ParentFont = False
          TabOrder = 0
        end
      end
      object panCurrentStationID: TPanel
        Left = 116
        Top = 134
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 12
      end
      object panTargetStationID: TPanel
        Left = 116
        Top = 156
        Width = 97
        Height = 17
        Color = clBlack
        Font.Charset = ANSI_CHARSET
        Font.Color = clWhite
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 13
      end
    end
  end
  object popClear: TPopupMenu
    Left = 134
    Top = 292
    object CmdBuffSave1: TMenuItem
      Caption = 'Cmd Buff Save'
      OnClick = CmdBuffSave1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object CmdBuffClear1: TMenuItem
      Caption = 'Cmd Buff Clear'
      OnClick = CmdBuffClear1Click
    end
  end
  object tmDisplayUpdate: TTimer
    Enabled = False
    OnTimer = tmDisplayUpdateTimer
    Left = 40
    Top = 288
  end
end
