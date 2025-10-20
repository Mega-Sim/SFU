object frmJobOrderOCS: TfrmJobOrderOCS
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'MCP -> CLW '#51089#50629#51648#49884' (ID:121)'
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
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
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
          Top = 134
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
        object GroupBox4: TGroupBox
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
        object GroupBox5: TGroupBox
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
      Caption = 'BODY (1/2)'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox1: TGroupBox
        Left = 16
        Top = 16
        Width = 641
        Height = 329
        Caption = ' BODY '
        TabOrder = 0
        object Label7: TLabel
          Left = 16
          Top = 28
          Width = 38
          Height = 13
          Caption = #51648#49884' ID'
        end
        object Label8: TLabel
          Left = 16
          Top = 55
          Width = 48
          Height = 13
          Caption = #51648#49884#44396#48516
        end
        object Label9: TLabel
          Left = 16
          Top = 82
          Width = 63
          Height = 13
          Caption = #51648#49884' '#49324#51060#53364
        end
        object Label10: TLabel
          Left = 552
          Top = 128
          Width = 63
          Height = 13
          Caption = #48152#49569#47932' Type'
          Visible = False
        end
        object Label11: TLabel
          Left = 16
          Top = 240
          Width = 36
          Height = 13
          Caption = #50864#49440#46020
        end
        object Label12: TLabel
          Left = 16
          Top = 128
          Width = 60
          Height = 13
          Caption = #47785#51201#51648#45432#46300
        end
        object Label16: TLabel
          Left = 16
          Top = 155
          Width = 73
          Height = 13
          Caption = #47785#51201#51648' Station'
        end
        object Label17: TLabel
          Left = 16
          Top = 181
          Width = 51
          Height = 13
          Caption = #45796#51020' '#45432#46300
        end
        object Label18: TLabel
          Left = 16
          Top = 269
          Width = 72
          Height = 13
          Caption = 'CHANGE NODE'
        end
        object Label19: TLabel
          Left = 16
          Top = 296
          Width = 92
          Height = 13
          Caption = 'FINAL_PORT_TYPE'
        end
        object Label21: TLabel
          Left = 445
          Top = 18
          Width = 25
          Height = 13
          Caption = 'HIGH'
        end
        object Label22: TLabel
          Left = 492
          Top = 18
          Width = 19
          Height = 13
          Caption = 'MID'
        end
        object Label23: TLabel
          Left = 539
          Top = 18
          Width = 23
          Height = 13
          Caption = 'LOW'
        end
        object Label24: TLabel
          Left = 289
          Top = 67
          Width = 135
          Height = 13
          Caption = #51060#51201#51116' Buffer '#50948#52824' (0-255)'
        end
        object Label25: TLabel
          Left = 289
          Top = 101
          Width = 119
          Height = 13
          Caption = 'Target '#51060#51201#51116' Tag Type'
        end
        object Label26: TLabel
          Left = 289
          Top = 128
          Width = 119
          Height = 13
          Caption = 'Target '#51060#51201#51116' Tag Type'
        end
        object Label27: TLabel
          Left = 289
          Top = 155
          Width = 119
          Height = 13
          Caption = 'Target '#51060#51201#51116' Tag Type'
        end
        object Label28: TLabel
          Left = 289
          Top = 181
          Width = 172
          Height = 13
          Caption = 'Node'#47196#48512#53552' '#51060#51201#51116' Tag'#44620#51648' '#44144#47532
        end
        object Label29: TLabel
          Left = 289
          Top = 208
          Width = 106
          Height = 13
          Caption = 'StationData Reserved'
        end
        object Label30: TLabel
          Left = 289
          Top = 240
          Width = 180
          Height = 13
          Caption = 'Node'#47196#48512#53552' '#51060#51201#51116' Tag'#44620#51648' (Hoist)'
        end
        object Label31: TLabel
          Left = 290
          Top = 267
          Width = 178
          Height = 13
          Caption = 'Node'#47196#48512#53552' '#51060#51201#51116' Tag'#44620#51648' (Shift)'
        end
        object Label32: TLabel
          Left = 289
          Top = 293
          Width = 189
          Height = 13
          Caption = 'Node'#47196#48512#53552' '#51060#51201#51116' Tag'#44620#51648' (Rotate)'
        end
        object cboCmdDiv: TComboBox
          Left = 112
          Top = 52
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
            'Test Mode')
        end
        object cboCmdCycle: TComboBox
          Left = 112
          Top = 79
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 1
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
        object cboWorkCarryType: TComboBox
          Left = 552
          Top = 147
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 2
          Text = 'FOUP'
          Visible = False
          Items.Strings = (
            'FOUP'
            'POD'
            'MAC'
            'FOSB'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboRiv: TComboBox
          Left = 167
          Top = 237
          Width = 90
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 3
          Text = 'Reserved_0'
          Items.Strings = (
            'Reserved_0'
            'Reserved_1'
            'Reserved_2'
            'Reserved_3'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6')
        end
        object edtCurAddr_H: TEdit
          Left = 120
          Top = 125
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          Text = '1'
        end
        object edtStopAddr_H: TEdit
          Left = 120
          Top = 152
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          Text = '1'
        end
        object edtDestAddr_H: TEdit
          Left = 120
          Top = 178
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          Text = '10'
        end
        object edtDestAddr_M: TEdit
          Left = 167
          Top = 178
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 7
          Text = '30'
        end
        object edtDestAddr_L: TEdit
          Left = 214
          Top = 178
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          Text = '200'
        end
        object chkHotLot: TCheckBox
          Left = 111
          Top = 235
          Width = 50
          Height = 25
          Caption = 'Hotlot'
          TabOrder = 9
        end
        object cboChangeNode: TComboBox
          Left = 112
          Top = 266
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 10
          Text = 'MAP'#48264#54840'_0'
          Items.Strings = (
            'MAP'#48264#54840'_0'
            'MAP'#48264#54840'_1'
            'MAP'#48264#54840'_2'
            'MAP'#48264#54840'_3'
            'Reserved_4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboFinalPortType: TComboBox
          Left = 112
          Top = 293
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 11
          Text = 'STOKER'
          Items.Strings = (
            'STOKER'
            'EQ'
            'STB'
            'UTB'
            'LOADER'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object edtBufferType: TEdit
          Left = 436
          Top = 64
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 12
          Text = '1'
        end
        object edtTeachingData_Hoist: TEdit
          Left = 492
          Top = 237
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 13
          Text = '1'
        end
        object edtTeachingData_Shift: TEdit
          Left = 492
          Top = 264
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 14
          Text = '1'
        end
        object edtTeachingData_Rotate: TEdit
          Left = 492
          Top = 290
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 15
          Text = '10'
        end
        object edtOrderID: TEdit
          Left = 112
          Top = 24
          Width = 49
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 16
          Text = 'edtOrderID'
        end
        object btnGetOrderID: TButton
          Left = 167
          Top = 24
          Width = 90
          Height = 25
          Caption = 'Get OrderID'
          TabOrder = 17
          OnClick = btnGetOrderIDClick
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'BODY (2/2)'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox2: TGroupBox
        Left = 16
        Top = 16
        Width = 641
        Height = 337
        Caption = ' BODY '
        TabOrder = 0
        object Label33: TLabel
          Left = 16
          Top = 28
          Width = 143
          Height = 13
          Caption = 'TEACHINGDATA_LOOKDOWN'
        end
        object Label34: TLabel
          Left = 16
          Top = 55
          Width = 127
          Height = 13
          Caption = 'TEACHINGDATA_PIOINFO'
        end
        object Label35: TLabel
          Left = 16
          Top = 82
          Width = 126
          Height = 13
          Caption = 'TEACHINGDATA_PIOTIME'
        end
        object Label36: TLabel
          Left = 16
          Top = 109
          Width = 123
          Height = 13
          Caption = 'TEACHINGDATA_OPTION'
        end
        object Label37: TLabel
          Left = 16
          Top = 141
          Width = 132
          Height = 13
          Caption = 'TeachingData_Reserved[8]'
        end
        object Label38: TLabel
          Left = 16
          Top = 173
          Width = 75
          Height = 13
          Caption = #44592#48376' '#48516#44592#48169#54693
        end
        object Label39: TLabel
          Left = 16
          Top = 237
          Width = 48
          Height = 13
          Caption = #44060#49884#48264#51648
        end
        object Label40: TLabel
          Left = 16
          Top = 269
          Width = 48
          Height = 13
          Caption = #51333#47308#48264#51648
        end
        object Label41: TLabel
          Left = 16
          Top = 301
          Width = 102
          Height = 13
          Caption = #44592#48376#50808' '#48516#44592' '#48264#51648#49688
        end
        object Label42: TLabel
          Left = 336
          Top = 25
          Width = 117
          Height = 13
          Caption = #44592#48376#50808' '#48516#44592' '#48264#51648' '#47785#47197
        end
        object cboTeachingData_LookDownLevel: TComboBox
          Left = 165
          Top = 25
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 0
          Text = 'LKDown_1'
          Items.Strings = (
            'LKDown_1'
            'LKDown_2'
            'LKDown_3'
            'LKDown_4'
            'LKDown_5'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboTeachingData_PIOInfo: TComboBox
          Left = 165
          Top = 52
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 1
          Text = 'EQPIO_ENABLE'
          Items.Strings = (
            'EQPIO_ENABLE'
            'EQPIO_Left'
            'EQPIO_Right'
            'EQPIO_IR'
            'EQPIO_RF'
            'Reserved_4'
            'PIO_CS0'
            'PIO_CS1')
        end
        object cboTeachingData_PIOTimeLevel: TComboBox
          Left = 165
          Top = 79
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 2
          Text = 'PIOTIME_1'
          Items.Strings = (
            'PIOTIME_1'
            'PIOTIME_2'
            'PIOTIME_3'
            'PIOTIME_4'
            'PIOTIME_5'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboTeachingData_Option: TComboBox
          Left = 165
          Top = 106
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 3
          Text = 'AUTORECOVERY'
          Items.Strings = (
            'AUTORECOVERY'
            'RFREADER'
            'TRANSSOUND'
            'OSCILATION'
            'HANDDECETEQ'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object GroupBox3: TGroupBox
          Left = 120
          Top = 168
          Width = 193
          Height = 49
          Caption = 'GroupBox3'
          TabOrder = 4
          object rdoPathData_Def_Direction_Left: TRadioButton
            Left = 16
            Top = 24
            Width = 57
            Height = 17
            Caption = 'Left'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object rdoPathData_Def_Direction_Right: TRadioButton
            Left = 79
            Top = 24
            Width = 57
            Height = 17
            Caption = 'Right'
            TabOrder = 1
          end
        end
        object edtPathData_StartNodeID: TEdit
          Left = 124
          Top = 234
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          Text = '1'
        end
        object edtPathData_EndNodeID: TEdit
          Left = 124
          Top = 269
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          Text = '1'
        end
        object edtPathData_ExtraNum: TEdit
          Left = 124
          Top = 298
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 7
          Text = '0'
        end
        object FeederGrid: TStringGrid
          Left = 336
          Top = 44
          Width = 188
          Height = 285
          ColCount = 2
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goEditing]
          ParentFont = False
          ScrollBars = ssVertical
          TabOrder = 8
          ColWidths = (
            64
            126)
        end
        object chkDriving: TCheckBox
          Left = 16
          Top = 192
          Width = 75
          Height = 17
          Caption = 'Driving'
          TabOrder = 9
        end
      end
    end
  end
  object btnClose: TButton
    Left = 558
    Top = 406
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 1
    OnClick = btnCloseClick
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
