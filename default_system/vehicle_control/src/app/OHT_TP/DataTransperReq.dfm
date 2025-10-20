object frmDataTransperReq: TfrmDataTransperReq
  Left = 0
  Top = 0
  Caption = #45936#51060#53552' '#51204#49569#50836#44396
  ClientHeight = 475
  ClientWidth = 441
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object labMap: TLabel
    Left = 24
    Top = 309
    Width = 62
    Height = 13
    Caption = 'MapVersion :'
  end
  object lblMapVersion: TLabel
    Left = 100
    Top = 309
    Width = 19
    Height = 13
    Caption = ' 0.0'
  end
  object GroupBox5: TGroupBox
    Left = 8
    Top = 8
    Width = 425
    Height = 297
    Caption = ' '#51204#49569#44396#48516' '#48143' '#50836#44396#44396#48516' '#49444#51221' '
    TabOrder = 0
    object rdoSendParam: TRadioGroup
      Left = 16
      Top = 24
      Width = 393
      Height = 56
      Caption = ' '#51204#49569#44396#48516' ('#51204#49569#48169#54693') '
      ItemIndex = 0
      Items.Strings = (
        'FTP -> OHT '#45936#51060#53552' '#51204#49569' (MAP '#45796#50868#47196#46300' '#46608#45716' '#54156#50920#50612' '#50629#45936#51060#53944')'
        'OHT -> FTP '#45936#51060#53552' '#51204#49569' ('#54000#52845#46108' MAP '#50629#47196#46300')')
      TabOrder = 0
    end
    object GroupBox6: TGroupBox
      Left = 448
      Top = 24
      Width = 153
      Height = 145
      Caption = 'FTP -> OHT '#50836#44396#44396#48516
      TabOrder = 1
      Visible = False
      object chkFtpOht_Req_0: TCheckBox
        Left = 16
        Top = 24
        Width = 121
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944'  '#51312#51221#50857
        TabOrder = 0
      end
      object chkFtpOht_Req_1: TCheckBox
        Left = 16
        Top = 40
        Width = 121
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944' '#50868#50857#50857
        TabOrder = 1
      end
      object chkFtpOht_Req_2: TCheckBox
        Left = 16
        Top = 56
        Width = 121
        Height = 17
        Caption = 'OS'
        TabOrder = 2
      end
      object chkFtpOht_Req_3: TCheckBox
        Left = 16
        Top = 72
        Width = 137
        Height = 17
        Caption = 'OS'#50857' '#49444#51221#54028#51068' ('#50696#50557')'
        TabOrder = 3
      end
      object chkFtpOht_Req_4: TCheckBox
        Left = 16
        Top = 88
        Width = 121
        Height = 17
        Caption = #50612#54540#47532#52992#51060#49496
        TabOrder = 4
      end
      object chkFtpOht_Req_5: TCheckBox
        Left = 16
        Top = 105
        Width = 121
        Height = 17
        Caption = #50868#50857' '#54028#46972#48120#53552
        TabOrder = 5
      end
      object chkFtpOht_Req_6: TCheckBox
        Left = 16
        Top = 122
        Width = 121
        Height = 17
        Caption = #52629' '#54028#46972#48120#53552' ('#50696#50557')'
        TabOrder = 6
      end
    end
    object GroupBox7: TGroupBox
      Left = 448
      Top = 175
      Width = 97
      Height = 147
      Caption = ' OHT -> FTP '#50836#44396
      ParentBackground = False
      TabOrder = 2
      Visible = False
      object chkOhtFtp_Req_0: TCheckBox
        Left = 8
        Top = 24
        Width = 86
        Height = 17
        Caption = #47196#44536' '#45936#51060#53552
        TabOrder = 0
      end
      object chkOhtFtp_Req_1: TCheckBox
        Left = 8
        Top = 42
        Width = 86
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944
        TabOrder = 1
      end
    end
    object GroupBox1: TGroupBox
      Left = 16
      Top = 86
      Width = 185
      Height = 99
      Caption = 'FTP -> OHT ('#45796#50868#47196#46300') '#50836#44396#44396#48516
      TabOrder = 3
      object rdoFtpOht_Req_0: TRadioButton
        Left = 16
        Top = 24
        Width = 121
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944'  '#51312#51221#50857
        Checked = True
        TabOrder = 0
        TabStop = True
      end
      object rdoFtpOht_Req_1: TRadioButton
        Left = 16
        Top = 48
        Width = 129
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944' '#50868#50857#50857
        TabOrder = 1
      end
      object rdoFtpOht_Req_4: TRadioButton
        Left = 16
        Top = 71
        Width = 129
        Height = 17
        Caption = #54156#50920#50612' '#50629#45936#51060#53944' '#49892#54665
        TabOrder = 2
      end
    end
    object GroupBox2: TGroupBox
      Left = 216
      Top = 86
      Width = 193
      Height = 99
      Caption = ' OHT -> FTP ('#50629#47196#46300') '#50836#44396#44396#48516
      TabOrder = 4
      object rdoOhtFtp_Req_1: TRadioButton
        Left = 16
        Top = 24
        Width = 153
        Height = 17
        Caption = #47605' '#54028#51068#49464#53944
        Checked = True
        TabOrder = 0
        TabStop = True
      end
    end
  end
  object btnSend: TButton
    Left = 24
    Top = 262
    Width = 115
    Height = 33
    Caption = #47749#47161' '#49892#54665
    TabOrder = 1
    OnClick = btnSendClick
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 328
    Width = 425
    Height = 141
    Caption = ' '#51652#54665#49345#54889' '#47196#44536' '
    TabOrder = 2
    object mmLog: TMemo
      Left = 16
      Top = 22
      Width = 393
      Height = 123
      ImeName = 'Microsoft IME 2010'
      ScrollBars = ssVertical
      TabOrder = 0
    end
  end
  object btnLogClear: TButton
    Left = 302
    Top = 262
    Width = 115
    Height = 33
    Caption = 'Log Clear'
    TabOrder = 3
    OnClick = btnLogClearClick
  end
  object GroupBox4: TGroupBox
    Left = 24
    Top = 199
    Width = 393
    Height = 57
    Caption = #49892#54665#49345#53468
    TabOrder = 4
    object panCmd_0: TPanel
      Left = 87
      Top = 19
      Width = 66
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
      Left = 159
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
      Left = 239
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
      Left = 320
      Top = 19
      Width = 64
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
      Width = 72
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
