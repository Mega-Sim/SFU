object HoistParamForm: THoistParamForm
  Left = 848
  Top = 127
  Caption = 'HoistParamForm'
  ClientHeight = 523
  ClientWidth = 517
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 505
    Height = 417
    Caption = ' HOIST Value Set '
    TabOrder = 0
    object Label2: TLabel
      Left = 27
      Top = 90
      Width = 71
      Height = 13
      Caption = #44592#51456' Pulse '#44050' '
    end
    object Label3: TLabel
      Left = 118
      Top = 90
      Width = 58
      Height = 13
      Caption = #48320#54872#44050'(mm)'
    end
    object Label1: TLabel
      Left = 13
      Top = 38
      Width = 99
      Height = 13
      Hint = 'Teaching'#51012' '#54616#45716' Master OHT'#51032' 0mm Pulse'#51221#48372
      Caption = 'Mater OHT(mm'#45800#50948')'
      ParentShowHint = False
      ShowHint = True
    end
    object Label4: TLabel
      Left = 206
      Top = 70
      Width = 58
      Height = 13
      Caption = #52769#51221#44050'(mm)'
    end
    object Label5: TLabel
      Left = 228
      Top = 16
      Width = 60
      Height = 13
      Caption = #44592#51456#54840#44592#44050
    end
    object Label6: TLabel
      Left = 155
      Top = 16
      Width = 36
      Height = 13
      Caption = #48320#54872#44050
    end
    object edtPulse_0: TEdit
      Left = 11
      Top = 118
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 0
    end
    object edtPosMM_0: TEdit
      Left = 107
      Top = 118
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 1
    end
    object btnMove_0: TButton
      Left = 279
      Top = 114
      Width = 65
      Height = 25
      Caption = 'Move 0'
      TabOrder = 2
      OnClick = btnMove_0Click
    end
    object edtPulse_1: TEdit
      Left = 11
      Top = 147
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 3
    end
    object btnMove_1: TButton
      Left = 279
      Top = 145
      Width = 65
      Height = 25
      Caption = 'Move 1'
      TabOrder = 4
      OnClick = btnMove_1Click
    end
    object edtPosMM_1: TEdit
      Left = 107
      Top = 147
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 5
    end
    object edtPulse_2: TEdit
      Left = 11
      Top = 176
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 6
    end
    object btnMove_2: TButton
      Left = 279
      Top = 174
      Width = 65
      Height = 25
      Caption = 'Move 2'
      TabOrder = 7
      OnClick = btnMove_2Click
    end
    object edtPosMM_2: TEdit
      Left = 107
      Top = 176
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 8
    end
    object edtPulse_3: TEdit
      Left = 11
      Top = 205
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 9
    end
    object btnMove_3: TButton
      Left = 279
      Top = 203
      Width = 65
      Height = 25
      Caption = 'Move 3'
      TabOrder = 10
      OnClick = btnMove_3Click
    end
    object edtPosMM_3: TEdit
      Left = 107
      Top = 205
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 11
    end
    object edtPulse_4: TEdit
      Left = 11
      Top = 234
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 12
    end
    object btnMove_4: TButton
      Left = 279
      Top = 232
      Width = 65
      Height = 25
      Caption = 'Move 4'
      TabOrder = 13
      OnClick = btnMove_4Click
    end
    object edtPosMM_4: TEdit
      Left = 107
      Top = 234
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 14
    end
    object edtPulse_5: TEdit
      Left = 11
      Top = 263
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 15
    end
    object btnMove_5: TButton
      Left = 279
      Top = 261
      Width = 65
      Height = 25
      Caption = 'Move 5'
      TabOrder = 16
      OnClick = btnMove_5Click
    end
    object edtPosMM_5: TEdit
      Left = 107
      Top = 263
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 17
    end
    object edtPulse_6: TEdit
      Left = 11
      Top = 292
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 18
    end
    object btnMove_6: TButton
      Left = 279
      Top = 290
      Width = 65
      Height = 25
      Caption = 'Move 6'
      TabOrder = 19
      OnClick = btnMove_6Click
    end
    object edtPosMM_6: TEdit
      Left = 107
      Top = 292
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 20
    end
    object edtPulse_7: TEdit
      Left = 11
      Top = 321
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 21
    end
    object btnMove_7: TButton
      Left = 279
      Top = 319
      Width = 65
      Height = 25
      Caption = 'Move 7'
      TabOrder = 22
      OnClick = btnMove_7Click
    end
    object edtPosMM_7: TEdit
      Left = 107
      Top = 321
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 23
    end
    object btnMove_8: TButton
      Left = 279
      Top = 348
      Width = 65
      Height = 25
      Caption = 'Move 8'
      TabOrder = 24
      OnClick = btnMove_8Click
    end
    object edtPosMM_8: TEdit
      Left = 107
      Top = 350
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 25
    end
    object edtPulse_8: TEdit
      Left = 11
      Top = 350
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 26
    end
    object btnMove_9: TButton
      Left = 279
      Top = 377
      Width = 65
      Height = 25
      Caption = 'Move 9'
      TabOrder = 27
      OnClick = btnMove_9Click
    end
    object edtPosMM_9: TEdit
      Left = 107
      Top = 379
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 28
    end
    object edtPulse_9: TEdit
      Left = 11
      Top = 379
      Width = 90
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 29
    end
    object btnMove_zero: TButton
      Left = 279
      Top = 85
      Width = 65
      Height = 25
      Caption = '0 pulse'
      TabOrder = 30
      OnClick = btnMove_0Click
    end
    object edtRealMM_master: TEdit
      Left = 220
      Top = 35
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 31
      OnKeyDown = edtRealMM_masterKeyDown
    end
    object edtRealMM_zero: TEdit
      Left = 193
      Top = 89
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 32
    end
    object edtRealMM_0: TEdit
      Left = 193
      Top = 118
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 33
    end
    object edtRealMM_1: TEdit
      Left = 193
      Top = 147
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 34
    end
    object edtRealMM_2: TEdit
      Left = 193
      Top = 176
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 35
    end
    object edtRealMM_3: TEdit
      Left = 193
      Top = 205
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 36
    end
    object edtRealMM_4: TEdit
      Left = 193
      Top = 234
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 37
    end
    object edtRealMM_5: TEdit
      Left = 193
      Top = 263
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 38
    end
    object edtRealMM_6: TEdit
      Left = 193
      Top = 292
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 39
    end
    object edtRealMM_7: TEdit
      Left = 193
      Top = 321
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 40
    end
    object edtRealMM_8: TEdit
      Left = 193
      Top = 350
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 41
    end
    object edtRealMM_9: TEdit
      Left = 193
      Top = 379
      Width = 80
      Height = 21
      ImeName = 'Microsoft IME 2010'
      NumbersOnly = True
      TabOrder = 42
    end
    object edtPosMM_master: TEdit
      Left = 134
      Top = 35
      Width = 80
      Height = 21
      Color = clBtnShadow
      ImeName = 'Microsoft IME 2010'
      ReadOnly = True
      TabOrder = 43
    end
    object btnConfirm_zero: TButton
      Left = 366
      Top = 85
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 44
      OnClick = btnConfirm_zeroClick
    end
    object btnConfirm_0: TButton
      Left = 366
      Top = 114
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 45
      OnClick = btnConfirm_0Click
    end
    object btnConfirm_1: TButton
      Left = 366
      Top = 145
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 46
      OnClick = btnConfirm_1Click
    end
    object btnConfirm_2: TButton
      Left = 366
      Top = 174
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 47
      OnClick = btnConfirm_2Click
    end
    object btnConfirm_3: TButton
      Left = 366
      Top = 203
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 48
      OnClick = btnConfirm_3Click
    end
    object btnConfirm_4: TButton
      Left = 366
      Top = 232
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 49
      OnClick = btnConfirm_4Click
    end
    object btnConfirm_5: TButton
      Left = 366
      Top = 261
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 50
      OnClick = btnConfirm_5Click
    end
    object btnConfirm_6: TButton
      Left = 366
      Top = 290
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 51
      OnClick = btnConfirm_6Click
    end
    object btnConfirm_7: TButton
      Left = 366
      Top = 319
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 52
      OnClick = btnConfirm_7Click
    end
    object btnConfirm_8: TButton
      Left = 366
      Top = 348
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 53
      OnClick = btnConfirm_8Click
    end
    object btnConfirm_9: TButton
      Left = 366
      Top = 379
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 54
      OnClick = btnConfirm_9Click
    end
    object btnCancel_zero: TButton
      Left = 437
      Top = 85
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 55
      OnClick = btnCancel_zeroClick
    end
    object btnCancel_0: TButton
      Left = 437
      Top = 114
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 56
      OnClick = btnCancel_0Click
    end
    object btnCancel_1: TButton
      Left = 437
      Top = 145
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 57
      OnClick = btnCancel_1Click
    end
    object btnCancel_2: TButton
      Left = 437
      Top = 174
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 58
      OnClick = btnCancel_2Click
    end
    object btnCancel_3: TButton
      Left = 437
      Top = 203
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 59
      OnClick = btnCancel_3Click
    end
    object btnCancel_4: TButton
      Left = 437
      Top = 232
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 60
      OnClick = btnCancel_4Click
    end
    object btnCancel_5: TButton
      Left = 437
      Top = 261
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 61
      OnClick = btnCancel_5Click
    end
    object btnCancel_6: TButton
      Left = 437
      Top = 290
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 62
      OnClick = btnCancel_6Click
    end
    object btnCancel_7: TButton
      Left = 437
      Top = 319
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 63
      OnClick = btnCancel_7Click
    end
    object btnCancel_8: TButton
      Left = 437
      Top = 348
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 64
      OnClick = btnCancel_8Click
    end
    object btnCancel_9: TButton
      Left = 437
      Top = 379
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 65
      OnClick = btnCancel_9Click
    end
    object btnConfirm_master: TButton
      Left = 366
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Confirm'
      TabOrder = 66
      OnClick = btnConfirm_masterClick
    end
    object btnCancel_master: TButton
      Left = 432
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Cancel'
      TabOrder = 67
      OnClick = btnCancel_masterClick
    end
  end
  object btnParamLoad: TButton
    Left = 47
    Top = 450
    Width = 166
    Height = 49
    Caption = 'OHT Param '#51069#50612#50724#44592
    TabOrder = 1
    OnClick = btnParamLoadClick
  end
  object btnParamSave: TButton
    Left = 308
    Top = 450
    Width = 175
    Height = 49
    Caption = 'OHT Param '#50640' '#51200#51109
    TabOrder = 2
    OnClick = btnParamSaveClick
  end
end
