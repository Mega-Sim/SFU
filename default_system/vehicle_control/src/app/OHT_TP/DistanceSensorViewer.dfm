object DistanceSensorViewrForm: TDistanceSensorViewrForm
  Left = 0
  Top = 0
  BiDiMode = bdLeftToRight
  Caption = #44144#47532' '#49468#49436' '#48624#50612
  ClientHeight = 605
  ClientWidth = 855
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = menuDistanceSensorViewer
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poDesigned
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object paintBoxDistanceSensor: TPaintBox
    Left = 0
    Top = 137
    Width = 855
    Height = 468
    Align = alClient
    OnPaint = paintBoxDistanceSensorPaint
    ExplicitLeft = -8
    ExplicitTop = 249
    ExplicitWidth = 753
    ExplicitHeight = 136
  end
  object panelDistanceSensorViewer: TPanel
    Left = 0
    Top = 0
    Width = 855
    Height = 137
    Align = alTop
    TabOrder = 0
    object gbSelectDistanceSensor: TRadioGroup
      Left = 0
      Top = 2
      Width = 185
      Height = 129
      Caption = #49468#49436' '#49440#53469
      ItemIndex = 0
      Items.Strings = (
        'Disable'
        #47337#45796#50868' '#49468#49436' (PBS)'
        #45824#52264#44048#51648' '#49468#49436' (UBG)'
        #51109#50528#47932' '#44048#51648' '#49468#49436' (OBS)'
        'FIne '#45824#52264'(UBG)'
        'SOSLAB '#45824#52264#44048#51648#49468#49436'(UBG)'
        'SOSLAB '#51109#50528#47932#49468#49436'(OBS)')
      TabOrder = 0
      OnClick = gbSelectDistanceSensorClick
    end
    object gbDistanceDetectInfo: TGroupBox
      Left = 191
      Top = 4
      Width = 178
      Height = 127
      Caption = #51109#50528#47932' '#44160#52636' '#51221#48372
      TabOrder = 1
      object txtMinDist: TLabel
        Left = 10
        Top = 18
        Width = 26
        Height = 13
        Caption = #44144#47532':'
      end
      object txtMinDegree: TLabel
        Left = 10
        Top = 39
        Width = 26
        Height = 13
        Caption = #44033#46020':'
      end
      object txtPaternInfo: TLabel
        Left = 11
        Top = 58
        Width = 26
        Height = 13
        Caption = #54056#53556':'
      end
    end
    object memoDebug: TMemo
      Left = 375
      Top = 6
      Width = 274
      Height = 125
      ImeName = 'Microsoft IME 2010'
      Lines.Strings = (
        'memoDebug')
      TabOrder = 2
      Visible = False
    end
  end
  object timerDistanceDataUpdate: TTimer
    Enabled = False
    Interval = 500
    OnTimer = timerDistanceDataUpdateTimer
    Left = 584
    Top = 32
  end
  object menuDistanceSensorViewer: TMainMenu
    BiDiMode = bdLeftToRight
    ParentBiDiMode = False
    Left = 480
    object N1: TMenuItem
      Caption = 'Read Area File'
      object mnReadObsFile: TMenuItem
        Caption = 'Read OBS area file ('#51109#50528#47932' '#49468#49436')'
        OnClick = mnReadObsFileClick
      end
      object mnReadUbgFile: TMenuItem
        Caption = 'Read UBG area file ('#45824#52264' '#49468#49436')'
        OnClick = mnReadUbgFileClick
      end
      object mnReadPbsFile: TMenuItem
        Caption = 'Read PBS area file ('#47337#45796#50868' '#49468#49436')'
        OnClick = mnReadPbsFileClick
      end
    end
  end
  object OpenDialogDistSenAreaFile: TOpenDialog
    Ctl3D = False
    Left = 336
    Top = 8
  end
end
