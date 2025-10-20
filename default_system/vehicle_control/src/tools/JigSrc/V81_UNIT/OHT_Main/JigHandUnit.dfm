object HandUnit: THandUnit
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'HandUnit'
  ClientHeight = 643
  ClientWidth = 414
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object txtHandUnit: TStaticText
    Left = 18
    Top = 16
    Width = 116
    Height = 37
    Caption = 'Hand Unit'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object chkHandLongrun: TCheckBox
    Left = 314
    Top = 36
    Width = 79
    Height = 17
    Caption = 'longrunChk'
    TabOrder = 1
    OnClick = chkHandLongrunClick
  end
  object btnHandInit: TButton
    Left = 188
    Top = 357
    Width = 75
    Height = 27
    Caption = 'Origin'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = btnHandInitClick
  end
  object btnHand_Run: TButton
    Left = 19
    Top = 113
    Width = 89
    Height = 60
    Caption = 'START'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = btnHand_RunClick
  end
  object pb_Hand: TProgressBar
    Left = 144
    Top = 69
    Width = 252
    Height = 26
    TabOrder = 4
  end
  object pnl_HandReadyStatus: TPanel
    Left = 19
    Top = 67
    Width = 54
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Ready'
    Color = clActiveCaption
    ParentBackground = False
    TabOrder = 5
  end
  object pnl_HandBusy: TPanel
    Left = 79
    Top = 67
    Width = 51
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Longrun'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 6
  end
  object btnHand_Reset: TButton
    Left = 210
    Top = 112
    Width = 93
    Height = 61
    Caption = 'RESET'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 7
    OnClick = btnHand_ResetClick
  end
  object pnlIn_HandClose: TPanel
    Left = 107
    Top = 424
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Hand_Close'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 8
  end
  object pnlIn_HandOpen: TPanel
    Left = 26
    Top = 424
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Hand_Open'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 9
  end
  object txtHand_ctrl: TStaticText
    Left = 18
    Top = 322
    Width = 88
    Height = 37
    Caption = 'Control'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 10
  end
  object pnlIn_HandAlarm: TPanel
    Left = 188
    Top = 424
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Hand_Alarm'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 11
  end
  object btnOut_HandOpen: TButton
    Left = 107
    Top = 357
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 12
    OnClick = btnOut_HandCloseClick
  end
  object btnOut_HandClose: TButton
    Left = 26
    Top = 357
    Width = 75
    Height = 25
    Caption = 'Open'
    TabOrder = 13
    OnClick = btnOut_HandOpenClick
  end
  object btnOut_AlarmReset: TButton
    Left = 188
    Top = 388
    Width = 75
    Height = 25
    Caption = 'AlarmReset'
    TabOrder = 14
    OnClick = btnOut_AlarmResetClick
  end
  object txtHandSetting: TStaticText
    Left = 18
    Top = 479
    Width = 83
    Height = 37
    Caption = 'Setting'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 15
  end
  object txtHandInterval: TStaticText
    Left = 37
    Top = 519
    Width = 74
    Height = 19
    Caption = 'Interval [ms]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 16
  end
  object txtHandCount: TStaticText
    Left = 59
    Top = 548
    Width = 36
    Height = 19
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 17
  end
  object txtHandRealCnt: TStaticText
    Left = 227
    Top = 302
    Width = 41
    Height = 22
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 18
  end
  object edtHand_RealCount: TEdit
    Left = 269
    Top = 302
    Width = 105
    Height = 21
    Color = clMoneyGreen
    TabOrder = 19
  end
  object txt_ErrMsg2: TStaticText
    Left = 34
    Top = 220
    Width = 58
    Height = 19
    Caption = 'Error Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 20
  end
  object edt_HandErrMsg: TEdit
    Left = 101
    Top = 219
    Width = 273
    Height = 23
    Color = clMoneyGreen
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 21
  end
  object edt_HandStepMsg: TEdit
    Left = 101
    Top = 190
    Width = 273
    Height = 23
    Color = clMoneyGreen
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 22
  end
  object txtHand_StepMsg: TStaticText
    Left = 34
    Top = 192
    Width = 53
    Height = 19
    Caption = 'Step Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 23
  end
  object btnHand_LoadParam: TButton
    Left = 282
    Top = 515
    Width = 96
    Height = 46
    Caption = 'Load Param'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 24
    OnClick = btnHand_LoadParamClick
  end
  object edtHand_Count: TEdit
    Left = 126
    Top = 544
    Width = 137
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 25
  end
  object edtHand_Interval: TEdit
    Left = 126
    Top = 515
    Width = 137
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 26
  end
  object mmoHand_Manual: TMemo
    Left = 282
    Top = 367
    Width = 124
    Height = 91
    Lines.Strings = (
      '[Hand Manual]'
      '1) Load Param'
      '2) Ready Check'
      '3) Origin Setting'
      '4) Lonrun Check'
      '5) Start')
    TabOrder = 27
  end
  object Txt_StartTime: TStaticText
    Left = 34
    Top = 250
    Width = 59
    Height = 19
    Caption = 'Start Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 28
  end
  object edt_HandDate: TEdit
    Left = 101
    Top = 248
    Width = 273
    Height = 21
    Color = clMoneyGreen
    TabOrder = 29
  end
  object btn_HandEStop: TButton
    Left = 114
    Top = 113
    Width = 90
    Height = 60
    Caption = 'STOP'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 30
    OnClick = btn_HandEStopClick
  end
  object pnlIn_HandInpos: TPanel
    Left = 188
    Top = 462
    Width = 75
    Height = 25
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Inpos'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 31
  end
  object Txt_Errtime: TStaticText
    Left = 34
    Top = 277
    Width = 61
    Height = 19
    Caption = 'Error Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 32
  end
  object edt_HandErrTime: TEdit
    Left = 101
    Top = 275
    Width = 273
    Height = 21
    Color = clMoneyGreen
    TabOrder = 33
  end
  object edtHand_Current_Count: TEdit
    Left = 126
    Top = 571
    Width = 137
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 34
  end
  object txtCurrentCnt: TStaticText
    Left = 28
    Top = 573
    Width = 83
    Height = 19
    Caption = 'Current_Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 35
  end
  object btnHand_Main_Reset: TButton
    Left = 309
    Top = 114
    Width = 90
    Height = 60
    Caption = 'Main_Reset'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 36
    OnClick = btnHand_Main_ResetClick
  end
  object Save_Param: TButton
    Left = 282
    Top = 567
    Width = 96
    Height = 42
    Caption = 'Save Param'
    TabOrder = 37
    OnClick = Save_ParamClick
  end
  object tmrHandUI: TTimer
    Interval = 100
    OnTimer = tmrHandUITimer
    Left = 360
    Top = 456
  end
end
