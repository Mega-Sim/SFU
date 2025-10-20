object HoistUnit: THoistUnit
  Left = 0
  Top = 0
  Caption = 'HoistUnit'
  ClientHeight = 651
  ClientWidth = 476
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
  object txtsetting: TStaticText
    Left = 24
    Top = 468
    Width = 83
    Height = 37
    Caption = 'Setting'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object txtHoistUnit: TStaticText
    Left = 26
    Top = 24
    Width = 115
    Height = 37
    Caption = 'Hoist Axis'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
  end
  object mmoHoist_Manual: TMemo
    Left = 356
    Top = 308
    Width = 112
    Height = 132
    Lines.Strings = (
      '[Manual]'
      '1) Load Param'
      '2) Servo On check'
      '3) Go Home'
      '4) Hoist Longrun '
      '    Checking'
      '5) Run')
    TabOrder = 2
  end
  object edt_HoistStepMsg: TEdit
    Left = 96
    Top = 307
    Width = 231
    Height = 23
    Color = clMoneyGreen
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
  end
  object txtHoistPos: TStaticText
    Left = 54
    Top = 520
    Width = 87
    Height = 19
    Caption = 'Hoist Pos [mm]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
  object txt5: TStaticText
    Left = 54
    Top = 549
    Width = 74
    Height = 19
    Caption = 'Interval [ms]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
  end
  object txt6: TStaticText
    Left = 71
    Top = 578
    Width = 36
    Height = 19
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
  end
  object edt_HoistErrMsg: TEdit
    Left = 96
    Top = 336
    Width = 231
    Height = 23
    Color = clMoneyGreen
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 7
  end
  object edt_HoistRealCount: TEdit
    Left = 96
    Top = 365
    Width = 231
    Height = 21
    Color = clMoneyGreen
    ReadOnly = True
    TabOrder = 8
  end
  object txtHoistStepMsg: TStaticText
    Left = 23
    Top = 309
    Width = 53
    Height = 19
    Caption = 'Step Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
  end
  object txtHoistErrMsg: TStaticText
    Left = 23
    Top = 338
    Width = 58
    Height = 19
    Caption = 'Error Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 10
  end
  object RealtimeCount: TStaticText
    Left = 23
    Top = 367
    Width = 63
    Height = 19
    Caption = 'Real Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
  end
  object progress_Hoist: TProgressBar
    Left = 264
    Top = 77
    Width = 192
    Height = 26
    TabOrder = 12
  end
  object btnHoistAxis_Run: TButton
    Left = 21
    Top = 111
    Width = 71
    Height = 41
    Caption = 'START'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 13
    OnClick = btnHoistAxis_RunClick
  end
  object btn_HoistReset: TButton
    Left = 183
    Top = 111
    Width = 75
    Height = 41
    Caption = 'Reset'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 14
    OnClick = btn_HoistResetClick
  end
  object btn_Servo_On: TButton
    Left = 23
    Top = 182
    Width = 74
    Height = 42
    Caption = 'Servo On'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 15
    OnClick = btn_Servo_OnClick
  end
  object btn_Servo_Off: TButton
    Left = 103
    Top = 182
    Width = 75
    Height = 42
    Caption = 'Servo Off'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 16
    OnClick = btn_Servo_OffClick
  end
  object pnl_HoistServoStat: TPanel
    Left = 26
    Top = 77
    Width = 54
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Servo'
    Color = clActiveCaption
    ParentBackground = False
    TabOrder = 17
  end
  object pnl_HoistBusy: TPanel
    Left = 200
    Top = 77
    Width = 51
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Longrun'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 18
  end
  object btn_HoistHome: TButton
    Left = 381
    Top = 111
    Width = 71
    Height = 41
    Caption = 'Go Home'
    TabOrder = 19
    OnClick = btn_HoistHomeClick
  end
  object txt16: TStaticText
    Left = 29
    Top = 237
    Width = 99
    Height = 19
    Caption = 'Current Pos [mm]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 20
  end
  object pnl_HoistCurPos: TPanel
    Left = 143
    Top = 237
    Width = 132
    Height = 19
    Caption = '... mm'
    TabOrder = 21
  end
  object btnHoistSetHome: TButton
    Left = 200
    Top = 182
    Width = 75
    Height = 42
    Caption = 'Set Home'
    TabOrder = 22
    Visible = False
    OnClick = btnHoistSetHomeClick
  end
  object pnl_HoistVel: TPanel
    Left = 143
    Top = 262
    Width = 132
    Height = 16
    Caption = 'pnl_HoistVel'
    TabOrder = 23
  end
  object txt17: TStaticText
    Left = 59
    Top = 262
    Width = 48
    Height = 19
    Caption = 'Velocity'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 24
  end
  object btn_HoistJog_P: TButton
    Left = 303
    Top = 232
    Width = 72
    Height = 32
    Caption = 'JOG +'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 25
    OnClick = btn_HoistJog_PClick
  end
  object btn_HoistJog_N: TButton
    Left = 381
    Top = 232
    Width = 75
    Height = 32
    Caption = 'JOG -'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 26
    OnClick = btn_HoistJog_NClick
  end
  object edt_RelMoveVal: TEdit
    Left = 361
    Top = 203
    Width = 95
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    NumbersOnly = True
    ParentFont = False
    TabOrder = 27
    Text = '0'
  end
  object chkHoistLongrun: TCheckBox
    Left = 361
    Top = 54
    Width = 95
    Height = 17
    Caption = 'Hoist Longrun'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 28
    OnClick = chkHoistLongrunClick
  end
  object edt_HoistMoveDist: TEdit
    Left = 156
    Top = 520
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    NumbersOnly = True
    ParentFont = False
    TabOrder = 29
    Text = '0'
  end
  object edt_HoistInterval: TEdit
    Left = 156
    Top = 549
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    NumbersOnly = True
    ParentFont = False
    TabOrder = 30
    Text = '0'
  end
  object edt_HoistCount: TEdit
    Left = 156
    Top = 578
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    NumbersOnly = True
    ParentFont = False
    TabOrder = 31
    Text = '0'
  end
  object btnHoist_LoadParam: TButton
    Left = 356
    Top = 520
    Width = 96
    Height = 55
    Caption = 'Load Param'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 32
    OnClick = btnHoist_LoadParamClick
  end
  object btnHoist_EStop: TButton
    Left = 103
    Top = 111
    Width = 74
    Height = 40
    Caption = 'STOP'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 33
    OnClick = btnHoist_EStopClick
  end
  object pnl_HoistHome: TPanel
    Left = 143
    Top = 77
    Width = 51
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Home'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 35
  end
  object txtProfile: TStaticText
    Left = 84
    Top = 624
    Width = 41
    Height = 19
    Caption = 'Profile'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 36
    Visible = False
  end
  object cbbHoistProfile: TComboBox
    Left = 151
    Top = 622
    Width = 171
    Height = 21
    TabOrder = 37
    Visible = False
    Items.Strings = (
      'Profile 1'
      'Profile 2')
  end
  object txtDate: TStaticText
    Left = 23
    Top = 394
    Width = 58
    Height = 19
    Caption = 'Start Date'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 38
  end
  object txtEndDate: TStaticText
    Left = 23
    Top = 426
    Width = 52
    Height = 19
    Caption = 'End Date'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 39
  end
  object edtStartDate: TEdit
    Left = 96
    Top = 392
    Width = 231
    Height = 21
    Color = clMoneyGreen
    ReadOnly = True
    TabOrder = 40
  end
  object edtEndDate: TEdit
    Left = 96
    Top = 424
    Width = 231
    Height = 21
    Color = clMoneyGreen
    ReadOnly = True
    TabOrder = 41
  end
  object btnHoist_Main_Reset: TButton
    Left = 278
    Top = 109
    Width = 97
    Height = 41
    Caption = 'Main_Reset'
    TabOrder = 42
    OnClick = btnHoist_Main_ResetClick
  end
  object pnl_HoistReady: TPanel
    Left = 86
    Top = 77
    Width = 49
    Height = 28
    BevelOuter = bvLowered
    Caption = 'Ready'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 34
  end
  object txt1: TStaticText
    Left = 303
    Top = 207
    Width = 52
    Height = 19
    Caption = 'Distance'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 43
  end
  object txt2: TStaticText
    Left = 343
    Top = 174
    Width = 76
    Height = 23
    Caption = 'JOG MOVE'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 44
  end
  object Save_Param: TButton
    Left = 356
    Top = 581
    Width = 96
    Height = 52
    Caption = 'Save Param'
    TabOrder = 45
    OnClick = Save_ParamClick
  end
  object tmr1: TTimer
    Interval = 100
    OnTimer = tmr1Timer
    Left = 18
    Top = 584
  end
end
