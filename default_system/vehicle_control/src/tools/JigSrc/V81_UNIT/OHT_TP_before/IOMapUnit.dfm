object IOMapForm: TIOMapForm
  Left = 1036
  Top = 162
  Caption = 'IO Map'
  ClientHeight = 668
  ClientWidth = 352
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pgcIOMap: TPageControl
    Left = 0
    Top = 0
    Width = 352
    Height = 668
    ActivePage = tabOutput
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object tabInput: TTabSheet
      Caption = 'Input'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object strGridInput: TStringGrid
        Left = 0
        Top = 0
        Width = 344
        Height = 638
        Align = alClient
        ColCount = 4
        DefaultColWidth = 20
        DefaultRowHeight = 15
        RowCount = 72
        FixedRows = 0
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnDrawCell = strGridInputDrawCell
        ColWidths = (
          37
          201
          55
          49)
        RowHeights = (
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15)
      end
    end
    object tabOutput: TTabSheet
      Caption = 'Output'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object strGridOutput: TStringGrid
        Left = 0
        Top = 0
        Width = 344
        Height = 638
        Align = alClient
        ColCount = 3
        DefaultColWidth = 20
        DefaultRowHeight = 15
        RowCount = 72
        FixedRows = 0
        PopupMenu = popupOutput
        TabOrder = 0
        OnClick = strGridOutputClick
        OnDrawCell = strGridOutputDrawCell
        ColWidths = (
          37
          217
          55)
        RowHeights = (
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15
          15)
      end
    end
    object tabCheckONOFF: TTabSheet
      Caption = 'CheckONOFF'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object strgridUseOnOff: TStringGrid
        Left = 0
        Top = 0
        Width = 344
        Height = 638
        Align = alClient
        ColCount = 3
        DefaultColWidth = 20
        DefaultRowHeight = 15
        RowCount = 34
        FixedRows = 0
        PopupMenu = popupCheckONOFF
        TabOrder = 0
        OnClick = strgridUseOnOffClick
        OnDrawCell = strgridUseOnOffDrawCell
        ColWidths = (
          20
          155
          55)
      end
    end
    object tabMotor: TTabSheet
      Caption = 'Motor Status'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object strGridMotor: TStringGrid
        Left = 0
        Top = 0
        Width = 344
        Height = 638
        Align = alClient
        ColCount = 3
        DefaultColWidth = 20
        DefaultRowHeight = 15
        RowCount = 34
        FixedRows = 0
        PopupMenu = popupCheckONOFF
        TabOrder = 0
        OnClick = strgridUseOnOffClick
        OnDrawCell = strGridMotorDrawCell
        ColWidths = (
          20
          182
          141)
      end
    end
  end
  object popupCheckONOFF: TPopupMenu
    AutoPopup = False
    OnPopup = popupCheckONOFFPopup
    Left = 292
    Top = 576
    object mniSetON: TMenuItem
      Caption = 'ON'
      OnClick = mniSetONClick
    end
    object mniSetOFF: TMenuItem
      Caption = 'OFF'
      OnClick = mniSetOFFClick
    end
  end
  object popupOutput: TPopupMenu
    AutoPopup = False
    OnPopup = popupOutputPopup
    Left = 256
    Top = 576
    object mniSetTrue: TMenuItem
      Caption = '1'
      OnClick = mniSetTrueClick
    end
    object mniSetFalse: TMenuItem
      Caption = '0'
      OnClick = mniSetFalseClick
    end
  end
end
