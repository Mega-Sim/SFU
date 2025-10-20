object frmUDP: TfrmUDP
  Left = 0
  Top = 330
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Log & Monitoring'
  ClientHeight = 328
  ClientWidth = 956
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poDefault
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 881
    Height = 289
    ActivePage = Log
    TabOrder = 0
    object Log: TTabSheet
      Caption = 'Log'
      object mBSLog: TMemo
        Left = 3
        Top = 3
        Width = 830
        Height = 255
        ImeName = 'Microsoft IME 2010'
        ScrollBars = ssVertical
        TabOrder = 0
      end
      object btnLogDel: TButton
        Left = 832
        Top = 192
        Width = 38
        Height = 66
        Caption = 'DEL'
        TabOrder = 1
        OnClick = btnLogDelClick
      end
    end
    object DirectCommandSend: TTabSheet
      Caption = 'Direct Command Send'
      ImageIndex = 1
      object GroupBox1: TGroupBox
        Left = 8
        Top = 0
        Width = 353
        Height = 63
        Caption = ' Broadcast Sender '
        TabOrder = 0
        object Label5: TLabel
          Left = 248
          Top = 15
          Width = 20
          Height = 13
          Caption = 'Port'
        end
        object edtBSPort: TEdit
          Left = 248
          Top = 34
          Width = 34
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = '9000'
        end
        object edtBSMessage: TEdit
          Left = 17
          Top = 21
          Width = 225
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          Text = 'edtBSMessage'
        end
      end
      object GroupBox3: TGroupBox
        Left = 16
        Top = 69
        Width = 353
        Height = 71
        Caption = ' Unicast Sender '
        TabOrder = 1
        object Label4: TLabel
          Left = 16
          Top = 24
          Width = 52
          Height = 13
          Caption = 'IP Address'
        end
        object Label6: TLabel
          Left = 205
          Top = 24
          Width = 20
          Height = 13
          Caption = 'Port'
        end
        object edtUSAddr: TEdit
          Left = 74
          Top = 16
          Width = 81
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = '127.0.01'
        end
        object edtUSPort: TEdit
          Left = 231
          Top = 16
          Width = 34
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          Text = '9000'
        end
        object edtUSMessage: TEdit
          Left = 8
          Top = 43
          Width = 274
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          Text = 'Edit1'
        end
      end
      object GroupBox2: TGroupBox
        Left = 375
        Top = 3
        Width = 490
        Height = 78
        Caption = ' Broadcast Receiver '
        TabOrder = 2
        object Label2: TLabel
          Left = 17
          Top = 15
          Width = 72
          Height = 13
          Caption = 'Reply Message'
        end
        object Label7: TLabel
          Left = 119
          Top = 13
          Width = 51
          Height = 13
          Caption = 'Listen Port'
        end
        object edtBRPort: TEdit
          Left = 176
          Top = 12
          Width = 34
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = '9000'
        end
        object Edit6: TEdit
          Left = 16
          Top = 34
          Width = 187
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          Text = 'edtBRMessage'
        end
      end
      object GroupBox4: TGroupBox
        Left = 375
        Top = 87
        Width = 490
        Height = 82
        Caption = ' Unicast Receiver '
        TabOrder = 3
        object Label1: TLabel
          Left = 17
          Top = 18
          Width = 72
          Height = 13
          Caption = 'Reply Message'
        end
        object Label8: TLabel
          Left = 119
          Top = 16
          Width = 51
          Height = 13
          Caption = 'Listen Port'
        end
        object edtURPort: TEdit
          Left = 176
          Top = 15
          Width = 34
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = '9001'
        end
        object Edit10: TEdit
          Left = 16
          Top = 37
          Width = 187
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          Text = 'Edit2'
        end
      end
      object Edit1: TEdit
        Left = 16
        Top = 146
        Width = 353
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 4
        Text = #54252#53944#51221#48372' '#54869#51064#48277' : netstat -a -n -p udp'
      end
    end
  end
  object btnBCthread: TButton
    Left = 891
    Top = 8
    Width = 57
    Height = 25
    Caption = 'BC thread'
    TabOrder = 1
    OnClick = btnBCthreadClick
  end
  object chkOHT_Addr_GetTest: TCheckBox
    Left = 16
    Top = 303
    Width = 265
    Height = 17
    Caption = 'OHT'#44032' '#51452#49548#49325#51228' '#50506#44256' '#51452#49548' '#44228#49549' B/C '#53580#49828#53944
    TabOrder = 2
  end
  object chkOCS_Addr_GetTest: TCheckBox
    Left = 384
    Top = 303
    Width = 217
    Height = 17
    Caption = 'OCS'#44032' '#51452#49548' '#44228#49549' B/C '#53580#49828#53944
    TabOrder = 3
    OnClick = chkOCS_Addr_GetTestClick
  end
  object chkAutoLogClear: TCheckBox
    Left = 632
    Top = 304
    Width = 153
    Height = 17
    Caption = '5'#48516#47560#45796' '#47196#44536' '#49325#51228
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object trmAddrRetryTest: TTimer
    Interval = 500
    OnTimer = trmAddrRetryTestTimer
    Left = 320
    Top = 304
  end
  object tmrLogClear: TTimer
    Interval = 300000
    OnTimer = tmrLogClearTimer
    Left = 816
    Top = 304
  end
end
