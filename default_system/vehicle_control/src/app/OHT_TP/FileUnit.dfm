object FileForm: TFileForm
  Left = 800
  Top = 217
  BorderStyle = bsSingle
  Caption = 'FileForm'
  ClientHeight = 248
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
    Height = 248
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
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      object comboFileSelect: TComboBox
        Left = 8
        Top = 4
        Width = 225
        Height = 23
        Style = csDropDownList
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
        ItemIndex = 0
        ParentFont = False
        TabOrder = 0
        Text = 'Upload[->OHT]'
        OnChange = comboFileSelectChange
        Items.Strings = (
          'Upload[->OHT]'
          'Download[<-OHT]'
          'Explore'
          'Delete'
          'OHT File List')
      end
      object btnGetFileInfo: TButton
        Left = 248
        Top = 4
        Width = 121
        Height = 25
        Caption = 'Get File Information'
        TabOrder = 1
        OnClick = btnGetFileInfoClick
      end
    end
    object panFileControl: TPanel
      Left = 2
      Top = 33
      Width = 416
      Height = 213
      Align = alClient
      BevelOuter = bvNone
      Caption = 'panFileControl'
      TabOrder = 1
      object pgcFileControl: TPageControl
        Left = 0
        Top = 0
        Width = 416
        Height = 213
        ActivePage = tabAMCUpdate
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
            Left = 80
            Top = 152
            Width = 75
            Height = 25
            Caption = 'Cancel'
            TabOrder = 0
            OnClick = btnDownCancelClick
          end
          object btnDownload: TButton
            Left = 0
            Top = 152
            Width = 75
            Height = 25
            Caption = 'DownLoad'
            TabOrder = 1
            OnClick = btnDownloadClick
          end
          object grpSelectFiles: TGroupBox
            Left = 4
            Top = 1
            Width = 401
            Height = 145
            Caption = ' Select Download Files '
            TabOrder = 2
            object chkMapDataFile: TCheckBox
              Left = 16
              Top = 24
              Width = 73
              Height = 17
              Caption = 'Map File'
              TabOrder = 0
            end
            object chkAMCOutFile: TCheckBox
              Left = 16
              Top = 82
              Width = 97
              Height = 17
              Caption = 'AMC Out'
              TabOrder = 5
            end
            object chkTeachingDataFile: TCheckBox
              Left = 16
              Top = 62
              Width = 105
              Height = 17
              Caption = 'Teaching Data'
              TabOrder = 4
            end
            object chkNodeDisableFile: TCheckBox
              Left = 184
              Top = 62
              Width = 153
              Height = 17
              Caption = 'NodeDisable Data'
              TabOrder = 3
            end
            object chkUserSelectFile: TCheckBox
              Left = 16
              Top = 101
              Width = 153
              Height = 17
              Caption = 'User Select DataFiles'
              TabOrder = 6
            end
            object edtUserSelectFileName: TEdit
              Left = 184
              Top = 98
              Width = 209
              Height = 23
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              TabOrder = 7
            end
            object chkParamDrivingFile: TCheckBox
              Left = 184
              Top = 24
              Width = 129
              Height = 17
              Caption = 'Parameter(Driving)'
              TabOrder = 1
            end
            object chkStationDataFile: TCheckBox
              Left = 16
              Top = 43
              Width = 105
              Height = 17
              Caption = 'Station Data'
              TabOrder = 2
            end
            object chkParamTransFile: TCheckBox
              Left = 184
              Top = 43
              Width = 129
              Height = 17
              Caption = 'Parameter(Trans)'
              TabOrder = 8
            end
          end
        end
        object tabUpload: TTabSheet
          Caption = 'Upload'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnUpload: TButton
            Left = 4
            Top = 148
            Width = 75
            Height = 25
            Caption = 'Upload'
            TabOrder = 0
            OnClick = btnUploadClick
          end
          object btnUpCancel: TButton
            Left = 92
            Top = 148
            Width = 75
            Height = 25
            Caption = 'Cancel'
            TabOrder = 1
            OnClick = btnDownCancelClick
          end
          object rgSelectUploadFile: TRadioGroup
            Left = 0
            Top = 0
            Width = 365
            Height = 145
            Caption = ' Select Upload File '
            Items.Strings = (
              'Map Data'
              'Station Data'
              'Teaching Data'
              'AMC Program')
            TabOrder = 2
          end
        end
        object tabDelete: TTabSheet
          Caption = 'Delete'
          ImageIndex = 2
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnDelete: TButton
            Left = 12
            Top = 20
            Width = 125
            Height = 85
            Caption = 'Delete Log(all)'
            TabOrder = 0
            OnClick = btnDeleteClick
          end
          object btnDelCancel: TButton
            Left = 144
            Top = 20
            Width = 113
            Height = 85
            Caption = 'Cancel'
            TabOrder = 1
            OnClick = btnDownCancelClick
          end
        end
        object tabAMCUpdate: TTabSheet
          Caption = 'AMC Update'
          ImageIndex = 3
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object memoAMCUpdate: TMemo
            Left = 0
            Top = 0
            Width = 408
            Height = 183
            Align = alClient
            BorderStyle = bsNone
            ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
            ScrollBars = ssVertical
            TabOrder = 0
          end
        end
        object tabFileList: TTabSheet
          Caption = 'GetFileList'
          ImageIndex = 4
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object Label1: TLabel
            Left = 14
            Top = 114
            Width = 45
            Height = 15
            Caption = #54028#51068#47749' : '
          end
          object Button1: TButton
            Left = 68
            Top = 144
            Width = 161
            Height = 25
            Caption = 'Search'
            TabOrder = 0
            OnClick = Button1Click
          end
          object RadioGroup1: TRadioGroup
            Left = 8
            Top = 8
            Width = 221
            Height = 97
            Caption = ' Select '
            ItemIndex = 0
            Items.Strings = (
              'Error '#54028#51068
              'AutoRecovery '#54028#51068
              'Etc ('#54028#51068#47749' '#51068#48512#51077#47141' '#44160#49353')')
            TabOrder = 1
          end
          object Edit1: TEdit
            Left = 68
            Top = 111
            Width = 161
            Height = 23
            ImeName = 'Microsoft IME 2003'
            TabOrder = 2
            OnChange = Edit1Change
          end
        end
        object tabAssistant: TTabSheet
          Caption = 'Assistant'
          ImageIndex = 5
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnAssitantClose: TButton
            Left = 10
            Top = 48
            Width = 121
            Height = 73
            Caption = 'Assistant Close'
            TabOrder = 0
            OnClick = btnAssitantCloseClick
          end
          object btnAssistantUpload: TButton
            Left = 142
            Top = 48
            Width = 121
            Height = 73
            Caption = 'Assistant Upload'
            TabOrder = 1
            OnClick = btnAssistantUploadClick
          end
          object btnAssistantExec: TButton
            Left = 274
            Top = 48
            Width = 121
            Height = 73
            Caption = 'Assistant Exec'
            TabOrder = 2
            OnClick = btnAssistantExecClick
          end
        end
      end
    end
  end
  object dlgOpenFile: TOpenDialog
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 328
    Top = 68
  end
  object dlgSaveFile: TSaveDialog
    Left = 374
    Top = 65
  end
end
