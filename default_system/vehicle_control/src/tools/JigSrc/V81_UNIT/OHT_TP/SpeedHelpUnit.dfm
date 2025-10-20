object SpeedHelpForm: TSpeedHelpForm
  Left = 617
  Top = 205
  BorderStyle = bsSingle
  Caption = 'Speed Level'
  ClientHeight = 468
  ClientWidth = 631
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
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 631
    Height = 402
    Align = alClient
    BorderStyle = bsNone
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
    Lines.Strings = (
      
        '           RPM  Speed[m/sec] Speed[m/min]'#9'[cts/sec]     [cts/mse' +
        'c]'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'5000        6.64 '#9'      398.24          682666.67           6' +
        '82.67 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'4500        5.97 '#9'      358.42          614400.00           6' +
        '14.40'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'3500        4.65 '#9'      278.77          477866.67           4' +
        '77.87 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'3000        3.98 '#9'      238.94          409600.00           4' +
        '09.60 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'2500        3.32 '#9'      199.12          341333.33           3' +
        '41.33 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'2000        2.66 '#9'      159.29          273066.67           2' +
        '73.06 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'1500        1.99 '#9'      119.47          204800.00           2' +
        '04.80 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'1000        1.33 '#9'      79.65           136553.33           1' +
        '36.55 '
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'500.0       0.66 '#9'      39.82           68266.667           6' +
        '8.266'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'300.0       0.39 '#9'      23.89           40960.000           4' +
        '0.960'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'100.0       0.13 '#9'      7.965           13653.333           1' +
        '3.653'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'50.00       0.07 '#9'      3.982           6826.0000           6' +
        '8.260'
      
        '----------------------------------------------------------------' +
        '-------------------'
      
        '  '#9'30.00       0.04 '#9'      2.389           4096.0000           4' +
        '0.960'
      
        '----------------------------------------------------------------' +
        '-------------------')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 0
    Top = 402
    Width = 631
    Height = 66
    Align = alBottom
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Panel2: TPanel
      Left = 2
      Top = 2
      Width = 627
      Height = 23
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 135
        Height = 23
        Align = alLeft
        Caption = '[m/sec]'
        Color = 10276301
        TabOrder = 0
      end
      object Panel4: TPanel
        Left = 135
        Top = 0
        Width = 135
        Height = 23
        Align = alLeft
        Caption = '[m/min]'
        TabOrder = 1
      end
      object Panel5: TPanel
        Left = 270
        Top = 0
        Width = 135
        Height = 23
        Align = alLeft
        Caption = '[cts/sec]'
        Color = 10276301
        TabOrder = 2
      end
      object Panel6: TPanel
        Left = 405
        Top = 0
        Width = 222
        Height = 23
        Align = alClient
        Caption = '[cts/msec]'
        TabOrder = 3
      end
    end
    object edtMPerSec: TEdit
      Left = 8
      Top = 32
      Width = 118
      Height = 21
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 1
      Text = '0'
      OnKeyDown = edtMPerSecKeyDown
    end
    object edtMPerMin: TEdit
      Left = 144
      Top = 32
      Width = 118
      Height = 21
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 2
      Text = '0'
      OnKeyDown = edtMPerSecKeyDown
    end
    object edtCPerSec: TEdit
      Left = 280
      Top = 32
      Width = 118
      Height = 21
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 3
      Text = '0'
      OnKeyDown = edtMPerSecKeyDown
    end
    object edtCPerMSec: TEdit
      Left = 416
      Top = 32
      Width = 118
      Height = 21
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 4
      Text = '0'
      OnKeyDown = edtMPerSecKeyDown
    end
  end
end
