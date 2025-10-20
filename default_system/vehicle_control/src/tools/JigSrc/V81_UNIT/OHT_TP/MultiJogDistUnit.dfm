object MultiJogDistForm: TMultiJogDistForm
  Left = 444
  Top = 162
  Caption = 'MultiJogDistForm'
  ClientHeight = 410
  ClientWidth = 600
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 600
    Height = 410
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 360
      Width = 91
      Height = 13
      Caption = 'Cmd Number = '
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 16
      Top = 384
      Width = 270
      Height = 12
      Caption = '*  Cmd Number'#44032' 0'#51060' '#50500#45768#47732' Reset '#45572#47480' '#54980' '#49884#51089
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #44404#47548#52404
      Font.Style = []
      ParentFont = False
    end
    object Panel2: TPanel
      Left = 1
      Top = 305
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 0
      object Panel3: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button1: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move8'
          TabOrder = 0
          OnClick = Button1Click
        end
      end
      object Panel4: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist8: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel5: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel8: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel6: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec8_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel7: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec8_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel8: TPanel
      Left = 1
      Top = 1
      Width = 598
      Height = 24
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 1
      object Panel9: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 24
        Align = alClient
        Caption = #47749#47161
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
      object Panel10: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 24
        Align = alLeft
        Caption = #44144#47532'[mm]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object Panel11: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 24
        Align = alLeft
        Caption = #49549#46020'[m/sec]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
      object Panel12: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 24
        Align = alLeft
        Caption = #44048#49549#46020'[m/sec2]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
      end
      object Panel13: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 24
        Align = alLeft
        Caption = #44032#49549#46020'[m/sec2]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
      end
    end
    object Panel14: TPanel
      Left = 1
      Top = 265
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 2
      object Panel15: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button2: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move7'
          TabOrder = 0
          OnClick = Button2Click
        end
      end
      object Panel16: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist7: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel17: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel7: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel18: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec7_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel19: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec7_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel20: TPanel
      Left = 1
      Top = 225
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 3
      object Panel21: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button3: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move6'
          TabOrder = 0
          OnClick = Button3Click
        end
      end
      object Panel22: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist6: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel23: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel6: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel24: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec6_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel25: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec6_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel26: TPanel
      Left = 1
      Top = 185
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 4
      object Panel27: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button4: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move5'
          TabOrder = 0
          OnClick = Button4Click
        end
      end
      object Panel28: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist5: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel29: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel5: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel30: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec5_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel31: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec5_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel32: TPanel
      Left = 1
      Top = 145
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 5
      object Panel33: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button5: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move4'
          TabOrder = 0
          OnClick = Button5Click
        end
      end
      object Panel34: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist4: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel35: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel4: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel36: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec4_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel37: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec4_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel38: TPanel
      Left = 1
      Top = 105
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 6
      object Panel39: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button6: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move3'
          TabOrder = 0
          OnClick = Button6Click
        end
      end
      object Panel40: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist3: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel41: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel3: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel42: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec3_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel43: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec3_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel44: TPanel
      Left = 1
      Top = 65
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 7
      object Panel45: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button7: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move2'
          TabOrder = 0
          OnClick = Button7Click
        end
      end
      object Panel46: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist2: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel47: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel2: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel48: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec2_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel49: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec2_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Panel50: TPanel
      Left = 1
      Top = 25
      Width = 598
      Height = 40
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel2'
      TabOrder = 8
      object Panel51: TPanel
        Left = 484
        Top = 0
        Width = 114
        Height = 40
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object Button8: TButton
          Left = 24
          Top = 8
          Width = 75
          Height = 25
          Caption = 'Move1'
          TabOrder = 0
          OnClick = Button8Click
        end
      end
      object Panel52: TPanel
        Left = 363
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object edtDist1: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel53: TPanel
        Left = 242
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object edtVel1: TEdit
          Left = 24
          Top = 11
          Width = 65
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel54: TPanel
        Left = 121
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object edtmpersec1_DEC: TEdit
          Left = 24
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
      object Panel55: TPanel
        Left = 0
        Top = 0
        Width = 121
        Height = 40
        Align = alLeft
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #44404#47548#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object edtmpersec1_ACC: TEdit
          Left = 16
          Top = 11
          Width = 81
          Height = 20
          ImeName = 'Microsoft IME 2003'
          TabOrder = 0
        end
      end
    end
    object Button9: TButton
      Left = 320
      Top = 360
      Width = 75
      Height = 25
      Caption = 'Reset'
      TabOrder = 9
      OnClick = Button9Click
    end
    object Button10: TButton
      Left = 408
      Top = 360
      Width = 129
      Height = 25
      Caption = #49692#52264#51201' '#51613#44032' '#47749#47161
      TabOrder = 10
      OnClick = Button10Click
    end
  end
  object Timer1: TTimer
    Interval = 200
    OnTimer = Timer1Timer
    Left = 552
    Top = 368
  end
end
