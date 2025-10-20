object SysInfoForm: TSysInfoForm
  Left = 573
  Top = 84
  Caption = 'SysInfoForm'
  ClientHeight = 529
  ClientWidth = 443
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object panMain: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 529
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object grpTimeInformation: TGroupBox
      Left = 2
      Top = 417
      Width = 439
      Height = 110
      Align = alClient
      Caption = ' Time Information '
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlue
      Font.Height = -17
      Font.Name = #47569#51008' '#44256#46357
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      object lbOHT: TLabel
        Left = 26
        Top = 32
        Width = 36
        Height = 17
        Caption = 'OHT :'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object labCapacity1: TLabel
        Left = 136
        Top = 96
        Width = 6
        Height = 23
      end
      object lnTP: TLabel
        Left = 26
        Top = 56
        Width = 29
        Height = 17
        Caption = 'TP  :'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
      end
      object labOHTTime: TLabel
        Left = 56
        Top = 16
        Width = 7
        Height = 13
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
      end
      object labTPTime: TLabel
        Left = 56
        Top = 32
        Width = 7
        Height = 13
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
      end
      object lbOHTtime: TLabel
        Left = 69
        Top = 32
        Width = 62
        Height = 17
        Caption = 'lbOHTtime'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object lbTPtime: TLabel
        Left = 69
        Top = 56
        Width = 50
        Height = 17
        Caption = 'lbTPtime'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
      end
      object btnChangeOHTTime: TButton
        Left = 305
        Top = 21
        Width = 123
        Height = 57
        Caption = 'Get OHT Time'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnClick = btnChangeOHTTimeClick
      end
    end
    object grpOHTDiskInformation: TGroupBox
      Left = 2
      Top = 2
      Width = 439
      Height = 415
      Align = alTop
      Caption = ' OHT LocalDisk Properties '
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlue
      Font.Height = -17
      Font.Name = #47569#51008' '#44256#46357
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      object btnReboot: TButton
        Left = 291
        Top = 312
        Width = 81
        Height = 25
        Caption = 'OHT Reboot'
        Style = bsSplitButton
        TabOrder = 0
        OnClick = btnRebootClick
      end
      object grpDisk_System: TGroupBox
        Left = 2
        Top = 25
        Width = 435
        Height = 193
        Align = alTop
        Caption = 'System Disk'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object chartDisk_System: TChart
          Left = 16
          Top = 16
          Width = 209
          Height = 171
          Legend.Visible = False
          SubFoot.CustomPosition = True
          SubFoot.Font.Color = -1
          SubFoot.Font.Style = []
          SubFoot.Left = 85
          SubFoot.Top = 150
          Title.CustomPosition = True
          Title.Font.Color = clBlack
          Title.Font.Height = -13
          Title.Font.Name = #47569#51008' '#44256#46357
          Title.Font.Style = [fsBold]
          Title.Font.Shadow.SmoothBlur = 10
          Title.Left = 85
          Title.Text.Strings = (
            'Capacity')
          Title.Top = 5
          ClipPoints = False
          View3DOptions.Elevation = 315
          View3DOptions.Orthogonal = False
          View3DOptions.Perspective = 0
          View3DOptions.Rotation = 360
          BevelOuter = bvNone
          TabOrder = 0
          ColorPaletteIndex = 13
          object psrsDiskSpace: TPieSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            XValues.Order = loAscending
            YValues.Name = 'Pie'
            YValues.Order = loNone
            CustomXRadius = 55
            Emboss.Visible = True
            Gradient.Direction = gdRadial
            Gradient.EndColor = 6144242
            Gradient.StartColor = 4565210
            Gradient.Visible = True
            GradientBright = -24
            OtherSlice.Legend.Visible = False
            PieValues.Name = 'Pie'
            PieValues.Order = loNone
            RotationAngle = 270
            Frame.InnerBrush.BackColor = clRed
            Frame.InnerBrush.Gradient.EndColor = clGray
            Frame.InnerBrush.Gradient.MidColor = clWhite
            Frame.InnerBrush.Gradient.StartColor = 4210752
            Frame.InnerBrush.Gradient.Visible = True
            Frame.MiddleBrush.BackColor = clYellow
            Frame.MiddleBrush.Gradient.EndColor = 8553090
            Frame.MiddleBrush.Gradient.MidColor = clWhite
            Frame.MiddleBrush.Gradient.StartColor = clGray
            Frame.MiddleBrush.Gradient.Visible = True
            Frame.OuterBrush.BackColor = clGreen
            Frame.OuterBrush.Gradient.EndColor = 4210752
            Frame.OuterBrush.Gradient.MidColor = clWhite
            Frame.OuterBrush.Gradient.StartColor = clSilver
            Frame.OuterBrush.Gradient.Visible = True
            Frame.ParentSeries = psrsDiskSpace
            Frame.Visible = False
            Frame.Width = 4
            Data = {
              04080000000000000000808540FF04000000436172730000000000608840FF06
              00000050686F6E65730000000000F08C40FF060000005461626C657300000000
              00206740FF080000004D6F6E69746F72730000000000F88C40FF050000004C61
              6D70730000000000606440FF090000004B6579626F6172647300000000000085
              40FF0500000042696B65730000000000708240FF06000000436861697273}
          end
        end
        object panDisk_System: TPanel
          Left = 251
          Top = 65
          Width = 175
          Height = 81
          Color = clGradientInactiveCaption
          ParentBackground = False
          TabOrder = 1
          object lbFree: TLabel
            Left = 14
            Top = 33
            Width = 74
            Height = 17
            Caption = 'Free Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbFreeSpace: TLabel
            Left = 94
            Top = 32
            Width = 73
            Height = 17
            Caption = 'lbFreeSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clFuchsia
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbTotal: TLabel
            Left = 8
            Top = 9
            Width = 80
            Height = 17
            Caption = 'Total Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbTotalSpace: TLabel
            Left = 94
            Top = 9
            Width = 79
            Height = 17
            Caption = 'lbTotalSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbUsed: TLabel
            Left = 9
            Top = 56
            Width = 79
            Height = 17
            Caption = 'Used Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbUsedSpace: TLabel
            Left = 94
            Top = 56
            Width = 78
            Height = 17
            Caption = 'lbUsedSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
        end
      end
      object grpDisk_Log: TGroupBox
        Left = 2
        Top = 218
        Width = 435
        Height = 195
        Align = alClient
        Caption = 'Log Disk'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = #47569#51008' '#44256#46357
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object chartDisk_Log: TChart
          Left = 16
          Top = 16
          Width = 209
          Height = 171
          Legend.Visible = False
          SubFoot.CustomPosition = True
          SubFoot.Font.Color = -1
          SubFoot.Font.Style = []
          SubFoot.Left = 85
          SubFoot.Top = 150
          Title.CustomPosition = True
          Title.Font.Color = clBlack
          Title.Font.Height = -13
          Title.Font.Name = #47569#51008' '#44256#46357
          Title.Font.Style = [fsBold]
          Title.Font.Shadow.SmoothBlur = 10
          Title.Left = 85
          Title.Text.Strings = (
            'Capacity')
          Title.Top = 5
          ClipPoints = False
          View3DOptions.Elevation = 315
          View3DOptions.Orthogonal = False
          View3DOptions.Perspective = 0
          View3DOptions.Rotation = 360
          BevelOuter = bvNone
          TabOrder = 0
          ColorPaletteIndex = 13
          object PieSeries1: TPieSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            XValues.Order = loAscending
            YValues.Name = 'Pie'
            YValues.Order = loNone
            CustomXRadius = 55
            Emboss.Visible = True
            Gradient.Direction = gdRadial
            Gradient.EndColor = 6144242
            Gradient.StartColor = 4565210
            Gradient.Visible = True
            GradientBright = -24
            OtherSlice.Legend.Visible = False
            PieValues.Name = 'Pie'
            PieValues.Order = loNone
            RotationAngle = 270
            Frame.InnerBrush.BackColor = clRed
            Frame.InnerBrush.Gradient.EndColor = clGray
            Frame.InnerBrush.Gradient.MidColor = clWhite
            Frame.InnerBrush.Gradient.StartColor = 4210752
            Frame.InnerBrush.Gradient.Visible = True
            Frame.MiddleBrush.BackColor = clYellow
            Frame.MiddleBrush.Gradient.EndColor = 8553090
            Frame.MiddleBrush.Gradient.MidColor = clWhite
            Frame.MiddleBrush.Gradient.StartColor = clGray
            Frame.MiddleBrush.Gradient.Visible = True
            Frame.OuterBrush.BackColor = clGreen
            Frame.OuterBrush.Gradient.EndColor = 4210752
            Frame.OuterBrush.Gradient.MidColor = clWhite
            Frame.OuterBrush.Gradient.StartColor = clSilver
            Frame.OuterBrush.Gradient.Visible = True
            Frame.ParentSeries = PieSeries1
            Frame.Visible = False
            Frame.Width = 4
            Data = {
              04080000000000000000808540FF04000000436172730000000000608840FF06
              00000050686F6E65730000000000F08C40FF060000005461626C657300000000
              00206740FF080000004D6F6E69746F72730000000000F88C40FF050000004C61
              6D70730000000000606440FF090000004B6579626F6172647300000000000085
              40FF0500000042696B65730000000000708240FF06000000436861697273}
          end
        end
        object panDisk_Log: TPanel
          Left = 251
          Top = 65
          Width = 175
          Height = 81
          Color = clGradientInactiveCaption
          ParentBackground = False
          TabOrder = 1
          object Label1: TLabel
            Left = 14
            Top = 33
            Width = 74
            Height = 17
            Caption = 'Free Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbLogFreeSpace: TLabel
            Left = 94
            Top = 32
            Width = 73
            Height = 17
            Caption = 'lbFreeSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clFuchsia
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label3: TLabel
            Left = 8
            Top = 9
            Width = 80
            Height = 17
            Caption = 'Total Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbLogTotalSpace: TLabel
            Left = 94
            Top = 9
            Width = 79
            Height = 17
            Caption = 'lbTotalSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label5: TLabel
            Left = 9
            Top = 56
            Width = 79
            Height = 17
            Caption = 'Used Space :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbLogUsedSpace: TLabel
            Left = 94
            Top = 56
            Width = 78
            Height = 17
            Caption = 'lbUsedSpace'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -13
            Font.Name = #47569#51008' '#44256#46357
            Font.Style = [fsBold]
            ParentFont = False
          end
        end
      end
    end
  end
  object MainMenu1: TMainMenu
    Left = 346
    Top = 18
    object Update1: TMenuItem
      Caption = 'Update'
      OnClick = Update1Click
    end
  end
  object tmrTime: TTimer
    Enabled = False
    OnTimer = tmrTimeTimer
    Left = 400
    Top = 24
  end
end
