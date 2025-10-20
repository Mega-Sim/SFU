object FileListForm: TFileListForm
  Left = 414
  Top = 118
  Caption = 'FileListForm'
  ClientHeight = 681
  ClientWidth = 437
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object ListView1: TListView
    Left = 0
    Top = 0
    Width = 437
    Height = 681
    Align = alClient
    Columns = <
      item
        Width = 10
      end
      item
        Caption = 'Name'
        Width = 150
      end
      item
        Caption = 'Date'
        Width = 200
      end>
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    GridLines = True
    ParentFont = False
    TabOrder = 0
    ViewStyle = vsReport
    ExplicitWidth = 444
  end
end
