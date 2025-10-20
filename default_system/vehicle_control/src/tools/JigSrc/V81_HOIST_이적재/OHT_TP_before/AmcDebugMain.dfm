object AmcDebugForm: TAmcDebugForm
  Left = 594
  Top = 206
  Caption = 'AmcDebugForm'
  ClientHeight = 579
  ClientWidth = 415
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Arial'
  Font.Style = [fsBold]
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 15
  object PageControl1: TPageControl
    Left = 0
    Top = 57
    Width = 415
    Height = 522
    ActivePage = Log_RawData
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object Log_RawData: TTabSheet
      Caption = 'Log'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 407
        Height = 492
        Align = alClient
        ImeName = 'Microsoft Office IME 2007'
        PopupMenu = LogClearPopup
        ScrollBars = ssBoth
        TabOrder = 0
      end
    end
    object ResultData: TTabSheet
      Caption = 'Result'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Memo2: TMemo
        Left = 0
        Top = 0
        Width = 381
        Height = 518
        Align = alClient
        BiDiMode = bdLeftToRight
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ImeName = 'Microsoft Office IME 2007'
        ParentBiDiMode = False
        ParentFont = False
        PopupMenu = ResultClearPopup
        ScrollBars = ssBoth
        TabOrder = 0
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 415
    Height = 57
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object LogTypeRadio: TRadioGroup
      Left = 0
      Top = 0
      Width = 127
      Height = 57
      Align = alLeft
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ItemIndex = 0
      Items.Strings = (
        'Error Log'
        'AMC Data ')
      ParentFont = False
      TabOrder = 0
    end
  end
  object MainMenu1: TMainMenu
    Left = 160
    Top = 104
    object Analysis: TMenuItem
      Caption = 'Analysis'
      OnClick = AnalysisClick
    end
    object TMenuItem
    end
    object TMenuItem
    end
    object Clear1: TMenuItem
      Caption = 'All Clear'
      OnClick = Clear1Click
    end
  end
  object LogClearPopup: TPopupMenu
    Left = 196
    Top = 112
    object LogClear1: TMenuItem
      Caption = 'LogClear'
      OnClick = LogClear1Click
    end
  end
  object ResultClearPopup: TPopupMenu
    Left = 260
    Top = 128
    object ResultClear1: TMenuItem
      Caption = 'Result Clear'
      OnClick = ResultClear1Click
    end
  end
end
