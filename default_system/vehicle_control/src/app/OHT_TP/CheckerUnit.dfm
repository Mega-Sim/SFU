object UnitChecker: TUnitChecker
  Left = 0
  Top = 0
  Caption = 'UnitChecker'
  ClientHeight = 687
  ClientWidth = 855
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object chkTest: TCheckBox
    Left = 553
    Top = 64
    Width = 97
    Height = 17
    Caption = 'chkTest'
    TabOrder = 14
    Visible = False
  end
  object grpTextZone: TGroupBox
    Left = 8
    Top = 5
    Width = 769
    Height = 81
    Color = clWhite
    ParentBackground = False
    ParentColor = False
    TabOrder = 5
    object shpTotalPopup: TShape
      Left = 3
      Top = 3
      Width = 760
      Height = 50
    end
    object lblTextPopup: TLabel
      Left = 19
      Top = 19
      Width = 5
      Height = 19
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblTimeLimit: TLabel
      Left = 648
      Top = 59
      Width = 57
      Height = 13
      Caption = #51228#54620' '#49884#44036' : '
    end
    object lblLimitTime: TLabel
      Left = 711
      Top = 59
      Width = 49
      Height = 13
      Caption = '00'#48516' 00'#52488
    end
  end
  object grpSTBDetect: TGroupBox
    Left = 430
    Top = 457
    Width = 420
    Height = 121
    Caption = 'STB Detect '#44277#52636#54616' / '#51060#51473#51201#51116
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
    object shpSTBNoDone: TShape
      Left = 243
      Top = 91
      Width = 88
      Height = 24
    end
    object shpSTBDoubleDone: TShape
      Left = 329
      Top = 91
      Width = 88
      Height = 24
    end
    object lblSTBDoubleDone: TLabel
      Left = 351
      Top = 97
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object lblSTBNoDone: TLabel
      Left = 271
      Top = 97
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object pnl1STBDetect: TPanel
      Left = 241
      Top = 8
      Width = 176
      Height = 83
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpSTBLDetect: TShape
        Left = 5
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object shpSTBRDetect: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object lblSTBRDetect: TLabel
        Left = 110
        Top = 6
        Width = 38
        Height = 15
        Caption = 'STB_R'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object lblSTBLDetect: TLabel
        Left = 24
        Top = 6
        Width = 37
        Height = 15
        Caption = 'STB_L'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnSTBNoCarrier: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44277#52636#54616
        TabOrder = 0
        OnClick = btnSTBNoCarrierClick
      end
      object btnSTBDoubleCarrier: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #51060#51473#51201#51116
        TabOrder = 1
        OnClick = btnSTBDoubleCarrierClick
      end
      object btnSTBDetectEnd: TButton
        Left = 2
        Top = 52
        Width = 171
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 2
        OnClick = btnSTBDetectEndClick
      end
    end
    object strngrdSTBDetect: TStringGrid
      Left = 3
      Top = 63
      Width = 234
      Height = 54
      ColCount = 3
      RowCount = 2
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20)
    end
    object chkSTBDirection: TCheckBox
      Left = 16
      Top = 32
      Width = 145
      Height = 17
      Caption = 'STB '#48169#54693' '#49440#53469'('#52404#53356' '#49884' R)'
      TabOrder = 2
    end
  end
  object btnSMPSCheck: TBitBtn
    Left = 311
    Top = 92
    Width = 113
    Height = 34
    Caption = 'SMPSVoltateCheck'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    OnClick = btnSMPSCheckClick
  end
  object grpSteer: TGroupBox
    Left = 8
    Top = 132
    Width = 420
    Height = 130
    Caption = 'Steer'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object grpSteeringIO: TGroupBox
      Left = 241
      Top = 15
      Width = 176
      Height = 80
      TabOrder = 0
      object Panel18: TPanel
        Left = 87
        Top = 2
        Width = 86
        Height = 45
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object RStrRightLt: TShape
          Left = 2
          Top = 22
          Width = 82
          Height = 20
          Align = alTop
          Shape = stRoundRect
          ExplicitLeft = 4
          ExplicitTop = 17
        end
        object FStrRightLt: TShape
          Left = 2
          Top = 2
          Width = 82
          Height = 20
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 77
        end
        object Label17: TLabel
          Left = 22
          Top = 7
          Width = 42
          Height = 15
          Caption = ' F-Right'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label18: TLabel
          Left = 21
          Top = 28
          Width = 44
          Height = 15
          Caption = ' R-Right'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object Panel19: TPanel
        Left = 0
        Top = 2
        Width = 86
        Height = 45
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object RStrLeftLt: TShape
          Left = 2
          Top = 22
          Width = 82
          Height = 20
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 80
        end
        object FStrLeftLt: TShape
          Left = 2
          Top = 2
          Width = 82
          Height = 20
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 80
        end
        object Label19: TLabel
          Left = 20
          Top = 6
          Width = 38
          Height = 15
          Caption = '  F-Left'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label20: TLabel
          Left = 20
          Top = 27
          Width = 40
          Height = 15
          Caption = '  R-Left'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object btnStart: TButton
        Left = 2
        Top = 50
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 2
        OnClick = btnStartClick
      end
      object btnStop: TButton
        Left = 88
        Top = 50
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 3
        OnClick = btnStopClick
      end
    end
    object mmoMemo: TMemo
      Left = 240
      Top = 98
      Width = 176
      Height = 26
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object strngrd1: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 109
      ColCount = 3
      RowCount = 6
      ScrollBars = ssVertical
      TabOrder = 2
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        22
        22
        23)
    end
  end
  object grpShiftHome: TGroupBox
    Left = 8
    Top = 262
    Width = 420
    Height = 110
    Caption = 'ShiftHome'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    object GroupBox1: TGroupBox
      Left = 240
      Top = 8
      Width = 176
      Height = 70
      TabOrder = 0
      object Panel2: TPanel
        Left = 1
        Top = 6
        Width = 172
        Height = 35
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object Shp_ShiftHome: TShape
          Left = 2
          Top = 2
          Width = 168
          Height = 25
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 80
        end
        object Label3: TLabel
          Left = 47
          Top = 7
          Width = 63
          Height = 15
          Caption = 'Shift HOME'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object btnShiftHomeStart: TButton
        Left = 2
        Top = 35
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 1
        OnClick = btnShiftHomeStartClick
      end
      object btnShiftHomeStop: TButton
        Left = 88
        Top = 35
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 2
        OnClick = btnShiftHomeStopClick
      end
    end
    object mmoShiftHome: TMemo
      Left = 240
      Top = 73
      Width = 176
      Height = 32
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object strngrdShiftHome: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 90
      ColCount = 3
      RowCount = 4
      ScrollBars = ssNone
      TabOrder = 2
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        22)
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = -8
    Width = 420
    Height = 101
    Caption = 'HoistHome'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Visible = False
    object GroupBox3: TGroupBox
      Left = 240
      Top = 15
      Width = 176
      Height = 70
      TabOrder = 0
      object Panel1: TPanel
        Left = 3
        Top = 3
        Width = 170
        Height = 55
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object ShpHoist: TShape
          Left = 88
          Top = 2
          Width = 81
          Height = 25
          Shape = stRoundRect
        end
        object Label1: TLabel
          Left = 93
          Top = 6
          Width = 66
          Height = 15
          Caption = 'Hoist HOME'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object lbl1: TLabel
          Left = 47
          Top = 8
          Width = 38
          Height = 13
          Caption = '*0.1mm'
        end
        object btnHoistStart: TButton
          Left = 2
          Top = 27
          Width = 85
          Height = 25
          Caption = #44160#49324#49884#51089
          Enabled = False
          TabOrder = 0
          OnClick = btnHoistStartClick
        end
        object btnHoistStop: TButton
          Left = 88
          Top = 27
          Width = 85
          Height = 25
          Caption = #44160#49324#51473#51648
          Enabled = False
          TabOrder = 1
          OnClick = btnHoistStopClick
        end
        object edtHoistStandard: TEdit
          Left = 1
          Top = 3
          Width = 45
          Height = 21
          ImeName = 'Microsoft IME 2010'
          NumbersOnly = True
          TabOrder = 2
          Text = '90'
        end
      end
    end
    object mmoHoist: TMemo
      Left = 240
      Top = 76
      Width = 176
      Height = 21
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object strngrdHoist: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 81
      ColCount = 3
      RowCount = 3
      ScrollBars = ssNone
      TabOrder = 2
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22)
    end
  end
  object grpEStop: TGroupBox
    Left = 7
    Top = 373
    Width = 420
    Height = 110
    Caption = 'E-Stop / BumperPress'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    object shpEStopDone: TShape
      Left = 240
      Top = 70
      Width = 176
      Height = 24
    end
    object lblEStopDone: TLabel
      Left = 310
      Top = 75
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object pnlEStop: TPanel
      Left = 240
      Top = 15
      Width = 176
      Height = 55
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpBumperPress: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object shpEStop: TShape
        Left = 2
        Top = 2
        Width = 84
        Height = 25
        Shape = stRoundRect
      end
      object lblEStop: TLabel
        Left = 26
        Top = 6
        Width = 37
        Height = 15
        Caption = 'E-Stop'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object lblBumperPress: TLabel
        Left = 114
        Top = 6
        Width = 45
        Height = 15
        Caption = 'Bumper'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnEStopStart: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 0
        OnClick = btnEStopStartClick
      end
      object btnEStopEnd: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 1
        OnClick = btnEStopEndCick
      end
    end
    object strngrdEStop1: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 90
      ColCount = 3
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        24
        24)
    end
  end
  object grpCheckAll: TGroupBox
    Left = 430
    Top = 92
    Width = 420
    Height = 110
    Color = clWhite
    ParentBackground = False
    ParentColor = False
    TabOrder = 6
    object lblCurrentTime: TLabel
      Left = 96
      Top = 10
      Width = 183
      Height = 16
      Caption = #54788#51116' '#49884#44036' : 0000-00-00 00:00:00'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lblYSCode: TLabel
      Left = 16
      Top = 68
      Width = 28
      Height = 19
      Caption = 'YS-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblVHLName: TLabel
      Left = 16
      Top = 40
      Width = 96
      Height = 19
      Caption = 'VHL NAME :'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object chkRailTest: TCheckBox
      Left = 296
      Top = 9
      Width = 121
      Height = 17
      Caption = #54408#51656#44160#49688' '#50668#48512
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object edtYSCode: TEdit
      Left = 50
      Top = 65
      Width = 359
      Height = 27
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
    end
  end
  object grpOscilation: TGroupBox
    Left = 430
    Top = 193
    Width = 420
    Height = 124
    Caption = 'Oscilation'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
    object shpOscilationDone: TShape
      Left = 240
      Top = 70
      Width = 176
      Height = 24
    end
    object lblOscilationDone: TLabel
      Left = 310
      Top = 75
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object lblOscilationTime: TLabel
      Left = 243
      Top = 100
      Width = 166
      Height = 14
      Caption = #44048#51648'~Error '#48156#49373' '#49884#44036' : 00.00'#52488
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object pnlOscilationDone: TPanel
      Left = 240
      Top = 15
      Width = 176
      Height = 55
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpOscilation: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object lblOsclation: TLabel
        Left = 106
        Top = 6
        Width = 57
        Height = 15
        Caption = 'Oscilation'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnOscilationStart: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 0
        OnClick = btnOscilationStartClick
      end
      object btnOsclationEnd: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 1
        OnClick = btnOsclationEndClick
      end
    end
    object strngrdOscilation: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 106
      ColCount = 3
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        24
        24)
    end
  end
  object grpLookDown: TGroupBox
    Left = 420
    Top = 323
    Width = 420
    Height = 128
    Caption = 'LookDown'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
    object shpLookDownDone: TShape
      Left = 240
      Top = 70
      Width = 176
      Height = 24
    end
    object lblLookDownDone: TLabel
      Left = 310
      Top = 75
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object lblLookDownTime: TLabel
      Left = 243
      Top = 100
      Width = 166
      Height = 14
      Caption = #44048#51648'~Error '#48156#49373' '#49884#44036' : 00.00'#52488
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object pnlLookDown: TPanel
      Left = 240
      Top = 15
      Width = 176
      Height = 55
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpLookDown: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object lblLookDown: TLabel
        Left = 100
        Top = 6
        Width = 60
        Height = 15
        Caption = 'LookDown'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnLookDownStart: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 0
        OnClick = btnLookDownStartClick
      end
      object btnLookDownEnd: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 1
        OnClick = btnLookDownEndClick
      end
    end
    object strngrdLookDown: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 110
      ColCount = 3
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        24
        24)
    end
  end
  object btnClose: TButton
    Left = 778
    Top = 8
    Width = 72
    Height = 75
    Caption = #45803#44592
    TabOrder = 10
    OnClick = btnCloseClick
  end
  object grpEQPort: TGroupBox
    Left = 430
    Top = 580
    Width = 420
    Height = 105
    Caption = 'EQ Port '#44277#52636#54616' / '#51060#51473#51201#51116
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
    object shpEQNoDone: TShape
      Left = 243
      Top = 75
      Width = 88
      Height = 24
    end
    object shpEQDoubleDone: TShape
      Left = 329
      Top = 75
      Width = 88
      Height = 24
    end
    object lblEQDoubleDone: TLabel
      Left = 351
      Top = 81
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object lblEQNoDone: TLabel
      Left = 271
      Top = 81
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object pnlEQ: TPanel
      Left = 241
      Top = 16
      Width = 176
      Height = 57
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object btnEQNo: TButton
        Left = 2
        Top = 3
        Width = 85
        Height = 25
        Caption = #44277#52636#54616
        TabOrder = 0
        OnClick = btnEQNoClick
      end
      object btnEQDone: TButton
        Left = 88
        Top = 3
        Width = 85
        Height = 25
        Caption = #51060#51473#51201#51116
        TabOrder = 1
        OnClick = btnEQDoneClick
      end
      object btnEQStop: TButton
        Left = 2
        Top = 28
        Width = 171
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 2
        OnClick = btnEQStopClick
      end
    end
    object strngrdEQ: TStringGrid
      Left = 3
      Top = 16
      Width = 234
      Height = 81
      ColCount = 3
      RowCount = 3
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22)
    end
  end
  object grpFoup: TGroupBox
    Left = 8
    Top = 484
    Width = 420
    Height = 99
    Caption = 'Foup Detect'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 12
    object shpFoupDetectDone: TShape
      Left = 240
      Top = 70
      Width = 176
      Height = 24
    end
    object lblFoupDetectDone: TLabel
      Left = 310
      Top = 75
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object pnlFoup: TPanel
      Left = 240
      Top = 15
      Width = 176
      Height = 55
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpFoupDetect: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object lblFoupDetect: TLabel
        Left = 100
        Top = 6
        Width = 64
        Height = 15
        Caption = 'FoupDetect'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnFoupDetectStart: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 0
        OnClick = btnFoupDetectStartClick
      end
      object btnFoupDetectStop: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 1
        OnClick = btnFoupDetectStopClick
      end
    end
    object strngrdFoupDetect: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 80
      ColCount = 3
      RowCount = 4
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22
        24)
    end
  end
  object grpFoupCover: TGroupBox
    Left = 8
    Top = 584
    Width = 420
    Height = 99
    Caption = 'Foup Cover'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 13
    object shpFoupCoverDone: TShape
      Left = 240
      Top = 70
      Width = 176
      Height = 24
    end
    object lblFoupCoverDone: TLabel
      Left = 310
      Top = 75
      Width = 36
      Height = 15
      Caption = #48120#51652#54665
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object pnlFoupCover: TPanel
      Left = 240
      Top = 15
      Width = 176
      Height = 55
      Align = alCustom
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object shpFoupCover: TShape
        Left = 90
        Top = 2
        Width = 82
        Height = 25
        Shape = stRoundRect
      end
      object lblFoupCover: TLabel
        Left = 100
        Top = 6
        Width = 60
        Height = 15
        Caption = 'FoupCover'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object btnFoupCoverStart: TButton
        Left = 2
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#49884#51089
        TabOrder = 0
        OnClick = btnFoupCoverStartClick
      end
      object btnFoupCoverStop: TButton
        Left = 88
        Top = 27
        Width = 85
        Height = 25
        Caption = #44160#49324#51473#51648
        TabOrder = 1
        OnClick = btnFoupCoverStopClick
      end
    end
    object strngrdFoupCover: TStringGrid
      Left = 3
      Top = 15
      Width = 234
      Height = 80
      ColCount = 3
      RowCount = 3
      ScrollBars = ssNone
      TabOrder = 1
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22)
    end
  end
  object btnUBGOBS: TButton
    Left = 195
    Top = 92
    Width = 110
    Height = 34
    Caption = 'UBG/OBS Check'
    TabOrder = 15
    OnClick = btnUBGOBSClick
  end
  object btnCIDCheck: TButton
    Left = 79
    Top = 92
    Width = 110
    Height = 34
    Caption = 'CID Check'
    TabOrder = 16
    OnClick = btnCIDCheckClick
  end
  object tmrIOUI: TTimer
    Interval = 200
    OnTimer = tmrIOUITimer
    Left = 192
    Top = 144
  end
  object tmrSimulation: TTimer
    Enabled = False
    Interval = 1500
    OnTimer = tmrSimulationTimer
    Left = 192
    Top = 184
  end
  object tmrSimulationShift: TTimer
    Enabled = False
    OnTimer = tmrSimulationShiftTimer
    Left = 184
    Top = 272
  end
  object tmrSimulationHoist: TTimer
    Enabled = False
    OnTimer = tmrSimulationHoistTimer
    Left = 192
  end
  object tmrEStop: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrEStopTimer
    Left = 163
    Top = 394
  end
  object tmrOscilation: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrOscilationTimer
    Left = 536
    Top = 233
  end
  object tmrLookDown: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrLookDownTimer
    Left = 563
    Top = 356
  end
  object tmrSTBNo: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrSTBNoTimer
    Left = 584
    Top = 499
  end
  object tmrSTBDouble: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrSTBDoubleTimer
    Left = 632
    Top = 499
  end
  object tmrEQNo: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrEQNoTimer
    Left = 608
    Top = 613
  end
  object tmrEQDouble: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrEQDoubleTimer
    Left = 640
    Top = 604
  end
  object tmrFoupDetect: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrFoupDetectTimer
    Left = 112
    Top = 512
  end
  object tmrFoupCover: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrFoupCoverTimer
    Left = 184
    Top = 624
  end
end
