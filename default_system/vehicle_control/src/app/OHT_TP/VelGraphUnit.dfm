object VelGraphForm: TVelGraphForm
  Left = 468
  Top = 166
  Caption = 'Motor Velocity Graph'
  ClientHeight = 701
  ClientWidth = 796
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 377
    Width = 796
    Height = 3
    Cursor = crVSplit
    Align = alTop
    ExplicitWidth = 804
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 796
    Height = 97
    Align = alTop
    BevelOuter = bvNone
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 600
      Top = 0
      Width = 200
      Height = 97
      Align = alLeft
      Caption = ' [#4 Rotate] '
      TabOrder = 0
      object Label38: TLabel
        Left = 128
        Top = 24
        Width = 35
        Height = 13
        Caption = '[rpm]'
      end
      object Label2: TLabel
        Left = 128
        Top = 48
        Width = 63
        Height = 13
        Caption = '[Max rpm]'
      end
      object Label4: TLabel
        Left = 128
        Top = 72
        Width = 63
        Height = 13
        Caption = '[Min rpm]'
      end
      object Panel29: TPanel
        Left = 10
        Top = 20
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object lblMotor4_1: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel4: TPanel
        Left = 10
        Top = 44
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object lblMotor4_2: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel5: TPanel
        Left = 10
        Top = 68
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 2
        object lblMotor4_3: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object GroupBox3: TGroupBox
      Left = 200
      Top = 0
      Width = 200
      Height = 97
      Align = alLeft
      Caption = ' [#2 Hoist] '
      TabOrder = 1
      object Label15: TLabel
        Left = 128
        Top = 24
        Width = 35
        Height = 13
        Caption = '[rpm]'
      end
      object Label16: TLabel
        Left = 128
        Top = 48
        Width = 63
        Height = 13
        Caption = '[Max rpm]'
      end
      object Label17: TLabel
        Left = 128
        Top = 72
        Width = 63
        Height = 13
        Caption = '[Min rpm]'
      end
      object Panel11: TPanel
        Left = 10
        Top = 20
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object lblMotor2_1: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel12: TPanel
        Left = 10
        Top = 44
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object lblMotor2_2: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel13: TPanel
        Left = 10
        Top = 68
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 2
        object lblMotor2_3: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object GroupBox4: TGroupBox
      Left = 0
      Top = 0
      Width = 200
      Height = 97
      Align = alLeft
      Caption = ' [#1 Driving] '
      TabOrder = 2
      object Label23: TLabel
        Left = 128
        Top = 16
        Width = 35
        Height = 13
        Caption = '[rpm]'
      end
      object Label24: TLabel
        Left = 128
        Top = 56
        Width = 63
        Height = 13
        Caption = '[Max rpm]'
      end
      object Label25: TLabel
        Left = 128
        Top = 78
        Width = 63
        Height = 13
        Caption = '[Min rpm]'
      end
      object Label3: TLabel
        Left = 128
        Top = 36
        Width = 49
        Height = 13
        Caption = '[m/sec]'
      end
      object Panel15: TPanel
        Left = 10
        Top = 12
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object lblMotor1_1: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel17: TPanel
        Left = 10
        Top = 74
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object lblMotor1_3: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel6: TPanel
        Left = 10
        Top = 30
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clCaptionText
        TabOrder = 2
        object lblMotor1_1_msec: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel16: TPanel
        Left = 10
        Top = 52
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 3
        object lblMotor1_2: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object GroupBox2: TGroupBox
      Left = 400
      Top = 0
      Width = 200
      Height = 97
      Align = alLeft
      Caption = ' [#3 Shift] '
      TabOrder = 3
      object Label7: TLabel
        Left = 128
        Top = 24
        Width = 35
        Height = 13
        Caption = '[rpm]'
      end
      object Label8: TLabel
        Left = 128
        Top = 48
        Width = 63
        Height = 13
        Caption = '[Max rpm]'
      end
      object Label9: TLabel
        Left = 128
        Top = 72
        Width = 63
        Height = 13
        Caption = '[Min rpm]'
      end
      object Panel7: TPanel
        Left = 10
        Top = 20
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 0
        object lblMotor3_1: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel8: TPanel
        Left = 10
        Top = 44
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 1
        object lblMotor3_2: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
      object Panel9: TPanel
        Left = 10
        Top = 68
        Width = 111
        Height = 20
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 2
        object lblMotor3_3: TLabel
          Left = 8
          Top = 3
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
  end
  object panDrivingSpeed: TPanel
    Left = 0
    Top = 97
    Width = 796
    Height = 280
    Align = alTop
    Caption = 'panDrivingSpeed'
    Color = clWhite
    TabOrder = 1
    object chartDrivingSpeed: TChart
      Left = 1
      Top = 1
      Width = 794
      Height = 278
      Legend.Alignment = laTop
      Legend.CheckBoxes = True
      Legend.LegendStyle = lsSeries
      Legend.TextStyle = ltsXValue
      Legend.TopPos = 46
      Title.Text.Strings = (
        'TChart')
      Title.Visible = False
      View3D = False
      Align = alClient
      TabOrder = 0
      PrintMargins = (
        15
        32
        15
        32)
      ColorPaletteIndex = 13
      object chkDisplayBothDrv: TCheckBox
        Left = 9
        Top = 5
        Width = 97
        Height = 17
        Caption = 'Display Driving F_R'
        TabOrder = 0
      end
      object chkTime: TCheckBox
        Left = 540
        Top = 5
        Width = 97
        Height = 17
        Caption = 'Time'
        TabOrder = 1
      end
      object fastlineDriving_F: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        Title = 'Driving_F'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object fastlineDriving_R: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        Title = 'Driving_R'
        LinePen.Color = clGreen
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 380
    Width = 796
    Height = 302
    Align = alClient
    Color = clWhite
    TabOrder = 2
    object chartHoistSpeed: TChart
      Left = 1
      Top = 1
      Width = 794
      Height = 300
      Legend.Alignment = laTop
      Legend.CheckBoxes = True
      Legend.LegendStyle = lsSeries
      Legend.TextStyle = ltsXValue
      Legend.TopPos = 46
      Title.Text.Strings = (
        'Hoist Gragh')
      Title.Visible = False
      View3D = False
      Align = alClient
      TabOrder = 0
      ExplicitHeight = 278
      PrintMargins = (
        15
        32
        15
        32)
      ColorPaletteIndex = 13
      object CheckBox1: TCheckBox
        Left = 9
        Top = 5
        Width = 97
        Height = 17
        Caption = 'Display Hoist'
        TabOrder = 0
      end
      object fastLineHoist: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        Title = 'Hoist'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 682
    Width = 796
    Height = 19
    Panels = <
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end>
  end
  object CheckBox2: TCheckBox
    Left = 120
    Top = 104
    Width = 97
    Height = 17
    Caption = 'if Error then Stop'
    TabOrder = 4
  end
  object chkLogSave1: TCheckBox
    Left = 232
    Top = 104
    Width = 97
    Height = 17
    Caption = 'LogSave[#1]'
    TabOrder = 5
  end
  object MainMenu1: TMainMenu
    Left = 736
    Top = 177
    object Start1: TMenuItem
      Caption = '&Start'
      object N1Start1: TMenuItem
        Caption = '&#1 Start'
        OnClick = N1Start1Click
      end
      object N24Start1: TMenuItem
        Caption = '&#2~4 Start'
        OnClick = N24Start1Click
      end
    end
    object Stop1: TMenuItem
      Caption = '&Stop'
      object N1Stop1: TMenuItem
        Caption = '&#1 Stop'
        OnClick = N1Stop1Click
      end
      object N24Stop1: TMenuItem
        Caption = '&#2~4 Stop'
        OnClick = N24Stop1Click
      end
    end
    object Save1: TMenuItem
      Caption = '&Capture'
      object Motor1Graph1: TMenuItem
        Caption = 'Motor1 Graph'
        OnClick = Motor1Graph1Click
      end
      object Motor24Graph1: TMenuItem
        Caption = 'Motor24 Graph'
        OnClick = Motor24Graph1Click
      end
    end
    object File1: TMenuItem
      Caption = 'File'
      object Open1: TMenuItem
        Caption = 'Open'
        OnClick = Open1Click
      end
    end
  end
  object VelTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = VelTimerTimer
    Left = 209
    Top = 186
  end
  object Motor1Timer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Motor1TimerTimer
    Left = 209
    Top = 234
  end
  object Motor24Timer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Motor24TimerTimer
    Left = 209
    Top = 274
  end
  object OpenDialog1: TOpenDialog
    Left = 736
    Top = 224
  end
  object OpenDialog2: TOpenDialog
    Left = 736
    Top = 272
  end
  object OpenDialog3: TOpenDialog
    Left = 737
    Top = 314
  end
end
