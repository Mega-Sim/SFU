object ShutterUnit: TShutterUnit
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'ShutterUnit'
  ClientHeight = 620
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
  object txtShutterUnit: TStaticText
    Left = 18
    Top = 16
    Width = 141
    Height = 37
    Caption = 'Shutter Unit'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object chkShutterLongrun: TCheckBox
    Left = 314
    Top = 36
    Width = 79
    Height = 17
    Caption = 'longrunChk'
    TabOrder = 1
    OnClick = chkShutterLongrunClick
  end
  object btnShutterInit: TButton
    Left = 18
    Top = 113
    Width = 88
    Height = 60
    Caption = 'ORIGIN'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btnShutterInitClick
  end
  object btnShutter_Run: TButton
    Left = 112
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
    OnClick = btnShutter_RunClick
  end
  object pb_Shutter: TProgressBar
    Left = 144
    Top = 69
    Width = 252
    Height = 26
    TabOrder = 4
  end
  object pnl_ShutterReadyStatus: TPanel
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
  object pnl_ShutterBusy: TPanel
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
  object btnShutter_Reset: TButton
    Left = 207
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
    OnClick = btnShutter_ResetClick
  end
  object pnlIn_ShutterFOpen: TPanel
    Left = 107
    Top = 396
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'F_Close'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 8
  end
  object pnlIn_ShutterFClose: TPanel
    Left = 26
    Top = 396
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'F_Open'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 9
  end
  object txtShutter_ctrl: TStaticText
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
  object pnlIn_ShutterFAlarm: TPanel
    Left = 188
    Top = 396
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'F_Alarm'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 11
  end
  object pnlIn_ShutterRAlarm: TPanel
    Left = 188
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'R_Alarm'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 12
  end
  object btnOut_ShutterOpen: TButton
    Left = 107
    Top = 365
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 13
    OnClick = btnOut_ShutterOpenClick
  end
  object btnOut_ShutterClose: TButton
    Left = 26
    Top = 365
    Width = 75
    Height = 25
    Caption = 'Open'
    TabOrder = 14
    OnClick = btnOut_ShutterCloseClick
  end
  object btnOut_AlarmReset: TButton
    Left = 188
    Top = 365
    Width = 75
    Height = 25
    Caption = 'AlarmReset'
    TabOrder = 15
    OnClick = btnOut_AlarmResetClick
  end
  object txtShutterSetting: TStaticText
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
    TabOrder = 16
  end
  object txtShutterInterval: TStaticText
    Left = 32
    Top = 535
    Width = 74
    Height = 19
    Caption = 'Interval [ms]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 17
  end
  object txtShutterCount: TStaticText
    Left = 65
    Top = 571
    Width = 36
    Height = 19
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 18
  end
  object txtShutterRealCnt: TStaticText
    Left = 59
    Top = 297
    Width = 36
    Height = 19
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 19
  end
  object edtShutter_RealCount: TEdit
    Left = 112
    Top = 295
    Width = 105
    Height = 21
    Color = clMoneyGreen
    TabOrder = 20
  end
  object txt_ErrMsg2: TStaticText
    Left = 37
    Top = 257
    Width = 58
    Height = 19
    Caption = 'Error Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 21
  end
  object edt_ShutterErrMsg: TEdit
    Left = 112
    Top = 257
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
  object edt_ShutterStepMsg: TEdit
    Left = 112
    Top = 218
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
    TabOrder = 23
  end
  object txtShutter_StepMsg: TStaticText
    Left = 42
    Top = 218
    Width = 53
    Height = 19
    Caption = 'Step Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 24
  end
  object btnShutter_LoadParam: TButton
    Left = 289
    Top = 532
    Width = 96
    Height = 55
    Caption = 'Load Param'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 25
    OnClick = btnShutter_LoadParamClick
  end
  object edtShutter_Count: TEdit
    Left = 142
    Top = 564
    Width = 121
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 26
  end
  object edtShutter_Interval: TEdit
    Left = 142
    Top = 535
    Width = 121
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 27
  end
  object mmoShutter_Manual: TMemo
    Left = 282
    Top = 367
    Width = 124
    Height = 91
    Lines.Strings = (
      '[Shutter Manual]'
      '1) Load Param'
      '2) Ready Check'
      '3) Origin Setting'
      '4) Lonrun Check'
      '5) Start')
    TabOrder = 28
  end
  object pnlIn_ShutterROpen: TPanel
    Left = 107
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'R_Close'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 29
  end
  object pnlIn_ShutterRClose: TPanel
    Left = 26
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'R_Open'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 30
  end
  object Txt_StartTime: TStaticText
    Left = 224
    Top = 297
    Width = 59
    Height = 19
    Caption = 'Start Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 31
  end
  object edt_ShutterDate: TEdit
    Left = 289
    Top = 295
    Width = 96
    Height = 21
    Color = clMoneyGreen
    TabOrder = 32
  end
  object btn_ShutterEStop: TButton
    Left = 306
    Top = 113
    Width = 90
    Height = 60
    Caption = 'E-STOP'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 33
    OnClick = btn_ShutterEStopClick
  end
  object pnlIn_ShutterInpos: TPanel
    Left = 188
    Top = 464
    Width = 75
    Height = 25
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Inpos'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 34
  end
  object Txt_Errtime: TStaticText
    Left = 224
    Top = 322
    Width = 61
    Height = 19
    Caption = 'Error Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 35
  end
  object edt_ShutterErrTime: TEdit
    Left = 289
    Top = 322
    Width = 96
    Height = 21
    Color = clMoneyGreen
    TabOrder = 36
  end
  object tmrShutterUI: TTimer
    Interval = 100
    OnTimer = tmrShutterUITimer
    Left = 336
    Top = 480
  end
end
