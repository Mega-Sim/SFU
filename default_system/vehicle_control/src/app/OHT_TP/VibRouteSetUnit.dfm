object VibRouteSet: TVibRouteSet
  Left = 0
  Top = 0
  Caption = 'Route Setting'
  ClientHeight = 146
  ClientWidth = 305
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblStartNode: TLabel
    Left = 14
    Top = 22
    Width = 88
    Height = 19
    Caption = 'WayPoint1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblEndNode: TLabel
    Left = 14
    Top = 61
    Width = 88
    Height = 19
    Caption = 'WayPoint2'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblRound: TLabel
    Left = 14
    Top = 102
    Width = 52
    Height = 19
    Caption = 'Round'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object edtWayPoint1: TEdit
    Left = 120
    Top = 24
    Width = 89
    Height = 21
    TabOrder = 0
  end
  object edtWayPoint2: TEdit
    Left = 120
    Top = 63
    Width = 89
    Height = 21
    TabOrder = 1
  end
  object edtRound: TEdit
    Left = 120
    Top = 104
    Width = 89
    Height = 21
    TabOrder = 2
  end
  object Set: TButton
    Left = 230
    Top = 96
    Width = 55
    Height = 36
    Caption = 'Set'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = SetClick
  end
end
