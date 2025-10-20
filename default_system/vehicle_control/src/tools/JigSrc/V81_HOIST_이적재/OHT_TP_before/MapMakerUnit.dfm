object MapMakerForm: TMapMakerForm
  Left = 636
  Top = 280
  Caption = 'MapMaker'
  ClientHeight = 498
  ClientWidth = 629
  Color = 15658734
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Arial'
  Font.Style = [fsBold]
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 15
  object memoMapMaker: TMemo
    Left = 7
    Top = 159
    Width = 618
    Height = 331
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #44404#47548#52404
    Font.Style = [fsBold]
    ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
    ParentFont = False
    PopupMenu = pmMemo
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object pgcMapMake: TPageControl
    Left = 8
    Top = 8
    Width = 617
    Height = 145
    ActivePage = MapMake
    Align = alCustom
    TabOrder = 1
    object MapMake: TTabSheet
      Caption = 'MapMake'
      ExplicitWidth = 645
      object shpCycle: TShape
        Left = 83
        Top = 12
        Width = 43
        Height = 23
      end
      object Label11: TLabel
        Left = 90
        Top = 16
        Width = 31
        Height = 15
        Caption = 'Cycle'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object ShpIsTargetNode: TShape
        Left = 127
        Top = 12
        Width = 43
        Height = 23
      end
      object Label13: TLabel
        Left = 129
        Top = 16
        Width = 39
        Height = 15
        Caption = 'T-Node'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object ShaAdjustMap: TShape
        Left = 171
        Top = 12
        Width = 76
        Height = 23
      end
      object Label9: TLabel
        Left = 178
        Top = 16
        Width = 60
        Height = 15
        Caption = 'AdjustMap'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object shpDrivingMarked: TShape
        Left = 249
        Top = 12
        Width = 43
        Height = 23
      end
      object Label15: TLabel
        Left = 256
        Top = 16
        Width = 29
        Height = 15
        Caption = 'Mark'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object shpCIDPause: TShape
        Left = 295
        Top = 12
        Width = 43
        Height = 23
      end
      object Label17: TLabel
        Left = 302
        Top = 16
        Width = 31
        Height = 15
        Caption = 'CID-P'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label87: TLabel
        Left = 19
        Top = 65
        Width = 219
        Height = 15
        Caption = 'Tartget Node('#52572#51333' '#46020#52265' '#45432#46300','#51452#54665#45432#46300')'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object edtMoveEnd: TEdit
        Left = 257
        Top = 62
        Width = 91
        Height = 23
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2010'
        MaxLength = 7
        NumbersOnly = True
        ParentFont = False
        TabOrder = 0
        Text = '0'
      end
      object btnMapMake: TButton
        Left = 493
        Top = 16
        Width = 105
        Height = 50
        Hint = 
          '[MapMake '#44032#45733' '#51312#44148']'#13#10'Cycle Off'#13#10'T-Node Off'#13#10'AdjustMap('#51312#51221#50857'Map) On'#13#10'Dr' +
          'iving Mark On'#13#10'CID-P Off'
        Caption = 'MapMake '#51652#54665
        TabOrder = 1
        OnClick = btnMapMakeClick
      end
      object btnSolution: TButton
        Left = 493
        Top = 80
        Width = 105
        Height = 25
        Caption = #51312#52824#48169#48277
        TabOrder = 2
        OnClick = btnSolutionClick
      end
    end
    object 주행검증: TTabSheet
      Caption = #51452#54665#44160#51613
      ImageIndex = 1
      ExplicitWidth = 615
      object Label12: TLabel
        Left = 3
        Top = 70
        Width = 159
        Height = 15
        Caption = #51452#54665#44160#51613' '#51652#54665#46108' Link '#44079#49688' : '
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabMapMakeMoveCount: TLabel
        Left = 176
        Top = 70
        Width = 7
        Height = 15
        Caption = '0'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabMapMakeCount: TLabel
        Left = 176
        Top = 49
        Width = 7
        Height = 15
        Caption = '0'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label3: TLabel
        Left = 3
        Top = 49
        Width = 159
        Height = 15
        Caption = 'MapMake '#51652#54665#46108'Link '#44079#49688' : '
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label8: TLabel
        Left = 5
        Top = 91
        Width = 122
        Height = 17
        Caption = #51452#54665' '#53580#49828#53944' '#51652#54665#47456' :'
        Font.Charset = HANGEUL_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object shpCIDPause2: TShape
        Left = 327
        Top = 12
        Width = 43
        Height = 23
      end
      object Label18: TLabel
        Left = 334
        Top = 16
        Width = 31
        Height = 15
        Caption = 'CID-P'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object shpDrivingMarked2: TShape
        Left = 281
        Top = 12
        Width = 43
        Height = 23
      end
      object Label16: TLabel
        Left = 288
        Top = 16
        Width = 29
        Height = 15
        Caption = 'Mark'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object ShaAdjustMapModify: TShape
        Left = 204
        Top = 12
        Width = 76
        Height = 23
      end
      object Label10: TLabel
        Left = 211
        Top = 16
        Width = 65
        Height = 15
        Caption = 'IsMapMake'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object ShaAdjustMap2: TShape
        Left = 127
        Top = 12
        Width = 76
        Height = 23
      end
      object Label6: TLabel
        Left = 134
        Top = 16
        Width = 60
        Height = 15
        Caption = 'AdjustMap'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object shpCycle2: TShape
        Left = 83
        Top = 12
        Width = 43
        Height = 23
      end
      object Label5: TLabel
        Left = 90
        Top = 16
        Width = 31
        Height = 15
        Caption = 'Cycle'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object lblpercent: TLabel
        Left = 576
        Top = 92
        Width = 16
        Height = 15
        Caption = '0%'
      end
      object ProgressBar1: TProgressBar
        Left = 135
        Top = 91
        Width = 426
        Height = 17
        TabOrder = 0
      end
      object BtnMapMakeMoveCheck: TButton
        Left = 402
        Top = 12
        Width = 105
        Height = 65
        Hint = 
          '[MapMake '#44032#45733' '#51312#44148']'#13#10'Cycle Off'#13#10'AdjustMap('#51312#51221#50857'Map) On'#13#10'IsMapMake On'#13#10 +
          'Driving Mark On'#13#10'CID-P Off'
        Caption = #44160#51613' Trigger'
        TabOrder = 1
        OnClick = BtnMapMakeMoveCheckClick
      end
      object Button1: TButton
        Left = 509
        Top = 12
        Width = 105
        Height = 65
        Hint = #48120#51652#54665' Node '#51652#54665' '#49692#49436' '#45824#48708' Max 10'#44060#47484' '#48372#50668#51456#45796
        Caption = #48120#51652#54665' Node'
        TabOrder = 2
      end
    end
    object E305_Link거리수정: TTabSheet
      Caption = 'E305_Link'#44144#47532#49688#51221
      ImageIndex = 2
      ExplicitWidth = 615
      object Label2: TLabel
        Left = 14
        Top = 24
        Width = 95
        Height = 15
        Caption = 'Link'#51032' '#49884#51089' '#45432#46300
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label4: TLabel
        Left = 141
        Top = 26
        Width = 95
        Height = 15
        Caption = 'Link'#51032' '#51333#47308' '#45432#46300
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label1: TLabel
        Left = 299
        Top = 24
        Width = 55
        Height = 15
        Caption = #48320#44221' '#44144#47532
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object edtStartNodeID: TEdit
        Left = 14
        Top = 45
        Width = 101
        Height = 23
        ImeName = 'Microsoft IME 2003'
        NumbersOnly = True
        TabOrder = 0
        Text = '0'
      end
      object edtEndNodeID: TEdit
        Left = 141
        Top = 47
        Width = 101
        Height = 23
        ImeName = 'Microsoft IME 2003'
        NumbersOnly = True
        TabOrder = 1
        Text = '0'
      end
      object edtMapMakerDist: TEdit
        Left = 269
        Top = 45
        Width = 121
        Height = 23
        ImeName = 'Microsoft IME 2003'
        NumbersOnly = True
        TabOrder = 2
        Text = '0'
      end
      object btnReq: TButton
        Left = 402
        Top = 24
        Width = 105
        Height = 49
        Caption = #54788#51116' '#44144#47532' '#50836#52397
        TabOrder = 3
        OnClick = btnReqClick
      end
      object btnAutoMapStart: TButton
        Left = 513
        Top = 24
        Width = 97
        Height = 49
        Caption = #44144#47532' '#48320#44221' '#50836#52397
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        OnClick = btnAutoMapStartClick
      end
    end
  end
  object pmMemo: TPopupMenu
    Left = 592
    Top = 160
    object mniMemoClear: TMenuItem
      Caption = '&Clear'
      OnClick = mniMemoClearClick
    end
  end
  object tmrUI: TTimer
    Interval = 200
    OnTimer = tmrUITimer
    Left = 536
    Top = 160
  end
end
