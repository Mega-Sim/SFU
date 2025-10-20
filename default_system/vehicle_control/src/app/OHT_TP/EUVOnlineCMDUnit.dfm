object EUVOnlineCMDForm: TEUVOnlineCMDForm
  Left = 0
  Top = 0
  Caption = 'EUVOnlineCMDForm'
  ClientHeight = 785
  ClientWidth = 827
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  ScreenSnap = True
  SnapBuffer = 40
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object panBottom: TPanel
    Left = 0
    Top = 258
    Width = 827
    Height = 527
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = #48148#53461#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitHeight = 443
    object panFunc: TPanel
      Left = 1
      Top = 1
      Width = 825
      Height = 104
      Align = alTop
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object pgcMainControl: TPageControl
        Left = 2
        Top = 2
        Width = 821
        Height = 100
        ActivePage = ts1
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentFont = False
        ParentShowHint = False
        ShowHint = False
        TabOrder = 0
        OnDrawTab = pgcMainControlDrawTab
        object TabSheet3: TTabSheet
          Caption = 'Move Cycle'
          ImageIndex = 6
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object Label87: TLabel
            Left = 15
            Top = 7
            Width = 249
            Height = 15
            Caption = #51333#47308#45432#46300'(From Station Check'#49884', '#51333#47308'Station)'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object btnForceOpen: TButton
            Left = 689
            Top = 3
            Width = 121
            Height = 64
            Caption = #53685#44284#54728#44032' '#44053#51228#44060#48169
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
            OnClick = btnForceOpenClick
          end
          object edtMoveEnd: TEdit
            Left = 280
            Top = 3
            Width = 70
            Height = 23
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 1
            Text = '11'
          end
          object cbCheckGoStation: TCheckBox
            Left = 14
            Top = 24
            Width = 93
            Height = 17
            Caption = 'From Station'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 2
          end
          object btnMapBaseMove: TButton
            Left = 585
            Top = 3
            Width = 98
            Height = 64
            Caption = 'Map'#44592#48152' '#51060#46041
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 3
            OnClick = btnMapBaseMoveClick
          end
          object edtMoveResult: TEdit
            Left = 3
            Top = 44
            Width = 335
            Height = 23
            ImeName = 'Microsoft IME 2010'
            TabOrder = 4
          end
          object btnOCSMapBaseMove: TButton
            Left = 486
            Top = 3
            Width = 93
            Height = 64
            Caption = 'OCS'#44592#48152' '#51060#46041
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 5
            OnClick = btnOCSMapBaseMoveClick
          end
        end
        object ts1: TTabSheet
          Caption = 'Jog Control'
          ImageIndex = 6
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object Label6: TLabel
            Left = 8
            Top = 10
            Width = 101
            Height = 15
            Caption = #44032'/'#44048#49549#46020'[m/sec2]'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label7: TLabel
            Left = 8
            Top = 32
            Width = 97
            Height = 15
            Caption = 'Speed[cts/msec]'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label9: TLabel
            Left = 226
            Top = 34
            Width = 33
            Height = 15
            Caption = '[m/m]'
          end
          object Label42: TLabel
            Left = 226
            Top = 52
            Width = 29
            Height = 15
            Caption = '[m/s]'
          end
          object Label4: TLabel
            Left = 138
            Top = 54
            Width = 29
            Height = 15
            Caption = '[m/s]'
          end
          object edtAcceleration: TEdit
            Left = 120
            Top = 4
            Width = 45
            Height = 23
            Color = clYellow
            ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
            TabOrder = 0
            Text = '1.6'
          end
          object edtDeceleration: TEdit
            Left = 168
            Top = 4
            Width = 45
            Height = 23
            Color = clAqua
            ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
            TabOrder = 1
            Text = '1.6'
          end
          object edtSpeed: TEdit
            Left = 112
            Top = 28
            Width = 45
            Height = 23
            Color = clYellow
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
            ParentFont = False
            TabOrder = 2
            Text = '10'
            OnChange = edtSpeedChange
          end
          object UpDown2: TUpDown
            Left = 157
            Top = 28
            Width = 16
            Height = 23
            Associate = edtSpeed
            Max = 500
            Increment = 5
            Position = 10
            TabOrder = 3
            Thousands = False
          end
          object panMPerMin: TPanel
            Left = 176
            Top = 32
            Width = 49
            Height = 17
            BevelInner = bvLowered
            BevelOuter = bvNone
            Color = clInfoBk
            ParentBackground = False
            TabOrder = 4
          end
          object panCenter: TPanel
            Left = 176
            Top = 52
            Width = 49
            Height = 17
            Hint = 'OHT '#51473#49900#49549#46020'['#44257#49440']'
            BevelInner = bvLowered
            BevelOuter = bvNone
            Color = clSilver
            ParentBackground = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 5
          end
          object panMPerSec: TPanel
            Left = 88
            Top = 52
            Width = 49
            Height = 17
            BevelInner = bvLowered
            BevelOuter = bvNone
            Caption = '1.0'
            Color = clBlue
            Font.Charset = ANSI_CHARSET
            Font.Color = clYellow
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentBackground = False
            ParentFont = False
            TabOrder = 6
          end
          object chkDist: TCheckBox
            Left = 8
            Top = 48
            Width = 81
            Height = 17
            Caption = 'Distance'
            TabOrder = 7
            OnClick = chkDistClick
          end
          object pgcJog: TPageControl
            Left = 649
            Top = 0
            Width = 164
            Height = 70
            ActivePage = tabJog
            Align = alRight
            TabOrder = 8
            object tabJog: TTabSheet
              Caption = 'tabJog'
              TabVisible = False
              ExplicitLeft = 0
              ExplicitTop = 0
              ExplicitWidth = 0
              ExplicitHeight = 0
              object pan1: TPanel
                Left = 0
                Top = 0
                Width = 156
                Height = 60
                Align = alClient
                BevelInner = bvRaised
                BevelOuter = bvLowered
                TabOrder = 0
                object lbJog: TLabel
                  Left = 2
                  Top = 2
                  Width = 152
                  Height = 13
                  Align = alTop
                  Caption = '<-backward  forward->'
                  Font.Charset = ANSI_CHARSET
                  Font.Color = clBlue
                  Font.Height = -13
                  Font.Name = #48148#53461#52404
                  Font.Style = []
                  ParentFont = False
                  ExplicitWidth = 147
                end
                object pan2: TPanel
                  Left = 2
                  Top = 15
                  Width = 152
                  Height = 43
                  Align = alClient
                  BevelInner = bvLowered
                  Caption = 'Panel26'
                  TabOrder = 0
                  object memoJogKeyControl: TMemo
                    Left = 2
                    Top = 2
                    Width = 148
                    Height = 39
                    Align = alClient
                    BorderStyle = bsNone
                    Color = clMenuHighlight
                    ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
                    Lines.Strings = (
                      'Click.....This Area')
                    TabOrder = 0
                    OnClick = memoJogKeyControlClick
                    OnExit = memoJogKeyControlExit
                    OnKeyDown = memoJogKeyControlKeyDown
                    OnKeyUp = memoJogKeyControlKeyUp
                  end
                end
              end
            end
            object tabMoveDist: TTabSheet
              Caption = 'tabMoveDist'
              ImageIndex = 1
              TabVisible = False
              ExplicitLeft = 0
              ExplicitTop = 0
              ExplicitWidth = 0
              ExplicitHeight = 0
              object pan3: TPanel
                Left = 0
                Top = 0
                Width = 156
                Height = 60
                Align = alClient
                BevelInner = bvRaised
                BevelOuter = bvLowered
                TabOrder = 0
                object Label48: TLabel
                  Left = 52
                  Top = 8
                  Width = 30
                  Height = 15
                  Caption = '[mm]'
                end
                object Label49: TLabel
                  Left = 64
                  Top = 37
                  Width = 26
                  Height = 15
                  Caption = '[cts]'
                end
                object edtDistmm: TEdit
                  Left = 2
                  Top = 4
                  Width = 47
                  Height = 23
                  ImeName = 'Microsoft IME 2003'
                  TabOrder = 0
                  Text = '1000'
                end
                object edtDistcts: TEdit
                  Left = 2
                  Top = 32
                  Width = 60
                  Height = 23
                  ImeName = 'Microsoft IME 2003'
                  TabOrder = 1
                  Text = '1'
                end
                object btnMove: TButton
                  Left = 96
                  Top = 28
                  Width = 56
                  Height = 26
                  Caption = 'Move'
                  TabOrder = 2
                  OnClick = btnMoveClick
                end
                object panDist_M: TPanel
                  Left = 88
                  Top = 4
                  Width = 65
                  Height = 20
                  BevelInner = bvLowered
                  Caption = '0'
                  PopupMenu = pmUnload
                  TabOrder = 3
                end
              end
            end
          end
          object chbBackWard: TCheckBox
            Left = 552
            Top = 3
            Width = 81
            Height = 17
            Caption = 'BackWard'
            TabOrder = 9
            OnClick = chbBackWardClick
          end
          object chkEmergencyJog: TCheckBox
            Left = 508
            Top = 26
            Width = 125
            Height = 17
            Caption = #51652#51077#48520#44032#44396#44036#51060#46041
            TabOrder = 10
            OnClick = chkEmergencyJogClick
          end
        end
        object TabSheet2: TTabSheet
          Caption = 'Trans'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object panTrans: TPanel
            Left = 0
            Top = 0
            Width = 945
            Height = 70
            Align = alCustom
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = 15662830
            TabOrder = 0
            object lbLongrunCount: TLabel
              Left = 132
              Top = 49
              Width = 43
              Height = 15
              Caption = 'Count:0'
              Color = clTeal
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentColor = False
              ParentFont = False
            end
            object lbTransTT: TLabel
              Left = 219
              Top = 49
              Width = 54
              Height = 15
              Caption = 'T/T : 0 ms'
              Color = clTeal
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentColor = False
              ParentFont = False
            end
            object lbLongrunInterval: TLabel
              Left = 132
              Top = 28
              Width = 100
              Height = 15
              Caption = 'Longrun '#51452#44592'(ms)'
            end
            object btnLoad: TButton
              Left = 6
              Top = 8
              Width = 57
              Height = 57
              Caption = 'Load'
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              TabOrder = 2
              OnMouseDown = btnLoadMouseDown
            end
            object btnUnload: TButton
              Left = 69
              Top = 8
              Width = 57
              Height = 57
              Caption = 'Unload'
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              TabOrder = 3
              OnMouseDown = btnUnloadMouseDown
            end
            object chkLongRun: TCheckBox
              Left = 132
              Top = 5
              Width = 113
              Height = 17
              Caption = 'Trans LongRun'
              Color = clBtnFace
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentColor = False
              ParentFont = False
              TabOrder = 1
              OnClick = chkLongRunClick
            end
            object btnMotor2Home: TBitBtn
              Left = 408
              Top = 5
              Width = 86
              Height = 31
              Caption = '&#2HOME'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 0
              OnClick = btnMotor2HomeClick
            end
            object btnMotor34Home: TBitBtn
              Left = 408
              Top = 37
              Width = 86
              Height = 28
              Caption = '&#3,4HOME'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 5
              OnClick = btnMotor34HomeClick
            end
            object edtLongRunInterval: TEdit
              Left = 240
              Top = 24
              Width = 34
              Height = 23
              ImeName = 'Microsoft IME 2010'
              TabOrder = 4
              Text = '50'
            end
            object btnMoveTeachingPoint: TBitBtn
              Left = 280
              Top = 5
              Width = 124
              Height = 31
              Caption = '&Move T-Point'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 6
              OnMouseDown = btnMoveTeachingPointMouseDown
            end
            object btnHoistHomeAutoSet: TBitBtn
              Left = 280
              Top = 37
              Width = 124
              Height = 28
              Caption = '&#2HomeAutoSet'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 7
              Visible = False
              OnClick = btnHoistHomeAutoSetClick
            end
            object btnMoveAllHome: TBitBtn
              Left = 500
              Top = 5
              Width = 110
              Height = 31
              Caption = '&#ALL HOME'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 8
              OnClick = btnMoveAllHomeClick
            end
            object btnAutoChange: TBitBtn
              Left = 500
              Top = 37
              Width = 110
              Height = 28
              Caption = '&#Auto Change'
              DoubleBuffered = True
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              Kind = bkAll
              NumGlyphs = 2
              ParentDoubleBuffered = False
              ParentFont = False
              TabOrder = 9
              OnClick = btnAutoChangeClick
            end
          end
        end
        object TabSheet5: TTabSheet
          Caption = 'Power ON/OFF'
          ImageIndex = 4
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object PWGrid: TStringGrid
            Left = 0
            Top = 8
            Width = 169
            Height = 25
            Color = 16509438
            ColCount = 2
            DefaultColWidth = 140
            DefaultRowHeight = 20
            RowCount = 1
            FixedRows = 0
            GradientEndColor = clMoneyGreen
            TabOrder = 0
            OnDrawCell = PWGridDrawCell
          end
          object GroupBox7: TGroupBox
            Left = 170
            Top = -1
            Width = 82
            Height = 68
            Caption = ' Hand '
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 1
            object btnHaON: TBitBtn
              Left = 5
              Top = 16
              Width = 33
              Height = 25
              Caption = 'ON'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 0
              OnClick = btnHaONClick
            end
            object btnHaOFF: TBitBtn
              Left = 46
              Top = 16
              Width = 33
              Height = 25
              Caption = 'OFF'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 1
              OnClick = btnHaOFFClick
            end
            object btnHaRst: TBitBtn
              Left = 17
              Top = 41
              Width = 50
              Height = 25
              Caption = 'RESET'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 2
              OnClick = btnHaRstClick
            end
          end
          object GroupBox1: TGroupBox
            Left = 250
            Top = -1
            Width = 135
            Height = 73
            Caption = 'Steering'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 2
            object Label46: TLabel
              Left = 8
              Top = 24
              Width = 32
              Height = 15
              Caption = 'Front:'
            end
            object Label47: TLabel
              Left = 8
              Top = 45
              Width = 33
              Height = 15
              Caption = 'Rear :'
            end
            object btnFSteerON: TBitBtn
              Left = 52
              Top = 16
              Width = 33
              Height = 25
              Caption = 'ON'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 0
              OnClick = btnFSteerONClick
            end
            object btnFSteerOFF: TBitBtn
              Left = 92
              Top = 16
              Width = 33
              Height = 25
              Caption = 'OFF'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 1
              OnClick = btnFSteerOFFClick
            end
            object btnRSteerON: TBitBtn
              Left = 52
              Top = 44
              Width = 33
              Height = 25
              Caption = 'ON'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 2
              OnClick = btnRSteerONClick
            end
            object btnRSteerOFF: TBitBtn
              Left = 92
              Top = 44
              Width = 33
              Height = 25
              Caption = 'OFF'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 3
              OnClick = btnRSteerOFFClick
            end
          end
          object GroupBox6: TGroupBox
            Left = 392
            Top = 8
            Width = 113
            Height = 57
            Caption = 'Safety Reset'
            TabOrder = 3
            object btnMCControlOff: TBitBtn
              Left = 64
              Top = 24
              Width = 33
              Height = 25
              Caption = 'OFF'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 0
              OnClick = btnMCControlOffClick
            end
            object btnMCControlOn: TBitBtn
              Left = 12
              Top = 24
              Width = 33
              Height = 25
              Caption = 'ON'
              DoubleBuffered = True
              ParentDoubleBuffered = False
              TabOrder = 1
              OnClick = btnMCControlOnClick
            end
          end
          object panMC_Status: TPanel
            Left = 5
            Top = 45
            Width = 156
            Height = 17
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = 'SafetyReset'
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentBackground = False
            ParentFont = False
            TabOrder = 4
          end
        end
        object tabRotate: TTabSheet
          Caption = 'Rotate'
          ImageIndex = 4
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object panAxis4_Origin: TPanel
            Left = 2
            Top = 5
            Width = 10
            Height = 24
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            ParentBackground = False
            TabOrder = 0
          end
          object btnRotate_Orign: TButton
            Left = 16
            Top = 5
            Width = 75
            Height = 25
            Caption = 'Origin'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 1
            OnClick = btnRotate_OrignClick
          end
          object panAxis4_Enable: TPanel
            Left = 2
            Top = 40
            Width = 10
            Height = 24
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clWhite
            ParentBackground = False
            TabOrder = 2
          end
          object btnRotate_Enable: TButton
            Left = 16
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Enable'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 3
            OnClick = btnRotate_EnableClick
          end
          object btnRotate_Disable: TButton
            Left = 100
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Disable'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 4
            OnClick = btnRotate_DisableClick
          end
          object btnRotate_Stop: TBitBtn
            Left = 100
            Top = 5
            Width = 75
            Height = 25
            Caption = 'Stop'
            DoubleBuffered = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Kind = bkAbort
            NumGlyphs = 2
            ParentDoubleBuffered = False
            ParentFont = False
            TabOrder = 5
            OnClick = btnRotate_StopClick
          end
          object btnRotate_0: TButton
            Left = 200
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Rotate 0'#186
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 6
            OnClick = btnRotate_0Click
          end
          object btnRotate_90: TButton
            Left = 290
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Rotate 90'#186
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 7
            OnClick = btnRotate_90Click
          end
          object btnRotate_180: TButton
            Left = 380
            Top = 40
            Width = 75
            Height = 25
            Caption = 'Rotate 180'#186
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 8
            OnClick = btnRotate_180Click
          end
          object btnRotateMoveCmd: TButton
            Left = 799
            Top = 11
            Width = 110
            Height = 25
            Caption = 'Rotate MoveCmd'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 9
            OnClick = btnRotateMoveCmdClick
          end
          object edtRotateMove: TEdit
            Left = 799
            Top = 42
            Width = 109
            Height = 23
            ImeName = 'Microsoft IME 2010'
            NumbersOnly = True
            TabOrder = 10
            Text = '0'
          end
        end
        object tabRotate2: TTabSheet
          Caption = 'Rotate'
          ImageIndex = 4
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object btnRotateOrigin: TButton
            Left = 0
            Top = 0
            Width = 97
            Height = 32
            Caption = 'Rotate Origin'
            TabOrder = 0
            OnClick = btnRotateOriginClick
          end
          object btnRotateReset: TButton
            Left = 0
            Top = 37
            Width = 97
            Height = 32
            Caption = 'Rotate Reset'
            TabOrder = 1
            OnClick = btnRotateResetClick
          end
          object btnRotate0: TButton
            Left = 112
            Top = 0
            Width = 97
            Height = 32
            Caption = 'Rotate 0'
            TabOrder = 2
            OnClick = btnRotate0Click
          end
          object btnRotate90: TButton
            Left = 112
            Top = 37
            Width = 97
            Height = 32
            Caption = 'Rotate 90'
            TabOrder = 3
            OnClick = btnRotate90Click
          end
          object btnRotate180: TButton
            Left = 224
            Top = 0
            Width = 97
            Height = 32
            Caption = 'Rotate 180'
            TabOrder = 4
            OnClick = btnRotate180Click
          end
          object btnRotate270: TButton
            Left = 224
            Top = 37
            Width = 97
            Height = 32
            Caption = 'Rotate 270'
            TabOrder = 5
            OnClick = btnRotate270Click
          end
          object btnRotateFree: TButton
            Left = 336
            Top = 0
            Width = 97
            Height = 32
            Caption = 'Rotate Free'
            TabOrder = 6
            OnClick = btnRotateFreeClick
          end
          object GroupBox5: TGroupBox
            Left = 467
            Top = 3
            Width = 193
            Height = 65
            TabOrder = 7
            DesignSize = (
              193
              65)
            object shpRotate_L_Limit: TShape
              Left = 7
              Top = 3
              Width = 62
              Height = 25
              Anchors = []
              Shape = stRoundRect
            end
            object Label60: TLabel
              Left = 12
              Top = 8
              Width = 39
              Height = 15
              Caption = 'L-Limit'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpRotate_R_Limit: TShape
              Left = 75
              Top = 3
              Width = 62
              Height = 25
              Anchors = []
              Shape = stRoundRect
            end
            object Label61: TLabel
              Left = 80
              Top = 8
              Width = 40
              Height = 15
              Caption = 'R-Limit'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpRotate_Inposition: TShape
              Left = 7
              Top = 34
              Width = 62
              Height = 25
              Anchors = []
              Shape = stRoundRect
            end
            object Label62: TLabel
              Left = 12
              Top = 39
              Width = 55
              Height = 15
              Caption = 'Inposition'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpRotate_Alarm: TShape
              Left = 75
              Top = 34
              Width = 62
              Height = 25
              Anchors = []
              Shape = stRoundRect
            end
            object Label66: TLabel
              Left = 80
              Top = 39
              Width = 34
              Height = 15
              Caption = 'Alarm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
          end
        end
        object TabSheet1: TTabSheet
          Caption = 'Driving'
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object lbStopNodeID: TLabel
            Left = 0
            Top = 4
            Width = 70
            Height = 13
            Caption = 'Stop Node ID'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
          end
          object lbStopStationID: TLabel
            Left = 8
            Top = 30
            Width = 68
            Height = 13
            Alignment = taCenter
            Caption = 'Stop Station'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
            Layout = tlCenter
          end
          object lbStopStationOffset: TLabel
            Left = 100
            Top = 30
            Width = 75
            Height = 13
            Alignment = taCenter
            Caption = 'Station Offset'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
          end
          object lbNextNodeID: TLabel
            Left = 197
            Top = 30
            Width = 184
            Height = 13
            Alignment = taCenter
            Caption = 'Next Node ID ('#51076#49884#47196' '#49884#51089#45432#46300' '#51077#47141')'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
          end
          object lbStopStationType: TLabel
            Left = 409
            Top = 28
            Width = 66
            Height = 13
            Alignment = taCenter
            Caption = 'StationType'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
          end
          object lbPreSteeringNodeID: TLabel
            Left = 275
            Top = 4
            Width = 65
            Height = 26
            Alignment = taCenter
            Caption = 'PreSteering'#13#10'NodeID'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
          end
          object btnGo: TBitBtn
            Left = 665
            Top = 3
            Width = 57
            Height = 63
            Caption = '&Go'
            DoubleBuffered = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Kind = bkAll
            NumGlyphs = 2
            ParentDoubleBuffered = False
            ParentFont = False
            TabOrder = 6
            OnClick = btnGoClick
          end
          object chkGoStation: TCheckBox
            Left = 171
            Top = 2
            Width = 84
            Height = 17
            Caption = 'Go Station'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ParentFont = False
            TabOrder = 1
            OnClick = chkGoStationClick
          end
          object edtStopNodeID: TEdit
            Left = 76
            Top = 0
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 0
            TextHint = 'Stop Node ID'
          end
          object edtStopStationID: TEdit
            Left = -2
            Top = 48
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 2
            TextHint = 'Stop Station ID'
          end
          object edtStopStationsOffset: TEdit
            Left = 93
            Top = 48
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 3
            TextHint = 'Stop Station Offset'
          end
          object edtNextNodeID: TEdit
            Left = 187
            Top = 48
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 4
            TextHint = 'Next Node ID'
          end
          object cbbStopStationType: TComboBox
            Left = 398
            Top = 46
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 5
            TextHint = 'Station Type'
            Items.Strings = (
              'Stop Tag'
              'QR Left'
              'QR Right')
          end
          object edtPreSteeringNodeID: TEdit
            Left = 347
            Top = 6
            Width = 89
            Height = 21
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            ImeName = 'Microsoft IME 2010'
            ParentFont = False
            TabOrder = 7
            TextHint = 'Pre Steering Node ID'
          end
          object chkFromStation: TCheckBox
            Left = 288
            Top = 48
            Width = 89
            Height = 17
            Caption = 'From Station'
            TabOrder = 8
          end
        end
      end
    end
    object panManual: TPanel
      Left = 1
      Top = 199
      Width = 825
      Height = 227
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      object grpSteering: TGroupBox
        Left = 2
        Top = 1
        Width = 223
        Height = 96
        Caption = ' ** Steering ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 0
        object Panel17: TPanel
          Left = 2
          Top = 17
          Width = 219
          Height = 77
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          object btnSteeringRight: TSpeedButton
            Left = 1
            Top = 55
            Width = 105
            Height = 22
            Cursor = crHandPoint
            Caption = '[ Right ] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              0000FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              0000FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0080808000FF000000FF000000FF000000FF000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnSteeringRightClick
          end
          object btnSteeringLeft: TSpeedButton
            Left = 1
            Top = 33
            Width = 104
            Height = 22
            Cursor = crHandPoint
            Caption = '[ Left ]  '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF000000FF000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              0000FF000000FF00000080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnSteeringLeftClick
          end
          object Label30: TLabel
            Left = 0
            Top = 0
            Width = 52
            Height = 15
            Caption = 'F_Torque'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label32: TLabel
            Left = 0
            Top = 16
            Width = 54
            Height = 15
            Caption = 'R_Torque'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object btnTractionLeft: TSpeedButton
            Left = 1
            Top = 33
            Width = 107
            Height = 22
            Cursor = crHandPoint
            Caption = '[ T_Left ]  '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clRed
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF000000FF000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              0000FF000000FF00000080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            Visible = False
            OnClick = btnTractionLeftClick
          end
          object btnTractionRight: TSpeedButton
            Left = 1
            Top = 55
            Width = 105
            Height = 22
            Cursor = crHandPoint
            Caption = '[ T_Right ] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clRed
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00000080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              0000FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF000000FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF000000FF00FF00FF000000FF000000FF00FF00FF00FF00FF00FF00FF00
              FF0080808000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              0000FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0080808000FF000000FF000000FF000000FF000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            Visible = False
            OnClick = btnTractionRightClick
          end
          object Panel18: TPanel
            Left = 163
            Top = 0
            Width = 56
            Height = 77
            Align = alRight
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object RStrRightLt: TShape
              Left = 2
              Top = 27
              Width = 52
              Height = 25
              Align = alTop
              Shape = stRoundRect
            end
            object FStrRightLt: TShape
              Left = 2
              Top = 2
              Width = 52
              Height = 25
              Align = alTop
              Shape = stRoundRect
            end
            object Label17: TLabel
              Left = 4
              Top = 8
              Width = 42
              Height = 15
              Caption = ' F-Right'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label18: TLabel
              Left = 4
              Top = 34
              Width = 44
              Height = 15
              Caption = ' R-Right'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label56: TLabel
              Left = 4
              Top = 62
              Width = 42
              Height = 13
              Caption = 'S : 10'
              Font.Charset = ANSI_CHARSET
              Font.Color = clGray
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel19: TPanel
            Left = 107
            Top = 0
            Width = 56
            Height = 77
            Align = alRight
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 1
            object RStrLeftLt: TShape
              Left = 2
              Top = 27
              Width = 52
              Height = 25
              Align = alTop
              Shape = stRoundRect
            end
            object FStrLeftLt: TShape
              Left = 2
              Top = 2
              Width = 52
              Height = 25
              Align = alTop
              Shape = stRoundRect
            end
            object Label19: TLabel
              Left = 4
              Top = 8
              Width = 38
              Height = 15
              Caption = '  F-Left'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label20: TLabel
              Left = 4
              Top = 34
              Width = 40
              Height = 15
              Caption = '  R-Left'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label55: TLabel
              Left = 4
              Top = 62
              Width = 42
              Height = 13
              Caption = 'M : 00'
              Font.Charset = ANSI_CHARSET
              Font.Color = clGray
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object panFStrTorque: TPanel
            Left = 60
            Top = 0
            Width = 41
            Height = 16
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = '0 0'
            Color = clSilver
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentFont = False
            TabOrder = 2
          end
          object panRStrTorque: TPanel
            Left = 60
            Top = 16
            Width = 41
            Height = 16
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = '0 0'
            Color = clSilver
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = []
            ParentFont = False
            TabOrder = 3
          end
        end
        object chkTractionMode: TCheckBox
          Left = 121
          Top = -1
          Width = 72
          Height = 18
          Caption = 'Traction'
          TabOrder = 1
          OnClick = chkTractionModeClick
        end
      end
      object grpHand: TGroupBox
        Left = 1
        Top = 99
        Width = 223
        Height = 119
        Caption = '    ** Hand              :Go Signal  '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 1
        object Panel20: TPanel
          Left = 2
          Top = 17
          Width = 219
          Height = 100
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          object btnHandOrigin: TSpeedButton
            Left = 1
            Top = 41
            Width = 128
            Height = 20
            Cursor = crHandPoint
            Caption = '[Hand Origin]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00000000000000000000000000000000000000000000000000000000000000
              000000000000000000000000000000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF000000
              000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FF00FF00FF00FF000000
              000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF00FFFFFF000000000000FFFF0000000000FF00FF00FF00FF00FF00
              FF000000000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF000000000000FFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF000000000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
              FF000000000000FFFF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF000000000000FFFF0000000000FFFFFF00FFFFFF000000
              000000FFFF000000000000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF000000000000FFFF00000000000000000000FF
              FF00000000000000800000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000FFFF0000FFFF000000
              0000FF00FF000000800000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnHandOriginClick
          end
          object btnHandClose: TSpeedButton
            Left = 1
            Top = 21
            Width = 128
            Height = 20
            Cursor = crHandPoint
            Caption = ' [Hand Close] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              EE000000424DEE0000000000000076000000280000000F0000000F0000000100
              0400000000007800000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00111111111111
              111011111111111111101111111111111110111111BBB11111101111BBBBBBB1
              11101111BBBBBBB11110111BB777777B1110111B000000BB1110111BBBBBBBBB
              11101111BBBBBBB111101111BBBBBBB11110111111BBB1111110111111111111
              111011111111111111101111111111111110}
            ParentFont = False
            Transparent = False
            OnClick = btnHandCloseClick
          end
          object btnHandOpen: TSpeedButton
            Left = 1
            Top = 1
            Width = 128
            Height = 20
            Cursor = crHandPoint
            AllowAllUp = True
            Caption = '[Hand Open] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00000000000000
              0000000000000000000000000000000000000000000000000000000000000000
              000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00808080000000
              0000008080000080800000808000008080000080800000808000008080000080
              80000080800000000000FF00FF00FF00FF00FF00FF00FF00FF008080800000FF
              FF00000000000080800000808000008080000080800000808000008080000080
              8000008080000080800000000000FF00FF00FF00FF00FF00FF0080808000FFFF
              FF0000FFFF000000000000808000008080000080800000808000008080000080
              800000808000008080000080800000000000FF00FF00FF00FF008080800000FF
              FF00FFFFFF0000FFFF0000000000008080000080800000808000008080000080
              80000080800000808000008080000080800000000000FF00FF0080808000FFFF
              FF0000FFFF00FFFFFF0000FFFF00000000000000000000000000000000000000
              00000000000000000000000000000000000000000000000000008080800000FF
              FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FF
              FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0080808000FFFF
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFF
              FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF008080800000FF
              FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
              000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF008080
              80008080800080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00000000000000000000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF000000000000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FF00FF00FF00FF0000000000FF00FF0000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              00000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnHandOpenClick
          end
          object btnHandReset: TSpeedButton
            Left = 1
            Top = 61
            Width = 128
            Height = 20
            Cursor = crHandPoint
            Caption = '[Hand Reset] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF000000FF000000FF00
              0000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF000000FF00
              FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF000000FF00FF00FF00FF00FF000000FF00
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF000000FF000000FF00
              0000FF000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnHandResetClick
          end
          object btnHandFree: TSpeedButton
            Left = 1
            Top = 81
            Width = 128
            Height = 20
            Cursor = crHandPoint
            Caption = '[Hand Free]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              42010000424D4201000000000000760000002800000011000000110000000100
              040000000000CC00000000000000000000001000000010000000000000000000
              BF0000BF000000BFBF00BF000000BF00BF00BFBF0000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00777777777777
              7777700000007770777777777077700000007778777777777877700000007080
              8080808080807000000077788888888888777000000077708888888880777000
              0000777888888888887770000000777088888888807770000000777888888888
              8877700000007770888888888077700000007778888888888877700000007770
              8888888880777000000077788888888888777000000070808080808080807000
              0000777877777777787770000000777077777777707770000000777777777777
              777770000000}
            ParentFont = False
            Transparent = False
            OnClick = btnHandFreeClick
          end
          object panHandSensor: TPanel
            Left = 126
            Top = 0
            Width = 93
            Height = 100
            Align = alRight
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object HandInPosLt: TShape
              Left = 2
              Top = 34
              Width = 89
              Height = 16
              Align = alTop
              Shape = stRoundRect
            end
            object HandAlarmLt: TShape
              Left = 2
              Top = 50
              Width = 89
              Height = 16
              Align = alTop
              Shape = stRoundRect
            end
            object HandCloseLt: TShape
              Left = 2
              Top = 18
              Width = 89
              Height = 16
              Align = alTop
              Shape = stRoundRect
            end
            object HandOpenLt: TShape
              Left = 2
              Top = 2
              Width = 89
              Height = 16
              Align = alTop
              Shape = stRoundRect
            end
            object Label21: TLabel
              Left = 31
              Top = 2
              Width = 30
              Height = 15
              Caption = 'Open'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label22: TLabel
              Left = 30
              Top = 18
              Width = 32
              Height = 15
              Caption = 'Close'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label23: TLabel
              Left = 29
              Top = 34
              Width = 35
              Height = 15
              Caption = 'In Pos'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label24: TLabel
              Left = 29
              Top = 50
              Width = 34
              Height = 15
              Caption = 'Alarm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object GLLt: TShape
              Left = 2
              Top = 66
              Width = 45
              Height = 16
              Shape = stRoundRect
            end
            object GRLt: TShape
              Left = 46
              Top = 66
              Width = 45
              Height = 16
              Shape = stRoundRect
            end
            object GLLa: TLabel
              Left = 6
              Top = 66
              Width = 37
              Height = 15
              Caption = 'Grip_L'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object GRLa: TLabel
              Left = 50
              Top = 66
              Width = 38
              Height = 15
              Caption = 'Grip_R'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpHandPushLt: TShape
              Left = 2
              Top = 81
              Width = 89
              Height = 17
              Align = alBottom
              Shape = stRoundRect
            end
            object Label65: TLabel
              Left = 14
              Top = 82
              Width = 65
              Height = 15
              Caption = 'Hand_Push'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
          end
        end
        object HandGoLt: TPanel
          Left = 88
          Top = 0
          Width = 15
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clLime
          ParentBackground = False
          TabOrder = 1
        end
        object HandOriginLt: TPanel
          Left = 10
          Top = 0
          Width = 15
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clLime
          ParentBackground = False
          TabOrder = 2
        end
      end
      object grpShutter: TGroupBox
        Left = 225
        Top = 97
        Width = 353
        Height = 119
        Caption = '      ** Shutter ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 2
        object panShutter: TPanel
          Left = 2
          Top = 17
          Width = 349
          Height = 100
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          DesignSize = (
            349
            100)
          object btnShutterClose: TSpeedButton
            Left = 1
            Top = 41
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[ Close ] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              EE000000424DEE0000000000000076000000280000000F0000000F0000000100
              0400000000007800000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00111111111111
              111011111111111111101111111111111110111111BBB11111101111BBBBBBB1
              11101111BBBBBBB11110111BB777777B1110111B000000BB1110111BBBBBBBBB
              11101111BBBBBBB111101111BBBBBBB11110111111BBB1111110111111111111
              111011111111111111101111111111111110}
            ParentFont = False
            Transparent = False
            OnClick = btnShutterCloseClick
          end
          object btnShutterOpen: TSpeedButton
            Left = 1
            Top = 21
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[ Open ]  '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00000000000000
              0000000000000000000000000000000000000000000000000000000000000000
              000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00808080000000
              0000008080000080800000808000008080000080800000808000008080000080
              80000080800000000000FF00FF00FF00FF00FF00FF00FF00FF008080800000FF
              FF00000000000080800000808000008080000080800000808000008080000080
              8000008080000080800000000000FF00FF00FF00FF00FF00FF0080808000FFFF
              FF0000FFFF000000000000808000008080000080800000808000008080000080
              800000808000008080000080800000000000FF00FF00FF00FF008080800000FF
              FF00FFFFFF0000FFFF0000000000008080000080800000808000008080000080
              80000080800000808000008080000080800000000000FF00FF0080808000FFFF
              FF0000FFFF00FFFFFF0000FFFF00000000000000000000000000000000000000
              00000000000000000000000000000000000000000000000000008080800000FF
              FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FF
              FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0080808000FFFF
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFF
              FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF008080800000FF
              FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
              000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF008080
              80008080800080808000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00000000000000000000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF000000000000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FF00FF00FF00FF0000000000FF00FF0000000000FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              00000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnShutterOpenClick
          end
          object btnShutterAlarmReset: TSpeedButton
            Left = 1
            Top = 1
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[AlarmRst] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              6E020000424D6E0200000000000076000000280000002A000000150000000100
              040000000000F8010000CE0E0000C40E00001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00777777777777
              7777777777F777F7777777777777770000007F777F7777777777777777F77F7F
              77F777777777770000007F77FFF77F7777777777777FF777FF7F777777777700
              00007FFFFFFFFFF77777777777777777777FFFF77777770000007FFFFFFFF311
              1777777777777777778888FF7777770000007FFFFFFF39DD1177777777777777
              88F7788FF777770000007FFFFFFF9D9DD11777777777877878F77788FF777700
              00007FFFFFF7D9F9DD1077777778787878F777788FF7770000007FF7FFF79DFD
              9D02077777787787787F777888FF770000007F777F7779DFD020207777877777
              7787F788888FF70000007F777777779D0A22020777777777777878788888F700
              0000777777777770ABA2203077777777777787F7788887000000777777777777
              0ABA0033777777777777787F78888700000077777777777770A08B0377777777
              777777877778870000007777777777777708FBB077777777777777787F778700
              00007777777777777770FFBB777777777777777787F777000000777777777777
              77770FFB7777777777777777787F77000000777777777777777770FF77777777
              777777777787F70000007777777777777777770F777777777777777777787700
              0000777777777777777777707777777777777777777787000000777777777777
              777777777777777777777777777777000000}
            NumGlyphs = 2
            ParentFont = False
            Transparent = False
            OnClick = btnShutterAlarmResetClick
          end
          object btnShutterOrigin: TSpeedButton
            Left = 1
            Top = 61
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[ Origin ] '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00000000000000000000000000000000000000000000000000000000000000
              000000000000000000000000000000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000FF000000FF00000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
              0000FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF000000
              000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FF00FF00FF00FF000000
              000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF00FFFFFF000000000000FFFF0000000000FF00FF00FF00FF00FF00
              FF000000000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00FFFFFF000000000000FFFF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF000000000000FFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
              FF000000000000FFFF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF000000000000FFFF0000000000FFFFFF00FFFFFF000000
              000000FFFF000000000000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF000000000000FFFF00000000000000000000FF
              FF00000000000000800000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000FFFF0000FFFF000000
              0000FF00FF000000800000008000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnShutterOriginClick
          end
          object btnShutterFree: TSpeedButton
            Left = 1
            Top = 81
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[ Free]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              42010000424D4201000000000000760000002800000011000000110000000100
              040000000000CC00000000000000000000001000000010000000000000000000
              BF0000BF000000BFBF00BF000000BF00BF00BFBF0000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00777777777777
              7777700000007770777777777077700000007778777777777877700000007080
              8080808080807000000077788888888888777000000077708888888880777000
              0000777888888888887770000000777088888888807770000000777888888888
              8877700000007770888888888077700000007778888888888877700000007770
              8888888880777000000077788888888888777000000070808080808080807000
              0000777877777777787770000000777077777777707770000000777777777777
              777770000000}
            ParentFont = False
            Transparent = False
            OnClick = btnShutterFreeClick
          end
          object btnShutterMiddleClose: TSpeedButton
            Left = 228
            Top = 49
            Width = 96
            Height = 20
            Cursor = crHandPoint
            Caption = '[ M Close ]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold, fsItalic]
            Glyph.Data = {
              EE000000424DEE0000000000000076000000280000000F0000000F0000000100
              0400000000007800000000000000000000001000000000000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00111111111111
              111011111111111111101111111111111110111111BBB11111101111BBBBBBB1
              11101111BBBBBBB11110111BB777777B1110111B000000BB1110111BBBBBBBBB
              11101111BBBBBBB111101111BBBBBBB11110111111BBB1111110111111111111
              111011111111111111101111111111111110}
            ParentFont = False
            Transparent = False
            OnClick = btnShutterMiddleCloseClick
          end
          object shpShutterMiddleClose_Front: TShape
            Left = 217
            Top = 24
            Width = 63
            Height = 24
            Shape = stRoundRect
          end
          object shpShutterMiddleClose_Rear: TShape
            Left = 283
            Top = 24
            Width = 63
            Height = 24
            Shape = stRoundRect
          end
          object Label45: TLabel
            Left = 222
            Top = 28
            Width = 55
            Height = 15
            Caption = 'FM-Close'
          end
          object Label70: TLabel
            Left = 287
            Top = 28
            Width = 57
            Height = 15
            Caption = 'RM-Close'
          end
          object panShutterRear: TPanel
            Left = 158
            Top = -2
            Width = 56
            Height = 100
            Anchors = []
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object shpShutterClose_Rear: TShape
              Left = 2
              Top = 26
              Width = 52
              Height = 24
              Align = alTop
              Anchors = []
              Shape = stRoundRect
              ExplicitLeft = 3
              ExplicitTop = 25
            end
            object shpShutterAlarm_Rear: TShape
              Left = 2
              Top = 50
              Width = 52
              Height = 24
              Align = alTop
              Anchors = []
              Shape = stRoundRect
            end
            object shpShutterInposition_Rear: TShape
              Left = 2
              Top = 74
              Width = 52
              Height = 24
              Align = alTop
              Anchors = []
              Shape = stRoundRect
            end
            object shpShutterOpen_Rear: TShape
              Left = 2
              Top = 2
              Width = 52
              Height = 24
              Align = alTop
              Anchors = []
              Shape = stRoundRect
              ExplicitTop = 26
            end
            object lbShutterOpen_Rear: TLabel
              Left = 6
              Top = 7
              Width = 43
              Height = 15
              Caption = 'R-Open'
            end
            object lbShutterClose_Rear: TLabel
              Left = 5
              Top = 30
              Width = 46
              Height = 15
              Caption = 'R-Close'
            end
            object lbShutterAlarm_Rear: TLabel
              Left = 4
              Top = 55
              Width = 48
              Height = 15
              Caption = 'R-Alarm'
            end
            object lbShutterInposition_Rear: TLabel
              Left = 6
              Top = 79
              Width = 45
              Height = 15
              Caption = 'R-InPos'
            end
          end
          object panShutterFront: TPanel
            Left = 98
            Top = -2
            Width = 56
            Height = 100
            Anchors = []
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 1
            object shpShutterOpen_Front: TShape
              Left = 2
              Top = 2
              Width = 52
              Height = 22
              Align = alTop
              Shape = stRoundRect
            end
            object shpShutterAlarm_Front: TShape
              Left = 2
              Top = 48
              Width = 52
              Height = 24
              Align = alTop
              Shape = stRoundRect
              ExplicitTop = 50
            end
            object shpShutterInposition_Front: TShape
              Left = 2
              Top = 72
              Width = 52
              Height = 24
              Align = alTop
              Shape = stRoundRect
              ExplicitTop = 74
            end
            object shpShutterClose_Front: TShape
              Left = 2
              Top = 24
              Width = 52
              Height = 24
              Align = alTop
              Shape = stRoundRect
              ExplicitTop = 48
            end
            object lbShutterOpen_Front: TLabel
              Left = 7
              Top = 7
              Width = 41
              Height = 15
              Caption = 'F-Open'
            end
            object lbShutterClose_Front: TLabel
              Left = 6
              Top = 30
              Width = 44
              Height = 15
              Caption = 'F-Close'
            end
            object lbShutterAlarm_Front: TLabel
              Left = 5
              Top = 55
              Width = 46
              Height = 15
              Caption = 'F-Alarm'
            end
            object lbShutterInposition_Front: TLabel
              Left = 6
              Top = 79
              Width = 43
              Height = 15
              Caption = 'F-InPos'
            end
          end
        end
        object panShtrOrigin: TPanel
          Left = 10
          Top = 0
          Width = 15
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clLime
          ParentBackground = False
          TabOrder = 1
        end
      end
      object grpTrackBuffer: TGroupBox
        Left = 653
        Top = 4
        Width = 169
        Height = 96
        Caption = ' ** Track Buffer ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 3
        Visible = False
        object panSTBOHTDetectButton: TPanel
          Left = 2
          Top = 17
          Width = 55
          Height = 77
          Align = alLeft
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          object btnSTB_R_WrokOn: TSpeedButton
            Left = 0
            Top = 3
            Width = 53
            Height = 18
            Cursor = crHandPoint
            AllowAllUp = True
            Caption = '[R_On]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = False
            OnClick = btnSTB_R_WrokOnClick
          end
          object btnSTB_R_WorkOff: TSpeedButton
            Left = 0
            Top = 22
            Width = 53
            Height = 18
            Cursor = crHandPoint
            Caption = '[R_Off]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = False
            OnClick = btnSTB_R_WorkOffClick
          end
          object btnSTB_L_WrokOn: TSpeedButton
            Left = 0
            Top = 41
            Width = 53
            Height = 18
            Cursor = crHandPoint
            Caption = '[L_On]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = False
            OnClick = btnSTB_L_WrokOnClick
          end
          object btnSTB_L_WrokOff: TSpeedButton
            Left = 0
            Top = 60
            Width = 53
            Height = 18
            Cursor = crHandPoint
            Caption = '[L_Off]'
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = False
            OnClick = btnSTB_L_WrokOffClick
          end
        end
        object panTBFoupStatus: TPanel
          Left = 112
          Top = 17
          Width = 55
          Height = 77
          Align = alClient
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 1
          object panSTBLeftTransAvailable: TPanel
            Left = 2
            Top = 2
            Width = 51
            Height = 25
            Align = alTop
            BiDiMode = bdLeftToRight
            Caption = 'STB_L'
            ParentBiDiMode = False
            ParentBackground = False
            TabOrder = 0
          end
          object panUTBTransAvailable: TPanel
            Left = 2
            Top = 50
            Width = 51
            Height = 25
            Align = alBottom
            BiDiMode = bdLeftToRight
            Caption = 'UTB'
            ParentBiDiMode = False
            ParentBackground = False
            TabOrder = 1
          end
          object panSTBRightTransAvailable: TPanel
            Left = 2
            Top = 27
            Width = 51
            Height = 25
            Align = alTop
            BiDiMode = bdLeftToRight
            Caption = 'STB_R'
            ParentBiDiMode = False
            ParentBackground = False
            TabOrder = 2
          end
        end
        object panSTBOHTDetect: TPanel
          Left = 57
          Top = 17
          Width = 55
          Height = 77
          Align = alLeft
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 2
          object shpSTBOHTL: TShape
            Left = 2
            Top = 2
            Width = 51
            Height = 36
            Align = alTop
            Shape = stRoundRect
            ExplicitTop = 38
          end
          object lbSTB_OHTLeft: TLabel
            Left = 3
            Top = 6
            Width = 49
            Height = 28
            Alignment = taCenter
            Caption = 'STB_OHT'#13#10'Left'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object shpSTBOHTR: TShape
            Left = 2
            Top = 38
            Width = 51
            Height = 36
            Align = alTop
            Shape = stRoundRect
            ExplicitTop = 37
          end
          object lbSTB_OHTRight: TLabel
            Left = 3
            Top = 42
            Width = 49
            Height = 28
            Alignment = taCenter
            Caption = 'STB_OHT'#13#10'Right'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
        end
      end
      object panUBG: TPanel
        Left = 583
        Top = 103
        Width = 19
        Height = 75
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 4
        object panUBG_OUT4: TPanel
          Left = 3
          Top = 38
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
        end
        object panUBG_OUT3: TPanel
          Left = 3
          Top = 26
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 1
        end
        object panUBG_OUT2: TPanel
          Left = 3
          Top = 14
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 2
        end
        object panUBG_OUT1: TPanel
          Left = 3
          Top = 2
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 3
        end
        object panUBG_OUT5: TPanel
          Left = 3
          Top = 50
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 4
        end
        object panUBG_OUT6: TPanel
          Left = 3
          Top = 62
          Width = 13
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 5
        end
      end
      object panUBG_FAIL: TPanel
        Left = 583
        Top = 178
        Width = 19
        Height = 16
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '7'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 5
      end
      object GroupBox3: TGroupBox
        Left = 227
        Top = 1
        Width = 422
        Height = 96
        Caption = ' ** CID ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 6
        object Panel5: TPanel
          Left = 2
          Top = 17
          Width = 418
          Height = 77
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          DesignSize = (
            418
            77)
          object shpCID_Link_Compt: TShape
            Left = 2
            Top = 0
            Width = 69
            Height = 29
            Anchors = []
            Shape = stRoundRect
          end
          object Label2: TLabel
            Left = 8
            Top = 5
            Width = 60
            Height = 15
            Caption = 'Link Comp'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object shpCID_Interlock: TShape
            Left = 74
            Top = 1
            Width = 66
            Height = 27
            Anchors = []
            Shape = stRoundRect
          end
          object Label8: TLabel
            Left = 79
            Top = 5
            Width = 57
            Height = 15
            Caption = 'Inter Lock'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object shpCID_Pass_OK: TShape
            Left = 140
            Top = 1
            Width = 66
            Height = 27
            Anchors = []
            Shape = stRoundRect
          end
          object shpCID_Status: TShape
            Left = 2
            Top = 30
            Width = 66
            Height = 25
            Anchors = []
            Shape = stRoundRect
          end
          object Label28: TLabel
            Left = 20
            Top = 35
            Width = 37
            Height = 15
            Caption = 'Status'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object shpCID_Reserved_1: TShape
            Left = 73
            Top = 28
            Width = 66
            Height = 26
            Anchors = []
            Shape = stRoundRect
          end
          object Label44: TLabel
            Left = 93
            Top = 34
            Width = 33
            Height = 15
            Caption = 'Rev. 1'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object shpCID_Reserved_2: TShape
            Left = 140
            Top = 29
            Width = 66
            Height = 26
            Anchors = []
            Shape = stRoundRect
          end
          object Label54: TLabel
            Left = 149
            Top = 5
            Width = 49
            Height = 15
            Caption = 'Pass OK'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object Label58: TLabel
            Left = 156
            Top = 34
            Width = 33
            Height = 15
            Caption = 'Rev. 2'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            Transparent = True
          end
          object btnCidRev1: TButton
            Left = 213
            Top = 56
            Width = 89
            Height = 19
            Caption = 'Common On'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
            OnClick = btnCidRev1Click
          end
          object btnCidRev2: TButton
            Left = 308
            Top = 56
            Width = 102
            Height = 17
            Caption = 'Rev. 2 On'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 1
            OnClick = btnCidRev2Click
          end
        end
        object btnCidLeftSel: TButton
          Left = 214
          Top = 13
          Width = 90
          Height = 19
          Caption = 'Left Sel On'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = btnCidLeftSelClick
        end
        object btnCidOccupReq: TButton
          Left = 214
          Top = 32
          Width = 90
          Height = 21
          Caption = 'Occup Req On'
          TabOrder = 2
          OnClick = btnCidOccupReqClick
        end
        object btnCidManual: TButton
          Left = 214
          Top = 53
          Width = 90
          Height = 20
          Caption = 'Manual On'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
          OnClick = btnCidManualClick
        end
        object btnCidRightSel: TButton
          Left = 310
          Top = 12
          Width = 102
          Height = 20
          Caption = 'Right Sel On'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
          OnClick = btnCidRightSelClick
        end
        object btnCidPassCompt: TButton
          Left = 310
          Top = 33
          Width = 102
          Height = 20
          Caption = 'PASS Compt On'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 5
          OnClick = btnCidPassComptClick
        end
        object btnCidOperation: TButton
          Left = 310
          Top = 52
          Width = 102
          Height = 21
          Caption = 'Operation On'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 6
          OnClick = btnCidOperationClick
        end
      end
      object GroupBox8: TGroupBox
        Left = 608
        Top = 100
        Width = 188
        Height = 109
        Caption = ' ** CID Info ** '
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentFont = False
        TabOrder = 7
        object SpeedButton1: TSpeedButton
          Left = 5
          Top = 18
          Width = 46
          Height = 14
          Cursor = crHandPoint
          AllowAllUp = True
          Caption = '[CID-O]'
          Flat = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = False
          OnClick = btnSTB_R_WrokOnClick
        end
        object SpeedButton2: TSpeedButton
          Left = 4
          Top = 43
          Width = 46
          Height = 18
          Cursor = crHandPoint
          AllowAllUp = True
          Caption = '[CID-R]'
          Flat = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = False
          OnClick = btnSTB_R_WrokOnClick
        end
        object edtCid_O_Link: TEdit
          Left = 56
          Top = 15
          Width = 57
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
        end
        object edtCid_R_Link: TEdit
          Left = 56
          Top = 42
          Width = 57
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
        end
        object btnCidMonitoring: TButton
          Left = 26
          Top = 71
          Width = 73
          Height = 30
          Caption = 'Left'
          TabOrder = 2
          OnClick = btnCidMonitoringClick
        end
        object edtCid_O_Right: TEdit
          Left = 121
          Top = 15
          Width = 57
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
        end
        object edtCid_R_Right: TEdit
          Left = 121
          Top = 44
          Width = 57
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
        end
        object btnCidMonitoring_Right: TButton
          Left = 105
          Top = 71
          Width = 73
          Height = 30
          Caption = 'Right'
          TabOrder = 5
          OnClick = btnCidMonitoring_RightClick
        end
      end
    end
    object panMark: TPanel
      Left = 1
      Top = 105
      Width = 825
      Height = 94
      Align = alTop
      BevelInner = bvRaised
      BevelOuter = bvLowered
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      object grpDrivingMark: TGroupBox
        Left = 2
        Top = 2
        Width = 188
        Height = 90
        Align = alLeft
        Caption = ' **Driving Mark ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 0
        object pan5: TPanel
          Left = 2
          Top = 17
          Width = 184
          Height = 71
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          object btnDrivingMarkForward: TSpeedButton
            Left = 1
            Top = 1
            Width = 95
            Height = 34
            Cursor = crHandPoint
            AllowAllUp = True
            Caption = '[ Mark F ]   '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF0080000000800000008000000080000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000800000008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF0080000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00800000008000000080000000800000008000
              00008000000080000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnDrivingMarkForwardClick
          end
          object btnDrivingBCR: TSpeedButton
            Left = 1
            Top = 35
            Width = 95
            Height = 34
            Cursor = crHandPoint
            Caption = '[ BCR ]      '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00FF00FFFF
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00000000000000800000000000FFFF
              FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000008000800080000000
              000000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000008000000080008000
              800000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00000000008080800000008000000080000000
              80000000FF000000000000FFFF00FFFFFF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF0000000000FF00FF00FF00FF00000080000000
              FF000000FF000000FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF000000
              FF000000FF000000FF000000FF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00000000000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00000000000000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              0000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00000000000000FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FFFFFF00FFFFFF00FFFFFF00000000000000FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              0000FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnDrivingBCRClick
          end
          object panDrivingMarkSensor: TPanel
            Left = 94
            Top = 0
            Width = 90
            Height = 71
            Align = alRight
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object Label64: TLabel
              Left = 4
              Top = 85
              Width = 63
              Height = 15
              Caption = ' R-StopSen'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpF_DrivingSen: TShape
              Left = 2
              Top = 2
              Width = 86
              Height = 34
              Align = alTop
              Shape = stRoundRect
            end
            object Label63: TLabel
              Left = 8
              Top = 12
              Width = 74
              Height = 15
              Caption = ' F-DrivingSen'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpBCRDriving: TShape
              Left = 2
              Top = 36
              Width = 86
              Height = 34
              Align = alTop
              Shape = stRoundRect
            end
            object Label67: TLabel
              Left = 10
              Top = 46
              Width = 70
              Height = 15
              Caption = 'BCR_Driving'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
          end
        end
      end
      object grpTransMark: TGroupBox
        Left = 190
        Top = 2
        Width = 195
        Height = 90
        Align = alLeft
        Caption = ' ** Trans(Stop) Mark ** '
        Color = 16768991
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentColor = False
        ParentFont = False
        TabOrder = 1
        object pan7: TPanel
          Left = 2
          Top = 17
          Width = 191
          Height = 71
          Align = alClient
          BevelOuter = bvNone
          Color = clSilver
          TabOrder = 0
          object btnStopTagBCR: TSpeedButton
            Left = 0
            Top = 35
            Width = 105
            Height = 34
            Cursor = crHandPoint
            Caption = '[ BCR ]      '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00FF00FFFF
              FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00000000000000800000000000FFFF
              FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000008000800080000000
              000000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF000000000000008000000080008000
              800000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00000000008080800000008000000080000000
              80000000FF000000000000FFFF00FFFFFF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF0000000000FF00FF00FF00FF00000080000000
              FF000000FF000000FF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF000000
              FF000000FF000000FF000000FF0000000000FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00FFFF
              FF00000000000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF0000000000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00000000000000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              0000FF00FF00FF00FF00FFFFFF00FFFFFF00FFFFFF0000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FF00FF00FFFFFF00FFFFFF00FFFFFF00000000000000FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF0000000000FF00
              FF00FFFFFF00FFFFFF00FFFFFF00000000000000FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF000000
              0000FFFFFF00FFFFFF0000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF000000000000000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnStopTagBCRClick
          end
          object btnTransMarkForward: TSpeedButton
            Left = 0
            Top = 1
            Width = 105
            Height = 34
            Cursor = crHandPoint
            AllowAllUp = True
            Caption = '[ Mark F ]   '
            Flat = True
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlack
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            Glyph.Data = {
              36040000424D3604000000000000360000002800000010000000100000000100
              2000000000000004000000000000000000000000000000000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF0080000000800000008000000080000000FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000800000008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF008000
              0000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF008000000080000000FF00FF00FF00
              FF00FF00FF0080000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00800000008000000080000000800000008000
              00008000000080000000FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00
              FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00FF00}
            ParentFont = False
            Transparent = False
            OnClick = btnTransMarkForwardClick
          end
          object panStopTagMarkSensor: TPanel
            Left = 101
            Top = 0
            Width = 90
            Height = 71
            Align = alRight
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object shpR_TransSen: TShape
              Left = 2
              Top = 24
              Width = 86
              Height = 22
              Align = alTop
              Shape = stRoundRect
              ExplicitTop = 28
            end
            object shpF_TransSen: TShape
              Left = 2
              Top = 2
              Width = 86
              Height = 22
              Align = alTop
              Shape = stRoundRect
            end
            object Label14: TLabel
              Left = 11
              Top = 6
              Width = 67
              Height = 15
              Caption = ' F-TransSen'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object shpBCRTransLt: TShape
              Left = 2
              Top = 46
              Width = 86
              Height = 22
              Align = alTop
              Shape = stRoundRect
              ExplicitTop = 54
            end
            object Label43: TLabel
              Left = 13
              Top = 50
              Width = 63
              Height = 15
              Caption = 'BCR_Trans'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
            object Label16: TLabel
              Left = 10
              Top = 28
              Width = 69
              Height = 15
              Caption = ' R-TransSen'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlack
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
              Transparent = True
            end
          end
        end
      end
      object grpQRMark: TGroupBox
        Left = 391
        Top = 6
        Width = 125
        Height = 90
        Caption = '** Trans(QR) Mark **'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold, fsItalic]
        ParentFont = False
        TabOrder = 2
        object btnQRTagLeftMark: TSpeedButton
          Left = 2
          Top = 17
          Width = 121
          Height = 21
          Cursor = crHandPoint
          Align = alTop
          AllowAllUp = True
          Caption = '[ Mark Left ]   '
          Flat = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Glyph.Data = {
            36030000424D3603000000000000360000002800000010000000100000000100
            1800000000000003000012170000121700000000000000000000FFFFFFFFFFFF
            FFFFFFFFFFFFFBFBFBF0F0F0E2E2E2DADADADADADAE2E2E2F0F0F0FBFBFBFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEEDEFED90C48F55AE5243AD3E3D
            B1394CB54A5DB55D7EB47ECFD1CFF0F0F0FEFEFEFFFFFFFFFFFFFFFFFFFEFEFE
            CAE1CA49AA443AA52C2FA2202AA41E3AB0314ABF4669D1697CDC815CBD5EA6BE
            A6EBEBEBFEFEFEFFFFFFFFFFFFCEE5CE40A03A2592141693041F9B0F2CA6203B
            B2334DC04A60D06274E17C93F39F6DCE73A6BEA6F0F0F0FFFFFFF7FAF74BA648
            2385141187001592031E9A0E299F1D81BE7A73BC6F5BCA5B6EDB747DE78689EA
            9158B95ACFD1CFFBFBFBA1D2A1348A290F77001184001491011A930A9DCA97FE
            FDFCE5EFE14EBB4C60D06168D76C69D86D72D4737EB37EF0F0F06CB56A277D1A
            0F750010800012890097C691FFFFFFFBFDFB74B76F44B83E4FC24C55C75455C7
            555DC85A5AB359E2E2E263AF61609E571777090F770095C18FFFFFFFFFFFFFCE
            E3CC94C68F98CB959CCD9864B76043B83D40B53948B245DADADA5AA85772A86B
            63A25A4C8F43FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC3DDC032A7
            2730A92538AD33DADADA67AF6475A96E70A76868A25FB5CDB2FFFFFFFFFFFFCB
            E0C789BD828BC0848CC1864DA442259F17239E1440AB3BE2E2E277B97683B17C
            73A86C6EA76666A25EB5CDB1FFFFFFFBFCFB67AA5D2A9B19299D192A9E1A2A9E
            1A3FA73057AF54F0F0F0A7D3A786B68177AA7073A86B6EA76566A25EB5CDB2FF
            FFFFE8F0E758A64D5CB04F5BB14E5BB14E6DBB638FC48FFBFBFBFBFDFB6BB169
            87B38077AA7073A86B6FA76667A25F95B79184AF7E62A85861AA5660AB556DB2
            6358B156EDEFEDFFFFFFFFFFFFD7EBD76EB06B86B38078AA7174A86C70A7686D
            A7656AA76268A75F67A75E74AF6B61B15DCAE1CAFEFEFEFFFFFFFFFFFFFFFFFF
            D7EBD76BB16986B68183B17C76A96F73A86B70A76979AD717BB2745FB25DCEE5
            CEFEFEFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBFDFBA8D4A877B97664AE625B
            A85762AE6073B971A1D2A1F8FAF8FFFFFFFFFFFFFFFFFFFFFFFF}
          ParentFont = False
          Transparent = False
          OnClick = btnQRTagLeftMarkClick
          ExplicitLeft = -2
        end
        object btnQRTagRightMark: TSpeedButton
          Left = 2
          Top = 38
          Width = 121
          Height = 21
          Cursor = crHandPoint
          Align = alTop
          AllowAllUp = True
          Caption = '[ Mark Right ]   '
          Flat = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Glyph.Data = {
            36030000424D3603000000000000360000002800000010000000100000000100
            1800000000000003000012170000121700000000000000000000FFFFFFFFFFFF
            FFFFFFFEFEFEFAFAFAEDEDEDDDDDDDD8D8D8D8D8D8DDDDDDEDEDEDFAFAFAFEFE
            FEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEE7EBE780BE7F4CAD483CAD363B
            B03749B84759B75974B375C8CDC8EDEDEDFDFDFDFFFFFFFFFFFFFFFFFFFEFEFE
            BCDBBC42A73C38A32A279E182AA41E3AB1324BBF4764D0657FDE845FC36397B8
            97E6E6E6FEFEFEFFFFFFFEFEFEC1DFC1359C2E2190101693041F9B0F2CA51F3B
            B2334DC04960D06174E17C91F29D68CF6E9ABA9AEEEEEEFEFEFEF3F8F343A13F
            2083121187001592021D9A0D3A9D2CA1C28E5EB3575BCB5C6DDA727BE68485E9
            8E58BD5BC9CEC9FBFBFB95CD953488280F77001184001490011B980B73AE62F7
            ECE0E5E4D164B7605ECE5F67D66A68D66B71D37377B377EFEFEF70B76E267B19
            0F7400108000128D0018950629971AC7D9BBFBF5F0E9EDDF62B65F54C55254C6
            535AC6575AB359E2E2E260AD5D619F581A790C16780891BE8BA5CBA0A8CDA3B5
            D5B1FEFEFDFEFEFDECF4EA59B15542B73B3EB53746B143DADADA5DA95A73A86B
            66A35D629C59FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFAED2AA31A7
            262FA82338AD33DADADA65AE6276AA6F70A86866A05E8EB48880AF796AA8607F
            B578FBFDFBFFFFFFDBEAD9359C29249F16239E1440AB3BE3E3E37EBD7D83B17C
            74A86C6FA7666AA76166A95B64A35BDCEADAFFFFFFE1EDDF3F9D32299E1932A1
            2243A9345FB15CF2F2F2B1D8B181B47C78AA7173A86B6FA7666AA76199BB94FF
            FFFFE3ECE166A95C5CB04F5BB04E5BB04E69BA619AC89AFCFCFCFFFFFF69B167
            8AB58377AA7073A86B6FA7676DA26597BA916BA46263A95961AA5760AA5671B3
            675AB258F2F2F2FFFFFFFFFFFFE2F1E260AB5E8BB58578AA7174A86D71A7696D
            A7656BA76269A76068A75E77B06F55AE52D5E5D5FEFEFEFFFFFFFFFFFFFFFFFF
            E2F1E26BB36A7FB37A86B28078AB7173A86C73A96C7CAF7575B26F63B461DAEA
            DAFEFEFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB2D9B27EBE7D72B56F6F
            B36C6FB56D7ABE79AED9AEFCFCFCFFFFFFFFFFFFFFFFFFFFFFFF}
          ParentFont = False
          Transparent = False
          OnClick = btnQRTagRightMarkClick
        end
        object panQRMark: TPanel
          Left = 2
          Top = 61
          Width = 121
          Height = 27
          Align = alBottom
          BevelOuter = bvNone
          BorderStyle = bsSingle
          Caption = 'QR Marked'
          Color = clWhite
          ParentBackground = False
          TabOrder = 0
        end
      end
      object mmCom: TMemo
        Left = 520
        Top = 2
        Width = 303
        Height = 90
        Align = alRight
        ImeName = 'Microsoft IME 2010'
        ScrollBars = ssVertical
        TabOrder = 3
      end
    end
    object panSensor: TPanel
      Left = 1
      Top = 426
      Width = 825
      Height = 100
      Align = alClient
      TabOrder = 3
      ExplicitHeight = 16
      object Label37: TLabel
        Left = 446
        Top = 1
        Width = 47
        Height = 15
        Caption = 'Servo A:'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblDetect: TLabel
        Left = 314
        Top = 6
        Width = 30
        Height = 15
        Caption = 'Detct'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblFObslbl: TLabel
        Left = 369
        Top = 6
        Width = 26
        Height = 15
        Caption = ' Obs'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object panM1_F: TPanel
        Left = 438
        Top = 19
        Width = 34
        Height = 17
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '#1_F'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
      end
      object panM1_R: TPanel
        Left = 473
        Top = 19
        Width = 34
        Height = 17
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '#1_R'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
      end
      object panM2: TPanel
        Left = 438
        Top = 36
        Width = 34
        Height = 17
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '#2'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
      end
      object panM3: TPanel
        Left = 473
        Top = 36
        Width = 34
        Height = 17
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '#3'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
      end
      object panM4: TPanel
        Left = 438
        Top = 53
        Width = 34
        Height = 17
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '#4'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
      end
      object panDetect: TPanel
        Left = 307
        Top = 21
        Width = 44
        Height = 52
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 5
        object panUBGFail: TPanel
          Left = 4
          Top = 38
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
        end
        object panUBGShort: TPanel
          Left = 4
          Top = 26
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 1
        end
        object panUBGMiddle: TPanel
          Left = 4
          Top = 14
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 2
        end
        object panUBGLong: TPanel
          Left = 4
          Top = 2
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 3
        end
      end
      object panHoist_Home: TPanel
        Left = 4
        Top = 4
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'Hoist_Home'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 6
      end
      object panShift_Home: TPanel
        Left = 4
        Top = 22
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'Shift_HM'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 7
      end
      object panOscillation: TPanel
        Left = 196
        Top = 22
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'Oscillation'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 8
      end
      object panUTB: TPanel
        Left = 100
        Top = 40
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'UTB Sen'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 9
      end
      object panLookdown: TPanel
        Left = 196
        Top = 4
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'LookDnSen'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 10
      end
      object panFoupDetect: TPanel
        Left = 100
        Top = 4
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'R-POD Detect'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 11
      end
      object panHand_TP_Con: TPanel
        Left = 4
        Top = 40
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'HAND_TP_Con'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 12
      end
      object panEUV_Detect1: TPanel
        Left = 100
        Top = 22
        Width = 44
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'EUV_1'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 13
      end
      object panOHT_Comm_cd: TPanel
        Left = 196
        Top = 40
        Width = 90
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'OHT_Com_CD'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 14
      end
      object panFObspan: TPanel
        Left = 362
        Top = 21
        Width = 44
        Height = 52
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 15
        object panOBSFail: TPanel
          Left = 4
          Top = 38
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
        end
        object panOBSShort: TPanel
          Left = 4
          Top = 26
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 1
        end
        object panOBSMiddle: TPanel
          Left = 4
          Top = 14
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 2
        end
        object panOBSLong: TPanel
          Left = 4
          Top = 2
          Width = 37
          Height = 12
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 3
        end
      end
      object panEUV_Detect2: TPanel
        Left = 145
        Top = 22
        Width = 45
        Height = 18
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'EUV_2'
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 16
      end
    end
  end
  object panTop: TPanel
    Left = 0
    Top = 0
    Width = 827
    Height = 226
    Align = alTop
    Caption = 'panTop'
    TabOrder = 1
    object panName: TPanel
      Left = 1
      Top = 1
      Width = 825
      Height = 16
      Align = alTop
      BevelOuter = bvNone
      Color = clBlue
      Font.Charset = ANSI_CHARSET
      Font.Color = clYellow
      Font.Height = -13
      Font.Name = #48148#53461#52404
      Font.Style = [fsBold]
      ParentBackground = False
      ParentFont = False
      TabOrder = 0
      object panPDownPercent: TPanel
        Left = 786
        Top = 0
        Width = 39
        Height = 16
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 0
        object lblPMACDown: TLabel
          Left = 8
          Top = 3
          Width = 21
          Height = 13
          Caption = '000'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -13
          Font.Name = #48148#53461#52404
          Font.Style = []
          ParentFont = False
        end
      end
    end
    object Panel7: TPanel
      Left = 592
      Top = 17
      Width = 234
      Height = 208
      Align = alRight
      TabOrder = 1
      object pgcPosVel: TPageControl
        Left = 1
        Top = 1
        Width = 232
        Height = 127
        ActivePage = TabSheet6
        Align = alClient
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object TabSheet6: TTabSheet
          Caption = 'Pos'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object lblM4Caption: TLabel
            Left = 1
            Top = 76
            Width = 14
            Height = 15
            Caption = '#4'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label10: TLabel
            Left = 1
            Top = 56
            Width = 14
            Height = 15
            Caption = '#3'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label5: TLabel
            Left = 1
            Top = 36
            Width = 14
            Height = 15
            Caption = '#2'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label3: TLabel
            Left = 1
            Top = 8
            Width = 14
            Height = 15
            Caption = '#1'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object shpRun_Driving_F: TShape
            Left = 197
            Top = 1
            Width = 25
            Height = 15
            Pen.Color = clSilver
            Shape = stCircle
          end
          object shpRun_Driving_R: TShape
            Left = 197
            Top = 18
            Width = 25
            Height = 15
            Pen.Color = clSilver
            Shape = stCircle
          end
          object shpRun_Hoist: TShape
            Left = 197
            Top = 38
            Width = 25
            Height = 15
            Pen.Color = clSilver
            Shape = stCircle
          end
          object shpRun_Shift: TShape
            Left = 197
            Top = 58
            Width = 25
            Height = 15
            Pen.Color = clSilver
            Shape = stCircle
          end
          object shpRun_Rotate: TShape
            Left = 197
            Top = 78
            Width = 25
            Height = 15
            Pen.Color = clSilver
            Shape = stCircle
          end
          object panPosition_Driving_F: TPanel
            Left = 28
            Top = 0
            Width = 169
            Height = 18
            Alignment = taLeftJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            PopupMenu = pmDrivingFServoOnOff
            TabOrder = 0
            object lblMotor1_pos_F: TLabel
              Left = 141
              Top = 1
              Width = 22
              Height = 15
              Caption = 'mm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
          object panPosition_Hoist: TPanel
            Left = 28
            Top = 36
            Width = 169
            Height = 18
            Alignment = taLeftJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            PopupMenu = pmHoistServoOnOff
            TabOrder = 1
            object lblMotor2_pos: TLabel
              Left = 141
              Top = 1
              Width = 22
              Height = 15
              Caption = 'mm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
          object panPosition_Shift: TPanel
            Left = 28
            Top = 56
            Width = 169
            Height = 18
            Alignment = taLeftJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            PopupMenu = pmShiftServoOnOff
            TabOrder = 2
            object lblMotor3_pos: TLabel
              Left = 141
              Top = 1
              Width = 22
              Height = 15
              Caption = 'mm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
          object panPosition_Rotate: TPanel
            Left = 28
            Top = 76
            Width = 169
            Height = 18
            Alignment = taLeftJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            PopupMenu = pmRotate
            TabOrder = 3
            object lblMotor4_pos: TLabel
              Left = 141
              Top = 1
              Width = 4
              Height = 15
              Caption = #186
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
          object panPosition_Driving_R: TPanel
            Left = 28
            Top = 16
            Width = 169
            Height = 18
            Alignment = taLeftJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            PopupMenu = pmDrivingRServoOnOff
            TabOrder = 4
            object lblMotor1_pos_R: TLabel
              Left = 141
              Top = 1
              Width = 22
              Height = 15
              Caption = 'mm'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ParentFont = False
            end
          end
          object panAx1_F_ServoON: TPanel
            Left = 18
            Top = 0
            Width = 10
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clLime
            ParentBackground = False
            TabOrder = 5
          end
          object panAx1_R_ServoON: TPanel
            Left = 18
            Top = 16
            Width = 10
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clFuchsia
            ParentBackground = False
            TabOrder = 6
          end
          object panAx2_ServoON: TPanel
            Left = 18
            Top = 36
            Width = 10
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clFuchsia
            ParentBackground = False
            TabOrder = 7
          end
          object panAx3_ServoON: TPanel
            Left = 18
            Top = 56
            Width = 10
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clLime
            ParentBackground = False
            TabOrder = 8
          end
          object panAx4_ServoON: TPanel
            Left = 18
            Top = 76
            Width = 10
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Color = clLime
            ParentBackground = False
            TabOrder = 9
          end
        end
        object TabSheet7: TTabSheet
          Caption = 'Vel'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object Label29: TLabel
            Left = 0
            Top = 8
            Width = 16
            Height = 13
            Caption = '#1'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label31: TLabel
            Left = 0
            Top = 48
            Width = 16
            Height = 13
            Caption = '#2'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label33: TLabel
            Left = 0
            Top = 64
            Width = 16
            Height = 13
            Caption = '#3'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label35: TLabel
            Left = 0
            Top = 78
            Width = 16
            Height = 13
            Caption = '#4'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #48148#53461#52404
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label38: TLabel
            Left = 104
            Top = 8
            Width = 31
            Height = 15
            Caption = '[rpm]'
          end
          object Label39: TLabel
            Left = 104
            Top = 44
            Width = 31
            Height = 15
            Caption = '[rpm]'
          end
          object Label40: TLabel
            Left = 104
            Top = 60
            Width = 31
            Height = 15
            Caption = '[rpm]'
          end
          object Label41: TLabel
            Left = 104
            Top = 76
            Width = 31
            Height = 15
            Caption = '[rpm]'
          end
          object Label12: TLabel
            Left = 178
            Top = 8
            Width = 43
            Height = 15
            Caption = '[m/sec]'
          end
          object Panel29: TPanel
            Left = 18
            Top = 4
            Width = 87
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 0
            object lblVel1_F_rpm: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel30: TPanel
            Left = 18
            Top = 44
            Width = 87
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 4
            object lblVel2_rpm: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel31: TPanel
            Left = 18
            Top = 60
            Width = 87
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 5
            object lblVel3_rpm: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel32: TPanel
            Left = 18
            Top = 75
            Width = 87
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 6
            object lblVel4_rpm: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel36: TPanel
            Left = 138
            Top = 4
            Width = 39
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 1
            object lblVel1_F_mpersec: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel39: TPanel
            Left = 18
            Top = 20
            Width = 87
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 2
            object lblVel1_R_rpm: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
          object Panel42: TPanel
            Left = 138
            Top = 20
            Width = 39
            Height = 18
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 3
            object lblVel1_R_mpersec: TLabel
              Left = 8
              Top = 3
              Width = 7
              Height = 13
              Caption = '0'
              Font.Charset = ANSI_CHARSET
              Font.Color = clBlue
              Font.Height = -13
              Font.Name = #48148#53461#52404
              Font.Style = []
              ParentFont = False
            end
          end
        end
      end
      object Panel4: TPanel
        Left = 1
        Top = 128
        Width = 232
        Height = 79
        Align = alBottom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 2
        object lblM4Origin: TLabel
          Left = 80
          Top = 1
          Width = 40
          Height = 15
          Caption = 'Origin: '
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblEdc: TLabel
          Left = 4
          Top = 3
          Width = 24
          Height = 15
          Caption = 'Edc:'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblErec: TLabel
          Left = 4
          Top = 22
          Width = 22
          Height = 15
          Caption = 'Erc:'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblKw: TLabel
          Left = 4
          Top = 40
          Width = 24
          Height = 15
          Caption = 'Kw :'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object panVer: TPanel
          Left = 158
          Top = 2
          Width = 72
          Height = 75
          Align = alRight
          BevelInner = bvLowered
          Caption = '14'
          Color = 8454143
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -64
          Font.Name = #48148#53461#52404
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
        object panEdc: TPanel
          Left = 32
          Top = 2
          Width = 45
          Height = 19
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '100'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
        end
        object panErec: TPanel
          Left = 32
          Top = 21
          Width = 45
          Height = 19
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
        end
        object panHIDError: TPanel
          Left = 2
          Top = 59
          Width = 82
          Height = 19
          BevelInner = bvRaised
          BevelOuter = bvLowered
          ParentShowHint = False
          ShowHint = False
          TabOrder = 3
          object lblErrString: TLabel
            Left = 2
            Top = 1
            Width = 69
            Height = 15
            Caption = '0 0000 0000'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
        end
        object panState_F: TPanel
          Left = 84
          Top = 18
          Width = 20
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 4
        end
        object panState_R: TPanel
          Left = 84
          Top = 33
          Width = 20
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 5
        end
        object panState_H: TPanel
          Left = 84
          Top = 47
          Width = 20
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 6
        end
        object panState_S: TPanel
          Left = 84
          Top = 62
          Width = 20
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 7
        end
        object panSource_F: TPanel
          Left = 108
          Top = 18
          Width = 48
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0x0080'
          TabOrder = 8
        end
        object panSource_R: TPanel
          Left = 108
          Top = 33
          Width = 48
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 9
        end
        object panSource_H: TPanel
          Left = 108
          Top = 47
          Width = 48
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 10
        end
        object panSource_S: TPanel
          Left = 108
          Top = 62
          Width = 48
          Height = 14
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '0'
          TabOrder = 11
        end
        object panKw: TPanel
          Left = 32
          Top = 40
          Width = 45
          Height = 19
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 12
        end
        object panAx4_Origin: TPanel
          Left = 132
          Top = 1
          Width = 20
          Height = 16
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Color = clLime
          TabOrder = 13
        end
      end
      object Panel44: TPanel
        Left = 104
        Top = 6
        Width = 120
        Height = 16
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = #9632'ServoON/'#9675'Running'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
    end
    object Panel13: TPanel
      Left = 1
      Top = 17
      Width = 591
      Height = 208
      Align = alClient
      BevelOuter = bvNone
      Caption = 'Panel13'
      TabOrder = 2
      object PMACMemo: TRichEdit
        Left = 0
        Top = 0
        Width = 591
        Height = 177
        Align = alClient
        BorderStyle = bsNone
        Font.Charset = HANGEUL_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        HideSelection = False
        ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
        ParentFont = False
        PopupMenu = pmOriginSet
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
      end
      object Panel14: TPanel
        Left = 0
        Top = 177
        Width = 591
        Height = 31
        Align = alBottom
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = 16514793
        TabOrder = 1
        object lbl1: TLabel
          Left = 203
          Top = 10
          Width = 69
          Height = 17
          Caption = 'AMC Volt :'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = '@'#47569#51008' '#44256#46357
          Font.Style = [fsBold]
          ParentFont = False
        end
        object panTeachingSave: TPanel
          Left = 95
          Top = 4
          Width = 101
          Height = 22
          Caption = 'Teaching Save'
          Color = clBlue
          Font.Charset = ANSI_CHARSET
          Font.Color = clWhite
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
          OnClick = panTeachingSaveClick
        end
        object panPickupTemper: TPanel
          Left = 2
          Top = 2
          Width = 87
          Height = 27
          Align = alLeft
          BevelOuter = bvNone
          Color = 16744576
          TabOrder = 1
          object lblP1: TLabel
            Left = 48
            Top = 1
            Width = 24
            Height = 15
            Caption = '12.3'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lblP2: TLabel
            Left = 48
            Top = 14
            Width = 24
            Height = 15
            Caption = '25.6'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label59: TLabel
            Left = 0
            Top = 1
            Width = 46
            Height = 15
            Caption = 'PICKUP:'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lblIPCTemp: TLabel
            Left = 16
            Top = 14
            Width = 28
            Height = 15
            Caption = 'IPC  :'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
        end
        object InputVolt1: TPanel
          Left = 278
          Top = 3
          Width = 54
          Height = 27
          Caption = 'InputVolt1'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -10
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentFont = False
          TabOrder = 2
        end
        object InputVolt2: TPanel
          Left = 338
          Top = 3
          Width = 54
          Height = 27
          Caption = 'InputVolt2'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -10
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentFont = False
          TabOrder = 3
        end
        object InputVolt2_1: TPanel
          Left = 398
          Top = 3
          Width = 54
          Height = 27
          Caption = 'InputVolt2'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -10
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentFont = False
          TabOrder = 4
        end
        object InputVolt3: TPanel
          Left = 458
          Top = 2
          Width = 52
          Height = 28
          Caption = 'InputVolt3'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -10
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentFont = False
          TabOrder = 5
        end
        object InputVolt4: TPanel
          Left = 516
          Top = 2
          Width = 54
          Height = 28
          Caption = 'InputVolt4'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -10
          Font.Name = 'Arial'
          Font.Style = [fsBold, fsItalic]
          ParentFont = False
          TabOrder = 6
        end
      end
      object pnlFanFailAlarm: TPanel
        Left = 129
        Top = 54
        Width = 338
        Height = 64
        Caption = 'Fan Fail!!!'
        Color = clRed
        Font.Charset = ANSI_CHARSET
        Font.Color = clYellow
        Font.Height = -27
        Font.Name = 'Calibri'
        Font.Style = [fsBold, fsItalic]
        ParentBackground = False
        ParentFont = False
        TabOrder = 2
        Visible = False
      end
      object pnlJogMoveAlarm: TPanel
        Left = 129
        Top = 54
        Width = 338
        Height = 64
        Caption = 'JOG MOVING !!!!'
        Color = clRed
        Font.Charset = ANSI_CHARSET
        Font.Color = clYellow
        Font.Height = -27
        Font.Name = 'Calibri'
        Font.Style = [fsBold, fsItalic]
        ParentBackground = False
        ParentFont = False
        TabOrder = 3
        Visible = False
        object pnl1: TPanel
          Left = 8
          Top = 50
          Width = 338
          Height = 64
          Caption = 'Fan Fail!!!'
          Color = clRed
          Font.Charset = ANSI_CHARSET
          Font.Color = clYellow
          Font.Height = -27
          Font.Name = 'Calibri'
          Font.Style = [fsBold, fsItalic]
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
          Visible = False
        end
      end
    end
  end
  object panMid: TPanel
    Left = 0
    Top = 226
    Width = 827
    Height = 32
    Align = alTop
    TabOrder = 2
    object Panel25: TPanel
      Left = 794
      Top = 1
      Width = 32
      Height = 30
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 0
      object SpeedButton4: TSpeedButton
        Left = 4
        Top = 4
        Width = 23
        Height = 22
        Flat = True
        Glyph.Data = {
          6E020000424D6E0200000000000076000000280000002A000000150000000100
          040000000000F8010000CE0E0000C40E00001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00777777777777
          7777777777777777777777777777770000007777777778887777777777777777
          77FFFF7777777700000077777777E444877777777777777778888F7777777700
          000077777777E444877777777777777778778F7777777700000077777777E444
          877777777777777778778F77777777000000777777777EEE7777777777777777
          7888877777777700000077777777777777777777777777777777777777777700
          000077777777E888877777777777777777FFFF7777777700000077777777E444
          877777777777777778888F7777777700000077777777E4444877777777777777
          787778F7777777000000777777777E4444877777777777777787778F77777700
          00007777777777E4444877777777777777787778F7777700000077777777777E
          444877777777777777778778F7777700000077777E888777E4487777777777FF
          F7778778F7777700000077777E448777E4487777777777877F778778F7777700
          000077777E444878444877777777778777F87778F7777700000077777E444484
          444877777777778777777778F77777000000777777E4444444E7777777777778
          777777787777770000007777777EE444EE777777777777778877788777777700
          0000777777777EEE777777777777777777888777777777000000777777777777
          777777777777777777777777777777000000}
        NumGlyphs = 2
      end
    end
    object Panel27: TPanel
      Left = 1
      Top = 1
      Width = 793
      Height = 30
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      object Label57: TLabel
        Left = 10
        Top = 7
        Width = 37
        Height = 15
        Caption = 'Rotate'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Label25: TLabel
        Left = 400
        Top = 7
        Width = 36
        Height = 15
        Caption = 'Pause'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
      object Rotate0Lt: TPanel
        Left = 65
        Top = 3
        Width = 45
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label1: TLabel
          Left = 10
          Top = 3
          Width = 22
          Height = 15
          Caption = 'R_0'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object Rotate90Lt: TPanel
        Left = 109
        Top = 3
        Width = 45
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label15: TLabel
          Left = 8
          Top = 4
          Width = 29
          Height = 15
          Caption = 'R_90'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object Rotate180Lt: TPanel
        Left = 153
        Top = 3
        Width = 45
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label50: TLabel
          Left = 5
          Top = 4
          Width = 36
          Height = 15
          Caption = 'R_180'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object Rotate270Lt: TPanel
        Left = 197
        Top = 3
        Width = 45
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label51: TLabel
          Left = 4
          Top = 4
          Width = 36
          Height = 15
          Caption = 'R_270'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object RotateInposLt: TPanel
        Left = 241
        Top = 3
        Width = 54
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label52: TLabel
          Left = 3
          Top = 4
          Width = 46
          Height = 15
          Caption = 'R_Inpos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object RotateAlarmLt: TPanel
        Left = 294
        Top = 3
        Width = 54
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clWhite
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = #48148#53461#52404
        Font.Style = []
        ParentFont = False
        TabOrder = 5
        object TLabel
          Left = 5
          Top = 2
          Width = 3
          Height = 15
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
        object Label53: TLabel
          Left = 3
          Top = 4
          Width = 49
          Height = 15
          Caption = 'R_Alarm'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = True
        end
      end
      object RotateOriginLt: TPanel
        Left = 52
        Top = 3
        Width = 13
        Height = 23
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Color = clLime
        TabOrder = 6
      end
      object panTpPause: TPanel
        Left = 442
        Top = 3
        Width = 45
        Height = 23
        BevelWidth = 2
        Caption = 'TP'
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 7
      end
      object panOcsPause: TPanel
        Left = 485
        Top = 3
        Width = 45
        Height = 23
        BevelWidth = 2
        Caption = 'OCS'
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 8
      end
      object panCidPause: TPanel
        Left = 530
        Top = 3
        Width = 45
        Height = 23
        BevelWidth = 2
        Caption = 'CID'
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 9
      end
      object panPassPermit: TPanel
        Left = 635
        Top = 2
        Width = 121
        Height = 25
        BevelOuter = bvNone
        BorderStyle = bsSingle
        Caption = #53685#44284#54728#44032' '#51216#50976
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentBackground = False
        ParentFont = False
        TabOrder = 10
      end
    end
  end
  object pmOriginSet: TPopupMenu
    Left = 408
    Top = 24
    object mniMemoClear: TMenuItem
      Caption = 'Memo Clear'
      OnClick = mniMemoClearClick
    end
    object mniN1: TMenuItem
      Caption = '-'
    end
    object mniHoistOriginSet: TMenuItem
      Caption = '#2 Origin Setting'
      Hint = 'i226=0'#50640#49436#47564' '#44032#45733#54632
      OnClick = mniHoistOriginSetClick
    end
    object mniShiftOriginSet: TMenuItem
      Caption = '#3 Origin Setting'
      OnClick = mniShiftOriginSetClick
    end
    object mniRotateOriginSet: TMenuItem
      Caption = '#4 Origin Setting'
      OnClick = mniRotateOriginSetClick
    end
    object mniN2: TMenuItem
      Caption = '-'
    end
    object mniAbsEncoderReading: TMenuItem
      Caption = 'Abs. Encoder Reading'
    end
  end
  object timerUpdateDisplay: TTimer
    Interval = 200
    OnTimer = timerUpdateDisplayTimer
    Left = 32
    Top = 72
  end
  object timerTransLongRun: TTimer
    Enabled = False
    Interval = 500
    OnTimer = timerTransLongRunTimer
    Left = 32
    Top = 121
  end
  object pmDrivingFServoOnOff: TPopupMenu
    OnPopup = pmDrivingFServoOnOffPopup
    Left = 517
    Top = 21
    object mniDrvFServoOn: TMenuItem
      Caption = 'Driving_F : Servo ON'
      OnClick = mniDrvFServoOnClick
    end
    object mniDrvFServoOff: TMenuItem
      Caption = 'Driving_F : Servo OFF'
      OnClick = mniDrvFServoOffClick
    end
  end
  object pmDrivingRServoOnOff: TPopupMenu
    OnPopup = pmDrivingRServoOnOffPopup
    Left = 521
    Top = 67
    object mniDrivingRServoOn: TMenuItem
      Caption = 'Driving_R : Servo ON'
      OnClick = mniDrivingRServoOnClick
    end
    object mniDrivingRServoOff: TMenuItem
      Caption = 'Driving_R : Servo OFF'
      OnClick = mniDrivingRServoOffClick
    end
  end
  object pmHoistServoOnOff: TPopupMenu
    OnPopup = pmHoistServoOnOffPopup
    Left = 521
    Top = 115
    object mniHoistServoOn: TMenuItem
      Caption = 'Hoist : Servo ON'
      OnClick = mniHoistServoOnClick
    end
    object mniHoistServoOff: TMenuItem
      Caption = 'Hoist : Servo OFF'
      OnClick = mniHoistServoOffClick
    end
  end
  object pmShiftServoOnOff: TPopupMenu
    OnPopup = pmShiftServoOnOffPopup
    Left = 517
    Top = 157
    object mniShiftServoOn: TMenuItem
      Caption = 'Shift : Servo ON'
      OnClick = mniShiftServoOnClick
    end
    object mniShiftServoOff: TMenuItem
      Caption = 'Shift : Servo OFF'
      OnClick = mniShiftServoOffClick
    end
  end
  object pmRotate: TPopupMenu
    OnPopup = pmRotatePopup
    Left = 413
    Top = 147
    object mniRotateServoOn: TMenuItem
      Caption = 'Rotate : Enable'
      OnClick = mniRotateServoOnClick
    end
    object mniRotateServoOff: TMenuItem
      Caption = 'Rotate : Disable'
      OnClick = mniRotateServoOffClick
    end
  end
  object pmUnload: TPopupMenu
    Left = 233
    Top = 73
    object mniUnloadEQ: TMenuItem
      Caption = '[EQ]'
      OnClick = mniUnloadEQClick
    end
    object mniUnloadEQ_X: TMenuItem
      Caption = '[EQ_X]'
      OnClick = mniUnloadEQ_XClick
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object mniUnloadSTOCKER: TMenuItem
      Caption = '[STOCKER]'
      OnClick = mniUnloadSTOCKERClick
    end
    object mniUnloadLOADER: TMenuItem
      Caption = '[LOADER]'
      OnClick = mniUnloadLOADERClick
    end
    object N9: TMenuItem
      Caption = '-'
    end
    object mniUnloadSTB_L: TMenuItem
      Caption = '[STB_L]'
      OnClick = mniUnloadSTB_LClick
    end
    object mniUnloadSTB_R: TMenuItem
      Caption = '[STB_R]'
      OnClick = mniUnloadSTB_RClick
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object mniUnloadUTB: TMenuItem
      Caption = '[UTB]'
      OnClick = mniUnloadUTBClick
    end
  end
  object pmLoad: TPopupMenu
    Left = 185
    Top = 73
    object mniLoadEQ: TMenuItem
      Caption = '[EQ]'
      OnClick = mniLoadEQClick
    end
    object mniLoadEQ_X: TMenuItem
      Caption = '[EQ_X]'
      OnClick = mniLoadEQ_XClick
    end
    object N15: TMenuItem
      Caption = '-'
    end
    object mniLoadSTOCKER: TMenuItem
      Caption = '[STOCKER]'
      OnClick = mniLoadSTOCKERClick
    end
    object mniLoadLOADER: TMenuItem
      Caption = '[LOADER]'
      OnClick = mniLoadLOADERClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object mniLoadSTB_L: TMenuItem
      Caption = '[STB_L]'
      OnClick = mniLoadSTB_LClick
    end
    object mniLoadSTB_R: TMenuItem
      Caption = '[STB_R]'
      OnClick = mniLoadSTB_RClick
    end
    object N32: TMenuItem
      Caption = '-'
    end
    object mniLoadUTB: TMenuItem
      Caption = '[UTB]'
      OnClick = mniLoadUTBClick
    end
  end
  object timerDrivingJog: TTimer
    Enabled = False
    Interval = 300
    OnTimer = timerDrivingJogTimer
    Left = 32
    Top = 24
  end
  object pmMoveTP: TPopupMenu
    Left = 104
    Top = 25
    object mniMoveTPEQ: TMenuItem
      Caption = '[EQ]'
      OnClick = mniMoveTPEQClick
    end
    object mniMoveTPEQ_X: TMenuItem
      Caption = '[EQ_X]'
      OnClick = mniMoveTPEQ_XClick
    end
    object mni3: TMenuItem
      Caption = '-'
    end
    object mniMoveTPSTOCKER: TMenuItem
      Caption = '[STOCKER]'
      OnClick = mniMoveTPSTOCKERClick
    end
    object mniMoveTPLOADER: TMenuItem
      Caption = '[LOADER]'
      OnClick = mniMoveTPLOADERClick
    end
    object mni6: TMenuItem
      Caption = '-'
    end
    object mniMoveTPSTB_L: TMenuItem
      Caption = '[STB_L]'
      OnClick = mniMoveTPSTB_LClick
    end
    object mniMoveTPSTB_R: TMenuItem
      Caption = '[STB_R]'
      OnClick = mniMoveTPSTB_RClick
    end
    object mni9: TMenuItem
      Caption = '-'
    end
    object mniMoveTPUTB: TMenuItem
      Caption = '[UTB]'
      OnClick = mniMoveTPUTBClick
    end
  end
  object tmrTransRun: TTimer
    OnTimer = tmrTransRunTimer
    Left = 112
    Top = 120
  end
  object tmrCidInfoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrCidInfoReadTimer
    Left = 192
    Top = 120
  end
end
