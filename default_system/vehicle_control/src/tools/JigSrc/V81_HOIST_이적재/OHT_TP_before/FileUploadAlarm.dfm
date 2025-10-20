object FileUploadAlarmForm: TFileUploadAlarmForm
  Left = 601
  Top = 311
  Caption = 'FileUploadAlarmForm'
  ClientHeight = 173
  ClientWidth = 383
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  DesignSize = (
    383
    173)
  PixelsPerInch = 96
  TextHeight = 13
  object lbAlarm: TLabel
    Left = 6
    Top = 8
    Width = 372
    Height = 13
    Anchors = [akLeft, akRight]
    AutoSize = False
    Caption = '*** '#44053#51228#51652#54665' '#54616#49892' '#44221#50864' '#51060#47492#51012' '#51077#47141#54644' '#51452#49464#50836'.  '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Layout = tlCenter
  end
  object Label2: TLabel
    Left = 8
    Top = 80
    Width = 97
    Height = 13
    Anchors = [akLeft, akTop, akRight, akBottom]
    AutoSize = False
    Caption = #50868#50689#51088' '#51060#47492' :'
  end
  object Label1: TLabel
    Left = 6
    Top = 32
    Width = 370
    Height = 13
    Anchors = [akLeft, akRight]
    AutoSize = False
    Caption = '** '#44053#51228#51652#54665' '#54980' '#50640#47084' '#49345#54889#51032' '#52293#51076#51008' '#50868#50689#51088#50640#44172' '#51080#49845#45768#45796'!!!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Layout = tlCenter
  end
  object Label3: TLabel
    Left = 6
    Top = 56
    Width = 370
    Height = 13
    Anchors = [akLeft, akRight]
    AutoSize = False
    Caption = '* '#52712#49548#47484' '#50896#54616#49884#45716' '#44221#50864' '#52712#49548'('#46608#45716' '#45803#44592') '#48260#53948#51012' '#53364#47533#54616#49464#50836'.'
    Layout = tlCenter
  end
  object editName: TEdit
    Left = 112
    Top = 76
    Width = 257
    Height = 21
    ImeName = 'Microsoft IME 2010'
    TabOrder = 0
  end
  object btnForce: TButton
    Left = 8
    Top = 112
    Width = 177
    Height = 57
    Caption = #44053' '#51228' '#51652' '#54665
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -21
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    OnClick = btnForceClick
  end
  object btnCancel: TButton
    Left = 200
    Top = 112
    Width = 177
    Height = 57
    Caption = #52712' '#49548
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btnCancelClick
  end
end
