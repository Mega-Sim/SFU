object ConversionForm: TConversionForm
  Left = 746
  Top = 163
  Caption = #45800#50948#48320#54872
  ClientHeight = 671
  ClientWidth = 231
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 231
    Height = 49
    Align = alTop
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 239
    object Label1: TLabel
      Left = 8
      Top = 2
      Width = 148
      Height = 15
      Caption = #47784#53552'1'#54924#51204#45817' Encoder'#44050' = '
    end
    object Label2: TLabel
      Left = 8
      Top = 26
      Width = 170
      Height = 15
      Caption = #47784#53552'1'#54924#51204#45817' '#51060#46041#44144#47532'[mm] = '
    end
    object Edit2: TEdit
      Left = 168
      Top = 0
      Width = 57
      Height = 23
      ImeName = 'Microsoft IME 2003'
      TabOrder = 0
      Text = 'Edit2'
    end
    object Edit3: TEdit
      Left = 168
      Top = 22
      Width = 57
      Height = 23
      ImeName = 'Microsoft IME 2003'
      TabOrder = 1
      Text = 'Edit3'
    end
  end
  object RadioGroup2: TRadioGroup
    Left = 0
    Top = 49
    Width = 231
    Height = 120
    Align = alTop
    Caption = ' Select OHT Ver '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ItemIndex = 2
    Items.Strings = (
      'Reticle OHT'
      'Foup 2WD(4.5)'
      'Foup 4WD(4.6 / 5.0)'
      'Foup 4WD(6.0)'
      'Foup 4WD(6.5) / TP OHT')
    ParentFont = False
    TabOrder = 1
    OnClick = RadioGroup2Click
    ExplicitWidth = 239
  end
  object RadioGroup1: TRadioGroup
    Left = 0
    Top = 169
    Width = 231
    Height = 345
    Align = alTop
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ItemIndex = 0
    Items.Strings = (
      ' 1. [cts/msec]->[cts/sec]'
      ' 2. [cts/sec]->[cts/msec]'
      ' 3. [cts] ->[mm]'
      ' 4. [mm]->[cts]'
      ' 5. [cts/sec]->[rpm]'
      ' 6. [rpm]->[cts/sec]'
      ' 7. [cts/msec]->[rpm]'
      ' 8. [rpm]->[cts/msec]'
      ' 9. [m/sec]->[rpm]'
      '10. [rpm]->[m/sec]'
      '11. [cts/sec]->[m/sec]'
      '12. [m/sec]->[cts/sec]'
      '13. [cts/msec]->[m/sec]'
      '14. [m/sec]->[cts/msec]'
      '15. [cts/sec2]->[m/sec2]'
      '16. [m/sec2]->[cts/sec2]'
      '17. [cts/msec2]->[m/sec2]'
      '18. [m/sec2]->[cts/msec2]')
    ParentFont = False
    TabOrder = 2
    ExplicitWidth = 239
  end
  object Edit1: TEdit
    Left = 8
    Top = 528
    Width = 97
    Height = 23
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ImeName = 'Microsoft IME 2003'
    ParentFont = False
    TabOrder = 3
    Text = '0'
  end
  object Button1: TButton
    Left = 120
    Top = 528
    Width = 81
    Height = 25
    Caption = #48320#54872
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 0
    Top = 559
    Width = 231
    Height = 112
    Align = alBottom
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ImeName = 'Microsoft IME 2003'
    ParentFont = False
    TabOrder = 5
    ExplicitTop = 563
    ExplicitWidth = 239
  end
end
