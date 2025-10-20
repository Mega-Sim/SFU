object frm_AutoBayTest: Tfrm_AutoBayTest
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = 'AutoBayTestUnit'
  ClientHeight = 702
  ClientWidth = 642
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object shp_Status: TShape
    Left = 310
    Top = 18
    Width = 152
    Height = 21
    Brush.Color = clLime
  end
  object lblMode: TLabel
    Left = 8
    Top = 16
    Width = 56
    Height = 19
    Caption = #51652#54665#47784#46300
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblStatus: TLabel
    Left = 250
    Top = 16
    Width = 28
    Height = 19
    Caption = #49345#53468
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblIndex: TLabel
    Left = 8
    Top = 62
    Width = 56
    Height = 19
    Caption = #51652#54665#44221#47196
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblProgress: TLabel
    Left = 22
    Top = 117
    Width = 42
    Height = 19
    Caption = #51652#54665#47456
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblMessage: TLabel
    Left = 8
    Top = 587
    Width = 54
    Height = 17
    Caption = 'Message'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblTestStatus: TLabel
    Left = 310
    Top = 17
    Width = 48
    Height = 21
    Alignment = taCenter
    Caption = #45824#44592#51473
    Color = clBtnShadow
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
  end
  object lblLongrun: TLabel
    Left = 250
    Top = 62
    Width = 28
    Height = 19
    Caption = #54924#49688
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnRouteRequest: TButton
    Left = 488
    Top = 16
    Width = 142
    Height = 33
    Caption = #44221#47196#50836#52397
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = btnRouteRequestClick
  end
  object btnStartButton: TButton
    Left = 8
    Top = 160
    Width = 142
    Height = 41
    Caption = #52376#51020#48512#53552' '#49884#51089
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    OnClick = btnStartButtonClick
  end
  object btnStartFromIndex: TButton
    Left = 168
    Top = 160
    Width = 142
    Height = 41
    Caption = #51060#50612#49436' '#49884#51089
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btnStartFromIndexClick
  end
  object btnSequenceStop: TButton
    Left = 488
    Top = 160
    Width = 142
    Height = 41
    Caption = #49884#53248#49828' '#51333#47308
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = btnSequenceStopClick
  end
  object btnEStop: TButton
    Left = 488
    Top = 64
    Width = 142
    Height = 41
    Caption = #48708#49345' '#51221#51648
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = btnEStopClick
  end
  object btnInitialization: TButton
    Left = 8
    Top = 216
    Width = 142
    Height = 41
    Caption = #52488#44592#54868
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    OnClick = btnInitializationClick
  end
  object btnRouteReset: TButton
    Left = 168
    Top = 216
    Width = 142
    Height = 41
    Caption = #44221#47196' '#51116#49444#51221
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
    OnClick = btnRouteResetClick
  end
  object edtRouteIndex: TEdit
    Left = 81
    Top = 64
    Width = 152
    Height = 21
    ReadOnly = True
    TabOrder = 7
  end
  object pbLongrun_per: TProgressBar
    Left = 81
    Top = 119
    Width = 549
    Height = 22
    TabOrder = 8
  end
  object strngrdRouteTable: TStringGrid
    Left = 8
    Top = 272
    Width = 622
    Height = 309
    BiDiMode = bdLeftToRight
    ColCount = 7
    FixedColor = clSkyBlue
    FixedCols = 0
    RowCount = 500
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected]
    ParentBiDiMode = False
    TabOrder = 9
    OnSelectCell = strngrdRouteTableSelectCell
    ColWidths = (
      24
      61
      66
      106
      95
      97
      146)
  end
  object mmoLongRunResult: TMemo
    Left = 8
    Top = 610
    Width = 623
    Height = 81
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    ImeName = 'Microsoft IME 2010'
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 10
  end
  object btnDataSave: TButton
    Left = 328
    Top = 216
    Width = 142
    Height = 41
    Caption = #51452#54665#44592#47197' '#51200#51109
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 11
    OnClick = btnDataSaveClick
  end
  object edtMode: TEdit
    Left = 81
    Top = 18
    Width = 152
    Height = 21
    ReadOnly = True
    TabOrder = 12
  end
  object btnMoveStartOneRoute: TButton
    Left = 328
    Top = 160
    Width = 142
    Height = 41
    Caption = #49440#53469#46108' '#44221#47196' '#51452#54665
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 13
    OnClick = btnMoveStartOneRouteClick
  end
  object btnSimulation: TButton
    Left = 488
    Top = 216
    Width = 142
    Height = 41
    Caption = #51452#54665' '#49884#48044#47112#51060#49496
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 14
    OnClick = btnSimulationClick
  end
  object chkLongrun: TCheckBox
    Left = 394
    Top = 91
    Width = 68
    Height = 17
    Caption = #47217#47088' '#52404#53356
    TabOrder = 15
    OnClick = chkLongrunClick
  end
  object edtCountSet: TEdit
    Left = 310
    Top = 64
    Width = 152
    Height = 21
    NumbersOnly = True
    TabOrder = 16
    Text = '1'
  end
  object tmrAutoBayLongRunTimer: TTimer
    Interval = 300
    OnTimer = tmrAutoBayLongRunTimerTimer
    Left = 560
    Top = 536
  end
end
