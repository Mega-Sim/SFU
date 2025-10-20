object ChIOFileForm: TChIOFileForm
  Left = 623
  Top = 239
  Width = 172
  Height = 137
  Caption = 'Change IO File Format'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 164
    Height = 103
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 24
      Top = 80
      Width = 82
      Height = 13
      Caption = 'Multi Select ∞°¥…'
    end
    object btnOpen: TButton
      Left = 40
      Top = 16
      Width = 65
      Height = 57
      Caption = 'Open'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'πŸ≈¡√º'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = btnOpenClick
    end
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 112
    Top = 8
  end
end
