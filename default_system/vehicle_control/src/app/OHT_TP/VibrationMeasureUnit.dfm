object frmVibrationMeasurement: TfrmVibrationMeasurement
  Left = 0
  Top = 166
  Caption = #51652#46041' '#47784#45768#53552#47553
  ClientHeight = 648
  ClientWidth = 964
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnInformation: TPanel
    Left = 0
    Top = 400
    Width = 481
    Height = 248
    Align = alLeft
    BorderStyle = bsSingle
    TabOrder = 0
    object pnSelectSavePath: TPanel
      Left = 1
      Top = 1
      Width = 475
      Height = 35
      Align = alTop
      TabOrder = 0
      object Label7: TLabel
        Left = 11
        Top = 3
        Width = 44
        Height = 16
        Caption = #47196#44536#51200#51109
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object btnChangeSaveFilePath: TButton
        Left = 244
        Top = 4
        Width = 62
        Height = 25
        Caption = #44221#47196#48320#44221
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = btnChangeSaveFilePathClick
      end
      object edtSaveFilePath: TEdit
        Left = 312
        Top = 6
        Width = 160
        Height = 23
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        ParentFont = False
        TabOrder = 1
        OnMouseEnter = edtSaveFilePathMouseEnter
        OnMouseLeave = edtSaveFilePathMouseLeave
      end
      object cbbSavingTime: TComboBox
        Left = 168
        Top = 8
        Width = 70
        Height = 21
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        Text = #51200#51109#49884#44036
      end
      object btLogSaveStart: TButton
        Left = 84
        Top = 4
        Width = 78
        Height = 25
        Caption = #49884#51089
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        OnClick = btLogSaveStartClick
      end
    end
    object pnVibInfo: TPanel
      Left = 1
      Top = 36
      Width = 475
      Height = 207
      Align = alClient
      TabOrder = 1
      object gbNodeInfo: TGroupBox
        Left = 2
        Top = 6
        Width = 163
        Height = 96
        Caption = #50948#52824' '#51221#48372
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object LabelOffsetDistance: TLabel
          Left = 84
          Top = 17
          Width = 67
          Height = 15
          Caption = #44144#47532'(Offset)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object LabelCurrentNode: TLabel
          Left = 9
          Top = 17
          Width = 51
          Height = 15
          Caption = #54788#51116' '#45432#46300
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtCurrentNode: TEdit
          Left = 3
          Top = 38
          Width = 75
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 0
          Text = '0'
        end
        object edtOffsetDistance: TEdit
          Left = 82
          Top = 38
          Width = 75
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 1
          Text = '0'
        end
      end
      object GroupBox2: TGroupBox
        Left = 2
        Top = 105
        Width = 163
        Height = 96
        Caption = #51312#54693' '#51221#48372
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGreen
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object Label2: TLabel
          Left = 8
          Top = 26
          Width = 63
          Height = 15
          Caption = 'Front Steer'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label3: TLabel
          Left = 90
          Top = 26
          Width = 61
          Height = 15
          Caption = 'Rear Steer'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtSteerFront: TEdit
          Left = 3
          Top = 47
          Width = 76
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 0
          Text = 'LEFT'
        end
        object edtSteerRear: TEdit
          Left = 84
          Top = 47
          Width = 76
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 1
          Text = 'RIGHT'
        end
      end
      object gbMotorVelocity: TGroupBox
        Left = 168
        Top = 6
        Width = 94
        Height = 195
        Caption = #47784#53552#49549#46020
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object LabelVhlVel: TLabel
          Left = 19
          Top = 18
          Width = 51
          Height = 15
          Caption = #51452#54665' '#49549#46020
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object LabelTransVel: TLabel
          Left = 15
          Top = 78
          Width = 60
          Height = 15
          Caption = #51060#51201#51116#49549#46020
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblSpeedMode: TLabel
          Left = 19
          Top = 138
          Width = 51
          Height = 15
          Caption = #51452#54665' '#49345#54889
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtVhlVel: TEdit
          Left = 3
          Top = 39
          Width = 88
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 0
          Text = '0'
        end
        object edtTransVel: TEdit
          Left = 3
          Top = 99
          Width = 88
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 1
          Text = '0'
        end
        object edtSpeedMode: TEdit
          Left = 3
          Top = 159
          Width = 88
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 2
          Text = '0'
        end
      end
      object gbVibrationInfo: TGroupBox
        Left = 400
        Top = 6
        Width = 65
        Height = 195
        Caption = #51652#46041
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object LabelVibX: TLabel
          Left = 19
          Top = 17
          Width = 27
          Height = 15
          Caption = 'X (G)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object LabelVibZ: TLabel
          Left = 19
          Top = 107
          Width = 26
          Height = 15
          Caption = 'Z (G)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object LabelVibY: TLabel
          Left = 19
          Top = 62
          Width = 26
          Height = 15
          Caption = 'Y (G)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object LabelVibRMS: TLabel
          Left = 19
          Top = 152
          Width = 26
          Height = 15
          Caption = 'RMS'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtVibX: TEdit
          Left = 8
          Top = 33
          Width = 49
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 0
          Text = '0'
        end
        object edtVibZ: TEdit
          Left = 8
          Top = 121
          Width = 49
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 1
          Text = '0'
        end
        object edtVibY: TEdit
          Left = 8
          Top = 77
          Width = 48
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 2
          Text = '0'
        end
        object edtVibRMS: TEdit
          Left = 8
          Top = 166
          Width = 48
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          ReadOnly = True
          TabOrder = 3
          Text = '0'
        end
      end
      object grp1: TGroupBox
        Left = 268
        Top = 6
        Width = 126
        Height = 195
        Caption = #45824#52264'   '#51109#50528#47932
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        object pnlDetect: TPanel
          Left = 12
          Top = 18
          Width = 45
          Height = 145
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 0
          object pnlOhtFail: TPanel
            Left = 8
            Top = 110
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
          end
          object pnlOhtShort: TPanel
            Left = 8
            Top = 77
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
          end
          object pnlOhtMiddle: TPanel
            Left = 8
            Top = 44
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
          end
          object pnlOhtLong: TPanel
            Left = 8
            Top = 11
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 3
          end
        end
        object pnl1: TPanel
          Left = 75
          Top = 18
          Width = 45
          Height = 145
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 1
          object pnlObsFail: TPanel
            Left = 8
            Top = 110
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
          end
          object pnlObsShort: TPanel
            Left = 8
            Top = 77
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
          end
          object pnlObsMiddle: TPanel
            Left = 8
            Top = 44
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
          end
          object pnlObsLong: TPanel
            Left = 8
            Top = 11
            Width = 27
            Height = 27
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 3
          end
        end
        object edtUBGRegion: TEdit
          Left = 12
          Top = 169
          Width = 45
          Height = 21
          TabOrder = 2
        end
        object edtOBSRegion: TEdit
          Left = 75
          Top = 169
          Width = 45
          Height = 21
          Color = clWhite
          TabOrder = 3
        end
      end
    end
  end
  object pnVibMeasureControl: TPanel
    Left = 787
    Top = 400
    Width = 177
    Height = 248
    Align = alRight
    BorderStyle = bsSingle
    TabOrder = 1
    object pgcAVSMenu: TPageControl
      Left = 1
      Top = 1
      Width = 171
      Height = 242
      ActivePage = tsPlayback
      Align = alClient
      TabOrder = 0
      object tsConnection: TTabSheet
        Caption = 'Connection'
        object LabelStatus: TLabel
          Left = 3
          Top = 3
          Width = 156
          Height = 13
          Caption = 'Status: Not connected to sensor'
        end
        object Label5: TLabel
          Left = 3
          Top = 84
          Width = 82
          Height = 13
          Caption = 'Temperature('#8451')'
        end
        object Label6: TLabel
          Left = 97
          Top = 84
          Width = 55
          Height = 13
          Caption = 'Battery(%)'
        end
        object lbTemperature: TLabel
          Left = 3
          Top = 103
          Width = 78
          Height = 19
          Alignment = taCenter
          AutoSize = False
          Caption = '0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #47569#51008' '#44256#46357
          Font.Style = []
          ParentFont = False
          Transparent = True
          Layout = tlCenter
        end
        object lbBattery: TLabel
          Left = 83
          Top = 103
          Width = 78
          Height = 19
          Alignment = taCenter
          AutoSize = False
          Caption = '0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #47569#51008' '#44256#46357
          Font.Style = []
          ParentFont = False
          Transparent = True
          Layout = tlCenter
        end
        object cgTemperature: TCGauge
          Left = 3
          Top = 126
          Width = 78
          Height = 17
          ShowText = False
        end
        object cgBattery: TCGauge
          Left = 87
          Top = 126
          Width = 73
          Height = 17
          ShowText = False
        end
        object btnInitData: TButton
          Left = 85
          Top = 22
          Width = 75
          Height = 25
          Caption = 'Init. Data'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          OnClick = btnInitDataClick
        end
        object btnSensorConnection: TButton
          Left = 3
          Top = 22
          Width = 75
          Height = 25
          Caption = 'Connect'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = btnSensorConnectionClick
        end
        object btnVibMeasureStop: TButton
          Left = 84
          Top = 53
          Width = 75
          Height = 25
          Caption = 'Stop'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
          OnClick = btnVibMeasureStopClick
        end
        object btnVibMeasureStart: TButton
          Left = 3
          Top = 53
          Width = 75
          Height = 25
          Caption = 'Start'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBtnText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
          OnClick = btnVibMeasureStartClick
        end
      end
      object tsPlayback: TTabSheet
        Caption = 'Playback'
        ImageIndex = 1
        object cgGauge: TCGauge
          Left = 0
          Top = 188
          Width = 163
          Height = 26
          Align = alBottom
          ExplicitTop = 120
        end
        object lbLoadCounter: TLabel
          Left = 133
          Top = 101
          Width = 27
          Height = 16
          Alignment = taRightJustify
          Caption = '0 / 0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object btFileOpen: TButton
          Left = 3
          Top = 3
          Width = 75
          Height = 25
          Caption = 'Open'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          OnClick = btFileOpenClick
        end
        object btInitData: TButton
          Left = 84
          Top = 3
          Width = 75
          Height = 25
          Caption = 'Init'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = btInitDataClick
        end
        object btStopDraw: TButton
          Left = 3
          Top = 34
          Width = 75
          Height = 25
          Caption = 'Stop'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
          OnClick = btStopDrawClick
        end
        object btExecuteFFT: TButton
          Left = 84
          Top = 34
          Width = 75
          Height = 25
          Caption = 'FFT'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
          OnClick = btExecuteFFTClick
        end
        object btnRestart: TButton
          Left = 3
          Top = 65
          Width = 75
          Height = 25
          Caption = 'Restart'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
          OnClick = btnRestartClick
        end
      end
    end
  end
  object PanelGraph: TPanel
    Left = 0
    Top = 0
    Width = 964
    Height = 400
    Align = alTop
    TabOrder = 2
    object ScrollBarVibTime: TScrollBar
      Left = 1
      Top = 382
      Width = 962
      Height = 17
      Align = alBottom
      Max = 1
      PageSize = 0
      TabOrder = 0
      OnChange = ScrollBarVibTimeChange
    end
    object chartDrivingMotorSpeed: TChart
      Left = 1
      Top = 1
      Width = 962
      Height = 190
      Legend.CheckBoxes = True
      Legend.HorizMargin = 5
      Legend.LegendStyle = lsSeries
      Legend.TextStyle = ltsValue
      Legend.TopPos = 1
      Legend.Visible = False
      MarginBottom = 1
      MarginLeft = 5
      MarginRight = 90
      MarginTop = 5
      MarginUnits = muPixels
      Title.Text.Strings = (
        'TChart')
      Title.Visible = False
      OnClickSeries = chartDrivingMotorSpeedClickSeries
      LeftAxis.LabelsSize = 20
      View3D = False
      Align = alTop
      TabOrder = 1
      PrintMargins = (
        15
        39
        15
        39)
      ColorPaletteIndex = 13
      object gbVel: TGroupBox
        AlignWithMargins = True
        Left = 877
        Top = 4
        Width = 81
        Height = 165
        Margins.Bottom = 20
        Align = alRight
        Caption = 'Velocity (m/s)'
        TabOrder = 0
        object shVelRear: TShape
          Left = 8
          Top = 86
          Width = 10
          Height = 6
          Brush.Color = clBlack
          Visible = False
        end
        object shVelFront: TShape
          Left = 8
          Top = 24
          Width = 10
          Height = 6
          Brush.Color = clBlue
        end
        object shVelTrans: TShape
          Left = 8
          Top = 54
          Width = 10
          Height = 6
          Brush.Color = clGreen
        end
        object Label1: TLabel
          Left = 64
          Top = 144
          Width = 7
          Height = 15
          Caption = 's'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label4: TLabel
          Left = 3
          Top = 123
          Width = 69
          Height = 15
          Caption = 'Time Offset :'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object cbVelFront: TCheckBox
          Left = 24
          Top = 18
          Width = 65
          Height = 17
          Caption = 'Front'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 0
          OnClick = cbVelFrontClick
        end
        object cbVelRear: TCheckBox
          Left = 24
          Top = 80
          Width = 73
          Height = 17
          Caption = 'Rear'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 1
          Visible = False
          OnClick = cbVelRearClick
        end
        object cbVelTrans: TCheckBox
          Left = 24
          Top = 48
          Width = 73
          Height = 17
          Caption = 'Trans'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 2
          OnClick = cbVelTransClick
        end
        object edtVelTimeOffset: TEdit
          Left = 3
          Top = 139
          Width = 55
          Height = 23
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ImeName = 'Microsoft IME 2010'
          ParentFont = False
          TabOrder = 3
          Text = '0.0'
        end
      end
      object SeriesFrontMotor: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Emboss.Color = 8553090
        Marks.Margins.Left = 1
        Marks.Margins.Top = 1
        Marks.Margins.Right = 1
        Marks.Margins.Bottom = 1
        Marks.Shadow.Color = 8553090
        Marks.Visible = False
        SeriesColor = clBlue
        Title = 'Front'
        LinePen.Color = clBlue
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesRearMotor: TFastLineSeries
        Active = False
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Margins.Left = 1
        Marks.Margins.Top = 1
        Marks.Margins.Right = 1
        Marks.Margins.Bottom = 1
        Marks.Visible = False
        SeriesColor = clGreen
        Title = 'Rear'
        LinePen.Color = clGreen
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesTransMotor: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Margins.Left = 1
        Marks.Margins.Top = 1
        Marks.Margins.Right = 1
        Marks.Margins.Bottom = 1
        Marks.Visible = False
        SeriesColor = 4227072
        Title = 'Trans'
        LinePen.Color = 4227072
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesMotorPos: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'Pos'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesMotorPosRange: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'PosRange'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
    object chartVibRms: TChart
      Left = 1
      Top = 192
      Width = 962
      Height = 190
      Gradient.SubGradient.Transparency = 43
      Legend.CheckBoxes = True
      Legend.HorizMargin = 12
      Legend.LegendStyle = lsSeries
      Legend.TextStyle = ltsValue
      Legend.TopPos = 5
      Legend.Visible = False
      MarginBottom = 1
      MarginLeft = 5
      MarginRight = 90
      MarginTop = 5
      MarginUnits = muPixels
      Title.Text.Strings = (
        'TChart')
      Title.Visible = False
      OnClickSeries = chartVibRmsClickSeries
      DepthAxis.Automatic = False
      DepthAxis.AutomaticMaximum = False
      DepthAxis.AutomaticMinimum = False
      DepthAxis.Maximum = 1.070000000000509000
      DepthAxis.Minimum = 0.070000000000501120
      DepthTopAxis.Automatic = False
      DepthTopAxis.AutomaticMaximum = False
      DepthTopAxis.AutomaticMinimum = False
      DepthTopAxis.Maximum = 1.070000000000509000
      DepthTopAxis.Minimum = 0.070000000000501120
      LeftAxis.LabelsSize = 20
      RightAxis.Automatic = False
      RightAxis.AutomaticMaximum = False
      RightAxis.AutomaticMinimum = False
      View3D = False
      Align = alBottom
      TabOrder = 2
      PrintMargins = (
        15
        40
        15
        40)
      ColorPaletteIndex = 13
      object GroupBox1: TGroupBox
        AlignWithMargins = True
        Left = 877
        Top = 4
        Width = 81
        Height = 165
        Margins.Bottom = 20
        Align = alRight
        Caption = 'Vibration (G)'
        TabOrder = 0
        object shVibY: TShape
          Left = 8
          Top = 71
          Width = 10
          Height = 6
          Brush.Color = clLime
        end
        object shVibRMS: TShape
          Left = 8
          Top = 24
          Width = 10
          Height = 6
          Brush.Color = clBlack
        end
        object shVibX: TShape
          Left = 8
          Top = 48
          Width = 10
          Height = 6
          Brush.Color = clYellow
        end
        object shVibZ: TShape
          Left = 8
          Top = 95
          Width = 10
          Height = 6
          Brush.Color = clAqua
        end
        object cbVibRMS: TCheckBox
          Left = 24
          Top = 18
          Width = 65
          Height = 17
          Caption = 'RMS'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 0
          OnClick = cbVibRMSClick
        end
        object cbVibX: TCheckBox
          Left = 24
          Top = 42
          Width = 73
          Height = 17
          Caption = 'X'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 1
          OnClick = cbVibXClick
        end
        object cbVibY: TCheckBox
          Left = 24
          Top = 65
          Width = 73
          Height = 17
          Caption = 'Y'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 2
          OnClick = cbVibYClick
        end
        object cbVibZ: TCheckBox
          Left = 24
          Top = 88
          Width = 73
          Height = 17
          Caption = 'Z'
          Checked = True
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          State = cbChecked
          TabOrder = 3
          OnClick = cbVibZClick
        end
      end
      object SeriesVibRMS: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Emboss.Color = 8618883
        Marks.Shadow.Color = 8618883
        Marks.Visible = False
        SeriesColor = clBlack
        Title = 'RMS'
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesVibX: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Emboss.Color = 8553090
        Marks.Shadow.Color = 8553090
        Marks.Visible = False
        SeriesColor = clYellow
        Title = 'X'
        LinePen.Color = clYellow
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesVibY: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Emboss.Color = 8553090
        Marks.Shadow.Color = 8553090
        Marks.Visible = False
        SeriesColor = clLime
        Title = 'Y'
        LinePen.Color = clLime
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesVibZ: TFastLineSeries
        Cursor = crHandPoint
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Emboss.Color = 8553090
        Marks.Shadow.Color = 8553090
        Marks.Visible = False
        SeriesColor = clAqua
        Title = 'Z'
        LinePen.Color = clAqua
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesVibPos: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Shadow.Color = 8553090
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'Pos'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesSpecOutPoint: TFastLineSeries
        Cursor = crHelp
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = 8388863
        Marks.Callout.Pen.Color = 8388863
        Marks.Callout.Style = psDownTriangle
        Marks.Callout.Visible = True
        Marks.Callout.Arrow.Visible = True
        Marks.Callout.Distance = 3
        Marks.Callout.Length = 0
        Marks.Emboss.Color = 8553090
        Marks.Font.Height = -1
        Marks.Shadow.Color = 8553090
        Marks.Shadow.Visible = False
        Marks.Style = smsSeriesTitle
        Marks.Transparent = True
        Marks.Visible = True
        Title = 'SpecOver'
        LinePen.Color = 10708548
        LinePen.Width = 9
        LinePen.Visible = False
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesVibPosRange: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'PosRange'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object pnSpecOutFilter: TPanel
    Left = 481
    Top = 400
    Width = 306
    Height = 248
    Align = alClient
    BorderStyle = bsSingle
    TabOrder = 3
    object ListBoxSpecOutList: TListBox
      Left = 1
      Top = 65
      Width = 300
      Height = 178
      Align = alClient
      ImeName = 'Microsoft IME 2010'
      ItemHeight = 13
      TabOrder = 0
      OnDblClick = ListBoxSpecOutListDblClick
    end
    object pnSpecOutFilterControl: TPanel
      Left = 1
      Top = 1
      Width = 300
      Height = 64
      Align = alTop
      TabOrder = 1
      object LabelThreshold: TLabel
        Left = 3
        Top = 6
        Width = 36
        Height = 15
        Caption = #49444#51221#44050
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabelThreshold2: TLabel
        Left = 106
        Top = 3
        Width = 8
        Height = 15
        Caption = 'G'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabelThreshold4: TLabel
        Left = 106
        Top = 32
        Width = 8
        Height = 15
        Caption = 'G'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabelThreshold3: TLabel
        Left = 3
        Top = 32
        Width = 36
        Height = 15
        Caption = #51201#50857#51473
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblMax: TLabel
        Left = 217
        Top = 3
        Width = 26
        Height = 16
        Caption = 'Max'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblCnt: TLabel
        Left = 220
        Top = 33
        Width = 22
        Height = 16
        Caption = 'Cnt'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object edtThresholdInput: TEdit
        Left = 62
        Top = 2
        Width = 38
        Height = 23
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        ParentFont = False
        TabOrder = 0
        Text = '0.6'
      end
      object btnThreshold: TButton
        Left = 128
        Top = 2
        Width = 83
        Height = 23
        Caption = #51201#50857
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = btnThresholdClick
      end
      object btnSpecOutListReset: TButton
        Left = 128
        Top = 28
        Width = 83
        Height = 25
        Caption = 'List Reset'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = btnSpecOutListResetClick
      end
      object edtThresholdOutput: TEdit
        Left = 62
        Top = 28
        Width = 38
        Height = 23
        Color = clMoneyGreen
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        ParentFont = False
        ReadOnly = True
        TabOrder = 3
        Text = '0.6'
      end
      object edtMax: TEdit
        Left = 249
        Top = 2
        Width = 38
        Height = 23
        Color = clCream
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        ParentFont = False
        ReadOnly = True
        TabOrder = 4
      end
      object edtCnt: TEdit
        Left = 248
        Top = 31
        Width = 38
        Height = 23
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        ParentFont = False
        ReadOnly = True
        TabOrder = 5
      end
    end
  end
  object TimerCheckConnect: TTimer
    Enabled = False
    Interval = 100
    OnTimer = TimerCheckConnectTimer
    Left = 408
    Top = 144
  end
  object SaveDialog: TSaveDialog
    Left = 832
    Top = 144
  end
  object OpenDialog: TOpenDialog
    Left = 880
    Top = 144
  end
  object tmrUIUpdate: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrUIUpdateTimer
    Left = 888
    Top = 320
  end
  object tmrSendAlive: TTimer
    Enabled = False
    OnTimer = tmrSendAliveTimer
    Left = 832
    Top = 320
  end
end
