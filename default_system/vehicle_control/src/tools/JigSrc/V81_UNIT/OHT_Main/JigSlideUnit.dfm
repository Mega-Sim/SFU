object SlideUnit: TSlideUnit
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'JigTest'
  ClientHeight = 630
  ClientWidth = 468
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
  object SlideUnit: TStaticText
    Left = 26
    Top = 24
    Width = 111
    Height = 37
    Caption = 'Slide Axis'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object edtSlide_moveDist: TEdit
    Left = 154
    Top = 514
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  object progress_Slide: TProgressBar
    Left = 200
    Top = 77
    Width = 260
    Height = 26
    TabOrder = 2
  end
  object txt4: TStaticText
    Left = 34
    Top = 518
    Width = 96
    Height = 19
    Caption = 'Sliding Pos [mm]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
  end
  object btnSlideAxis_Run: TButton
    Left = 25
    Top = 109
    Width = 72
    Height = 41
    Caption = 'START'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnClick = btnSlideAxis_RunClick
  end
  object edtSlide_Interval: TEdit
    Left = 154
    Top = 543
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
  end
  object txt5: TStaticText
    Left = 56
    Top = 543
    Width = 74
    Height = 19
    Caption = 'Interval [ms]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
  end
  object edtSlide_Count: TEdit
    Left = 154
    Top = 572
    Width = 171
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
  end
  object txt6: TStaticText
    Left = 91
    Top = 575
    Width = 36
    Height = 19
    Caption = 'Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
  end
  object edt_SlideErrMsg: TEdit
    Left = 94
    Top = 333
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
    TabOrder = 9
  end
  object txt_ErrMsg1: TStaticText
    Left = 21
    Top = 335
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
  object btn_SlideReset: TButton
    Left = 186
    Top = 109
    Width = 75
    Height = 41
    Caption = 'Reset'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
    OnClick = btn_SlideResetClick
  end
  object Slide_LoadParam: TButton
    Left = 350
    Top = 511
    Width = 96
    Height = 42
    Caption = 'Load Param'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 12
    OnClick = Slide_LoadParamClick
  end
  object Slide_EStop: TButton
    Left = 103
    Top = 109
    Width = 77
    Height = 41
    Caption = 'STOP'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 13
    OnClick = Slide_EStopClick
  end
  object btn_Servo_On: TButton
    Left = 23
    Top = 173
    Width = 74
    Height = 41
    Caption = 'Servo On'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 14
    OnClick = btn_Servo_OnClick
  end
  object btn_Servo_Off: TButton
    Left = 103
    Top = 173
    Width = 75
    Height = 41
    Caption = 'Servo Off'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 15
    OnClick = btn_Servo_OffClick
  end
  object pnl_SlideServoStat: TPanel
    Left = 26
    Top = 75
    Width = 54
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Servo'
    Color = clActiveCaption
    ParentBackground = False
    TabOrder = 16
  end
  object pnl_SlideBusy: TPanel
    Left = 143
    Top = 75
    Width = 51
    Height = 28
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Longrun'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 17
  end
  object btn_SlideHome: TButton
    Left = 371
    Top = 109
    Width = 89
    Height = 41
    Caption = 'Go Home'
    TabOrder = 18
    OnClick = btn_SlideHomeClick
  end
  object edt_SlideStepMsg: TEdit
    Left = 94
    Top = 304
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
    TabOrder = 19
  end
  object Slide_StepMsg: TStaticText
    Left = 24
    Top = 305
    Width = 53
    Height = 19
    Caption = 'Step Msg'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 20
  end
  object txt16: TStaticText
    Left = 25
    Top = 242
    Width = 99
    Height = 19
    Caption = 'Current Pos [mm]'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 21
  end
  object pnl_SlideCurPos: TPanel
    Left = 143
    Top = 242
    Width = 132
    Height = 19
    Caption = '... mm'
    TabOrder = 22
  end
  object btn_SlideSetHome: TButton
    Left = 200
    Top = 8
    Width = 64
    Height = 22
    Caption = 'Set Home'
    TabOrder = 23
    Visible = False
    OnClick = btn_SlideSetHomeClick
  end
  object pnl_SlideVel: TPanel
    Left = 143
    Top = 267
    Width = 132
    Height = 16
    Caption = 'pnl_SlideVel'
    TabOrder = 24
  end
  object txt17: TStaticText
    Left = 40
    Top = 267
    Width = 48
    Height = 19
    Caption = 'Velocity'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 25
  end
  object btn_SlideJog_P: TButton
    Left = 304
    Top = 237
    Width = 75
    Height = 31
    Caption = 'JOG +'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 26
    OnClick = btn_SlideJog_PClick
  end
  object btn_SlideJogM: TButton
    Left = 385
    Top = 236
    Width = 75
    Height = 33
    Caption = 'JOG -'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 27
    OnClick = btn_SlideJogMClick
  end
  object edt_RelMoveVal: TEdit
    Left = 362
    Top = 207
    Width = 98
    Height = 23
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 28
  end
  object chkSlidelongrun: TCheckBox
    Left = 342
    Top = 54
    Width = 118
    Height = 17
    Caption = 'Slide Longrun'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 29
    OnClick = btn_LongrunClick
  end
  object edt_Slide_RealCount: TEdit
    Left = 94
    Top = 362
    Width = 231
    Height = 21
    Color = clMoneyGreen
    TabOrder = 30
  end
  object RealtimeCount: TStaticText
    Left = 22
    Top = 364
    Width = 63
    Height = 19
    Caption = 'Real Count'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 31
  end
  object setting: TStaticText
    Left = 30
    Top = 460
    Width = 83
    Height = 37
    Caption = 'Setting'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 32
  end
  object Slide_Manual: TMemo
    Left = 350
    Top = 305
    Width = 110
    Height = 103
    Lines.Strings = (
      '[Slide Manual]'
      '1) Load Param'
      '2) Servo On check'
      '3) Go Home'
      '4) Slide Longrun'
      '    Checking'
      '5) Run')
    TabOrder = 33
  end
  object edtStartDate: TEdit
    Left = 94
    Top = 389
    Width = 231
    Height = 21
    Color = clMoneyGreen
    TabOrder = 34
  end
  object edtEndDate: TEdit
    Left = 94
    Top = 416
    Width = 231
    Height = 21
    Color = clMoneyGreen
    TabOrder = 35
  end
  object pnl_SlideReady: TPanel
    Left = 86
    Top = 77
    Width = 51
    Height = 26
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'Ready'
    Color = clGradientActiveCaption
    ParentBackground = False
    TabOrder = 36
  end
  object txt1: TStaticText
    Left = 25
    Top = 389
    Width = 58
    Height = 19
    Caption = 'Start Date'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 37
  end
  object txt2: TStaticText
    Left = 25
    Top = 418
    Width = 52
    Height = 19
    Caption = 'End Date'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 38
  end
  object txt3: TStaticText
    Left = 304
    Top = 212
    Width = 52
    Height = 19
    Caption = 'Distance'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Calibri'
    Font.Style = []
    ParentFont = False
    TabOrder = 39
  end
  object btnSlide_Main_Reset: TButton
    Left = 267
    Top = 109
    Width = 89
    Height = 41
    Caption = 'Main_Reset'
    TabOrder = 40
    OnClick = btnSlide_Main_ResetClick
  end
  object txt7: TStaticText
    Left = 342
    Top = 178
    Width = 76
    Height = 23
    Caption = 'JOG MOVE'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Calibri'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 41
  end
  object Slide_SaveParam: TButton
    Left = 350
    Top = 559
    Width = 96
    Height = 42
    Caption = 'Save Param'
    TabOrder = 42
    OnClick = Slide_SaveParamClick
  end
  object tmr1: TTimer
    Interval = 100
    OnTimer = tmr1Timer
    Left = 16
    Top = 571
  end
end
