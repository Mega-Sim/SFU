object frmAutoJob: TfrmAutoJob
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = #51088#46041' '#46041#51089
  ClientHeight = 526
  ClientWidth = 1428
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
  object FeederGrid: TStringGrid
    Left = 8
    Top = 64
    Width = 1412
    Height = 361
    ColCount = 18
    DefaultColWidth = 40
    DefaultRowHeight = 22
    RowCount = 2
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
    ParentFont = False
    TabOrder = 0
    OnSelectCell = FeederGridSelectCell
    ColWidths = (
      40
      137
      157
      40
      49
      143
      203
      60
      63
      56
      56
      58
      57
      59
      57
      55
      51
      50)
  end
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 873
    Height = 41
    Color = clSkyBlue
    ParentBackground = False
    TabOrder = 1
    object Label1: TLabel
      Left = 451
      Top = 14
      Width = 56
      Height = 14
      Caption = #47749#47161#50612' '#49688
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object btnNew: TButton
      Left = 8
      Top = 8
      Width = 73
      Height = 25
      Caption = 'New'
      TabOrder = 0
      OnClick = btnNewClick
    end
    object btnSave: TButton
      Left = 707
      Top = 8
      Width = 73
      Height = 25
      Caption = 'Save'
      TabOrder = 1
      OnClick = btnSaveClick
    end
    object btnLoad: TButton
      Left = 786
      Top = 8
      Width = 73
      Height = 25
      Caption = 'Load'
      TabOrder = 2
      OnClick = btnLoadClick
    end
    object btnAdd: TButton
      Left = 268
      Top = 8
      Width = 73
      Height = 25
      Caption = 'Add'
      TabOrder = 3
      OnClick = btnAddClick
    end
    object btnDelete: TButton
      Left = 367
      Top = 8
      Width = 73
      Height = 25
      Caption = 'Delete'
      TabOrder = 4
      OnClick = btnDeleteClick
    end
    object rdoTrunDefault: TRadioGroup
      Left = 586
      Top = 4
      Width = 113
      Height = 31
      Caption = #44592#48376#48516#44592' '#48169#54693
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        'LEFT'
        'RIGHT')
      TabOrder = 5
    end
    object edtCount: TEdit
      Left = 513
      Top = 9
      Width = 44
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ImeName = 'Microsoft IME 2010'
      ParentFont = False
      ReadOnly = True
      TabOrder = 6
      Text = '0'
    end
  end
  object Panel2: TPanel
    Left = 8
    Top = 431
    Width = 809
    Height = 87
    Color = clCream
    ParentBackground = False
    TabOrder = 2
    object Label2: TLabel
      Left = 8
      Top = 55
      Width = 117
      Height = 19
      Caption = #49892#54665#51473' '#47749#47161#46972#51064
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object btnRun: TButton
      Left = 8
      Top = 8
      Width = 50
      Height = 33
      Caption = 'Run'
      TabOrder = 0
      OnClick = btnRunClick
    end
    object btbPause: TButton
      Left = 63
      Top = 8
      Width = 50
      Height = 33
      Caption = 'Pause'
      Enabled = False
      TabOrder = 1
      OnClick = btbPauseClick
    end
    object btnStop: TButton
      Left = 175
      Top = 8
      Width = 50
      Height = 33
      Caption = 'Stop'
      Enabled = False
      TabOrder = 2
      OnClick = btnStopClick
    end
    object chkAutoOcsStatus: TCheckBox
      Left = 267
      Top = 8
      Width = 153
      Height = 17
      Caption = 'OCS '#49345#53468#48372#44256
      TabOrder = 3
      OnClick = chkAutoOcsStatusClick
    end
    object rdoGrpPass: TRadioGroup
      Left = 369
      Top = 7
      Width = 201
      Height = 41
      Caption = #53685#44284#54728#44032' '#52376#47532
      Columns = 3
      ItemIndex = 0
      Items.Strings = (
        #54637#49345'ACK'
        #54637#49345'NAK'
        #47924#51025#45813)
      TabOrder = 4
      OnClick = rdoGrpPassClick
    end
    object rdoGrpOpenReq: TRadioGroup
      Left = 576
      Top = 8
      Width = 217
      Height = 41
      Caption = #53685#44284#54728#44032' '#44060#48169#50836#44396' '#52376#47532
      Columns = 3
      ItemIndex = 0
      Items.Strings = (
        #54637#49345'ACK'
        #54637#49345'NAK'
        #47924#51025#45813)
      TabOrder = 5
      OnClick = rdoGrpOpenReqClick
    end
    object btnResume: TButton
      Left = 119
      Top = 8
      Width = 50
      Height = 33
      Caption = 'Resume'
      Enabled = False
      TabOrder = 6
      OnClick = btnResumeClick
    end
    object prgAutoSend: TProgressBar
      Left = 267
      Top = 31
      Width = 86
      Height = 17
      Max = 9
      Step = 1
      TabOrder = 7
    end
    object btnPassConfirmSet: TButton
      Left = 370
      Top = 54
      Width = 103
      Height = 25
      Caption = #53685#44284#54728#44032' '#49444#51221
      TabOrder = 8
      OnClick = btnPassConfirmSetClick
    end
    object btnOpenResponseSet: TButton
      Left = 576
      Top = 55
      Width = 103
      Height = 25
      Caption = #44060#48169#50836#44396' '#49444#51221
      TabOrder = 9
      OnClick = btnOpenResponseSetClick
    end
    object Button2: TButton
      Left = 267
      Top = 54
      Width = 72
      Height = 25
      Caption = #53580#49828#53944
      TabOrder = 10
      Visible = False
      OnClick = Button2Click
    end
    object edtRunCount: TEdit
      Left = 136
      Top = 52
      Width = 70
      Height = 27
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ImeName = 'Microsoft IME 2010'
      ParentFont = False
      TabOrder = 11
      Text = '1'
    end
  end
  object btnClose: TButton
    Left = 1296
    Top = 469
    Width = 124
    Height = 49
    Caption = 'Close'
    TabOrder = 3
    OnClick = btnCloseClick
  end
  object FWidthComboBox: TComboBox
    Left = 437
    Top = 88
    Width = 140
    Height = 24
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ImeName = #54620#44397#50612'('#54620#44544') (MS-IME95)'
    ParentFont = False
    TabOrder = 4
    Visible = False
    OnChange = FWidthComboBoxChange
    Items.Strings = (
      #51452#54665#44048#49549'+'#51060#51116#44048#49549
      #51452#54665#44048#49549'+'#51060#51116#51593
      #51452#54665#51593'+'#51060#51116#44048#49549
      #51452#54665#51593'+'#51060#51116#51593)
  end
  object cboMajorCmd: TComboBox
    Left = 122
    Top = 16
    Width = 148
    Height = 24
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ImeName = #54620#44397#50612'('#54620#44544') (MS-IME95)'
    ItemIndex = 0
    ParentFont = False
    TabOrder = 5
    Text = 'MOVE_ON'
    Items.Strings = (
      'MOVE_ON'
      'MOVE_OFF'
      'ORD_MOVE_NORMAL'
      'ORD_UP_NORMAL'
      'ORD_DN_NORMAL')
  end
  object Button1: TButton
    Left = 1313
    Top = 8
    Width = 72
    Height = 25
    Caption = #53580#49828#53944
    TabOrder = 6
    Visible = False
    OnClick = Button1Click
  end
  object Panel3: TPanel
    Left = 823
    Top = 431
    Width = 322
    Height = 87
    Color = clActiveCaption
    ParentBackground = False
    TabOrder = 7
    object edtFileName: TEdit
      Left = 8
      Top = 28
      Width = 305
      Height = 21
      Color = clInfoBk
      Enabled = False
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 0
      Text = 'UNTITLED.PRG'
    end
    object Edit1: TEdit
      Left = 8
      Top = 5
      Width = 73
      Height = 21
      Alignment = taCenter
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ImeName = 'Microsoft IME 2010'
      ParentFont = False
      ReadOnly = True
      TabOrder = 1
      Text = 'FILE NAME'
    end
  end
  object btnViewStatus: TButton
    Left = 896
    Top = 8
    Width = 97
    Height = 41
    Caption = #47196#44536' '#48372#44592
    TabOrder = 8
    OnClick = btnViewStatusClick
  end
  object JobOpenDialog: TOpenDialog
    Left = 824
    Top = 368
  end
  object JobSaveAsDialog: TSaveDialog
    Left = 728
    Top = 368
  end
  object tmrCmdSend: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrCmdSendTimer
    Left = 640
    Top = 368
  end
  object tmrOcsStatus: TTimer
    Enabled = False
    OnTimer = tmrOcsStatusTimer
    Left = 176
    Top = 376
  end
  object tmrRetry: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = tmrRetryTimer
    Left = 936
    Top = 368
  end
end
