object DebugForm: TDebugForm
  Left = 509
  Top = 214
  Caption = 'DebugForm'
  ClientHeight = 368
  ClientWidth = 611
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 611
    Height = 22
    Align = alTop
    Caption = 'Panel1'
    TabOrder = 0
    ExplicitWidth = 619
    object DebugEdit: TEdit
      Left = 16
      Top = 0
      Width = 121
      Height = 21
      BorderStyle = bsNone
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ParentFont = False
      TabOrder = 0
      OnKeyDown = DebugEditKeyDown
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 22
    Width = 611
    Height = 346
    Align = alClient
    Caption = 'Panel2'
    TabOrder = 1
    ExplicitWidth = 619
    ExplicitHeight = 350
    object DebugListB: TCheckListBox
      Left = 1
      Top = 1
      Width = 617
      Height = 348
      Align = alClient
      BorderStyle = bsNone
      Color = 14540253
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = []
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ItemHeight = 13
      ParentFont = False
      PopupMenu = PopupMenu1
      TabOrder = 0
      OnClick = DebugListBClick
      OnDblClick = DebugListBDblClick
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 240
    Top = 137
    object Clear1: TMenuItem
      Caption = 'All Clear'
      OnClick = Clear1Click
    end
  end
end
