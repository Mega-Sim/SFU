object MHoistForm: TMHoistForm
  Left = 392
  Top = 284
  Caption = 'Move Hoist'
  ClientHeight = 184
  ClientWidth = 194
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
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 194
    Height = 184
    Align = alClient
    Caption = ' #2(Hoist) '
    Color = 15662830
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    TabOrder = 0
    object Label5: TLabel
      Left = 4
      Top = 24
      Width = 56
      Height = 13
      Caption = '#2 Pos:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Dist: TLabel
      Left = 8
      Top = 56
      Width = 40
      Height = 13
      Caption = 'Dist='
    end
    object Label2: TLabel
      Left = 152
      Top = 56
      Width = 40
      Height = 13
      Caption = '[cts]'
    end
    object Label3: TLabel
      Left = 8
      Top = 80
      Width = 48
      Height = 13
      Caption = 'speed='
    end
    object Label4: TLabel
      Left = 152
      Top = 80
      Width = 40
      Height = 13
      Caption = '[rpm]'
    end
    object Label6: TLabel
      Left = 8
      Top = 104
      Width = 64
      Height = 13
      Caption = 'Acctime='
    end
    object Label7: TLabel
      Left = 152
      Top = 104
      Width = 48
      Height = 13
      Caption = '[msec]'
    end
    object Panel10: TPanel
      Left = 66
      Top = 20
      Width = 118
      Height = 20
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object lblMotor2: TLabel
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
      object shp2Run: TShape
        Left = 95
        Top = 1
        Width = 25
        Height = 17
        Pen.Color = clSilver
        Shape = stCircle
      end
    end
    object btnHoistMoveDST: TBitBtn
      Left = 8
      Top = 138
      Width = 97
      Height = 40
      Caption = '&MOVE[2]'
      DoubleBuffered = True
      Glyph.Data = {
        76010000424D760100000000000076000000280000001B000000100000000100
        0400000000000001000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333330000033333333333333333337333333300000333333777773
        3333334773333330000033333444447333333CC477333330000033333CCCC473
        3333CCCC47733330000033333CCCC473333CCCCCC4773330000033333CCCC473
        33CCCCCC44433330000033333CCCC4733333CCCC47333330000033333CCCC477
        3333CCCC473333300000333CCCCCCCCC3333CCCC4733333000003333CCCCCCC3
        3333CCCC47333330000033333CCCCC333333CCCC473333300000333333CCC333
        3333CCCC4333333000003333333C333333333333333333300000333333333333
        3333333333333330000033333333333333333333333333300000}
      ModalResult = 8
      ParentDoubleBuffered = False
      TabOrder = 1
      OnClick = btnHoistMoveDSTClick
    end
    object Panel1: TPanel
      Left = 50
      Top = 52
      Width = 103
      Height = 20
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 2
      object lblDist: TLabel
        Left = 8
        Top = 3
        Width = 35
        Height = 13
        Caption = '10000'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
      end
    end
    object edtSpeed: TEdit
      Tag = 1
      Left = 74
      Top = 74
      Width = 80
      Height = 23
      Color = 12189695
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ParentFont = False
      TabOrder = 3
      Text = '3000'
    end
    object edtAccel: TEdit
      Tag = 1
      Left = 74
      Top = 98
      Width = 80
      Height = 23
      Color = 12189695
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ParentFont = False
      TabOrder = 4
      Text = '500'
    end
    object btnCancel: TBitBtn
      Left = 112
      Top = 138
      Width = 81
      Height = 40
      Caption = 'Cancel'
      DoubleBuffered = True
      ModalResult = 8
      ParentDoubleBuffered = False
      TabOrder = 5
      OnClick = btnCancelClick
    end
  end
end
