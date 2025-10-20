object LogFileForm: TLogFileForm
  Left = 1366
  Top = 345
  BorderStyle = bsSingle
  Caption = 'LogFileForm'
  ClientHeight = 232
  ClientWidth = 420
  Color = 15658734
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object panFile: TPanel
    Left = 0
    Top = 0
    Width = 420
    Height = 232
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = 'panFile'
    TabOrder = 0
    object Panel2: TPanel
      Left = 2
      Top = 2
      Width = 416
      Height = 31
      Align = alTop
      BevelOuter = bvLowered
      TabOrder = 0
      object comboFileSelect: TComboBox
        Left = 8
        Top = 4
        Width = 225
        Height = 23
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
        ParentFont = False
        TabOrder = 0
        Text = 'Select Upload / Download'
        OnChange = comboFileSelectChange
        Items.Strings = (
          'Download[<-OHT]'
          'Explore'
          'OHT File List')
      end
    end
    object panFileControl: TPanel
      Left = 2
      Top = 33
      Width = 416
      Height = 197
      Align = alClient
      BevelOuter = bvNone
      Caption = 'panFileControl'
      TabOrder = 1
      object PageControl3: TPageControl
        Left = 0
        Top = 0
        Width = 416
        Height = 197
        ActivePage = tabDownload
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object tabDownload: TTabSheet
          Caption = 'Download'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnDownCancel: TButton
            Left = 120
            Top = 80
            Width = 73
            Height = 41
            Caption = 'Cancel'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
          object btnDownload: TButton
            Left = 8
            Top = 56
            Width = 105
            Height = 65
            Caption = 'Log DownLoad'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 1
            OnClick = btnDownloadClick
          end
          object UserLckbox: TCheckBox
            Left = 8
            Top = 16
            Width = 169
            Height = 17
            Caption = 'User Select LogFiles'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 2
          end
          object Logedt: TEdit
            Left = 152
            Top = 16
            Width = 209
            Height = 23
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
            ParentFont = False
            TabOrder = 3
          end
          object Panel1: TPanel
            Left = 200
            Top = 56
            Width = 161
            Height = 89
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 4
            object Label1: TLabel
              Left = 13
              Top = 8
              Width = 74
              Height = 15
              Caption = '['#49884#44036' '#54869#51064'!!!!!]'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label2: TLabel
              Left = 11
              Top = 58
              Width = 28
              Height = 15
              Caption = 'PC   :'
              Font.Charset = ANSI_CHARSET
              Font.Color = 4194368
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object lblPCTime: TLabel
              Left = 48
              Top = 58
              Width = 57
              Height = 15
              Caption = 'lblPCTime'
              Font.Charset = ANSI_CHARSET
              Font.Color = 4194368
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label3: TLabel
              Left = 11
              Top = 34
              Width = 30
              Height = 15
              Caption = 'OHT :'
              Font.Charset = ANSI_CHARSET
              Font.Color = 8388863
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object lblOHTTime: TLabel
              Left = 48
              Top = 34
              Width = 65
              Height = 15
              Caption = 'lblOHTTime'
              Font.Charset = ANSI_CHARSET
              Font.Color = 8388863
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
        end
        object tabGetFileList: TTabSheet
          Caption = 'GetFileList'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object RadioGroup1: TRadioGroup
            Left = 8
            Top = 8
            Width = 145
            Height = 105
            Caption = ' Select '
            ItemIndex = 0
            Items.Strings = (
              'Error '
              'AutoRecovery'
              'Etc...')
            TabOrder = 0
          end
          object Button1: TButton
            Left = 16
            Top = 120
            Width = 75
            Height = 25
            Caption = 'Get..'
            TabOrder = 1
            OnClick = Button1Click
          end
          object Edit1: TEdit
            Left = 160
            Top = 88
            Width = 201
            Height = 23
            ImeName = 'Microsoft IME 2003'
            TabOrder = 2
          end
        end
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 288
    Top = 12
  end
  object SaveDialog1: TSaveDialog
    Left = 262
    Top = 9
  end
end
