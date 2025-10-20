object frmOcsVersionRequest: TfrmOcsVersionRequest
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'OCS -> OHT '#48260#51204#47928#51032
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
  object btnClose: TButton
    Left = 558
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 0
    OnClick = btnCloseClick
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 686
    Height = 392
    ActivePage = TabSheet1
    TabOrder = 1
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
  end
  object btnSend: TButton
    Left = 416
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Send Packet'
    TabOrder = 2
    OnClick = btnSendClick
  end
end
