object HIDVelForm: THIDVelForm
  Left = 80
  Top = 203
  Caption = 'HIDVel[vel vs cts]'
  ClientHeight = 701
  ClientWidth = 1184
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 923
    Height = 701
    Align = alClient
    Caption = 'Panel1'
    TabOrder = 0
    object Chart1: TChart
      Left = 1
      Top = 1
      Width = 921
      Height = 699
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
      BottomAxis.Title.Caption = '[mm]'
      LeftAxis.Automatic = False
      LeftAxis.AutomaticMaximum = False
      LeftAxis.AutomaticMinimum = False
      LeftAxis.ExactDateTime = False
      LeftAxis.Increment = 0.100000000000000000
      LeftAxis.Maximum = 4.000000000000000000
      LeftAxis.Minimum = -0.200000000000000000
      LeftAxis.Title.Caption = '[m/sec]'
      View3D = False
      View3DWalls = False
      Align = alClient
      BevelOuter = bvNone
      Color = clWhite
      TabOrder = 0
      ColorPaletteIndex = 13
      object Series1: TFastLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'Driving_F'
        LinePen.Color = clRed
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object Panel2: TPanel
    Left = 923
    Top = 0
    Width = 261
    Height = 701
    Align = alRight
    Caption = 'Panel2'
    TabOrder = 1
    object memHIDLog: TMemo
      Left = 1
      Top = 1
      Width = 259
      Height = 699
      Align = alClient
      BorderStyle = bsNone
      ImeName = 'Microsoft Office IME 2007'
      ScrollBars = ssBoth
      TabOrder = 0
      WordWrap = False
    end
  end
  object MainMenu1: TMainMenu
    Left = 144
    Top = 64
    object Draw1: TMenuItem
      Caption = 'Draw'
      OnClick = Draw1Click
    end
    object Clear1: TMenuItem
      Caption = 'Clear'
      OnClick = Clear1Click
    end
  end
end
