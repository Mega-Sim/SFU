object frm_RouteRequest: Tfrm_RouteRequest
  Left = 0
  Top = 0
  Caption = 'frm_RouteRequest'
  ClientHeight = 393
  ClientWidth = 337
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblModeSelect: TLabel
    Left = 16
    Top = 15
    Width = 99
    Height = 19
    Caption = 'Mode Select'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblLinkCount: TLabel
    Left = 23
    Top = 53
    Width = 87
    Height = 19
    Caption = 'Link Count'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnRouteRequest: TButton
    Left = 222
    Top = 338
    Width = 96
    Height = 25
    Caption = 'Request'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = btnRouteRequestClick
  end
  object cbbModeSelect: TComboBox
    Left = 157
    Top = 17
    Width = 165
    Height = 21
    TabOrder = 1
    OnClick = cbbModeSelectClick
    Items.Strings = (
      'Node')
  end
  object edtLinkCount: TEdit
    Left = 157
    Top = 55
    Width = 165
    Height = 21
    NumbersOnly = True
    TabOrder = 2
    Text = '0'
  end
  object strngrdLaneCutList: TStringGrid
    Left = 16
    Top = 98
    Width = 306
    Height = 234
    ColCount = 2
    FixedCols = 0
    RowCount = 21
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
    TabOrder = 3
    OnGetEditMask = strngrdLaneCutListGetEditMask
    OnKeyDown = strngrdLaneCutListKeyDown
    ColWidths = (
      132
      129)
  end
  object btnInitialization: TButton
    Left = 16
    Top = 338
    Width = 96
    Height = 25
    Caption = 'Initialization'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = btnInitializationClick
  end
  object btnFileLoad: TButton
    Left = 120
    Top = 338
    Width = 96
    Height = 25
    Caption = 'File Load'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    OnClick = btnFileLoadClick
  end
  object dlgOpenTextFileDig_LaneCut: TOpenTextFileDialog
    Filter = 'txt|*.txt'
    Options = [ofReadOnly, ofHideReadOnly, ofEnableSizing]
    Left = 280
    Top = 344
  end
end
