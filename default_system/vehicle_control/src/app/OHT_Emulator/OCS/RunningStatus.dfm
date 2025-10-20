object frmRunningStatus: TfrmRunningStatus
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'Running Status'
  ClientHeight = 337
  ClientWidth = 1231
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
  object Panel2: TPanel
    Left = 592
    Top = 8
    Width = 631
    Height = 321
    Color = clGradientInactiveCaption
    ParentBackground = False
    TabOrder = 0
    object Edit2: TEdit
      Left = 8
      Top = 8
      Width = 81
      Height = 21
      Color = clInfoBk
      Enabled = False
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 0
      Text = 'AUTO JOB LOG'
    end
    object mBSLog: TMemo
      Left = 8
      Top = 35
      Width = 609
      Height = 246
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 1
    end
    object btnClear: TButton
      Left = 350
      Top = 4
      Width = 105
      Height = 25
      Caption = 'Log Clear'
      TabOrder = 2
      OnClick = btnClearClick
    end
    object btnHide: TButton
      Left = 493
      Top = 287
      Width = 124
      Height = 25
      Caption = 'Hide'
      TabOrder = 3
      OnClick = btnHideClick
    end
  end
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 578
    Height = 321
    Color = clGradientInactiveCaption
    ParentBackground = False
    TabOrder = 1
    object Edit1: TEdit
      Left = 8
      Top = 8
      Width = 73
      Height = 21
      Color = clInfoBk
      Enabled = False
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 0
      Text = 'OHT STATUS'
    end
    object GroupBox2: TGroupBox
      Left = 8
      Top = 179
      Width = 233
      Height = 129
      Caption = #51452#54665#49345#53468
      TabOrder = 1
      object Running_0: TEdit
        Left = 32
        Top = 16
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 0
        Text = #51221#50948#52824' '#51221#51648
      end
      object Running_1: TEdit
        Left = 32
        Top = 43
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 1
        Text = #51452#54665#51473
      end
      object Running_2: TEdit
        Left = 32
        Top = 70
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 2
        Text = 'BZ '#45824#44592#51473
      end
      object Running_3: TEdit
        Left = 32
        Top = 97
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 3
        Text = #52628#46028#48169#51648' '#45824#44592#51473
      end
      object Edit23: TEdit
        Left = 9
        Top = 16
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 4
        Text = '0'
      end
      object Edit24: TEdit
        Left = 9
        Top = 43
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 5
        Text = '1'
      end
      object Edit25: TEdit
        Left = 9
        Top = 70
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 6
        Text = '2'
      end
      object Edit26: TEdit
        Left = 9
        Top = 97
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 7
        Text = '3'
      end
      object Edit27: TEdit
        Left = 121
        Top = 16
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 8
        Text = '4'
      end
      object Running_4: TEdit
        Left = 144
        Top = 16
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 9
        Text = #44048#49549' '#51221#51648#51473
      end
      object Edit29: TEdit
        Left = 121
        Top = 43
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 10
        Text = '5'
      end
      object Running_5: TEdit
        Left = 144
        Top = 43
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 11
        Text = #49341#52404
      end
      object Edit31: TEdit
        Left = 121
        Top = 70
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 12
        Text = '6'
      end
      object Running_6: TEdit
        Left = 144
        Top = 70
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 13
        Text = #51221#52404
      end
      object Edit33: TEdit
        Left = 121
        Top = 97
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 14
        Text = '7'
      end
      object Running_7: TEdit
        Left = 144
        Top = 97
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 15
        Text = #51221#50948#52824' '#50724#48260#47088
      end
    end
    object GroupBox1: TGroupBox
      Left = 8
      Top = 44
      Width = 233
      Height = 129
      Caption = #44032#46041#49345#53468
      TabOrder = 2
      object Work_0: TEdit
        Left = 32
        Top = 16
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 0
        Text = #44592#46041#51473
      end
      object Work_1: TEdit
        Left = 32
        Top = 43
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 1
        Text = #47112#51060#50500#50883' '#48320#44221
      end
      object Work_2: TEdit
        Left = 32
        Top = 70
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 2
        Text = #51060#49345
      end
      object Work_3: TEdit
        Left = 32
        Top = 97
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 3
        Text = 'Reserved'
      end
      object Edit7: TEdit
        Left = 9
        Top = 16
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 4
        Text = '0'
      end
      object Edit8: TEdit
        Left = 9
        Top = 43
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 5
        Text = '1'
      end
      object Edit9: TEdit
        Left = 9
        Top = 70
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 6
        Text = '2'
      end
      object Edit10: TEdit
        Left = 9
        Top = 97
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 7
        Text = '3'
      end
      object Edit11: TEdit
        Left = 121
        Top = 16
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 8
        Text = '4'
      end
      object Work_4: TEdit
        Left = 144
        Top = 16
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 9
        Text = #51088#46041
      end
      object Edit13: TEdit
        Left = 121
        Top = 43
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 10
        Text = '5'
      end
      object Work_5: TEdit
        Left = 144
        Top = 43
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 11
        Text = #48152#49569#44032#45733
      end
      object Edit15: TEdit
        Left = 121
        Top = 70
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 12
        Text = '6'
      end
      object Work_6: TEdit
        Left = 144
        Top = 70
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 13
        Text = 'Reserved'
      end
      object Edit17: TEdit
        Left = 121
        Top = 97
        Width = 17
        Height = 21
        Color = clInfoBk
        ImeName = 'Microsoft IME 2010'
        ReadOnly = True
        TabOrder = 14
        Text = '7'
      end
      object Work_7: TEdit
        Left = 144
        Top = 97
        Width = 83
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 15
        Text = 'Reserved'
      end
    end
  end
end
