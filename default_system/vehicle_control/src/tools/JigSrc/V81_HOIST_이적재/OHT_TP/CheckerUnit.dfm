object UnitChecker: TUnitChecker
  Left = 0
  Top = 0
  Caption = 'UnitChecker'
  ClientHeight = 610
  ClientWidth = 584
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object btnSMPSCheck: TBitBtn
    Left = 336
    Top = 569
    Width = 113
    Height = 33
    Caption = 'SMPSVoltateCheck'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    OnClick = btnSMPSCheckClick
  end
  object grpSteer: TGroupBox
    Left = 8
    Top = 8
    Width = 563
    Height = 185
    Caption = 'Steer'
    TabOrder = 1
    object btnStart: TButton
      Left = 447
      Top = 32
      Width = 105
      Height = 41
      Caption = #44160#49324#49884#51089
      TabOrder = 0
      OnClick = btnStartClick
    end
    object btnStop: TButton
      Left = 447
      Top = 79
      Width = 105
      Height = 43
      Caption = #44160#49324#51473#51648
      TabOrder = 1
      OnClick = btnStopClick
    end
    object grpSteeringIO: TGroupBox
      Left = 264
      Top = 17
      Width = 172
      Height = 80
      Caption = 'SteeringIO'
      TabOrder = 2
      object Panel18: TPanel
        Left = 91
        Top = 17
        Width = 81
        Height = 58
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object RStrRightLt: TShape
          Left = 2
          Top = 27
          Width = 77
          Height = 25
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 52
        end
        object FStrRightLt: TShape
          Left = 2
          Top = 2
          Width = 77
          Height = 25
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 52
        end
        object Label17: TLabel
          Left = 14
          Top = 8
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
          Left = 13
          Top = 32
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
        Left = 4
        Top = 17
        Width = 84
        Height = 58
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object RStrLeftLt: TShape
          Left = 2
          Top = 27
          Width = 80
          Height = 25
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 52
        end
        object FStrLeftLt: TShape
          Left = 2
          Top = 2
          Width = 80
          Height = 25
          Align = alTop
          Shape = stRoundRect
          ExplicitWidth = 52
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
          Top = 33
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
    end
    object mmoMemo: TMemo
      Left = 264
      Top = 98
      Width = 177
      Height = 84
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 3
    end
    object strngrd1: TStringGrid
      Left = 3
      Top = 17
      Width = 255
      Height = 165
      ColCount = 3
      RowCount = 6
      ScrollBars = ssNone
      TabOrder = 4
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
  object btnClose: TButton
    Left = 455
    Top = 569
    Width = 105
    Height = 33
    Caption = #45803#44592
    TabOrder = 2
    OnClick = btnCloseClick
  end
  object grpShiftHome: TGroupBox
    Left = 8
    Top = 208
    Width = 568
    Height = 145
    Caption = 'ShiftHome'
    TabOrder = 3
    object GroupBox1: TGroupBox
      Left = 267
      Top = 3
      Width = 174
      Height = 70
      Caption = 'ShiftHomeIO'
      TabOrder = 0
      object Panel2: TPanel
        Left = 3
        Top = 15
        Width = 168
        Height = 52
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object Shp_ShiftHome: TShape
          Left = 2
          Top = 2
          Width = 164
          Height = 41
          Align = alTop
          Shape = stRoundRect
          ExplicitTop = 8
          ExplicitWidth = 75
        end
        object Label3: TLabel
          Left = 47
          Top = 15
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
    end
    object mmoShiftHome: TMemo
      Left = 270
      Top = 73
      Width = 171
      Height = 69
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object btnShiftHomeStart: TButton
      Left = 447
      Top = 16
      Width = 105
      Height = 41
      Caption = #44160#49324#49884#51089
      TabOrder = 2
      OnClick = btnShiftHomeStartClick
    end
    object btnShiftHomeStop: TButton
      Left = 447
      Top = 63
      Width = 105
      Height = 43
      Caption = #44160#49324#51473#51648
      TabOrder = 3
      OnClick = btnShiftHomeStopClick
    end
    object strngrdShiftHome: TStringGrid
      Left = 6
      Top = 16
      Width = 255
      Height = 126
      ColCount = 3
      RowCount = 4
      ScrollBars = ssNone
      TabOrder = 4
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
    Top = 376
    Width = 568
    Height = 145
    Caption = 'HoistHome'
    TabOrder = 4
    object Label2: TLabel
      Left = 517
      Top = 117
      Width = 38
      Height = 13
      Caption = '*0.1mm'
    end
    object GroupBox3: TGroupBox
      Left = 267
      Top = 3
      Width = 174
      Height = 70
      Caption = 'HoistHomeIO'
      TabOrder = 0
      object Panel1: TPanel
        Left = 3
        Top = 15
        Width = 168
        Height = 52
        Align = alCustom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object ShpHoist: TShape
          Left = 2
          Top = 2
          Width = 164
          Height = 41
          Align = alTop
          Shape = stRoundRect
          ExplicitTop = 8
          ExplicitWidth = 75
        end
        object Label1: TLabel
          Left = 47
          Top = 15
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
      end
    end
    object mmoHoist: TMemo
      Left = 270
      Top = 67
      Width = 171
      Height = 69
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object btnHoistStart: TButton
      Left = 447
      Top = 16
      Width = 105
      Height = 41
      Caption = #44160#49324#49884#51089
      Enabled = False
      TabOrder = 2
      OnClick = btnHoistStartClick
    end
    object btnHoistStop: TButton
      Left = 447
      Top = 63
      Width = 105
      Height = 43
      Caption = #44160#49324#51473#51648
      Enabled = False
      TabOrder = 3
      OnClick = btnHoistStopClick
    end
    object strngrdHoist: TStringGrid
      Left = 6
      Top = 16
      Width = 255
      Height = 126
      ColCount = 3
      RowCount = 3
      ScrollBars = ssNone
      TabOrder = 4
      ColWidths = (
        64
        121
        64)
      RowHeights = (
        24
        20
        22)
    end
    object edtHoistStandard: TEdit
      Left = 466
      Top = 112
      Width = 45
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 5
      Text = '90'
    end
  end
  object tmrIOUI: TTimer
    Interval = 200
    OnTimer = tmrIOUITimer
    Left = 280
    Top = 560
  end
  object tmrSimulation: TTimer
    Enabled = False
    Interval = 1500
    OnTimer = tmrSimulationTimer
    Left = 544
    Top = 176
  end
  object tmrSimulationShift: TTimer
    Enabled = False
    OnTimer = tmrSimulationShiftTimer
    Left = 544
    Top = 336
  end
  object tmrSimulationHoist: TTimer
    Enabled = False
    OnTimer = tmrSimulationHoistTimer
    Left = 544
    Top = 512
  end
end
