object RotateOrgForm: TRotateOrgForm
  Left = 510
  Top = 410
  Caption = 'Rotate '
  ClientHeight = 160
  ClientWidth = 162
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label15: TLabel
    Left = 9
    Top = 4
    Width = 48
    Height = 13
    Caption = 'Rotate'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label1: TLabel
    Left = 8
    Top = 104
    Width = 152
    Height = 13
    Caption = #44592#44228#51201' '#50896#51216#48373#44480#49884#51032' offset'#44050
  end
  object Label2: TLabel
    Left = 8
    Top = 144
    Width = 108
    Height = 13
    Caption = '==> '#48152#46300#49884' '#51221#49688#44050
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 8
    Top = 120
    Width = 112
    Height = 13
    Caption = '( -3276700 ~ 3276700 )'
  end
  object panAx4_ServoON: TPanel
    Left = 10
    Top = 22
    Width = 10
    Height = 18
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = clLime
    TabOrder = 0
  end
  object Panel12: TPanel
    Left = 20
    Top = 22
    Width = 141
    Height = 18
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 1
    object lblMotor4_pos: TLabel
      Left = 8
      Top = 3
      Width = 7
      Height = 13
      Caption = '0'
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlue
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentFont = False
    end
    object shp4Run: TShape
      Left = 118
      Top = 1
      Width = 25
      Height = 15
      Pen.Color = clSilver
      Shape = stCircle
    end
  end
  object Edit1: TEdit
    Left = 10
    Top = 48
    Width = 151
    Height = 21
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ImeName = 'Microsoft IME 2003'
    ParentFont = False
    TabOrder = 2
  end
  object btnOK: TButton
    Left = 8
    Top = 72
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnClose: TButton
    Left = 88
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 4
    OnClick = btnCloseClick
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer1Timer
    Left = 72
    Top = 8
  end
end
