object ConnectForm: TConnectForm
  Left = 1038
  Top = 418
  BorderStyle = bsNone
  Caption = 'Login...'
  ClientHeight = 161
  ClientWidth = 385
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 385
    Height = 161
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 0
    object Label1: TLabel
      Left = 12
      Top = 8
      Width = 77
      Height = 15
      Caption = 'Select OHT.....'
      Color = clBlack
      Font.Charset = ANSI_CHARSET
      Font.Color = clWhite
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
    end
    object btnConnectEdit: TButton
      Left = 12
      Top = 115
      Width = 55
      Height = 25
      Caption = 'Edit'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnMouseDown = btnConnectEditMouseDown
    end
    object ConnectName: TComboBox
      Left = 9
      Top = 28
      Width = 241
      Height = 23
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
      ParentFont = False
      TabOrder = 1
      TextHint = 'Select OHT'
      OnClick = ConnectNameClick
      OnEnter = ConnectNameEnter
      Items.Strings = (
        '1212'
        '1212'
        '121212')
    end
    object btnConnect: TButton
      Left = 73
      Top = 115
      Width = 55
      Height = 25
      Caption = 'Connect'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      OnClick = btnConnectClick
    end
    object btnCancel: TButton
      Left = 134
      Top = 115
      Width = 55
      Height = 25
      Caption = 'Cancel'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = btnCancelClick
    end
    object btnConnectAssistant: TButton
      Left = 195
      Top = 103
      Width = 124
      Height = 37
      Caption = 'Assistant Connect'
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold, fsItalic]
      ParentFont = False
      TabOrder = 4
      OnClick = btnConnectAssistantClick
    end
    object LinePickupColor: TPanel
      Left = 256
      Top = 28
      Width = 42
      Height = 50
      Caption = 'Color'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 5
      OnClick = LinePickupColorClick
    end
    object btnIPEdit: TButton
      Left = 304
      Top = 28
      Width = 49
      Height = 25
      Caption = 'IP:Edit'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 6
      OnClick = btnIPEditClick
    end
    object btnIPLoad: TButton
      Left = 304
      Top = 54
      Width = 49
      Height = 25
      Caption = 'IP:Load'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 7
      OnClick = btnIPLoadClick
    end
    object ConnectIP: TEdit
      Left = 9
      Top = 57
      Width = 241
      Height = 21
      BorderStyle = bsNone
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
      ParentFont = False
      ReadOnly = True
      TabOrder = 8
      TextHint = 'IP Address'
      OnKeyUp = ConnectIPKeyUp
    end
  end
  object pmOHTInfo: TPopupMenu
    TrackButton = tbLeftButton
    Left = 324
    Top = 96
    object pmnuAdd: TMenuItem
      Caption = 'Add'
      object pmnuAddYES: TMenuItem
        Caption = 'YES'
        OnClick = pmnuAddYESClick
      end
    end
    object pmnuEdit: TMenuItem
      Caption = 'Edit'
      object pmnuEditYES: TMenuItem
        Caption = 'YES'
        OnClick = pmnuEditYESClick
      end
    end
    object pmnuDelete: TMenuItem
      Caption = 'Delete'
      object pmnuDeleteYES: TMenuItem
        Caption = 'YES'
        OnClick = pmnuDeleteYESClick
      end
    end
  end
end
