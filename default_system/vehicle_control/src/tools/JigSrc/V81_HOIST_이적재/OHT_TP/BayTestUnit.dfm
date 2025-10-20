object frm_Bay_Test: Tfrm_Bay_Test
  Left = 0
  Top = 0
  Caption = 'Bay Moving/Trans Test'
  ClientHeight = 716
  ClientWidth = 615
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Shape_Status: TShape
    Left = 348
    Top = 8
    Width = 98
    Height = 25
    Brush.Color = clRed
  end
  object Label1: TLabel
    Left = 4
    Top = 7
    Width = 96
    Height = 21
    Caption = #53580#49828#53944' '#47784#46300' :'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 35
    Width = 92
    Height = 21
    Caption = #51652#54665' '#47784#46300'   :'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 245
    Top = 39
    Width = 91
    Height = 17
    Caption = #53580#49828#53944' '#51652#54665#47456' :'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 8
    Top = 579
    Width = 69
    Height = 17
    Caption = #8251' Message'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 8
    Top = 63
    Width = 74
    Height = 17
    Caption = #8251' Test Case'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 259
    Top = 11
    Width = 83
    Height = 17
    Caption = #53580#49828#53944' '#49345#53468' : '
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label_TestStatus: TLabel
    Left = 357
    Top = 11
    Width = 82
    Height = 17
    Alignment = taCenter
    Caption = '*'#53580#49828#53944' '#51221#51648'*'
    Color = clBtnShadow
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object TestModeBox: TComboBox
    Left = 106
    Top = 8
    Width = 127
    Height = 25
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    ImeName = 'Microsoft IME 2010'
    ParentFont = False
    TabOrder = 0
    Text = #47784#46300' '#49440#53469
    OnClick = TestModeBoxClick
    Items.Strings = (
      '1'#54924' '#47784#46300
      #48152#48373' '#47784#46300)
  end
  object ExcuteModeBox: TComboBox
    Left = 106
    Top = 36
    Width = 127
    Height = 25
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    ImeName = 'Microsoft IME 2010'
    ParentFont = False
    TabOrder = 1
    Text = #47784#46300' '#49440#53469
    OnClick = ExcuteModeBoxClick
    Items.Strings = (
      #49692#52264' (Sequential)'
      #47924#51089#50948' (Random)')
  end
  object btn_LongRunStart: TButton
    Left = 142
    Top = 685
    Width = 75
    Height = 25
    Caption = 'Start'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btn_LongRunStartClick
  end
  object btn_LongRunStop: TButton
    Left = 400
    Top = 685
    Width = 75
    Height = 25
    Caption = 'Stop'
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = btn_LongRunStopClick
  end
  object mmLongRunResult: TMemo
    Left = 1
    Top = 598
    Width = 602
    Height = 81
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    ImeName = 'Microsoft IME 2010'
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 4
  end
  object strGridLongrunInfor: TStringGrid
    Left = 8
    Top = 77
    Width = 848
    Height = 496
    Align = alCustom
    ColCount = 6
    DefaultColWidth = 20
    DefaultRowHeight = 15
    RowCount = 300
    Font.Charset = HANGEUL_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = []
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine]
    ParentFont = False
    TabOrder = 5
    ColWidths = (
      37
      123
      99
      104
      68
      71)
    RowHeights = (
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15
      15)
  end
  object Btn_OpenLongrunFile: TButton
    Left = 466
    Top = 8
    Width = 75
    Height = 25
    Caption = 'File Open'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = #47569#51008' '#44256#46357
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
    OnClick = Btn_OpenLongrunFileClick
  end
  object Longrun_per: TProgressBar
    Left = 349
    Top = 39
    Width = 253
    Height = 17
    TabOrder = 7
  end
  object btn_OCSBase_LongrunStart: TButton
    Left = 272
    Top = 685
    Width = 70
    Height = 25
    Caption = 'OCS Map '#44592#48152' '#51060#46041' Start'
    TabOrder = 8
  end
  object OpenTextFileDig_Longrun: TOpenTextFileDialog
    Filter = 'txt|*.txt'
    Options = [ofReadOnly, ofHideReadOnly, ofEnableSizing]
    Left = 472
    Top = 560
  end
  object BayLongRunTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = BayLongRunTimerTimer
    Left = 544
    Top = 560
  end
end
