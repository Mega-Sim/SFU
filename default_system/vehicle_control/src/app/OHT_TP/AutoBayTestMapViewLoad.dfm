object frm_MapViewLoad: Tfrm_MapViewLoad
  Left = 0
  Top = 0
  Caption = 'frm_MapViewLoad'
  ClientHeight = 179
  ClientWidth = 310
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblMapVersion: TLabel
    Left = 8
    Top = 24
    Width = 95
    Height = 19
    Caption = 'MapVersion'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblStatus: TLabel
    Left = 165
    Top = 24
    Width = 52
    Height = 19
    Caption = 'Status'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnMapViewLoad: TButton
    Left = 8
    Top = 120
    Width = 137
    Height = 47
    Caption = 'MapView Load'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = btnMapViewLoadClick
  end
  object btnSimulation: TButton
    Left = 165
    Top = 120
    Width = 137
    Height = 47
    Caption = 'Simulation'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    OnClick = btnSimulationClick
  end
  object edtMapVersion: TEdit
    Left = 8
    Top = 64
    Width = 137
    Height = 21
    Color = clMoneyGreen
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
    Text = 'No Reponse'
  end
  object edtStatus: TEdit
    Left = 165
    Top = 64
    Width = 137
    Height = 21
    Color = clMoneyGreen
    ReadOnly = True
    TabOrder = 3
    Text = 'No Response'
  end
  object tmrUDPTimerMapview: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrUDPTimerMapviewTimer
    Left = 248
    Top = 16
  end
end
