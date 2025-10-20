object APMonitoringForm: TAPMonitoringForm
  Left = 273
  Top = 439
  Caption = 'AP Signal Monitoring'
  ClientHeight = 394
  ClientWidth = 1082
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = mmAPMonitoring
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object chartAPSignal: TChart
    Left = 0
    Top = 0
    Width = 1082
    Height = 394
    Legend.Alignment = laTop
    Legend.CustomPosition = True
    Legend.Left = 935
    Legend.LegendStyle = lsSeries
    Legend.ResizeChart = False
    Legend.Top = 10
    Legend.TopPos = 46
    Title.Text.Strings = (
      'AP Signal Monitoring')
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = 0.559999999999854600
    DepthAxis.Minimum = -0.440000000000138400
    DepthTopAxis.Automatic = False
    DepthTopAxis.AutomaticMaximum = False
    DepthTopAxis.AutomaticMinimum = False
    DepthTopAxis.Maximum = 0.559999999999854600
    DepthTopAxis.Minimum = -0.440000000000138400
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.ExactDateTime = False
    LeftAxis.Increment = 1.000000000000000000
    LeftAxis.Maximum = 10.600000000004560000
    LeftAxis.Minimum = 0.600000000004421900
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    View3D = False
    Align = alClient
    TabOrder = 0
    PrintMargins = (
      15
      32
      15
      32)
    ColorPaletteIndex = 13
    object Label1: TLabel
      Left = 77
      Top = 16
      Width = 85
      Height = 13
      Caption = '[AP Mac Address]'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object AP_MAC: TPanel
      Left = 168
      Top = 12
      Width = 129
      Height = 22
      Caption = '00:00:00:00:00:00'
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentBackground = False
      ParentFont = False
      TabOrder = 0
    end
    object seriseAPSignal: TLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      LinePen.Color = 10708548
      Pointer.Brush.Gradient.EndColor = 10708548
      Pointer.Gradient.EndColor = 10708548
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object mmAPMonitoring: TMainMenu
    Left = 192
    Top = 185
    object mniStart: TMenuItem
      Caption = '&Start'
      OnClick = mniStartClick
    end
    object mniStop: TMenuItem
      Caption = '&Stop'
      OnClick = mniStopClick
    end
    object mniCapture: TMenuItem
      Caption = '&Capture'
      OnClick = mniCaptureClick
    end
  end
  object timerAPSignal: TTimer
    Enabled = False
    Interval = 200
    OnTimer = timerAPSignalTimer
    Left = 233
    Top = 170
  end
  object dlgOpenFile: TOpenDialog
    Left = 152
    Top = 176
  end
end
