object FirmwareUpdateForm: TFirmwareUpdateForm
  Left = 0
  Top = 0
  Caption = 'Firmware Update'
  ClientHeight = 407
  ClientWidth = 632
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 543
    Top = 202
    Width = 225
    Height = 142
    Caption = ' '#50836#44396#44396#48516' '#49440#53469' '
    TabOrder = 0
    Visible = False
    object chkMapSet_Proto: TCheckBox
      Left = 16
      Top = 24
      Width = 201
      Height = 17
      Caption = #47605' '#54028#51068#49464#53944'  '#51312#51221#50857' (FTP -> OHT)'
      TabOrder = 0
      OnClick = chkMapSet_ProtoClick
    end
    object chkMapSet_Run: TCheckBox
      Left = 16
      Top = 47
      Width = 193
      Height = 17
      Caption = #47605' '#54028#51068#49464#53944' '#50868#50857#50857' (FTP -> OHT)'
      TabOrder = 1
      OnClick = chkMapSet_RunClick
    end
    object chkMapSet_Export: TCheckBox
      Left = 16
      Top = 70
      Width = 198
      Height = 17
      Caption = #47605' '#54028#51068#49464#53944' '#50808#48512#51204#49569' (OHT -> FTP)'
      TabOrder = 2
      OnClick = chkMapSet_ExportClick
    end
    object chkFirmwareUpdate: TCheckBox
      Left = 16
      Top = 117
      Width = 193
      Height = 17
      Caption = 'Firmware Update (Ver 2.0)'
      Checked = True
      State = cbChecked
      TabOrder = 3
      OnClick = chkFirmwareUpdateClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 8
    Width = 372
    Height = 145
    Caption = ' FTP '#47196#44536#51064' '#51221#48372' '
    TabOrder = 1
    object Label23: TLabel
      Left = 24
      Top = 15
      Width = 36
      Height = 13
      Caption = 'IP Addr'
    end
    object Label24: TLabel
      Left = 24
      Top = 42
      Width = 97
      Height = 13
      Caption = 'User Name (16Byte)'
    end
    object Label29: TLabel
      Left = 26
      Top = 69
      Width = 93
      Height = 13
      Caption = #54056#49828#50892#46300' (16Byte)'
    end
    object Label9: TLabel
      Left = 16
      Top = 121
      Width = 110
      Height = 13
      Caption = 'DataSendTo (256Byte)'
    end
    object Label8: TLabel
      Left = 50
      Top = 96
      Width = 69
      Height = 13
      Caption = #48260#51204' (32Byte)'
    end
    object IpAddr_H: TEdit
      Left = 140
      Top = 12
      Width = 41
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 0
      Text = '127'
    end
    object IpAddr_M1: TEdit
      Left = 187
      Top = 12
      Width = 41
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 1
      Text = '0'
    end
    object IpAddr_M2: TEdit
      Left = 234
      Top = 12
      Width = 41
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 2
      Text = '0'
    end
    object IpAddr_L: TEdit
      Left = 281
      Top = 12
      Width = 41
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 3
      Text = '1'
    end
    object edtUserName: TEdit
      Left = 140
      Top = 39
      Width = 182
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 4
      Text = 'anonymous'
    end
    object edtPassword: TEdit
      Left = 140
      Top = 66
      Width = 182
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 5
      Text = '12345'
    end
    object edtDataSendTo: TEdit
      Left = 132
      Top = 121
      Width = 229
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 6
      Text = '/OHT_MAP_DATA/'
    end
    object edtVersion: TEdit
      Left = 141
      Top = 93
      Width = 212
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 7
      Text = '1234567890A234567890B234567890C2'
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 222
    Width = 616
    Height = 177
    Caption = ' '#51652#54665#49345#54889' '
    TabOrder = 2
    object mmLog: TMemo
      Left = 9
      Top = 15
      Width = 589
      Height = 159
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 0
    end
  end
  object btnUpdate: TButton
    Left = 386
    Top = 121
    Width = 112
    Height = 33
    Caption = #50629#45936#51060#53944' '#49892#54665
    TabOrder = 3
    OnClick = btnUpdateClick
  end
  object btnVersionInfoGet: TButton
    Left = 504
    Top = 120
    Width = 112
    Height = 33
    Caption = #48260#51204#51221#48372' '#50836#52397
    TabOrder = 4
    OnClick = btnVersionInfoGetClick
  end
  object btnSettingSave: TButton
    Left = 386
    Top = 30
    Width = 112
    Height = 33
    Caption = 'FTP '#49444#51221' '#51200#51109
    TabOrder = 5
    OnClick = btnSettingSaveClick
  end
  object btnClear: TButton
    Left = 447
    Top = 173
    Width = 145
    Height = 36
    Caption = 'Log Clear'
    TabOrder = 6
    OnClick = btnClearClick
  end
  object btnSettingLoad: TButton
    Left = 504
    Top = 30
    Width = 112
    Height = 33
    Caption = 'FTP '#49444#51221' '#51069#44592
    TabOrder = 7
    OnClick = btnSettingLoadClick
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 159
    Width = 419
    Height = 57
    Caption = #49892#54665#49345#53468
    TabOrder = 8
    object panCmd_0: TPanel
      Left = 95
      Top = 19
      Width = 80
      Height = 23
      Caption = #51025#45813#49688#49888
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 0
    end
    object panCmd_1: TPanel
      Left = 181
      Top = 19
      Width = 74
      Height = 23
      Caption = 'FTP'#51217#49549' '#51473
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 1
    end
    object panCmd_2: TPanel
      Left = 261
      Top = 19
      Width = 75
      Height = 23
      Caption = #54028#51068#51204#49569' '#51473
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 2
    end
    object panCmd_3: TPanel
      Left = 342
      Top = 19
      Width = 67
      Height = 23
      Caption = #50756#47308#52376#47532
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 3
    end
    object panCmd_Start: TPanel
      Left = 9
      Top = 19
      Width = 80
      Height = 23
      Caption = #47749#47161#51204#49569
      Color = clWhite
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 4
    end
  end
end
