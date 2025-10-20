object HIDVelSecForm: THIDVelSecForm
  Left = 62
  Top = 0
  Caption = 'HIDVel[vel vs sec]'
  ClientHeight = 690
  ClientWidth = 1186
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Chart1: TChart
    Left = 0
    Top = 0
    Width = 920
    Height = 690
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    Legend.Alignment = laTop
    Legend.ColorWidth = 45
    Legend.Font.Charset = ANSI_CHARSET
    Legend.Font.Height = -13
    Legend.Font.Name = #48148#53461#52404
    Legend.Shadow.HorizSize = 0
    Legend.Shadow.VertSize = 0
    Legend.Symbol.Width = 45
    Legend.TopPos = 39
    Legend.VertMargin = 2
    Legend.Visible = False
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
    BottomAxis.ExactDateTime = False
    BottomAxis.Increment = 0.010000000000000000
    BottomAxis.Title.Caption = '[sec]'
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.ExactDateTime = False
    LeftAxis.Increment = 0.100000000000000000
    LeftAxis.Maximum = 3.400000000000000000
    LeftAxis.Minimum = -0.200000000000000000
    LeftAxis.Title.Caption = '[m/sec]'
    View3D = False
    View3DWalls = False
    Align = alClient
    BevelOuter = bvNone
    Color = clWhite
    TabOrder = 0
    ExplicitHeight = 670
    ColorPaletteIndex = 13
    object Series1: TFastLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      SeriesColor = clRed
      Title = 'OHT1'
      LinePen.Color = clRed
      LinePen.Width = 2
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series2: TFastLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      SeriesColor = clBlue
      Title = 'OHT2'
      LinePen.Color = clBlue
      LinePen.Width = 2
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object Series3: TFastLineSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Visible = False
      SeriesColor = 8388672
      Title = 'OHT3'
      LinePen.Color = 8388672
      LinePen.Width = 2
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object PageControl1: TPageControl
    Left = 920
    Top = 0
    Width = 266
    Height = 690
    ActivePage = TabSheet1
    Align = alRight
    TabOrder = 1
    ExplicitHeight = 670
    object TabSheet1: TTabSheet
      Caption = 'OHT1'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object memHIDLog1: TMemo
        Left = 0
        Top = 0
        Width = 258
        Height = 642
        Align = alClient
        BorderStyle = bsNone
        ImeName = 'Microsoft Office IME 2007'
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'OHT2'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object memHIDLog2: TMemo
        Left = 0
        Top = 0
        Width = 258
        Height = 669
        Align = alClient
        BorderStyle = bsNone
        Color = 14935011
        ImeName = 'Microsoft Office IME 2007'
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'OHT3'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object memHIDLog3: TMemo
        Left = 0
        Top = 0
        Width = 258
        Height = 669
        Align = alClient
        BorderStyle = bsNone
        Color = 10548218
        ImeName = 'Microsoft Office IME 2007'
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
    end
  end
  object MainMenu1: TMainMenu
    Left = 144
    Top = 64
    object Draw1: TMenuItem
      Caption = 'Draw'
      object Drawsec1: TMenuItem
        Caption = 'Draw[sec]'
        OnClick = Drawsec1Click
      end
      object Drawmm1: TMenuItem
        Caption = 'Draw[mm]'
        OnClick = Drawmm1Click
      end
    end
    object Clear1: TMenuItem
      Caption = 'Clear'
      OnClick = Clear1Click
    end
    object Capture1: TMenuItem
      Caption = 'Capture'
      OnClick = Capture1Click
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 152
    Top = 176
  end
end
