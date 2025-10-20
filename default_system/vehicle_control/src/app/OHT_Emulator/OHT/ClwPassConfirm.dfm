object frmClwPassConfirm: TfrmClwPassConfirm
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'CLW -> MCP '#53685#44284#54728#44032' '#50836#44396' (ID:241)'
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
    Width = 696
    Height = 401
    ActivePage = TabSheet2
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'OHT Status '
      object GroupBox1: TGroupBox
        Left = 16
        Top = 9
        Width = 657
        Height = 345
        Caption = ' STATUS '
        TabOrder = 0
        object Label1: TLabel
          Left = 16
          Top = 27
          Width = 48
          Height = 13
          Caption = #44032#46041#49345#53468
        end
        object Label3: TLabel
          Left = 16
          Top = 54
          Width = 48
          Height = 13
          Caption = #51452#54665#49345#53468
        end
        object Label4: TLabel
          Left = 16
          Top = 81
          Width = 63
          Height = 13
          Caption = #51060#51116#44592' '#49345#53468
        end
        object Label5: TLabel
          Left = 16
          Top = 108
          Width = 75
          Height = 13
          Caption = #50976#51648#48372#49688' '#49345#53468
        end
        object Label6: TLabel
          Left = 16
          Top = 135
          Width = 90
          Height = 13
          Caption = #51116#54616' '#48143' '#49688#54616#49345#53468
        end
        object Label7: TLabel
          Left = 8
          Top = 183
          Width = 114
          Height = 13
          Caption = #54788#51116#48264#51648' ('#44033#44033' 0-255)'
        end
        object Label8: TLabel
          Left = 16
          Top = 210
          Width = 99
          Height = 13
          Caption = #51221#51648#51473' station '#48264#51648
        end
        object Label9: TLabel
          Left = 16
          Top = 236
          Width = 99
          Height = 13
          Caption = #47785#51201#51648' station '#48264#51648
        end
        object Label10: TLabel
          Left = 16
          Top = 263
          Width = 48
          Height = 13
          Caption = #48516#44592#48169#54693
        end
        object Label11: TLabel
          Left = 16
          Top = 290
          Width = 75
          Height = 13
          Caption = #51452#54665#44144#47532' (mm)'
        end
        object Label12: TLabel
          Left = 128
          Top = 161
          Width = 25
          Height = 13
          Caption = 'HIGH'
        end
        object Label13: TLabel
          Left = 175
          Top = 161
          Width = 19
          Height = 13
          Caption = 'MID'
        end
        object Label14: TLabel
          Left = 222
          Top = 161
          Width = 23
          Height = 13
          Caption = 'LOW'
        end
        object Label15: TLabel
          Left = 280
          Top = 27
          Width = 75
          Height = 13
          Caption = #49892#54665#51473' '#49324#51060#53364
        end
        object Label16: TLabel
          Left = 280
          Top = 54
          Width = 102
          Height = 13
          Caption = #45824#44592' '#49324#51060#53364' (0-255)'
        end
        object Label17: TLabel
          Left = 280
          Top = 81
          Width = 90
          Height = 13
          Caption = #50640#47084#53076#46300' (int, 4B)'
        end
        object Label18: TLabel
          Left = 280
          Top = 108
          Width = 87
          Height = 13
          Caption = #51204#54028#44053#46020' (1Byte)'
        end
        object Label20: TLabel
          Left = 269
          Top = 163
          Width = 126
          Height = 13
          Caption = #45936#51060#53552' '#51204#49569#49345#53468' (1Byte)'
        end
        object Label21: TLabel
          Left = 16
          Top = 309
          Width = 92
          Height = 13
          Caption = '('#48276#50948' : 0 ~ 65535)'
        end
        object Label22: TLabel
          Left = 448
          Top = 77
          Width = 192
          Height = 13
          Caption = #49892#54665#51473' '#49324#51060#53364' '#48143' '#45824#44592#49324#51060#53364' '#52280#44256#54364
          Color = clGrayText
          ParentColor = False
        end
        object cboWorkingStatus: TComboBox
          Left = 112
          Top = 24
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 0
          Text = #44592#46041#51473
          Items.Strings = (
            #44592#46041#51473
            #47112#51060#50500#50883' '#48320#44221#50857' '#51452#54665#51473
            #51060#49345
            'Reserved_3'
            #51088#46041
            #48152#49569#44032#45733
            'Reserved_6'
            'Reserved_7')
        end
        object cboRunningStatus: TComboBox
          Left = 112
          Top = 51
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 1
          Text = #51221#50948#52824' '#51221#51648
          Items.Strings = (
            #51221#50948#52824' '#51221#51648
            #51452#54665#51473
            'BZ '#45824#44592#51473
            #52628#46028#48169#51648' '#45824#44592#51473
            #44048#49549' '#51221#51648#51473
            #49341#52404
            #51221#52404
            #51221#50948#52824' '#50724#48260#47088)
        end
        object cboCarriage: TComboBox
          Left = 112
          Top = 78
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 2
          Text = #51116#54616'1'
          Items.Strings = (
            #51116#54616'1'
            #51116#54616'2(Reserved)'
            #51060#51473#48152#51077
            #44277#52636#54616
            #51060#51116#44592' HP'
            #51060#51116#44592' HP2(Reserved)'
            'Reserved'
            'E84 '#53440#51076#50500#50883)
        end
        object cboMaintananceState: TComboBox
          Left = 112
          Top = 105
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 3
          Text = #48764#45252'OK'
          Items.Strings = (
            #48764#45252'OK'
            #48764#45252' NG'
            #52789#44036#51060#46041#54869#51064' OK'
            #52789#44036#51060#46041#54869#51064' NG'
            #47112#51068'1 '#53804#51077#50836#44396
            #47112#51068'2 '#53804#51077#50836#44396
            'HT '#51312#51089' '#51221#51648#51473
            #54980#53748' '#51452#54665#51473)
        end
        object cboCarryType: TComboBox
          Left = 112
          Top = 132
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 4
          Text = #51116#54616#51333#47448' '#52404#53356#51473
          Items.Strings = (
            #51116#54616#51333#47448' '#52404#53356#51473
            #51116#54616' '#51333#47448' 1'
            #51116#54616' '#51333#47448' 2'
            #51116#54616' '#51333#47448' 3'
            #51116#54616' '#51333#47448' 4'
            'Reserved_5'
            'Reserved_6'
            'Reserved_7')
        end
        object cboTunDirection: TComboBox
          Left = 112
          Top = 260
          Width = 145
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 5
          Text = #50724#47480#51901' '#48516#44592
          Items.Strings = (
            #50724#47480#51901' '#48516#44592
            #50812#51901' '#48516#44592
            #48516#44592' '#48520#44032
            'Reserved'
            'Reserved'
            'Reserved'
            'Reserved'
            'Reserved')
        end
        object edtCurAddr_H: TEdit
          Left = 128
          Top = 180
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          Text = '1'
        end
        object edtCurAddr_M: TEdit
          Left = 175
          Top = 180
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 7
          Text = '10'
        end
        object edtCurAddr_L: TEdit
          Left = 222
          Top = 180
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          Text = '100'
        end
        object edtStopAddr_H: TEdit
          Left = 128
          Top = 207
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          Text = '1'
        end
        object edtStopAddr_M: TEdit
          Left = 175
          Top = 207
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 10
          Text = '20'
        end
        object edtStopAddr_L: TEdit
          Left = 222
          Top = 207
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 11
          Text = '150'
        end
        object edtDestAddr_H: TEdit
          Left = 128
          Top = 233
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 12
          Text = '10'
        end
        object edtDestAddr_M: TEdit
          Left = 175
          Top = 233
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 13
          Text = '30'
        end
        object edtDestAddr_L: TEdit
          Left = 222
          Top = 233
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 14
          Text = '200'
        end
        object edtRunningDistance: TEdit
          Left = 128
          Top = 287
          Width = 89
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 15
          Text = '20000'
        end
        object edtErrorCode: TEdit
          Left = 382
          Top = 78
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 16
          Text = '0'
        end
        object edtRadioWaveLevel: TEdit
          Left = 382
          Top = 105
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 17
          Text = '234'
        end
        object edtDataTransStatus: TEdit
          Left = 401
          Top = 159
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 18
          Text = '64'
        end
        object Memo1: TMemo
          Left = 448
          Top = 96
          Width = 201
          Height = 241
          ImeName = 'Microsoft IME 2010'
          Lines.Strings = (
            '0x00 (0)'#65306#49324#51060#53364' '#50630#51020
            '0x01 (1)'#65306#50948#52824#54869#51064' '#49324#51060#53364' '#51473
            '0x02 (2)'#65306#51060#46041' '#49324#51060#53364' '
            #51473'(Station)'
            '0x03 (3)'#65306#50732#47548' '#49324#51060#53364' '#51473
            '0x04 (4)'#65306#45236#47548' '#49324#51060#53364' '#51473
            '0x05 (5)'#65306#48764#45252' '#49324#51060#53364' '#51473
            '0x08 (8)'#65306#51060#46041' '#49324#51060#53364' '#51473'('#45432#46300')'
            '0x09 (9) : '#52789#44036' '#51060#46041' '#49324#51060#53364' '#51473'(ID '
            '101'#51032' '#44221#50864#47564')'
            '0x2C (44)'#65306#44228#52769' '#49324#51060#53364'(Yellow '
            'VHL '#51228#50612#50857')'
            '0x2D (45)'#65306#55137#51064#44396' '#51060#46041' '#49324#51060#53364' '
            #51473'('#52397#49548#44592' VHL '#51228#50612#50857')'
            '0x2E (46)'#65306#46041#44036' '#51060#46041' '#49324#51060#53364'(ID '
            '101'#51032' '#44221#50864#47564')'
            '0x2F (47)'#65306#53748#54588' '#51060#46041' '#49324#51060#53364'(ID '
            '101'#51032' '#44221#50864#47564')'
            ''
            #65309#65309#65309#51060#54616', '#49688#46041' '#47784#46300#49884#50640' '#50976#54952
            #65309#65309#65309
            '0x21 (33) '#65306#51452#54924' '#51452#54665#51473' '#49324#51060#53364' '
            #51473'('#48120#49324#50857')'
            '0x22 (34)'#65306#49688#46041' '#51312#51089' '#51473
            '0x23 (35)'#65306#51452#54665' '#54617#49845' '#49324#51060#53364' '#51473
            '0x24 (36)'#65306#51060#51116#48512' '#54617#49845' '#51473'('#54000#52845
            #51473')'
            '0x25 (37)'#65306#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056
            #53556'1)'
            '0x26 (38)'#65306#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056
            #53556'2)('#48120#49324#50857')'
            '0x27 (39)'#65306#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056
            #53556'3)('#48120#49324#50857')')
          ScrollBars = ssVertical
          TabOrder = 19
        end
        object cboWaitingCycle: TComboBox
          Left = 388
          Top = 50
          Width = 199
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 20
          Text = '(0x00) '#49324#51060#53364' '#50630#51020
          Items.Strings = (
            '(0x00) '#49324#51060#53364' '#50630#51020
            '(0x01) '#50948#52824#54869#51064' '#49324#51060#53364' '#51473
            '(0x02) '#51060#46041' '#49324#51060#53364' '#51473'(Station)'
            '(0x03) '#50732#47548' '#49324#51060#53364' '#51473
            '(0x04) '#45236#47548' '#49324#51060#53364' '#51473
            '(0x05) '#48764#45252' '#49324#51060#53364' '#51473
            '(0x06) '#49324#50857#50504#54632
            '(0x07) '#49324#50857#50504#50516
            '(0x08) '#51060#46041' '#49324#51060#53364' '#51473'('#45432#46300')'
            '(0x09) '#52789#44036' '#51060#46041' '#49324#51060#53364' '#51473'(ID 101'#51032' '#44221#50864#47564')'
            '(0x2C) '#44228#52769' '#49324#51060#53364'(Yellow VHL '#51228#50612#50857')'
            '(0x2D) '#55137#51064#44396' '#51060#46041' '#49324#51060#53364' '#51473'('#52397#49548#44592' VHL '#51228#50612#50857')'
            '(0x2E) '#46041#44036' '#51060#46041' '#49324#51060#53364'(ID 101'#51032' '#44221#50864#47564')'
            '(0x2F) '#53748#54588' '#51060#46041' '#49324#51060#53364'(ID 101'#51032' '#44221#50864#47564')'
            '(0x21) '#51452#54924' '#51452#54665#51473' '#49324#51060#53364' '#51473'('#48120#49324#50857')'
            '(0x22) '#49688#46041' '#51312#51089' '#51473
            '(0x23) '#51452#54665' '#54617#49845' '#49324#51060#53364' '#51473
            '(0x24) '#51060#51116#48512' '#54617#49845' '#51473'('#54000#52845#51473')'
            '(0x25) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'1)'
            '(0x26) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'2)('#48120#49324#50857')'
            '(0x27) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'3)('#48120#49324#50857')')
        end
        object Memo2: TMemo
          Left = 269
          Top = 186
          Width = 173
          Height = 142
          ImeName = 'Microsoft IME 2010'
          Lines.Strings = (
            'Bit '#45800#50948
            '0 ~2 : '#45936#51060#53552' '#51204#49569#51648#49884' ID'
            '3 : '#45936#51060#53552' '#51204#49569' '#44396#48516
            '4 ~5 : '#45936#51060#53552' '#51204#49569#49345#53468#44396#48516
            '6 : '#45936#51060#53552' '#51204#49569' '#49457#44277
            '7 : '#45936#51060#53552' '#51204#49569' '#49892#54056
            'ex) 6 => 1,000,000(BIN) = 64'
            '(DEC)'
            '      7 => 10,000,000(BIN) = '
            '128(DEC)')
          TabOrder = 21
        end
        object cboRunningCycle: TComboBox
          Left = 388
          Top = 23
          Width = 199
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 22
          Text = '(0x00) '#49324#51060#53364' '#50630#51020
          Items.Strings = (
            '(0x00) '#49324#51060#53364' '#50630#51020
            '(0x01) '#50948#52824#54869#51064' '#49324#51060#53364' '#51473
            '(0x02) '#51060#46041' '#49324#51060#53364' '#51473'(Station)'
            '(0x03) '#50732#47548' '#49324#51060#53364' '#51473
            '(0x04) '#45236#47548' '#49324#51060#53364' '#51473
            '(0x05) '#48764#45252' '#49324#51060#53364' '#51473
            '(0x06) '#49324#50857#50504#54632
            '(0x07) '#49324#50857#50504#50516
            '(0x08) '#51060#46041' '#49324#51060#53364' '#51473'('#45432#46300')'
            '(0x09) '#52789#44036' '#51060#46041' '#49324#51060#53364' '#51473'(ID 101'#51032' '#44221#50864#47564')'
            '(0x2C) '#44228#52769' '#49324#51060#53364'(Yellow VHL '#51228#50612#50857')'
            '(0x2D) '#55137#51064#44396' '#51060#46041' '#49324#51060#53364' '#51473'('#52397#49548#44592' VHL '#51228#50612#50857')'
            '(0x2E) '#46041#44036' '#51060#46041' '#49324#51060#53364'(ID 101'#51032' '#44221#50864#47564')'
            '(0x2F) '#53748#54588' '#51060#46041' '#49324#51060#53364'(ID 101'#51032' '#44221#50864#47564')'
            '(0x21) '#51452#54924' '#51452#54665#51473' '#49324#51060#53364' '#51473'('#48120#49324#50857')'
            '(0x22) '#49688#46041' '#51312#51089' '#51473
            '(0x23) '#51452#54665' '#54617#49845' '#49324#51060#53364' '#51473
            '(0x24) '#51060#51116#48512' '#54617#49845' '#51473'('#54000#52845#51473')'
            '(0x25) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'1)'
            '(0x26) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'2)('#48120#49324#50857')'
            '(0x27) '#53580#49828#53944' '#49324#51060#53364' '#51473'('#54056#53556'3)('#48120#49324#50857')')
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
        object Label2: TLabel
          Left = 16
          Top = 28
          Width = 63
          Height = 13
          Caption = #49884#53248#49828' '#45336#48260
        end
        object Label23: TLabel
          Left = 8
          Top = 87
          Width = 53
          Height = 13
          Caption = 'STOP '#48264#51648
        end
        object Label24: TLabel
          Left = 8
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
          Left = 8
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
        object GroupBox5: TGroupBox
          Left = 15
          Top = 183
          Width = 610
          Height = 106
          Caption = #53685#44284' '#45824#44592#49884#44036
          TabOrder = 8
          object Label28: TLabel
            Left = 16
            Top = 21
            Width = 449
            Height = 13
            Caption = 
              #45800#50948' msec.'#48276#50948' 0 msec~16,777,214 msec(=0 xFFFFFE= '#50557' 4.6 h). 10'#51652#49688' ms'#47196 +
              ' '#51077#47141#54624' '#44163
          end
          object edtPassWaitTime: TEdit
            Left = 120
            Top = 40
            Width = 217
            Height = 21
            ImeName = 'Microsoft IME 2010'
            TabOrder = 0
            Text = '64'
          end
          object chkManualPass: TCheckBox
            Left = 16
            Top = 72
            Width = 577
            Height = 17
            Caption = 
              #51064#52845' '#51312#51089#51004#47196' STOP '#48264#51648#47196#48512#53552' RESET '#48264#51648#47484' '#53685#44284#54616#45716' '#44221#50864' '#52404#53356' (= 0xFFFFFF = 16,777,215' +
              ' msec)'
            TabOrder = 1
          end
        end
        object edtResetSub_H: TEdit
          Left = 120
          Top = 137
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          Text = '10'
        end
        object edtResetSub_M: TEdit
          Left = 167
          Top = 137
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 10
          Text = '30'
        end
        object edtResetSub_L: TEdit
          Left = 214
          Top = 138
          Width = 41
          Height = 21
          ImeName = 'Microsoft IME 2010'
          TabOrder = 11
          Text = '200'
        end
      end
    end
  end
  object btnSend: TButton
    Left = 416
    Top = 415
    Width = 136
    Height = 33
    Caption = 'Send Packet'
    TabOrder = 1
    OnClick = btnSendClick
  end
  object btnClose: TButton
    Left = 568
    Top = 415
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 2
    OnClick = btnCloseClick
  end
end
