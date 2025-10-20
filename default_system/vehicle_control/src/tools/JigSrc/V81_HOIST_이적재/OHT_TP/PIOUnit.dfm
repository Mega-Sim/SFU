object PIOForm: TPIOForm
  Left = 2480
  Top = 288
  Caption = 'PIO Display'
  ClientHeight = 361
  ClientWidth = 746
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 249
    Height = 361
    Align = alLeft
    BevelOuter = bvNone
    Caption = 'Panel1'
    TabOrder = 0
    ExplicitHeight = 327
    object Panel4: TPanel
      Left = 0
      Top = 0
      Width = 249
      Height = 361
      Align = alLeft
      Caption = 'Panel1'
      TabOrder = 0
      ExplicitHeight = 327
      object shpGoLeft: TShape
        Left = 4
        Top = 292
        Width = 25
        Height = 22
        Shape = stCircle
      end
      object Label3: TLabel
        Left = 32
        Top = 296
        Width = 42
        Height = 15
        Caption = 'Left GO'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label5: TLabel
        Left = 30
        Top = 319
        Width = 49
        Height = 15
        Caption = 'Right GO'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object shpGoRight: TShape
        Left = 3
        Top = 316
        Width = 25
        Height = 21
        Shape = stCircle
      end
      object shpEQPIO_Left_Sel: TShape
        Left = 134
        Top = 292
        Width = 25
        Height = 22
        Shape = stCircle
      end
      object Label6: TLabel
        Left = 162
        Top = 296
        Width = 61
        Height = 15
        Caption = 'Left Select'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object shpEQPIO_Right_Sel: TShape
        Left = 133
        Top = 316
        Width = 25
        Height = 21
        Shape = stCircle
      end
      object Label9: TLabel
        Left = 160
        Top = 319
        Width = 68
        Height = 15
        Caption = 'Right Select'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label10: TLabel
        Left = 21
        Top = 273
        Width = 14
        Height = 13
        Caption = 'ID:'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = '@'#47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbl1: TLabel
        Left = 141
        Top = 273
        Width = 18
        Height = 13
        Caption = 'CH:'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = '@'#47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object btnPIOEnable: TButton
        Left = 8
        Top = 237
        Width = 89
        Height = 25
        Caption = 'PIO Enable'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = btnPIOEnableClick
      end
      object Button2: TButton
        Left = 128
        Top = 237
        Width = 113
        Height = 25
        Caption = 'Set RF_Channel'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = Button2Click
      end
      object Panel5: TPanel
        Left = 1
        Top = 1
        Width = 247
        Height = 232
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 2
        object Panel6: TPanel
          Left = 0
          Top = 0
          Width = 97
          Height = 232
          Align = alLeft
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          object shapLREQ: TShape
            Left = 8
            Top = 32
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label1: TLabel
            Left = 40
            Top = 34
            Width = 35
            Height = 13
            Caption = 'L_REQ'
          end
          object shapUREQ: TShape
            Left = 8
            Top = 74
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapREADY: TShape
            Left = 8
            Top = 116
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapHOAVBL: TShape
            Left = 8
            Top = 158
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapES: TShape
            Left = 8
            Top = 200
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label2: TLabel
            Left = 40
            Top = 76
            Width = 35
            Height = 13
            Caption = 'U_REQ'
          end
          object Label4: TLabel
            Left = 40
            Top = 118
            Width = 35
            Height = 13
            Caption = 'READY'
          end
          object Label7: TLabel
            Left = 40
            Top = 160
            Width = 49
            Height = 13
            Caption = 'HO_AVBL'
          end
          object Label8: TLabel
            Left = 40
            Top = 202
            Width = 14
            Height = 13
            Caption = 'ES'
          end
          object Panel7: TPanel
            Left = 2
            Top = 2
            Width = 93
            Height = 24
            Align = alTop
            BevelInner = bvLowered
            Caption = 'Input'
            Color = 8454143
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
        end
        object Panel8: TPanel
          Left = 97
          Top = 0
          Width = 55
          Height = 232
          Align = alClient
          BevelOuter = bvNone
          Caption = '<->'
          Font.Charset = HANGEUL_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = #54756#46300#46972#51064
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
        end
        object Panel9: TPanel
          Left = 152
          Top = 0
          Width = 95
          Height = 232
          Align = alRight
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          object shapVALID: TShape
            Left = 8
            Top = 32
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label11: TLabel
            Left = 40
            Top = 34
            Width = 35
            Height = 13
            Caption = 'VALID'
            OnDblClick = PIOOutput
          end
          object shapCS0: TShape
            Left = 9
            Top = 74
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapTRREQ: TShape
            Left = 8
            Top = 117
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapBUSY: TShape
            Left = 9
            Top = 159
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapCOMPT: TShape
            Left = 10
            Top = 200
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label12: TLabel
            Left = 41
            Top = 76
            Width = 28
            Height = 13
            Caption = 'CS_0'
            OnDblClick = PIOOutput
          end
          object Label15: TLabel
            Left = 40
            Top = 119
            Width = 42
            Height = 13
            Caption = 'TR_REQ'
            OnDblClick = PIOOutput
          end
          object Label16: TLabel
            Left = 41
            Top = 161
            Width = 28
            Height = 13
            Caption = 'BUSY'
            OnDblClick = PIOOutput
          end
          object Label17: TLabel
            Left = 42
            Top = 202
            Width = 35
            Height = 13
            Caption = 'COMPT'
            OnDblClick = PIOOutput
          end
          object Panel15: TPanel
            Left = 2
            Top = 2
            Width = 91
            Height = 24
            Align = alTop
            BevelInner = bvLowered
            Caption = 'Output'
            Color = 8454143
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
        end
      end
      object CheckBox1: TCheckBox
        Left = 90
        Top = 336
        Width = 153
        Height = 17
        Caption = ' Memo Display Stop'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
      end
      object edtID: TEdit
        Left = 41
        Top = 268
        Width = 87
        Height = 21
        ImeName = 'Microsoft IME 2010'
        NumbersOnly = True
        TabOrder = 4
        Text = 'ID'
      end
      object edtCH: TEdit
        Left = 165
        Top = 269
        Width = 75
        Height = 21
        ImeName = 'Microsoft IME 2010'
        NumbersOnly = True
        TabOrder = 5
        Text = 'CH'
      end
    end
  end
  object Panel3: TPanel
    Left = 249
    Top = 0
    Width = 497
    Height = 361
    Align = alClient
    BevelOuter = bvNone
    Caption = 'Panel3'
    TabOrder = 1
    ExplicitHeight = 327
    object Panel10: TPanel
      Left = 0
      Top = 0
      Width = 497
      Height = 249
      Align = alTop
      BevelInner = bvLowered
      Caption = 'Panel3'
      TabOrder = 0
      object Panel11: TPanel
        Left = 2
        Top = 2
        Width = 493
        Height = 23
        Align = alTop
        BevelOuter = bvNone
        Caption = 'Panel10'
        TabOrder = 0
        object Panel12: TPanel
          Left = 0
          Top = 0
          Width = 308
          Height = 23
          Align = alLeft
          Caption = 'Input'
          Color = 8421440
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
        object Panel13: TPanel
          Left = 308
          Top = 0
          Width = 185
          Height = 23
          Align = alClient
          Caption = 'Output'
          Color = 8421440
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
        end
      end
      object Panel14: TPanel
        Left = 2
        Top = 25
        Width = 308
        Height = 222
        Align = alLeft
        Caption = 'Panel13'
        TabOrder = 1
        object Memo1: TMemo
          Left = 1
          Top = 1
          Width = 306
          Height = 220
          Align = alClient
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
          ParentFont = False
          PopupMenu = PopupMenu2
          ScrollBars = ssVertical
          TabOrder = 0
        end
      end
      object Panel16: TPanel
        Left = 310
        Top = 25
        Width = 185
        Height = 222
        Align = alClient
        Caption = 'Panel14'
        TabOrder = 2
        object Memo2: TMemo
          Left = 1
          Top = 1
          Width = 183
          Height = 220
          Align = alClient
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
          ParentFont = False
          PopupMenu = PopupMenu2
          ScrollBars = ssVertical
          TabOrder = 0
        end
      end
    end
    object EQPIOGB: TGroupBox
      Left = 4
      Top = 254
      Width = 241
      Height = 73
      Caption = ' EQ PIO '
      Color = 15662830
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 1
      object EqPIOInPanel: TPanel
        Left = 32
        Top = 16
        Width = 121
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '0 0 0 0 0 0 0 0'
        TabOrder = 0
      end
      object EqPioInCaption: TPanel
        Left = 5
        Top = 16
        Width = 29
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'IN'
        TabOrder = 1
      end
      object EqPIOOutCaption: TPanel
        Left = 5
        Top = 43
        Width = 29
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'OUT'
        TabOrder = 2
      end
      object EqPIOOutPanel: TPanel
        Left = 32
        Top = 43
        Width = 121
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '0 0 0 0 0 0 0 0'
        TabOrder = 3
      end
    end
    object HandPIOGB: TGroupBox
      Left = 248
      Top = 254
      Width = 241
      Height = 73
      Caption = ' Hand PIO '
      Color = 15662830
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 2
      object HandPIOInPanel: TPanel
        Left = 32
        Top = 16
        Width = 129
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '0 0 0 0 0 0 0 0 '
        TabOrder = 0
      end
      object HandPIOOutPanel: TPanel
        Left = 32
        Top = 43
        Width = 129
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '0 0 0 0 0 0 0 0 '
        TabOrder = 1
      end
      object HandPIOOutCaption: TPanel
        Left = 5
        Top = 43
        Width = 29
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'OUT'
        TabOrder = 2
      end
      object HandPIOInCaption: TPanel
        Left = 5
        Top = 16
        Width = 29
        Height = 24
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'IN'
        TabOrder = 3
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 113
    Top = 32
    object OutReset1: TMenuItem
      Caption = 'Out Reset'
      OnClick = OutReset1Click
    end
  end
  object PopupMenu2: TPopupMenu
    Left = 459
    Top = 121
    object InputClear1: TMenuItem
      Caption = 'Clear'
      OnClick = InputClear1Click
    end
  end
  object timerUpdateDisplay: TTimer
    OnTimer = timerUpdateDisplayTimer
    Left = 616
    Top = 64
  end
end
