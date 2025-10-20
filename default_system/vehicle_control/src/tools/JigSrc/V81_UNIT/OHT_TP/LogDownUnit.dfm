object LogDownForm: TLogDownForm
  Left = 572
  Top = 327
  Caption = 'Log Download'
  ClientHeight = 213
  ClientWidth = 302
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 302
    Height = 213
    Align = alClient
    BevelInner = bvLowered
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object Label5: TLabel
      Left = 128
      Top = 115
      Width = 13
      Height = 15
      Caption = #51068
    end
    object Label4: TLabel
      Left = 72
      Top = 115
      Width = 13
      Height = 15
      Caption = #50900
    end
    object rgLogSelect: TRadioGroup
      Left = 6
      Top = 9
      Width = 201
      Height = 137
      Caption = ' = Select Log File = '
      ItemIndex = 1
      Items.Strings = (
        'All Log Files'
        'Today'
        ''
        '')
      TabOrder = 2
    end
    object btnDownloadStart: TButton
      Left = 8
      Top = 152
      Width = 75
      Height = 49
      Caption = 'OK'
      TabOrder = 0
      OnClick = btnDownloadStartClick
    end
    object btnDownloadCancel: TButton
      Left = 88
      Top = 176
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = btnDownloadCancelClick
    end
    object Panel2: TPanel
      Left = 32
      Top = 80
      Width = 169
      Height = 25
      BevelOuter = bvNone
      TabOrder = 3
      object Label1: TLabel
        Left = 40
        Top = 6
        Width = 13
        Height = 15
        Caption = #50900
      end
      object Label2: TLabel
        Left = 96
        Top = 6
        Width = 13
        Height = 15
        Caption = #51068
      end
      object Label3: TLabel
        Left = 147
        Top = 6
        Width = 13
        Height = 15
        Caption = #49884
      end
      object edtMonth: TEdit
        Left = 8
        Top = 2
        Width = 25
        Height = 23
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        TabOrder = 0
        Text = '10'
        OnMouseDown = edtMonthMouseDown
      end
      object edtDay: TEdit
        Left = 64
        Top = 2
        Width = 25
        Height = 23
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        TabOrder = 1
        Text = '10'
        OnMouseDown = edtDayMouseDown
      end
      object edtHour: TEdit
        Left = 120
        Top = 2
        Width = 25
        Height = 23
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        TabOrder = 2
        Text = '13'
        OnMouseDown = edtHourMouseDown
      end
    end
    object edtSpecifyMonth: TEdit
      Left = 40
      Top = 112
      Width = 25
      Height = 23
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 4
    end
    object edtSpecifyDay: TEdit
      Left = 96
      Top = 112
      Width = 25
      Height = 23
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      TabOrder = 5
    end
    object chkHour0: TCheckBox
      Left = 213
      Top = 8
      Width = 97
      Height = 17
      Caption = '00'
      TabOrder = 6
    end
    object chkHour1: TCheckBox
      Tag = 1
      Left = 213
      Top = 24
      Width = 97
      Height = 17
      Caption = '01'
      TabOrder = 7
    end
    object chkHour2: TCheckBox
      Tag = 2
      Left = 213
      Top = 40
      Width = 97
      Height = 17
      Caption = '02'
      TabOrder = 8
    end
    object chkHour3: TCheckBox
      Tag = 3
      Left = 213
      Top = 56
      Width = 97
      Height = 17
      Caption = '03'
      TabOrder = 9
    end
    object chkHour4: TCheckBox
      Tag = 4
      Left = 213
      Top = 72
      Width = 97
      Height = 17
      Caption = '04'
      TabOrder = 10
    end
    object chkHour5: TCheckBox
      Tag = 5
      Left = 213
      Top = 88
      Width = 97
      Height = 17
      Caption = '05'
      TabOrder = 11
    end
    object chkHour6: TCheckBox
      Tag = 6
      Left = 213
      Top = 104
      Width = 97
      Height = 17
      Caption = '06'
      TabOrder = 12
    end
    object chkHour7: TCheckBox
      Tag = 7
      Left = 213
      Top = 120
      Width = 97
      Height = 17
      Caption = '07'
      TabOrder = 13
    end
    object chkHour8: TCheckBox
      Tag = 8
      Left = 213
      Top = 136
      Width = 97
      Height = 17
      Caption = '08'
      TabOrder = 14
    end
    object chkHour9: TCheckBox
      Tag = 9
      Left = 213
      Top = 152
      Width = 97
      Height = 17
      Caption = '09'
      TabOrder = 15
    end
    object chkHour10: TCheckBox
      Tag = 10
      Left = 213
      Top = 168
      Width = 97
      Height = 17
      Caption = '10'
      TabOrder = 16
    end
    object chkHour11: TCheckBox
      Tag = 11
      Left = 213
      Top = 184
      Width = 97
      Height = 17
      Caption = '11'
      TabOrder = 17
    end
    object chkHour12: TCheckBox
      Tag = 12
      Left = 261
      Top = 8
      Width = 97
      Height = 17
      Caption = '12'
      TabOrder = 18
    end
    object chkHour13: TCheckBox
      Tag = 13
      Left = 261
      Top = 24
      Width = 97
      Height = 17
      Caption = '13'
      TabOrder = 19
    end
    object chkHour14: TCheckBox
      Tag = 14
      Left = 261
      Top = 40
      Width = 97
      Height = 17
      Caption = '14'
      TabOrder = 20
    end
    object chkHour15: TCheckBox
      Tag = 15
      Left = 261
      Top = 56
      Width = 97
      Height = 17
      Caption = '15'
      TabOrder = 21
    end
    object chkHour16: TCheckBox
      Tag = 16
      Left = 261
      Top = 72
      Width = 97
      Height = 17
      Caption = '16'
      TabOrder = 22
    end
    object chkHour17: TCheckBox
      Tag = 17
      Left = 261
      Top = 88
      Width = 97
      Height = 17
      Caption = '17'
      TabOrder = 23
    end
    object chkHour18: TCheckBox
      Tag = 18
      Left = 261
      Top = 104
      Width = 97
      Height = 17
      Caption = '18'
      TabOrder = 24
    end
    object chkHour19: TCheckBox
      Tag = 19
      Left = 261
      Top = 120
      Width = 97
      Height = 17
      Caption = '19'
      TabOrder = 25
    end
    object chkHour20: TCheckBox
      Tag = 20
      Left = 261
      Top = 136
      Width = 97
      Height = 17
      Caption = '20'
      TabOrder = 26
    end
    object chkHour21: TCheckBox
      Tag = 21
      Left = 261
      Top = 152
      Width = 97
      Height = 17
      Caption = '21'
      TabOrder = 27
    end
    object chkHour22: TCheckBox
      Tag = 22
      Left = 261
      Top = 168
      Width = 97
      Height = 17
      Caption = '22'
      TabOrder = 28
    end
    object chkHour23: TCheckBox
      Tag = 23
      Left = 261
      Top = 184
      Width = 97
      Height = 17
      Caption = '23'
      TabOrder = 29
    end
  end
end
