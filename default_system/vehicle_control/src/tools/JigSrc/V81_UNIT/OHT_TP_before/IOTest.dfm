object IOTestForm: TIOTestForm
  Left = 834
  Top = 342
  Caption = 'IO Test for OHT'
  ClientHeight = 281
  ClientWidth = 595
  Color = 15658734
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Arial'
  Font.Style = [fsBold]
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 15
  object GroupBox5: TGroupBox
    Left = 2
    Top = 0
    Width = 167
    Height = 137
    Caption = ' Sound Control '
    Color = 15662830
    ParentColor = False
    TabOrder = 0
    object SoundRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 153
      Height = 57
      Caption = ' Select Sound '
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        'Sound 1'
        'Sound 2'
        'Sound 3'
        'Sound 4')
      TabOrder = 0
    end
    object Button6: TButton
      Left = 7
      Top = 81
      Width = 66
      Height = 40
      Caption = 'Sound ON'
      TabOrder = 1
      OnClick = SoundOnBtnClick
    end
    object Button7: TButton
      Left = 80
      Top = 80
      Width = 81
      Height = 41
      Caption = 'Sound OFF'
      TabOrder = 2
      OnClick = SoundOffBtnClick
    end
  end
  object GroupBox6: TGroupBox
    Left = 306
    Top = 0
    Width = 97
    Height = 137
    Caption = ' Lookdn Sen '
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 1
    object Panel5: TPanel
      Left = 2
      Top = 17
      Width = 73
      Height = 118
      Align = alLeft
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Color = 15658734
      TabOrder = 2
      object shpLkdnTrouble: TShape
        Left = 2
        Top = 27
        Width = 69
        Height = 25
        Align = alTop
        Shape = stRoundRect
      end
      object shpLkdnDetect: TShape
        Left = 2
        Top = 2
        Width = 69
        Height = 25
        Align = alTop
        Shape = stRoundRect
      end
      object Label12: TLabel
        Left = 18
        Top = 6
        Width = 37
        Height = 15
        Caption = 'Detect'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label13: TLabel
        Left = 16
        Top = 32
        Width = 42
        Height = 15
        Caption = 'Trouble'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
    end
    object Button1: TButton
      Left = 5
      Top = 69
      Width = 84
      Height = 25
      Caption = 'Enable'
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 5
      Top = 99
      Width = 84
      Height = 25
      Caption = 'Disable'
      TabOrder = 1
      OnClick = Button2Click
    end
  end
  object GroupBox7: TGroupBox
    Left = 166
    Top = 0
    Width = 139
    Height = 137
    Caption = ' BCR '
    Color = 15662830
    ParentColor = False
    TabOrder = 2
    object ReadBCRBtn: TButton
      Left = 8
      Top = 36
      Width = 116
      Height = 26
      Caption = 'Read BarCode'
      TabOrder = 0
      OnClick = ReadBCRBtnClick
    end
    object Panel9: TPanel
      Left = 8
      Top = 12
      Width = 65
      Height = 23
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Caption = 'Curnode'
      TabOrder = 1
    end
    object BCRPanel: TPanel
      Left = 70
      Top = 12
      Width = 59
      Height = 23
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Caption = '000'
      TabOrder = 2
    end
    object Button8: TButton
      Left = 8
      Top = 62
      Width = 116
      Height = 26
      Caption = 'Set Curnode_Bcr'
      TabOrder = 3
      OnClick = Button8Click
    end
    object Button9: TButton
      Left = 8
      Top = 108
      Width = 116
      Height = 26
      Caption = 'Set Curnode_Num'
      TabOrder = 4
      OnClick = Button9Click
    end
    object edtCurNode: TEdit
      Left = 10
      Top = 90
      Width = 113
      Height = 16
      BorderStyle = bsNone
      Color = clGreen
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 5
      Text = '0'
    end
  end
  object GroupBox2: TGroupBox
    Left = 144
    Top = 136
    Width = 140
    Height = 129
    Caption = 'Detect Sen '
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 3
    object btnDetectPwrOn: TButton
      Left = 48
      Top = 13
      Width = 86
      Height = 25
      Caption = 'Pwr ON'
      TabOrder = 0
      OnClick = btnDetectPwrOnClick
    end
    object btnDetectPwrOff: TButton
      Left = 48
      Top = 43
      Width = 86
      Height = 25
      Caption = 'Pwr OFF'
      TabOrder = 1
      OnClick = btnDetectPwrOffClick
    end
    object btnDetectEnable: TButton
      Left = 48
      Top = 96
      Width = 86
      Height = 25
      Caption = 'Select'
      TabOrder = 2
      OnClick = btnDetectEnableClick
    end
    object Panel2: TPanel
      Left = 2
      Top = 17
      Width = 44
      Height = 110
      Align = alLeft
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Color = 15658734
      TabOrder = 3
      object shpDetectShort: TShape
        Left = 2
        Top = 34
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpDetectMiddle: TShape
        Left = 2
        Top = 18
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpDetectTrouble: TShape
        Left = 2
        Top = 50
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpDetectLong: TShape
        Left = 2
        Top = 2
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object Label4: TLabel
        Left = 18
        Top = 2
        Width = 7
        Height = 15
        Caption = 'L'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label5: TLabel
        Left = 18
        Top = 18
        Width = 10
        Height = 15
        Caption = 'M'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label6: TLabel
        Left = 18
        Top = 34
        Width = 8
        Height = 15
        Caption = 'S'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label20: TLabel
        Left = 18
        Top = 50
        Width = 6
        Height = 15
        Caption = 'F'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object panDetect: TPanel
        Left = 2
        Top = 67
        Width = 40
        Height = 41
        Align = alBottom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'OFF'
        Color = 8454143
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object lblDetect: TLabel
          Left = 2
          Top = 2
          Width = 14
          Height = 13
          Caption = #8746
          Font.Charset = ANSI_CHARSET
          Font.Color = clRed
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object comboDetect: TComboBox
      Left = 48
      Top = 72
      Width = 89
      Height = 23
      ImeName = 'Microsoft IME 2003'
      TabOrder = 4
      Text = 'Area'
      Items.Strings = (
        '1[200]'
        '2[400]'
        '3[600]'
        '4[800]'
        '5[1000]'
        '6[1400]'
        '7[1800]'
        '8[2200]'
        '9[2600]'
        '10[3000]'
        '11[3400]'
        '12[3800]'
        '13[4200]'
        '14[4600]'
        '15[5000]')
    end
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 136
    Width = 140
    Height = 129
    Caption = ' F_Obs Sen'
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 4
    object btnObsPwrOn: TButton
      Left = 48
      Top = 13
      Width = 86
      Height = 25
      Caption = 'Pwr ON'
      TabOrder = 0
      OnClick = btnObsPwrOnClick
    end
    object btnObsPwrOff: TButton
      Left = 48
      Top = 43
      Width = 86
      Height = 25
      Caption = 'Pwr OFF'
      TabOrder = 1
      OnClick = btnObsPwrOffClick
    end
    object btnObsEnable: TButton
      Left = 48
      Top = 96
      Width = 86
      Height = 25
      Caption = 'Select'
      TabOrder = 2
      OnClick = btnObsEnableClick
    end
    object Panel1: TPanel
      Left = 2
      Top = 17
      Width = 44
      Height = 110
      Align = alLeft
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Color = 15658734
      TabOrder = 3
      object shpObsShort: TShape
        Left = 2
        Top = 34
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpObsMiddle: TShape
        Left = 2
        Top = 18
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpObsTrouble: TShape
        Left = 2
        Top = 50
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object shpObsLong: TShape
        Left = 2
        Top = 2
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object Label1: TLabel
        Left = 18
        Top = 2
        Width = 7
        Height = 15
        Caption = 'L'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label2: TLabel
        Left = 18
        Top = 18
        Width = 10
        Height = 15
        Caption = 'M'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label3: TLabel
        Left = 18
        Top = 34
        Width = 8
        Height = 15
        Caption = 'S'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label21: TLabel
        Left = 18
        Top = 50
        Width = 6
        Height = 15
        Caption = 'F'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object panObs: TPanel
        Left = 2
        Top = 67
        Width = 40
        Height = 41
        Align = alBottom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'OFF'
        Color = 8454143
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object lblObs: TLabel
          Left = 2
          Top = 2
          Width = 14
          Height = 13
          Caption = #9633
          Font.Charset = ANSI_CHARSET
          Font.Color = clRed
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object comboObs: TComboBox
      Left = 48
      Top = 72
      Width = 89
      Height = 23
      ImeName = 'Microsoft IME 2003'
      TabOrder = 4
      Text = 'Area'
      Items.Strings = (
        '1[200]'
        '2[400]'
        '3[600]'
        '4[800]'
        '5[1000]'
        '6[1200]'
        '7[1400]'
        '8[1600]'
        '9[1800]'
        '10[2000]'
        '11[2200]'
        '12[2400]'
        '13[2600]'
        '14[2800]'
        '15[3000]')
    end
  end
  object GroupBox3: TGroupBox
    Left = 288
    Top = 136
    Width = 273
    Height = 145
    Caption = ' Servo Alarm Check '
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 5
    object btnAlarmReset: TButton
      Left = 8
      Top = 116
      Width = 97
      Height = 25
      Caption = 'Alarm Reset'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = btnAlarmResetClick
    end
    object Panel3: TPanel
      Left = 2
      Top = 15
      Width = 139
      Height = 98
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Color = 15658734
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      object shpAlarm2: TShape
        Left = 96
        Top = 40
        Width = 25
        Height = 15
        Shape = stRoundRect
      end
      object shpAlarm1_R: TShape
        Left = 96
        Top = 23
        Width = 25
        Height = 15
        Shape = stRoundRect
      end
      object shpAlarm4: TShape
        Left = 96
        Top = 73
        Width = 25
        Height = 15
        Shape = stRoundRect
      end
      object shpAlarm1_F: TShape
        Left = 96
        Top = 6
        Width = 25
        Height = 15
        Shape = stRoundRect
      end
      object Label7: TLabel
        Left = 2
        Top = 6
        Width = 72
        Height = 15
        Caption = ' #1 Driving_F'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label8: TLabel
        Left = 2
        Top = 23
        Width = 74
        Height = 15
        Caption = ' #1 Driving_R'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label9: TLabel
        Left = 2
        Top = 40
        Width = 49
        Height = 15
        Caption = ' #2 Hoist'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label10: TLabel
        Left = 2
        Top = 74
        Width = 57
        Height = 15
        Caption = ' #4 Rotate'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object shpAlarm3: TShape
        Left = 96
        Top = 56
        Width = 25
        Height = 15
        Shape = stRoundRect
      end
      object Label11: TLabel
        Left = 2
        Top = 57
        Width = 46
        Height = 15
        Caption = ' #3 Shift'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
    end
    object rgServoAlarm: TRadioGroup
      Left = 144
      Top = 8
      Width = 121
      Height = 105
      Caption = ' Select '
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ItemIndex = 0
      Items.Strings = (
        '#1 Driving_F'
        '#1 Driving_R'
        '#2 Hoist'
        '#3 Shift'
        '#4 Rotate'
        'All')
      ParentFont = False
      TabOrder = 2
    end
    object btnServoON: TButton
      Left = 112
      Top = 116
      Width = 75
      Height = 25
      Caption = 'Servo ON'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = btnServoONClick
    end
    object btnServoOFF: TButton
      Left = 192
      Top = 116
      Width = 75
      Height = 25
      Caption = 'Servo OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 4
      OnClick = btnServoOFFClick
    end
  end
  object GroupBox4: TGroupBox
    Left = 409
    Top = 8
    Width = 105
    Height = 49
    Caption = 'Detect Sen '
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 6
    object Panel4: TPanel
      Left = 2
      Top = 17
      Width = 44
      Height = 30
      Align = alLeft
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Color = 15658734
      TabOrder = 0
      object shpDetect: TShape
        Left = 2
        Top = 2
        Width = 40
        Height = 16
        Align = alTop
        Shape = stRoundRect
      end
      object Label14: TLabel
        Left = 18
        Top = 2
        Width = 7
        Height = 15
        Caption = 'L'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
    end
  end
  object IOTestTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = IOTestTimerTimer
    Left = 456
    Top = 200
  end
end
