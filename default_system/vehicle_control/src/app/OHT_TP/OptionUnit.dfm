object OptionForm: TOptionForm
  Left = 0
  Top = 0
  Caption = 'Options'
  ClientHeight = 453
  ClientWidth = 554
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object splOption: TSplitter
    Left = 121
    Top = 0
    Width = 12
    Height = 453
    ExplicitLeft = 124
    ExplicitHeight = 312
  end
  object tvOption: TTreeView
    Left = 0
    Top = 0
    Width = 121
    Height = 453
    Align = alLeft
    Indent = 19
    TabOrder = 0
    OnClick = tvOptionClick
    Items.NodeData = {
      0302000000240000000000000000000000FFFFFFFFFFFFFFFF00000000000000
      000000000001034C006F006700240000000000000000000000FFFFFFFFFFFFFF
      FF0000000000000000000000000103450074006300}
    ExplicitHeight = 312
  end
  object PageControl1: TPageControl
    Left = 133
    Top = 0
    Width = 421
    Height = 453
    ActivePage = LogSheet
    Align = alClient
    TabOrder = 1
    object LogSheet: TTabSheet
      Caption = 'Log'
      TabVisible = False
      ExplicitTop = 24
      ExplicitWidth = 359
      ExplicitHeight = 327
      object LogValueListEditor1: TValueListEditor
        Left = 0
        Top = 0
        Width = 413
        Height = 443
        Align = alClient
        TabOrder = 0
        OnKeyDown = LogValueListEditor1KeyDown
        ExplicitWidth = 359
        ExplicitHeight = 327
        ColWidths = (
          197
          210)
      end
    end
    object EtcSheet: TTabSheet
      Caption = 'Etc'
      ImageIndex = 1
      TabVisible = False
      ExplicitLeft = 20
      ExplicitTop = 56
      ExplicitWidth = 359
      ExplicitHeight = 327
    end
  end
end
