object frmOcsDataTransperOrder: TfrmOcsDataTransperOrder
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'MCP -> CLW '#45936#51060#53552' '#51204#49569#51648#49884
  ClientHeight = 461
  ClientWidth = 712
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 686
    Height = 392
    ActivePage = TabSheet2
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'OCS Status'
      object grpStatus: TGroupBox
        Left = 14
        Top = 5
        Width = 649
        Height = 337
        Caption = ' STATUS '
        TabOrder = 0
        object Label1: TLabel
          Left = 16
          Top = 27
          Width = 48
          Height = 13
          Caption = #49345#53468#51648#49884
        end
        object Label3: TLabel
          Left = 16
          Top = 126
          Width = 48
          Height = 13
          Caption = #51221#51648#51648#49884
        end
        object Label4: TLabel
          Left = 16
          Top = 209
          Width = 48
          Height = 13
          Caption = #51221#51648#50836#51064
        end
        object Label5: TLabel
          Left = 16
          Top = 236
          Width = 72
          Height = 13
          Caption = #50976#51648#48372#49688#51648#49884
        end
        object Label6: TLabel
          Left = 16
          Top = 263
          Width = 42
          Height = 13
          Caption = 'E/M'#51648#49884
        end
        object Label2: TLabel
          Left = 16
          Top = 290
          Width = 87
          Height = 13
          Caption = #45936#51060#53552' '#51204#49569#51648#49884
        end
        object cboStopReason: TComboBox
          Left = 112
          Top = 206
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 0
          Text = 'Reserved_0'
          Items.Strings = (
            'Reserved_0'
            'Reserved_1'
            'Reserved_2'
            'Reserved_3'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboMaintananceOrder: TComboBox
          Left = 112
          Top = 233
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 1
          Text = 'Insert_OK'
          Items.Strings = (
            'Insert_OK'
            'Insert_NG'
            'FloorTransfer_OK'
            'FloorTransfer_NG'
            'Board_OK'
            'Board_NG'
            'Reserved_6'
            'Reserved_7')
        end
        object cboEmOder: TComboBox
          Left = 112
          Top = 260
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 2
          Text = 'EM_Cut'
          Items.Strings = (
            'EM_Cut'
            'Reserved_1'
            'Reserved_2'
            'Reserved_3'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboDataTransOrder: TComboBox
          Left = 112
          Top = 287
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 3
          Text = 'CheckDataTransfer'
          Items.Strings = (
            'CheckDataTransfer'
            'Reserved_1'
            'Reserved_2'
            'Reserved_3'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object GroupBox1: TGroupBox
          Left = 80
          Top = 27
          Width = 369
          Height = 78
          Caption = 'GroupBox1'
          TabOrder = 4
          object chkStatus_0: TCheckBox
            Left = 16
            Top = 24
            Width = 81
            Height = 17
            Caption = #44592#46041' (0)'
            TabOrder = 0
          end
          object chkStatus_1: TCheckBox
            Left = 103
            Top = 24
            Width = 81
            Height = 17
            Caption = #47112#51060#50500#50883' (1)'
            TabOrder = 1
          end
          object chkStatus_2: TCheckBox
            Left = 184
            Top = 24
            Width = 81
            Height = 17
            Caption = 'chkStatus_2'
            TabOrder = 2
          end
          object chkStatus_3: TCheckBox
            Left = 264
            Top = 24
            Width = 81
            Height = 17
            Caption = 'chkStatus_3'
            TabOrder = 3
          end
          object chkStatus_4: TCheckBox
            Left = 16
            Top = 47
            Width = 81
            Height = 17
            Caption = 'chkStatus_4'
            TabOrder = 4
          end
          object chkStatus_5: TCheckBox
            Left = 103
            Top = 47
            Width = 81
            Height = 17
            Caption = 'chkStatus_5'
            TabOrder = 5
          end
          object chkStatus_6: TCheckBox
            Left = 184
            Top = 47
            Width = 81
            Height = 17
            Caption = #45236#47548#54728#44032' (6)'
            TabOrder = 6
          end
          object chkStatus_7: TCheckBox
            Left = 264
            Top = 47
            Width = 81
            Height = 17
            Caption = #50732#47548#54728#44032' (7)'
            TabOrder = 7
          end
        end
        object GroupBox2: TGroupBox
          Left = 80
          Top = 122
          Width = 521
          Height = 78
          Caption = 'GroupBox1'
          TabOrder = 5
          object chkStopOrder_0: TCheckBox
            Left = 16
            Top = 24
            Width = 105
            Height = 17
            Caption = #51452#54665#44048#49549#51221#51648' (0)'
            TabOrder = 0
          end
          object chkStopOrder_1: TCheckBox
            Left = 127
            Top = 24
            Width = 107
            Height = 17
            Caption = #51452#54665' '#51593#49884#51221#51648' (1)'
            TabOrder = 1
          end
          object chkStopOrder_2: TCheckBox
            Left = 240
            Top = 24
            Width = 81
            Height = 17
            Caption = 'chkStatus_2'
            TabOrder = 2
          end
          object chkStopOrder_3: TCheckBox
            Left = 376
            Top = 24
            Width = 81
            Height = 17
            Caption = 'chkStatus_3'
            TabOrder = 3
          end
          object chkStopOrder_4: TCheckBox
            Left = 16
            Top = 47
            Width = 105
            Height = 17
            Caption = #51060#51116' '#44048#49549#51221#51648'(4)'
            TabOrder = 4
          end
          object chkStopOrder_5: TCheckBox
            Left = 127
            Top = 47
            Width = 107
            Height = 17
            Caption = #51060#51116' '#51593#49884#51221#51648'(5)'
            TabOrder = 5
          end
          object chkStopOrder_6: TCheckBox
            Left = 240
            Top = 47
            Width = 81
            Height = 17
            Caption = 'Reserve (6)'
            TabOrder = 6
          end
          object chkStopOrder_7: TCheckBox
            Left = 376
            Top = 47
            Width = 129
            Height = 17
            Caption = #49912#51060#53364' '#49892#54665#48520#44032' (7)'
            TabOrder = 7
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'BODY'
      ImageIndex = 1
      object GroupBox4: TGroupBox
        Left = 16
        Top = 16
        Width = 649
        Height = 337
        Caption = 'BODY'
        TabOrder = 0
        object Label7: TLabel
          Left = 16
          Top = 28
          Width = 63
          Height = 13
          Caption = #49884#53248#49828' '#45336#48260
        end
        object Label23: TLabel
          Left = 312
          Top = 23
          Width = 36
          Height = 13
          Caption = 'IP Addr'
        end
        object Label24: TLabel
          Left = 312
          Top = 49
          Width = 97
          Height = 13
          Caption = 'User Name (16Byte)'
        end
        object Label29: TLabel
          Left = 312
          Top = 76
          Width = 93
          Height = 13
          Caption = #54056#49828#50892#46300' (16Byte)'
        end
        object Label8: TLabel
          Left = 311
          Top = 103
          Width = 69
          Height = 13
          Caption = #48260#51204' (32Byte)'
        end
        object Label9: TLabel
          Left = 312
          Top = 135
          Width = 110
          Height = 13
          Caption = 'DataSendTo (256Byte)'
        end
        object edtOrderID: TEdit
          Left = 112
          Top = 24
          Width = 49
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = 'edtOrderID'
        end
        object btnGetOrderID: TButton
          Left = 167
          Top = 24
          Width = 90
          Height = 25
          Caption = 'Get OrderID'
          TabOrder = 1
          OnClick = btnGetOrderIDClick
        end
        object IpAddr_H: TEdit
          Left = 416
          Top = 20
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          Text = '127'
        end
        object IpAddr_M1: TEdit
          Left = 463
          Top = 20
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          Text = '0'
        end
        object IpAddr_M2: TEdit
          Left = 510
          Top = 20
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          Text = '0'
        end
        object edtUserName: TEdit
          Left = 416
          Top = 46
          Width = 182
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          Text = 'anonymous'
        end
        object edtVersion: TEdit
          Left = 386
          Top = 100
          Width = 212
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          Text = '1234567890A234567890B234567890C2'
        end
        object IpAddr_L: TEdit
          Left = 557
          Top = 20
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 7
          Text = '1'
        end
        object edtPassword: TEdit
          Left = 416
          Top = 73
          Width = 182
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          Text = '12345'
        end
        object edtDataSendTo: TEdit
          Left = 428
          Top = 132
          Width = 203
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          Text = '/OHT_MAP_DATA/'
        end
        object GroupBox5: TGroupBox
          Left = 24
          Top = 73
          Width = 281
          Height = 248
          Caption = ' '#51204#49569#44396#48516' '#48143' '#50836#44396#44396#48516' '#49444#51221' '
          TabOrder = 10
          object rdoSendParam: TRadioGroup
            Left = 16
            Top = 24
            Width = 185
            Height = 56
            Caption = ' '#51204#49569#44396#48516' ('#51204#49569#48169#54693') '
            ItemIndex = 0
            Items.Strings = (
              'FTP -> OHT'#47196' '#45936#51060#53552' '#51204#49569
              'OHT -> FTP'#47196' '#45936#51060#53552' '#51204#49569)
            TabOrder = 0
            OnClick = rdoSendParamClick
          end
          object GroupBox6: TGroupBox
            Left = 16
            Top = 86
            Width = 153
            Height = 145
            Caption = 'FTP -> OHT '#50836#44396#44396#48516
            TabOrder = 1
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
              Caption = 'Firmware Update'
              Checked = True
              State = cbChecked
              TabOrder = 6
            end
          end
          object GroupBox7: TGroupBox
            Left = 176
            Top = 86
            Width = 97
            Height = 147
            Caption = ' OHT -> FTP '#50836#44396
            Enabled = False
            TabOrder = 2
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
        end
      end
    end
  end
  object btnSend: TButton
    Left = 416
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Send Packet'
    TabOrder = 1
    OnClick = btnSendClick
  end
  object btnClose: TButton
    Left = 558
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 2
    OnClick = btnCloseClick
  end
end
