object FFTViewerForm: TFFTViewerForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'FFT Viewer'
  ClientHeight = 359
  ClientWidth = 925
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlTopMenu: TPanel
    Left = 0
    Top = 0
    Width = 925
    Height = 59
    Align = alTop
    TabOrder = 0
    object pnlTopLeft: TPanel
      Left = 1
      Top = 1
      Width = 280
      Height = 57
      Align = alLeft
      TabOrder = 0
      object ledtEndPosition: TLabeledEdit
        Left = 100
        Top = 18
        Width = 74
        Height = 21
        EditLabel.Width = 45
        EditLabel.Height = 13
        EditLabel.Caption = 'End (sec)'
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 0
      end
      object ledtStartPosition: TLabeledEdit
        Left = 9
        Top = 18
        Width = 73
        Height = 21
        EditLabel.Width = 51
        EditLabel.Height = 13
        EditLabel.Caption = 'Start (sec)'
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 1
      end
      object btClear: TButton
        Left = 200
        Top = 1
        Width = 79
        Height = 55
        Align = alRight
        Caption = 'Clear'
        TabOrder = 2
        OnClick = btClearClick
      end
    end
    object Panel1: TPanel
      Left = 281
      Top = 1
      Width = 643
      Height = 57
      Align = alClient
      TabOrder = 1
      object rgTopFreqGroup: TRadioGroup
        Left = 1
        Top = 1
        Width = 641
        Height = 55
        Align = alClient
        Caption = 'Top Amplitude Frequency'
        Columns = 5
        Items.Strings = (
          '1'
          '2'
          '3'
          '4'
          '5')
        TabOrder = 0
        OnClick = rgTopFreqGroupClick
      end
    end
  end
  object pnlChartArea: TPanel
    Left = 0
    Top = 59
    Width = 925
    Height = 300
    Align = alClient
    Caption = 'pnlChartArea'
    TabOrder = 1
    object chtFFT: TChart
      Left = 1
      Top = 1
      Width = 923
      Height = 298
      AllowPanning = pmHorizontal
      Legend.Visible = False
      Title.Text.Strings = (
        'TChart')
      BottomAxis.Automatic = False
      BottomAxis.AutomaticMaximum = False
      BottomAxis.AutomaticMinimum = False
      BottomAxis.Maximum = 200.000000000000000000
      BottomAxis.Title.Caption = 'Frequency (Hz)'
      LeftAxis.Automatic = False
      LeftAxis.AutomaticMaximum = False
      LeftAxis.Maximum = 1.000000000000000000
      LeftAxis.Title.Caption = 'Amplitude(A)'
      TopAxis.Labels = False
      TopAxis.LabelsOnAxis = False
      View3D = False
      Zoom.Allow = False
      Align = alClient
      TabOrder = 0
      ColorPaletteIndex = 3
      object SeriesFFTDataPoint: TPointSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = clRed
        Title = 'SeriesFFTDataPoint'
        ClickableLine = False
        Pointer.Brush.Gradient.EndColor = clRed
        Pointer.Gradient.EndColor = clRed
        Pointer.InflateMargins = True
        Pointer.Style = psDownTriangle
        Pointer.Visible = True
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object SeriesFFTDataLine: TLineSeries
        Marks.Arrow.Visible = True
        Marks.Callout.Brush.Color = clBlack
        Marks.Callout.Arrow.Visible = True
        Marks.Visible = False
        SeriesColor = 8388863
        LinePen.Color = 8388863
        Pointer.InflateMargins = True
        Pointer.Style = psRectangle
        Pointer.Visible = False
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
end
