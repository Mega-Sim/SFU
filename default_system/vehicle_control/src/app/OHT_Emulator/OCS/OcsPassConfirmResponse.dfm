object frmOcsPassConfirmResponse: TfrmOcsPassConfirmResponse
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'MCP -> CLW  '#53685#44284' '#54728#44032' '#44060#48169#50836#44396' '#51025#45813' (ID : 244)'
  ClientHeight = 457
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
        Top = 3
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
          Top = 110
          Width = 48
          Height = 13
          Caption = #51221#51648#51648#49884
        end
        object Label4: TLabel
          Left = 16
          Top = 185
          Width = 48
          Height = 13
          Caption = #51221#51648#50836#51064
        end
        object Label5: TLabel
          Left = 16
          Top = 212
          Width = 72
          Height = 13
          Caption = #50976#51648#48372#49688#51648#49884
        end
        object Label6: TLabel
          Left = 16
          Top = 239
          Width = 42
          Height = 13
          Caption = 'E/M'#51648#49884
        end
        object Label2: TLabel
          Left = 16
          Top = 266
          Width = 87
          Height = 13
          Caption = #45936#51060#53552' '#51204#49569#51648#49884
        end
        object cboStopReason: TComboBox
          Left = 112
          Top = 182
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
          Top = 209
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
          Top = 236
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
          Top = 263
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
        object GroupBox4: TGroupBox
          Left = 80
          Top = 19
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
        object GroupBox5: TGroupBox
          Left = 80
          Top = 103
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
        object chkAutoReply: TCheckBox
          Left = 304
          Top = 187
          Width = 313
          Height = 25
          Caption = 'CLW'#44032' OCS'#47196' '#53685#44284' '#54728#44032' '#44060#48169#50836#44396'  (ID 243)'#49884' '#51088#46041#48152#51025
          TabOrder = 6
        end
        object mBSLog: TMemo
          Left = 303
          Top = 214
          Width = 298
          Height = 107
          ImeName = 'Microsoft IME 2010'
          ScrollBars = ssVertical
          TabOrder = 7
        end
        object btnDel: TButton
          Left = 608
          Top = 280
          Width = 33
          Height = 41
          Caption = 'Del'
          TabOrder = 8
          OnClick = btnDelClick
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'BODY'
      ImageIndex = 1
      object GroupBox2: TGroupBox
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
          Left = 16
          Top = 87
          Width = 53
          Height = 13
          Caption = 'STOP '#48264#51648
        end
        object Label24: TLabel
          Left = 16
          Top = 113
          Width = 91
          Height = 13
          Caption = 'RESET '#48264#51648' (Main)'
        end
        object Label25: TLabel
          Left = 120
          Top = 65
          Width = 25
          Height = 13
          Caption = 'HIGH'
        end
        object Label26: TLabel
          Left = 167
          Top = 65
          Width = 19
          Height = 13
          Caption = 'MID'
        end
        object Label27: TLabel
          Left = 214
          Top = 65
          Width = 23
          Height = 13
          Caption = 'LOW'
        end
        object Label29: TLabel
          Left = 16
          Top = 140
          Width = 87
          Height = 13
          Caption = 'RESET '#48264#51648' (Sub)'
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
        object edtTargetStop_H: TEdit
          Left = 120
          Top = 84
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          Text = '1'
        end
        object edtTargetStop_M: TEdit
          Left = 167
          Top = 83
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          Text = '20'
        end
        object edtTargetStop_L: TEdit
          Left = 214
          Top = 84
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          Text = '150'
        end
        object edtResetMain_H: TEdit
          Left = 120
          Top = 110
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          Text = '10'
        end
        object edtResetMain_M: TEdit
          Left = 167
          Top = 110
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          Text = '30'
        end
        object edtResetMain_L: TEdit
          Left = 214
          Top = 111
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 7
          Text = '200'
        end
        object edtResetSub_H: TEdit
          Left = 120
          Top = 137
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          Text = '10'
        end
        object edtResetSub_M: TEdit
          Left = 167
          Top = 137
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          Text = '30'
        end
        object edtResetSub_L: TEdit
          Left = 214
          Top = 138
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 10
          Text = '200'
        end
      end
    end
  end
  object btnSend: TButton
    Left = 232
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
  object btnHide: TButton
    Left = 375
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Hide'
    TabOrder = 3
    OnClick = btnHideClick
  end
end
