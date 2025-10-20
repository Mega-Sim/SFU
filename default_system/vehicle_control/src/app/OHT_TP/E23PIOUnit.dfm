object E23PIOForm: TE23PIOForm
  Left = 1069
  Top = 96
  Caption = 'PIO Display'
  ClientHeight = 402
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
  PixelsPerInch = 96
  TextHeight = 13
  object Shape1: TShape
    Left = 8
    Top = 74
    Width = 25
    Height = 17
    Shape = stCircle
  end
  object Label5: TLabel
    Left = 40
    Top = 76
    Width = 26
    Height = 13
    Caption = 'CS_0'
    OnDblClick = PIOOutput
  end
  object Panel2: TPanel
    Left = 0
    Top = 361
    Width = 746
    Height = 41
    Align = alBottom
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitTop = 327
    object shapGo: TShape
      Left = 8
      Top = 8
      Width = 33
      Height = 25
      Shape = stCircle
    end
    object Label3: TLabel
      Left = 56
      Top = 12
      Width = 63
      Height = 13
      Caption = 'Go Signal'
    end
    object CheckBox1: TCheckBox
      Left = 256
      Top = 8
      Width = 217
      Height = 17
      Caption = ' Memo Display Stop'
      TabOrder = 0
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 225
    Height = 361
    Align = alLeft
    BevelOuter = bvNone
    Caption = 'Panel1'
    TabOrder = 1
    ExplicitHeight = 327
    object Panel4: TPanel
      Left = 0
      Top = 0
      Width = 225
      Height = 361
      Align = alClient
      Caption = 'Panel1'
      TabOrder = 0
      ExplicitHeight = 327
      object Panel5: TPanel
        Left = 1
        Top = 1
        Width = 223
        Height = 272
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object Panel6: TPanel
          Left = 0
          Top = 0
          Width = 97
          Height = 272
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
            Top = 158
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapABORT: TShape
            Left = 8
            Top = 116
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
            Caption = 'ABORT'
          end
          object Label7: TLabel
            Left = 40
            Top = 160
            Width = 35
            Height = 13
            Caption = 'READY'
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
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
        end
        object Panel8: TPanel
          Left = 97
          Top = 0
          Width = 31
          Height = 272
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
          Left = 128
          Top = 0
          Width = 95
          Height = 272
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
            Top = 29
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label11: TLabel
            Left = 40
            Top = 31
            Width = 35
            Height = 13
            Caption = 'VALID'
            OnDblClick = PIOOutput
          end
          object shapCS0: TShape
            Left = 8
            Top = 58
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapTRREQ: TShape
            Left = 8
            Top = 124
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapBUSY: TShape
            Left = 8
            Top = 158
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object shapCOMPT: TShape
            Left = 8
            Top = 188
            Width = 25
            Height = 17
            Shape = stCircle
          end
          object Label12: TLabel
            Left = 40
            Top = 60
            Width = 28
            Height = 13
            Caption = 'CS_0'
            OnDblClick = PIOOutput
          end
          object Label15: TLabel
            Left = 40
            Top = 126
            Width = 42
            Height = 13
            Caption = 'TR_REQ'
            OnDblClick = PIOOutput
          end
          object Label16: TLabel
            Left = 40
            Top = 160
            Width = 28
            Height = 13
            Caption = 'BUSY'
            OnDblClick = PIOOutput
          end
          object Label17: TLabel
            Left = 40
            Top = 190
            Width = 35
            Height = 13
            Caption = 'COMPT'
            OnDblClick = PIOOutput
          end
          object Label6: TLabel
            Left = 40
            Top = 92
            Width = 28
            Height = 13
            Caption = 'CS_1'
            OnDblClick = PIOOutput
          end
          object shapCS1: TShape
            Left = 8
            Top = 90
            Width = 25
            Height = 17
            Shape = stCircle
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
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
        end
      end
      object btnPIOEnable: TButton
        Left = 2
        Top = 288
        Width = 89
        Height = 25
        Caption = 'PIO Enable'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = btnPIOEnableClick
      end
      object Button2: TButton
        Left = 110
        Top = 288
        Width = 113
        Height = 25
        Caption = 'Set RF_Channel'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnClick = Button2Click
      end
    end
  end
  object Panel3: TPanel
    Left = 225
    Top = 0
    Width = 521
    Height = 361
    Align = alClient
    BevelInner = bvLowered
    Caption = 'Panel3'
    TabOrder = 2
    ExplicitHeight = 327
    object Panel10: TPanel
      Left = 2
      Top = 2
      Width = 517
      Height = 23
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel10'
      TabOrder = 0
      object Panel11: TPanel
        Left = 0
        Top = 0
        Width = 262
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
      object Panel12: TPanel
        Left = 262
        Top = 0
        Width = 255
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
    object Panel13: TPanel
      Left = 2
      Top = 25
      Width = 263
      Height = 334
      Align = alLeft
      Caption = 'Panel13'
      TabOrder = 1
      ExplicitHeight = 300
      object Memo1: TMemo
        Left = 1
        Top = 1
        Width = 261
        Height = 332
        Align = alClient
        BorderStyle = bsNone
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        ParentFont = False
        PopupMenu = PopupMenu2
        ScrollBars = ssVertical
        TabOrder = 0
        ExplicitHeight = 298
      end
    end
    object Panel14: TPanel
      Left = 265
      Top = 25
      Width = 254
      Height = 334
      Align = alClient
      Caption = 'Panel14'
      TabOrder = 2
      ExplicitHeight = 300
      object Memo2: TMemo
        Left = 1
        Top = 1
        Width = 252
        Height = 332
        Align = alClient
        BorderStyle = bsNone
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        ParentFont = False
        PopupMenu = PopupMenu2
        ScrollBars = ssVertical
        TabOrder = 0
        ExplicitHeight = 298
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 97
    Top = 168
    object OutReset1: TMenuItem
      Caption = 'Out Reset'
      OnClick = OutReset1Click
    end
  end
  object PopupMenu2: TPopupMenu
    Left = 411
    Top = 169
    object InputClear1: TMenuItem
      Caption = 'Clear'
      OnClick = InputClear1Click
    end
  end
end
