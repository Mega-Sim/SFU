object RotateUnit: TRotateUnit
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'RotateUnit'
  ClientHeight = 642
  ClientWidth = 414
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object txtRotateUnit: TStaticText
    Left = 18
    Top = 16
    Width = 133
    Height = 37
    Caption = 'Rotate Unit'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object chkRotateLongrun: TCheckBox
    Left = 317
    Top = 36
    Width = 79
    Height = 17
    Caption = 'longrunChk'
    TabOrder = 1
    OnClick = chkRotateLongrunClick
  end
  object btnRotateInit: TButton
    Left = 188
    Top = 396
    Width = 75
    Height = 25
    Caption = 'ORIGIN'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = btnRotateInitClick
  end
  object btnRotate_Run: TButton
    Left = 17
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
    OnClick = btnRotate_RunClick
  end
  object pb_Rotate: TProgressBar
    Left = 143
    Top = 69
    Width = 252
    Height = 26
    TabOrder = 4
  end
  object pnl_RotateReadyStatus: TPanel
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
  object pnl_RotateBusy: TPanel
    Left = 79
    Top = 67
    Width = 58
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Rotate On'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 6
  end
  object btnRotate_Reset: TButton
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
    OnClick = btnRotate_ResetClick
  end
  object pnlIn_Rotate90: TPanel
    Left = 107
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = '90'#39
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 8
  end
  object pnlIn_Rotate0: TPanel
    Left = 26
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = '0'#39
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 9
  end
  object txtRotate_ctrl: TStaticText
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
  object pnlIn_RotateAlarm: TPanel
    Left = 188
    Top = 430
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Alarm'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 11
  end
  object btnOut_Rotate90: TButton
    Left = 107
    Top = 365
    Width = 75
    Height = 25
    Caption = 'Move 90'#39
    TabOrder = 12
    OnClick = btnOut_Rotate90Click
  end
  object btnOut_Rotate0: TButton
    Left = 26
    Top = 365
    Width = 75
    Height = 25
    Caption = 'Move 0'#39
    TabOrder = 13
    OnClick = btnOut_Rotate0Click
  end
  object btnOut_AlarmReset: TButton
    Left = 188
    Top = 365
    Width = 75
    Height = 25
    Caption = 'AlarmReset'
    TabOrder = 14
    OnClick = btnOut_AlarmResetClick
  end
  object txtRotateSetting: TStaticText
    Left = 18
    Top = 492
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
  object txtRotateInterval: TStaticText
    Left = 43
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
    TabOrder = 16
  end
  object txtRotateCount: TStaticText
    Left = 65
    Top = 568
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
  object txtRotateRealCnt: TStaticText
    Left = 233
    Top = 298
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
  object edt_Rotate_RealCount: TEdit
    Left = 280
    Top = 298
    Width = 105
    Height = 21
    Color = clMoneyGreen
    TabOrder = 19
  end
  object txt_ErrMsg2: TStaticText
    Left = 37
    Top = 217
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
  object edt_RotateErrMsg: TEdit
    Left = 112
    Top = 215
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
  object edt_RotateStepMsg: TEdit
    Left = 112
    Top = 186
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
  object txtRotate_StepMsg: TStaticText
    Left = 37
    Top = 187
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
  object btnRotate_LoadParam: TButton
    Left = 289
    Top = 532
    Width = 96
    Height = 45
    Caption = 'Load Param'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 24
    OnClick = btnRotate_LoadParamClick
  end
  object edtRotate_Count: TEdit
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
    TabOrder = 25
  end
  object edtRotate_Interval: TEdit
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
    TabOrder = 26
  end
  object mmoRotate_Manual: TMemo
    Left = 282
    Top = 367
    Width = 124
    Height = 91
    Lines.Strings = (
      '[Rotate Manual]'
      '1) Load Param'
      '2) Ready Check'
      '3) Origin Setting'
      '4) Lonrun Check'
      '5) Start')
    TabOrder = 27
  end
  object pnlIn_Rotate270: TPanel
    Left = 107
    Top = 458
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = '270'#39
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 28
  end
  object pnlIn_Rotate180: TPanel
    Left = 26
    Top = 458
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = '180'#39
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 29
  end
  object Txt_StartTime: TStaticText
    Left = 37
    Top = 246
    Width = 59
    Height = 19
    Caption = 'Start Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 30
  end
  object edt_RotateDate: TEdit
    Left = 112
    Top = 244
    Width = 273
    Height = 21
    Color = clMoneyGreen
    TabOrder = 31
  end
  object btn_RotateEStop: TButton
    Left = 112
    Top = 113
    Width = 92
    Height = 60
    Caption = 'STOP'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 32
    OnClick = btn_RotateEStopClick
  end
  object pnlIn_RotateInpos: TPanel
    Left = 188
    Top = 458
    Width = 75
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Inpos'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 33
  end
  object Txt_Errtime: TStaticText
    Left = 37
    Top = 271
    Width = 61
    Height = 19
    Caption = 'Error Time'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 34
  end
  object edt_RotateErrTime: TEdit
    Left = 112
    Top = 271
    Width = 273
    Height = 21
    Color = clMoneyGreen
    TabOrder = 35
  end
  object btnOut_Rotate180: TButton
    Left = 26
    Top = 396
    Width = 75
    Height = 25
    Caption = 'Move 180'#39
    TabOrder = 36
    OnClick = btnOut_Rotate180Click
  end
  object btnOut_Rotate270: TButton
    Left = 107
    Top = 396
    Width = 75
    Height = 25
    Caption = 'Move 270'#39
    TabOrder = 37
    OnClick = btnOut_Rotate270Click
  end
  object txt1: TStaticText
    Left = 37
    Top = 593
    Width = 80
    Height = 19
    Caption = 'Current Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 38
  end
  object edtRotate_Current_Count: TEdit
    Left = 142
    Top = 593
    Width = 121
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 39
  end
  object btnRotate_Main_Reset: TButton
    Left = 310
    Top = 112
    Width = 96
    Height = 61
    Caption = 'Main_Reset'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 40
    OnClick = btnRotate_Main_ResetClick
  end
  object Save_Param: TButton
    Left = 289
    Top = 583
    Width = 96
    Height = 42
    Caption = 'Save Param'
    TabOrder = 41
    OnClick = Save_ParamClick
  end
  object tmrRotateUI: TTimer
    Interval = 100
    OnTimer = tmrRotateUITimer
    Left = 336
    Top = 480
  end
end
