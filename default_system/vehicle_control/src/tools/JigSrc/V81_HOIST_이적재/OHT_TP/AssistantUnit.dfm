object AssistantForm: TAssistantForm
  Left = 671
  Top = 280
  Caption = 'Assistant Comtrol'
  ClientHeight = 347
  ClientWidth = 392
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object stsbarAssistant: TStatusBar
    Left = 0
    Top = 328
    Width = 392
    Height = 19
    DoubleBuffered = False
    Panels = <>
    ParentDoubleBuffered = False
    ExplicitWidth = 408
  end
  object panBottom: TPanel
    Left = 0
    Top = 236
    Width = 392
    Height = 92
    Align = alBottom
    TabOrder = 1
    ExplicitWidth = 408
  end
  object panMiddle: TPanel
    Left = 0
    Top = 111
    Width = 392
    Height = 125
    Align = alBottom
    TabOrder = 2
    ExplicitWidth = 408
    object grpProgramControl: TGroupBox
      Left = 203
      Top = 1
      Width = 188
      Height = 123
      Align = alClient
      Caption = ' OHT Control '
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      ExplicitWidth = 204
      object btnOHTMainClose: TButton
        Left = 32
        Top = 18
        Width = 140
        Height = 25
        Caption = 'OHT Close'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = btnOHTMainCloseClick
      end
      object btnOHTMainExec: TButton
        Left = 32
        Top = 80
        Width = 140
        Height = 25
        Caption = 'OHT Exec'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = btnOHTMainExecClick
      end
      object btnOHTMainUpload: TButton
        Left = 32
        Top = 49
        Width = 140
        Height = 25
        Caption = 'OHT Upload'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = btnOHTMainUploadClick
      end
    end
    object grpFBWFControl: TGroupBox
      Left = 1
      Top = 1
      Width = 202
      Height = 123
      Align = alLeft
      Caption = 'FBWF Control'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #47569#51008' '#44256#46357
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      object btReqFBWFDisable: TButton
        Left = 31
        Top = 80
        Width = 140
        Height = 25
        Caption = 'FBWF Disable'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = btReqFBWFDisableClick
      end
      object btReqFBWFEnable: TButton
        Left = 31
        Top = 49
        Width = 140
        Height = 25
        Caption = 'FBWF Enable'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = btReqFBWFEnableClick
      end
      object btReqFBWFStat: TButton
        Left = 31
        Top = 18
        Width = 140
        Height = 25
        Caption = 'FBWF Status'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = btReqFBWFStatClick
      end
    end
  end
  object panTop: TPanel
    Left = 0
    Top = 0
    Width = 392
    Height = 111
    Align = alClient
    TabOrder = 3
    ExplicitWidth = 408
    object grpOHTInfo: TGroupBox
      Left = 1
      Top = 1
      Width = 390
      Height = 109
      Align = alClient
      Caption = 'OHT Infomations'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #47569#51008' '#44256#46357
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      ExplicitWidth = 408
      object lbOHTVersion: TLabel
        Left = 216
        Top = 24
        Width = 51
        Height = 15
        Caption = 'Unknown'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object lbVersion: TLabel
        Left = 112
        Top = 21
        Width = 98
        Height = 15
        Caption = 'Current Version :'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label1: TLabel
        Left = 114
        Top = 87
        Width = 96
        Height = 15
        Caption = 'Log disk status :'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblLog: TLabel
        Left = 127
        Top = 65
        Width = 83
        Height = 15
        Caption = 'Log Disk Info :'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbDriveConnection: TLabel
        Left = 216
        Top = 87
        Width = 51
        Height = 15
        Caption = 'Unknown'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object lbSpace: TLabel
        Left = 216
        Top = 45
        Width = 51
        Height = 15
        Caption = 'Unknown'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object Label2: TLabel
        Left = 313
        Top = 43
        Width = 69
        Height = 15
        Caption = '(Free / Total)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object Label3: TLabel
        Left = 106
        Top = 43
        Width = 104
        Height = 15
        Caption = 'System Disk Info :'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbLogSpace: TLabel
        Left = 216
        Top = 66
        Width = 51
        Height = 15
        Caption = 'Unknown'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object lbl1: TLabel
        Left = 313
        Top = 65
        Width = 69
        Height = 15
        Caption = '(Free / Total)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object panVer: TPanel
        Left = 15
        Top = 24
        Width = 82
        Height = 61
        BevelInner = bvLowered
        Caption = '14'
        Color = clYellow
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -64
        Font.Name = #48148#53461#52404
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 0
      end
    end
  end
  object dlgOpen: TOpenDialog
    Left = 272
    Top = 256
  end
end
