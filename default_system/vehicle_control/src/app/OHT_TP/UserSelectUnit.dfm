object UserSelectForm: TUserSelectForm
  Left = 754
  Top = 252
  BorderStyle = bsNone
  Caption = 'UserSelectForm'
  ClientHeight = 138
  ClientWidth = 281
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 281
    Height = 138
    Align = alClient
    BevelInner = bvLowered
    Color = clSilver
    ParentBackground = False
    TabOrder = 0
    object lbPw: TLabel
      Left = 83
      Top = 71
      Width = 61
      Height = 16
      Caption = 'Password'
      Color = clSilver
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      Transparent = False
    end
    object rgUserSelect: TRadioGroup
      Left = 2
      Top = 2
      Width = 79
      Height = 95
      Align = alCustom
      Caption = ' '#49324#50857#51088' '
      Color = clSilver
      ItemIndex = 0
      Items.Strings = (
        #50868#50689#51088
        #44060#48156#51088)
      ParentBackground = False
      ParentColor = False
      TabOrder = 3
      OnClick = rgUserSelectClick
    end
    object btnCancel: TBitBtn
      Left = 150
      Top = 103
      Width = 81
      Height = 25
      DoubleBuffered = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      Kind = bkCancel
      NumGlyphs = 2
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      OnClick = btnCancelClick
    end
    object edtPwd: TEdit
      Left = 150
      Top = 71
      Width = 121
      Height = 20
      CharCase = ecUpperCase
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #44404#47548#52404
      Font.Style = []
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ParentFont = False
      PasswordChar = '*'
      TabOrder = 1
      Text = '2580'
      OnKeyDown = edtPwdKeyDown
    end
    object btnSelect: TBitBtn
      Left = 39
      Top = 103
      Width = 75
      Height = 25
      Caption = 'OK'
      DoubleBuffered = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial Black'
      Font.Style = [fsBold]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00DDDDDDDDDDDD
        DDDDDDDD00DDDDDDDDDDDDDDA70DDDDDDDDDDDDD7A70DDDDDDDDDDDDA7A70DDD
        DDDDDDDD7A7A70DDDDDDDDDDA7A7A70DDDDDDDDD7A7A7A70DDDDDDDDA7A7A7A7
        DDDDDDDD7A7A7A7DDDDDDDDDA7A7A7DDDDDDDDDD7A7A7DDDDDDDDDDDA7A7DDDD
        DDDDDDDD7A7DDDDDDDDDDDDDA7DDDDDDDDDDDDDDDDDDDDDDDDDD}
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 2
      OnClick = btnSelectClick
    end
  end
end
