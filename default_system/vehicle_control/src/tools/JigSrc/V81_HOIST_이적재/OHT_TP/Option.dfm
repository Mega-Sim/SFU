object frmOption: TfrmOption
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Option'
  ClientHeight = 338
  ClientWidth = 493
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 477
    Height = 294
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Basic Setting'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox1: TGroupBox
        Left = 11
        Top = 3
        Width = 225
        Height = 246
        Caption = ' Network Setting '
        TabOrder = 0
        object Label1: TLabel
          Left = 8
          Top = 24
          Width = 66
          Height = 13
          Caption = 'Machine Type'
        end
        object Label2: TLabel
          Left = 8
          Top = 51
          Width = 53
          Height = 13
          Caption = 'Machine ID'
        end
        object Label4: TLabel
          Left = 8
          Top = 83
          Width = 71
          Height = 13
          Caption = 'Broadcast Port'
        end
        object Label3: TLabel
          Left = 8
          Top = 110
          Width = 80
          Height = 13
          Caption = 'B. Time Out (ms)'
        end
        object Label5: TLabel
          Left = 8
          Top = 147
          Width = 58
          Height = 13
          Caption = 'Control Port'
        end
        object Label6: TLabel
          Left = 8
          Top = 174
          Width = 81
          Height = 13
          Caption = 'C. Time Out (ms)'
        end
        object Label7: TLabel
          Left = 8
          Top = 206
          Width = 136
          Height = 13
          Caption = 'Auto Status Send Time (sec)'
        end
        object edtMachineID: TEdit
          Left = 94
          Top = 48
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = 'MCP01'
        end
        object edtBcPort: TEdit
          Left = 94
          Top = 80
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          Text = '9000'
        end
        object edtBcTimeOut: TEdit
          Left = 94
          Top = 107
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          Text = '1000'
        end
        object edtCtPort: TEdit
          Left = 94
          Top = 144
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          Text = '9001'
        end
        object edtCpTimeOut: TEdit
          Left = 94
          Top = 171
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          Text = '5000'
        end
        object edtAutoStatusSendTime: TEdit
          Left = 160
          Top = 206
          Width = 49
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          Text = '10'
        end
        object rdoOCS: TRadioButton
          Left = 91
          Top = 23
          Width = 41
          Height = 17
          Caption = 'OCS'
          TabOrder = 6
        end
        object rdoOHT: TRadioButton
          Left = 138
          Top = 21
          Width = 40
          Height = 21
          Caption = 'OHT'
          Checked = True
          TabOrder = 7
          TabStop = True
        end
        object rdoTP: TRadioButton
          Left = 184
          Top = 23
          Width = 95
          Height = 17
          Caption = 'TP'
          TabOrder = 8
        end
      end
      object GroupBox2: TGroupBox
        Left = 242
        Top = 3
        Width = 224
        Height = 121
        Caption = #49345#45824#48169#50640#44172' Command'#48372#45244#46412' (OCS '#51204#50857')'
        TabOrder = 1
        object Label8: TLabel
          Left = 8
          Top = 51
          Width = 53
          Height = 13
          Caption = 'Machine ID'
        end
        object edtOhtMachineID: TEdit
          Left = 94
          Top = 48
          Width = 115
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          Text = 'OHT001'
        end
      end
    end
  end
  object btnClose: TButton
    Left = 384
    Top = 304
    Width = 97
    Height = 33
    Caption = 'Close'
    TabOrder = 1
    OnClick = btnCloseClick
  end
end
