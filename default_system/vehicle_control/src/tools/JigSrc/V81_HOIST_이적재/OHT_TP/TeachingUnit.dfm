object TeachingForm: TTeachingForm
  Left = 514
  Top = 386
  Caption = 'TeachingForm'
  ClientHeight = 335
  ClientWidth = 601
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    601
    335)
  PixelsPerInch = 96
  TextHeight = 13
  object Label5: TLabel
    Left = 65
    Top = 195
    Width = 34
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO ID'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 30
    Top = 210
    Width = 69
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO Channel'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label7: TLabel
    Left = 50
    Top = 179
    Width = 49
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO Type'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label8: TLabel
    Left = 58
    Top = 187
    Width = 49
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO Type'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label9: TLabel
    Left = 73
    Top = 203
    Width = 34
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO ID'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label10: TLabel
    Left = 38
    Top = 218
    Width = 69
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'PIO Channel'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object panAutoTeaching: TPanel
    Left = 0
    Top = 332
    Width = 601
    Height = 3
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = clAqua
    TabOrder = 0
    object Label2: TLabel
      Left = 16
      Top = 72
      Width = 100
      Height = 15
      Caption = 'Auto Teaching IP :'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label14: TLabel
      Left = 16
      Top = 6
      Width = 131
      Height = 15
      Caption = 'Auto Teaching '#51652#54665#49345#54889
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object btnATConnect: TButton
      Left = 164
      Top = 92
      Width = 90
      Height = 24
      Caption = 'Connect'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = btnATConnectClick
    end
    object btnATRead: TButton
      Left = 359
      Top = 93
      Width = 55
      Height = 50
      Caption = 'Read'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      OnClick = btnATReadClick
    end
    object btnATMove: TButton
      Left = 419
      Top = 93
      Width = 55
      Height = 50
      Caption = 'Move'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      OnClick = btnATMoveClick
    end
    object btnATReadMove: TButton
      Left = 479
      Top = 93
      Width = 105
      Height = 49
      Caption = 'Read/Move'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = btnATReadMoveClick
    end
    object edIP: TEdit
      Left = 13
      Top = 91
      Width = 145
      Height = 23
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
      ParentFont = False
      TabOrder = 4
      Text = '127.0.0.1'
    end
    object btnATDiscon: TButton
      Left = 164
      Top = 119
      Width = 90
      Height = 24
      Caption = 'Disconnect'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 5
      OnClick = btnATDisconClick
    end
    object btn12mmUp: TButton
      Left = 273
      Top = 92
      Width = 81
      Height = 24
      Caption = '12mm Up'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 6
      OnClick = btn12mmUpClick
    end
    object btn12mmDown: TButton
      Left = 273
      Top = 119
      Width = 81
      Height = 24
      Caption = '12mm Dn'
      Font.Charset = HANGEUL_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 7
      OnClick = btn12mmDownClick
    end
    object btnTeachStart: TButton
      Left = 368
      Top = 14
      Width = 105
      Height = 33
      Caption = 'Teaching Start'
      TabOrder = 8
      OnClick = btnTeachStartClick
    end
    object btnTeachStop: TButton
      Left = 479
      Top = 16
      Width = 105
      Height = 33
      Caption = 'Teaching Stop'
      TabOrder = 9
      OnClick = btnTeachStopClick
    end
    object edtTeaching: TEdit
      Left = 16
      Top = 24
      Width = 346
      Height = 21
      ImeName = 'Microsoft IME 2010'
      TabOrder = 10
    end
    object cboParam: TComboBox
      Left = 141
      Top = 51
      Width = 221
      Height = 21
      Style = csDropDownList
      ImeName = 'Microsoft IME 2010'
      ItemIndex = 0
      TabOrder = 11
      Text = 'UPDATE_DRIVING_INI'#9
      Visible = False
      Items.Strings = (
        'UPDATE_DRIVING_INI'#9
        'UPDATE_TRANS_INI'#9
        'UPDATE_EQPIO_INI'#9
        'UPDATE_DIAG_PARAM_INI'#9
        'UPDATE_PATHSEARCH_INI'#9
        'UPDATE_IOMAP_INI'#9
        'MODE CHANGE (AUTO -> MANUAL)'
        'AMC Update and Restart'
        'AUTO MODE CHANGE (MANUAL -> AUTO MARK -> AUTO)')
    end
    object Button1: TButton
      Left = 368
      Top = 51
      Width = 137
      Height = 25
      Caption = #54028#46972#48120#53552' apply test'
      TabOrder = 12
      Visible = False
      OnClick = Button1Click
    end
  end
  object pgcTeaching: TPageControl
    Left = 0
    Top = 0
    Width = 601
    Height = 332
    ActivePage = tabTeachingData
    Align = alTop
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object tabJog: TTabSheet
      Caption = 'Jog'
      object panTop: TPanel
        Left = 0
        Top = 0
        Width = 593
        Height = 273
        Align = alTop
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = 'panTop'
        TabOrder = 0
        object grpHoistTeaching: TGroupBox
          Left = 2
          Top = 2
          Width = 197
          Height = 269
          Align = alLeft
          Caption = ' #2(Hoist) '
          Color = 15662830
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentColor = False
          ParentFont = False
          TabOrder = 0
          object lblHoistPos: TLabel
            Left = 4
            Top = 24
            Width = 42
            Height = 15
            Caption = '#2 Pos:'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Panel10: TPanel
            Left = 66
            Top = 13
            Width = 118
            Height = 20
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
            object lbHoistCurCts: TLabel
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
            object shpHoistRunning: TShape
              Left = 95
              Top = 1
              Width = 25
              Height = 17
              Pen.Color = clSilver
              Shape = stCircle
            end
          end
          object btnHoistMoveHome: TBitBtn
            Left = 4
            Top = 98
            Width = 72
            Height = 56
            Caption = '#2'
            DoubleBuffered = True
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
            Layout = blGlyphRight
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 1
            OnClick = btnHoistMoveHomeClick
          end
          object btnHoistMoveUp: TBitBtn
            Left = 82
            Top = 98
            Width = 109
            Height = 28
            Caption = '&#2   UP[-]'
            DoubleBuffered = True
            Glyph.Data = {
              F6000000424DF600000000000000760000002800000010000000100000000100
              0400000000008000000000000000000000001000000010000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333333333333333337777733333333334444473333333333CCCC473
              333333333CCCC473333333333CCCC473333333333CCCC473333333333CCCC477
              7333333CCCCCCCCC33333333CCCCCCC3333333333CCCCC333333333333CCC333
              33333333333C3333333333333333333333333333333333333333}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 2
            OnMouseDown = btnHoistMoveUpMouseDown
            OnMouseUp = btnHoistMoveUpMouseUp
          end
          object btnHoistMoveDown: TBitBtn
            Left = 82
            Top = 126
            Width = 109
            Height = 28
            Caption = '&#2 DOWN[+]'
            DoubleBuffered = True
            Glyph.Data = {
              F6000000424DF600000000000000760000002800000010000000100000000100
              0400000000008000000000000000000000001000000010000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              333333333333733333333333333477333333333333CC4773333333333CCCC477
              33333333CCCCCC477333333CCCCCC444333333333CCCC473333333333CCCC473
              333333333CCCC473333333333CCCC473333333333CCCC473333333333CCCC433
              3333333333333333333333333333333333333333333333333333}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 3
            OnMouseDown = btnHoistMoveDownMouseDown
            OnMouseUp = btnHoistMoveUpMouseUp
          end
          object edtHoistNegLimit: TEdit
            Left = 8
            Top = 51
            Width = 81
            Height = 23
            Color = 14737632
            ImeName = 'Microsoft IME 2003'
            TabOrder = 4
            Text = '-1000'
          end
          object btnSetHoistNegLimit: TButton
            Left = 94
            Top = 46
            Width = 97
            Height = 20
            Caption = 'Set [-] Limit'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 5
            OnClick = btnSetHoistNegLimitClick
          end
          object edtHoistPosLimit: TEdit
            Left = 8
            Top = 72
            Width = 81
            Height = 23
            Color = 16764647
            ImeName = 'Microsoft IME 2003'
            TabOrder = 6
            Text = '1000000'
          end
          object btnSetHoistPosLimit: TButton
            Left = 92
            Top = 72
            Width = 97
            Height = 20
            Caption = 'Set [+] Limit'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 7
            OnClick = btnSetHoistPosLimitClick
          end
          object panHoistMotor: TPanel
            Left = 66
            Top = 31
            Width = 118
            Height = 20
            Alignment = taRightJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = 'mm'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 8
            object lbHoistCurPos: TLabel
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
          object grpHoistJog: TGroupBox
            Left = 5
            Top = 157
            Width = 187
            Height = 108
            Caption = 'Hoist Jog Command(mm)'
            TabOrder = 9
            object chkHoistJogABS: TCheckBox
              Left = 13
              Top = 18
              Width = 70
              Height = 17
              Caption = #51208#45824#44050' = '
              TabOrder = 0
              OnMouseUp = chkHoistJogABSMouseUp
            end
            object chkHoistJogDist: TCheckBox
              Left = 13
              Top = 41
              Width = 70
              Height = 17
              Caption = #49345#45824#44050' = '
              TabOrder = 1
              OnMouseUp = chkHoistJogDistMouseUp
            end
            object edtHoistJogABS: TEdit
              Tag = 1
              Left = 89
              Top = 15
              Width = 93
              Height = 23
              Color = 12189695
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 2
              Text = '0'
              OnClick = edtHoistJogABSClick
            end
            object edtHoistJogDist: TEdit
              Tag = 2
              Left = 89
              Top = 38
              Width = 77
              Height = 23
              Color = 13688831
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 3
              Text = '1'
              OnClick = edtHoistJogDistClick
            end
            object udHoistDist: TUpDown
              Tag = 2
              Left = 166
              Top = 38
              Width = 16
              Height = 23
              Associate = edtHoistJogDist
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 4
              OnChanging = udHoistDistChanging
            end
            object btnHoistMoveUpDist: TBitBtn
              Left = 3
              Top = 67
              Width = 83
              Height = 30
              Caption = '&UP[2]'
              DoubleBuffered = True
              Glyph.Data = {
                F6000000424DF600000000000000760000002800000010000000100000000100
                0400000000008000000000000000000000001000000010000000000000000000
                8000008000000080800080000000800080008080000080808000C0C0C0000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
                333333333333333333333333337777733333333334444473333333333CCCC473
                333333333CCCC473333333333CCCC473333333333CCCC473333333333CCCC477
                7333333CCCCCCCCC33333333CCCCCCC3333333333CCCCC333333333333CCC333
                33333333333C3333333333333333333333333333333333333333}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 5
              OnClick = btnHoistMoveUpDistClick
            end
            object btnHoistMoveDownDist: TBitBtn
              Left = 101
              Top = 67
              Width = 83
              Height = 30
              Caption = '&DN[2]'
              DoubleBuffered = True
              Glyph.Data = {
                F6000000424DF600000000000000760000002800000010000000100000000100
                0400000000008000000000000000000000001000000010000000000000000000
                8000008000000080800080000000800080008080000080808000C0C0C0000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
                333333333333733333333333333477333333333333CC4773333333333CCCC477
                33333333CCCCCC477333333CCCCCC444333333333CCCC473333333333CCCC473
                333333333CCCC473333333333CCCC473333333333CCCC473333333333CCCC433
                3333333333333333333333333333333333333333333333333333}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 6
              OnClick = btnHoistMoveDownDistClick
            end
            object btnHoistMoveABS: TBitBtn
              Left = 34
              Top = 67
              Width = 105
              Height = 30
              Caption = '&MOVE[2]'
              DoubleBuffered = True
              Glyph.Data = {
                76010000424D760100000000000076000000280000001B000000100000000100
                0400000000000001000000000000000000001000000000000000000000000000
                8000008000000080800080000000800080008080000080808000C0C0C0000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
                3333333333333330000033333333333333333337333333300000333333777773
                3333334773333330000033333444447333333CC477333330000033333CCCC473
                3333CCCC47733330000033333CCCC473333CCCCCC4773330000033333CCCC473
                33CCCCCC44433330000033333CCCC4733333CCCC47333330000033333CCCC477
                3333CCCC473333300000333CCCCCCCCC3333CCCC4733333000003333CCCCCCC3
                3333CCCC47333330000033333CCCCC333333CCCC473333300000333333CCC333
                3333CCCC4333333000003333333C333333333333333333300000333333333333
                3333333333333330000033333333333333333333333333300000}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 7
              OnClick = btnHoistMoveABSClick
            end
          end
        end
        object grpRotateTeaching: TGroupBox
          Left = 396
          Top = 2
          Width = 197
          Height = 269
          Align = alLeft
          Caption = ' #4(Rotate) '
          Color = 15662830
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentColor = False
          ParentFont = False
          TabOrder = 1
          object lblRotatePos: TLabel
            Left = 4
            Top = 24
            Width = 42
            Height = 15
            Caption = '#4 Pos:'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Panel3: TPanel
            Left = 66
            Top = 12
            Width = 118
            Height = 20
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
            object lblMotor4: TLabel
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
            object shp4Run: TShape
              Left = 95
              Top = 1
              Width = 25
              Height = 17
              Pen.Color = clSilver
              Shape = stCircle
            end
          end
          object btnMotor4Home: TBitBtn
            Left = 4
            Top = 98
            Width = 72
            Height = 56
            Caption = '#4'
            DoubleBuffered = True
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
            Layout = blGlyphRight
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 1
            OnClick = btnMotor4HomeClick
          end
          object btn4CCWCmd: TBitBtn
            Left = 82
            Top = 126
            Width = 109
            Height = 28
            Caption = '&#4CCW[-]'
            DoubleBuffered = True
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              180000000000000300000000000000000000000000000000000000BFBF00BFBF
              00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF
              0000FF0000FF0000FF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF
              00BFBF00BFBFFF0000FF0000BF0000BF0000BF0000BF0000BF0000FF0000BF00
              0000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00
              BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
              FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF00
              00FF0000BF000000BFBF00BFBF00BFBFFF0000BF000000BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBFFF0000
              BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
              BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
              BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBFFF0000BF000000BFBF00BFBFFF0000BF000000BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBFFF000000BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF
              FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF00
              00FF0000BF000000BFBF00BFBF00BFBF00BFBFBF0000BF000000BFBF00BFBF00
              BFBF00BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
              00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
              00BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBFBF0000BF0000BF0000BF0000BF000000BFBF}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 2
            OnMouseDown = btn4CCWCmdMouseDown
            OnMouseUp = btn4CWCmdMouseUp
          end
          object btn4CWCmd: TBitBtn
            Left = 82
            Top = 98
            Width = 109
            Height = 28
            Caption = '&#4 CW[+]'
            DoubleBuffered = True
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              180000000000000300000000000000000000000000000000000000BFBF00BFBF
              00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFBF0000BF0000BF00
              00BF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
              00BFBFBF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
              00BF000000BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00
              BFBF00BFBF00BFBFFF0000FF0000BF0000FF0000BF000000BFBF00BFBF00BFBF
              FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF000000BFBF00BF
              BFFF0000FF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
              BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
              BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBF00BFBF
              FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
              BFFF0000FF0000BF000000BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF
              00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF00
              00BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF0000BF
              0000BF0000BF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF
              00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF0000FF0000FF000000BFBF00BF
              BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
              BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 3
            OnMouseDown = btn4CWCmdMouseDown
            OnMouseUp = btn4CWCmdMouseUp
          end
          object panRotateMotor: TPanel
            Left = 66
            Top = 30
            Width = 118
            Height = 20
            Alignment = taRightJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = #186'    '
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 4
            object lblDMotor4: TLabel
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
          object btnRotateOrigin: TButton
            Left = 120
            Top = 52
            Width = 65
            Height = 25
            Caption = 'Set Origin'
            Font.Charset = ANSI_CHARSET
            Font.Color = clBlue
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 5
            OnClick = btnRotateOriginClick
          end
          object grpRotateJog: TGroupBox
            Left = 5
            Top = 157
            Width = 187
            Height = 108
            Caption = 'Rotate Jog Command'
            TabOrder = 6
            object chkRotateJogABS: TCheckBox
              Left = 13
              Top = 18
              Width = 70
              Height = 17
              Caption = #51208#45824#44050' = '
              TabOrder = 0
              OnMouseUp = chkRotateJogABSMouseUp
            end
            object chkRotateJogDist: TCheckBox
              Left = 13
              Top = 41
              Width = 70
              Height = 17
              Caption = #49345#45824#44050' = '
              TabOrder = 1
              OnMouseUp = chkRotateJogDistMouseUp
            end
            object edtRotateJogABS: TEdit
              Tag = 1
              Left = 89
              Top = 15
              Width = 93
              Height = 23
              Color = 12189695
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 2
              Text = '0'
              OnClick = edtRotateJogABSClick
            end
            object edtRotateJogDist: TEdit
              Tag = 2
              Left = 89
              Top = 38
              Width = 77
              Height = 23
              Color = 13688831
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 3
              Text = '1'
              OnClick = edtHoistJogABSClick
            end
            object udRotateDist: TUpDown
              Tag = 2
              Left = 166
              Top = 38
              Width = 16
              Height = 23
              Associate = edtRotateJogDist
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 4
              OnChanging = udRotateDistChanging
            end
            object btnRotateMoveRight: TBitBtn
              Left = 101
              Top = 67
              Width = 83
              Height = 30
              Caption = '&CCW[4]'
              DoubleBuffered = True
              Glyph.Data = {
                36030000424D3603000000000000360000002800000010000000100000000100
                180000000000000300000000000000000000000000000000000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF
                0000FF0000FF0000FF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF
                00BFBF00BFBFFF0000FF0000BF0000BF0000BF0000BF0000BF0000FF0000BF00
                0000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00
                BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
                FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF00
                00FF0000BF000000BFBF00BFBF00BFBFFF0000BF000000BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBF00BFBFFF0000BF000000BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBFFF000000BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF
                FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF00
                00FF0000BF000000BFBF00BFBF00BFBF00BFBFBF0000BF000000BFBF00BFBF00
                BFBF00BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
                00BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBFBF0000BF0000BF0000BF0000BF000000BFBF}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 6
              OnClick = btnRotateMoveRightClick
            end
            object btnRotateMoveLeft: TBitBtn
              Left = 3
              Top = 67
              Width = 83
              Height = 30
              Caption = '&CW[4]'
              DoubleBuffered = True
              Glyph.Data = {
                36030000424D3603000000000000360000002800000010000000100000000100
                180000000000000300000000000000000000000000000000000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFBF0000BF0000BF00
                00BF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF
                00BFBFBF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
                00BF000000BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00
                BFBF00BFBF00BFBFFF0000FF0000BF0000FF0000BF000000BFBF00BFBF00BFBF
                FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF000000BFBF00BF
                BFFF0000FF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000BFBF00BFBF
                FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BFFF0000FF0000BF000000BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF
                00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF00
                00BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF0000BF
                0000BF0000BF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF0000FF0000FF000000BFBF00BF
                BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 7
              OnClick = btnRotateMoveLeftClick
            end
            object btnRotateMoveABS: TBitBtn
              Left = 45
              Top = 67
              Width = 105
              Height = 30
              Caption = '&MOVE[4]'
              DoubleBuffered = True
              Glyph.Data = {
                36060000424D360600000000000036000000280000001F000000100000000100
                180000000000000600000000000000000000000000000000000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFBF0000BF0000BF00
                00BF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
                00BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF
                0000FF0000FF000000BFBF00BFBF00BFBF00BFBF00BFBF00000000BFBF00BFBF
                00BFBFBF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF00
                00BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF0000BF0000BF
                0000BF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00000000BFBF00BFBF
                FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF00
                00FF0000BF000000BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00
                BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00000000BFBF00BFBF
                FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF000000BFBF00BF
                BFFF0000FF0000BF000000BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF000000BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF0000BF000000000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000000000BFBFFF0000
                BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBFFF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000000000BFBF00BFBF
                FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BFFF0000FF0000BF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF0000BF000000000000BFBF00BFBF
                FF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBFFF00
                00FF0000BF000000BFBF00BFBFFF0000BF000000BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBFFF000000BFBF00BFBFFF0000FF0000BF000000000000BFBF00BFBF
                00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00BFBF00BFBFFF0000FF00
                00BF000000BFBF00BFBF00BFBFFF0000FF0000BF000000BFBF00BFBF00BFBF00
                BFBF00BFBFFF0000FF0000BF0000FF0000BF000000BFBF00000000BFBF00BFBF
                00BFBF00BFBFFF0000FF0000BF0000BF0000BF0000BF0000BF0000FF0000BF00
                0000BFBF00BFBF00BFBF00BFBF00BFBFBF0000BF000000BFBF00BFBF00BFBF00
                BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBFFF0000FF0000FF0000FF0000FF000000BFBF00BF
                BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBFFF0000FF0000FF0000BF000000BFBF00BFBF00000000BFBF00BFBF
                00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BF
                BF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00BFBF00
                BFBF00BFBFBF0000BF0000BF0000BF0000BF000000BFBF000000}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 5
              OnClick = btnRotateMoveABSClick
            end
          end
        end
        object grpShiftTeaching: TGroupBox
          Left = 199
          Top = 2
          Width = 197
          Height = 269
          Align = alLeft
          Caption = ' #3(Shift) '
          Color = 15654638
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentColor = False
          ParentFont = False
          TabOrder = 2
          object lblShiftPos: TLabel
            Left = 4
            Top = 24
            Width = 42
            Height = 15
            Caption = '#3 Pos:'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Panel4: TPanel
            Left = 66
            Top = 12
            Width = 118
            Height = 20
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
            object lbShiftCurCts: TLabel
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
            object shpShiftRunning: TShape
              Left = 95
              Top = 1
              Width = 25
              Height = 17
              Pen.Color = clSilver
              Shape = stCircle
            end
          end
          object btnShiftMoveHome: TBitBtn
            Left = 4
            Top = 98
            Width = 72
            Height = 56
            Caption = '#3'
            DoubleBuffered = True
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
            Layout = blGlyphRight
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 1
            OnClick = btnShiftMoveHomeClick
          end
          object btnShiftMoveRight: TBitBtn
            Left = 82
            Top = 98
            Width = 109
            Height = 28
            Caption = '&#3 RIGHT[+]'
            DoubleBuffered = True
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              1800000000000003000000000000000000000000000000000000008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080808080008080008080008080008080008080008080008080008080008080
              008080008080008080008080008080008080808080FF00000080800080800080
              8000808000808000808000808000808080808080808080808080808080808080
              8080808080FF0000FF0000008080008080008080008080008080008080008080
              808080800000800000800000800000800000800000FF0000FF0000FF00000080
              80008080008080008080008080008080808080800000FF0000FF0000FF0000FF
              0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
              808080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00
              00FF0000008080008080008080008080808080800000FF0000FF0000FF0000FF
              0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
              008080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00000080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080FF0000FF0000008080008080008080008080008080008080008080
              008080008080008080008080008080008080008080FF00000080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 2
            OnMouseDown = btnShiftMoveRightMouseDown
            OnMouseUp = btnShiftMoveLeftMouseUp
          end
          object btnShiftMoveLeft: TBitBtn
            Left = 82
            Top = 126
            Width = 109
            Height = 28
            Caption = '&#3  LEFT[-]'
            DoubleBuffered = True
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              1800000000000003000000000000000000000000000000000000008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808080
              8080008080008080008080008080008080008080008080008080008080008080
              008080008080008080008080FF00008080800080800080800080800080800080
              80008080008080008080008080008080008080008080008080FF0000FF000080
              8080808080808080808080808080808080808080008080008080008080008080
              008080008080FF0000FF0000FF00008000008000008000008000008000008000
              00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
              0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
              FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
              00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
              0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
              008080008080FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
              00008080008080008080008080008080008080008080008080FF0000FF000000
              8080008080008080008080008080008080008080008080008080008080008080
              008080008080008080008080FF00000080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080008080008080
              0080800080800080800080800080800080800080800080800080800080800080
              8000808000808000808000808000808000808000808000808000808000808000
              8080008080008080008080008080008080008080008080008080}
            ModalResult = 8
            ParentDoubleBuffered = False
            TabOrder = 3
            OnMouseDown = btnShiftMoveLeftMouseDown
            OnMouseUp = btnShiftMoveLeftMouseUp
          end
          object edtShiftNegLimit: TEdit
            Left = 8
            Top = 51
            Width = 81
            Height = 23
            Color = 14737632
            ImeName = 'Microsoft IME 2003'
            TabOrder = 4
            Text = '-25600'
          end
          object edtShiftPosLimit: TEdit
            Left = 8
            Top = 71
            Width = 81
            Height = 23
            Color = 16764647
            ImeName = 'Microsoft IME 2003'
            TabOrder = 5
            Text = '415000'
          end
          object btnSetShiftNegLimit: TButton
            Left = 92
            Top = 51
            Width = 97
            Height = 20
            Caption = 'Set [-] Limit'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 6
            OnClick = btnSetShiftNegLimitClick
          end
          object btnSetShiftPosLimit: TButton
            Left = 92
            Top = 71
            Width = 97
            Height = 20
            Caption = 'Set [+] Limit'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 7
            OnClick = btnSetShiftPosLimitClick
          end
          object panShiftMotor: TPanel
            Left = 66
            Top = 30
            Width = 118
            Height = 20
            Alignment = taRightJustify
            BevelInner = bvRaised
            BevelOuter = bvLowered
            Caption = 'mm'
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Arial'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 8
            object lbShiftCurPos: TLabel
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
          object grpShiftJog: TGroupBox
            Left = 5
            Top = 157
            Width = 187
            Height = 108
            Caption = 'Shift Jog Command(mm)'
            TabOrder = 9
            object chkShiftJogABS: TCheckBox
              Left = 13
              Top = 18
              Width = 70
              Height = 17
              Caption = #51208#45824#44050' = '
              TabOrder = 0
              OnMouseUp = chkShiftJogABSMouseUp
            end
            object chkShiftJogDist: TCheckBox
              Left = 13
              Top = 41
              Width = 70
              Height = 17
              Caption = #49345#45824#44050' = '
              TabOrder = 1
              OnMouseUp = chkShiftJogDistMouseUp
            end
            object edtShiftJogABS: TEdit
              Tag = 1
              Left = 89
              Top = 15
              Width = 93
              Height = 23
              Color = 12189695
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 2
              Text = '0'
              OnClick = edtShiftJogABSClick
            end
            object edtShiftJogDist: TEdit
              Tag = 2
              Left = 89
              Top = 38
              Width = 77
              Height = 23
              Color = 13688831
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -12
              Font.Name = 'Arial'
              Font.Style = [fsBold]
              ImeName = #54620#44397#50612' '#51077#47141' '#49884#49828#53596' (IME 2000)'
              ParentFont = False
              TabOrder = 3
              Text = '1'
              OnClick = edtShiftJogDistClick
            end
            object udShiftDist: TUpDown
              Tag = 2
              Left = 166
              Top = 38
              Width = 16
              Height = 23
              Associate = edtShiftJogDist
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 4
              OnChanging = udShiftDistChanging
            end
            object btnShiftMoveLeftDist: TBitBtn
              Left = 3
              Top = 67
              Width = 83
              Height = 30
              Caption = '&LF[3]'
              DoubleBuffered = True
              Glyph.Data = {
                36030000424D3603000000000000360000002800000010000000100000000100
                1800000000000003000000000000000000000000000000000000008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808080
                8080008080008080008080008080008080008080008080008080008080008080
                008080008080008080008080FF00008080800080800080800080800080800080
                80008080008080008080008080008080008080008080008080FF0000FF000080
                8080808080808080808080808080808080808080008080008080008080008080
                008080008080FF0000FF0000FF00008000008000008000008000008000008000
                00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
                FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
                00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
                008080008080FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
                00008080008080008080008080008080008080008080008080FF0000FF000000
                8080008080008080008080008080008080008080008080008080008080008080
                008080008080008080008080FF00000080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 5
              OnClick = btnShiftMoveLeftDistClick
            end
            object btnShiftMoveRightDist: TBitBtn
              Left = 101
              Top = 67
              Width = 83
              Height = 30
              Caption = '&RT[3]'
              DoubleBuffered = True
              Glyph.Data = {
                36030000424D3603000000000000360000002800000010000000100000000100
                1800000000000003000000000000000000000000000000000000008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080808080008080008080008080008080008080008080008080008080008080
                008080008080008080008080008080008080808080FF00000080800080800080
                8000808000808000808000808000808080808080808080808080808080808080
                8080808080FF0000FF0000008080008080008080008080008080008080008080
                808080800000800000800000800000800000800000FF0000FF0000FF00000080
                80008080008080008080008080008080808080800000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
                808080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00
                00FF0000008080008080008080008080808080800000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
                008080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00000080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080FF0000FF0000008080008080008080008080008080008080008080
                008080008080008080008080008080008080008080FF00000080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 7
              OnClick = btnShiftMoveRightDistClick
            end
            object btnShiftMoveABS: TBitBtn
              Left = 45
              Top = 67
              Width = 105
              Height = 30
              Caption = '&MOVE[3]'
              DoubleBuffered = True
              Glyph.Data = {
                76050000424D76050000000000003600000028000000100000001C0000000100
                1800000000004005000000000000000000000000000000000000008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080808080008080008080008080008080008080008080008080008080008080
                008080008080008080008080008080008080808080FF00000080800080800080
                8000808000808000808000808000808080808080808080808080808080808080
                8080808080FF0000FF0000008080008080008080008080008080008080008080
                808080800000800000800000800000800000800000FF0000FF0000FF00000080
                80008080008080008080008080008080808080800000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
                808080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00
                00FF0000008080008080008080008080808080800000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000FF0000008080008080008080008080008080
                008080800000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00000080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080FF0000FF0000008080008080008080008080008080008080008080
                008080008080008080008080008080008080008080FF00000080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                008080008080008080008080FF00008080800080800080800080800080800080
                80008080008080008080008080008080008080008080008080FF0000FF000080
                8080808080808080808080808080808080808080008080008080008080008080
                008080008080FF0000FF0000FF00008000008000008000008000008000008000
                00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
                FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
                00808080008080008080008080008080008080FF0000FF0000FF0000FF0000FF
                0000FF0000FF0000FF0000FF0000800000808080008080008080008080008080
                008080008080FF0000FF0000FF0000FF0000FF0000FF0000FF0000FF00008000
                00008080008080008080008080008080008080008080008080FF0000FF000000
                8080008080008080008080008080008080008080008080008080008080008080
                008080008080008080008080FF00000080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080008080008080
                0080800080800080800080800080800080800080800080800080800080800080
                8000808000808000808000808000808000808000808000808000808000808000
                8080008080008080008080008080008080008080008080008080}
              ModalResult = 8
              ParentDoubleBuffered = False
              TabOrder = 6
              OnClick = btnShiftMoveABSClick
            end
          end
        end
      end
    end
    object tabTeachingData: TTabSheet
      Caption = 'Teaching Data'
      ImageIndex = 1
      object grpTeachingData: TGroupBox
        Left = 235
        Top = 5
        Width = 227
        Height = 296
        Caption = 'Input New Teaching Data'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object lblPortType: TLabel
          Left = 46
          Top = 24
          Width = 53
          Height = 15
          Caption = 'Port Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblHoist: TLabel
          Left = 17
          Top = 47
          Width = 82
          Height = 15
          Caption = 'New Hoist Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblShift: TLabel
          Left = 20
          Top = 70
          Width = 79
          Height = 15
          Caption = 'New Shift Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblRotate: TLabel
          Left = 9
          Top = 93
          Width = 90
          Height = 15
          Caption = 'New Rotate Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblPIODirection: TLabel
          Left = 25
          Top = 116
          Width = 74
          Height = 15
          Caption = 'PIO Direction'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblPIOTimeLevel: TLabel
          Left = 15
          Top = 139
          Width = 84
          Height = 15
          Caption = 'PIO Time Level'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lblLookDownLevel: TLabel
          Left = 7
          Top = 162
          Width = 92
          Height = 15
          Caption = 'Lookdown Level'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label11: TLabel
          Left = 50
          Top = 183
          Width = 49
          Height = 15
          Caption = 'PIO Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label12: TLabel
          Left = 65
          Top = 200
          Width = 34
          Height = 15
          Caption = 'PIO ID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label13: TLabel
          Left = 30
          Top = 218
          Width = 69
          Height = 15
          Caption = 'PIO Channel'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtHoist: TEdit
          Left = 112
          Top = 43
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          TextHint = 'Hoist'
        end
        object cbbPortType: TComboBox
          Left = 112
          Top = 20
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          TextHint = 'Port Type'
          Items.Strings = (
            'EQ'
            'EQ_X'
            'Stocker'
            'Loader'
            'STB_Left'
            'STB_Right'
            'UTB')
        end
        object edtShift: TEdit
          Left = 112
          Top = 66
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          TextHint = 'Shift'
        end
        object edtRotate: TEdit
          Left = 112
          Top = 89
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          TextHint = 'Rotate'
        end
        object edtPIOTimeLevel: TEdit
          Left = 112
          Top = 135
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          TextHint = 'PIO Timer Level'
        end
        object cbbPIODirection: TComboBox
          Left = 105
          Top = 106
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          TextHint = 'PIO Direction'
          Items.Strings = (
            'Left'
            'Right')
        end
        object edtLookDownLevel: TEdit
          Left = 112
          Top = 158
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          TextHint = 'LookDown Level'
        end
        object grpTransOption: TGroupBox
          Left = 3
          Top = 235
          Width = 212
          Height = 57
          Caption = 'Trans Option'
          TabOrder = 7
          object chkRFReader: TCheckBox
            Left = 4
            Top = 15
            Width = 97
            Height = 17
            Caption = 'RFReader'
            TabOrder = 1
          end
          object chkTransSound: TCheckBox
            Left = 4
            Top = 32
            Width = 97
            Height = 17
            Caption = 'TransSound'
            TabOrder = 3
          end
          object chkOscillation: TCheckBox
            Left = 107
            Top = 15
            Width = 97
            Height = 17
            Caption = 'Oscillation'
            TabOrder = 0
          end
          object chkHandDetectEQ: TCheckBox
            Left = 107
            Top = 32
            Width = 102
            Height = 17
            Caption = 'HandDetectEQ'
            TabOrder = 2
          end
        end
        object cbbPIOType: TComboBox
          Left = 112
          Top = 179
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          TextHint = 'PIO Type'
          Items.Strings = (
            'IR'
            'RF'
            'Hybrid')
        end
        object edtPIOID: TEdit
          Left = 112
          Top = 200
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          TextHint = 'PIO ID'
        end
        object edtPIOChannel: TEdit
          Left = 112
          Top = 219
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 10
          TextHint = 'PIO Channel'
        end
      end
      object btnTeachingSave: TButton
        Left = 477
        Top = 15
        Width = 113
        Height = 44
        Caption = 'Teaching Save'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = btnTeachingSaveClick
      end
      object btnTeachingDataReq: TButton
        Left = 477
        Top = 67
        Width = 113
        Height = 44
        Caption = #44592#51316' Data '#50836#52397
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = btnTeachingDataReqClick
      end
      object btnDefault: TButton
        Left = 477
        Top = 120
        Width = 113
        Height = 44
        Caption = 'Default Value'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        OnClick = btnDefaultClick
      end
      object grpPreTeachingData: TGroupBox
        Left = 3
        Top = 5
        Width = 227
        Height = 296
        Caption = 'Previous Teaching Data'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        DesignSize = (
          227
          296)
        object lbl1: TLabel
          Left = 46
          Top = 24
          Width = 53
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Port Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl2: TLabel
          Left = 45
          Top = 47
          Width = 54
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Hoist Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl3: TLabel
          Left = 48
          Top = 70
          Width = 51
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Shift Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl4: TLabel
          Left = 37
          Top = 93
          Width = 62
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Rotate Pos'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl5: TLabel
          Left = 25
          Top = 116
          Width = 74
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'PIO Direction'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl6: TLabel
          Left = 15
          Top = 139
          Width = 84
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'PIO Time Level'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object lbl7: TLabel
          Left = 7
          Top = 162
          Width = 92
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Lookdown Level'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label1: TLabel
          Left = 50
          Top = 183
          Width = 49
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'PIO Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label3: TLabel
          Left = 65
          Top = 200
          Width = 34
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'PIO ID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label4: TLabel
          Left = 30
          Top = 218
          Width = 69
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'PIO Channel'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object edtPre_Hoist: TEdit
          Left = 112
          Top = 43
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 1
          TextHint = 'Hoist'
        end
        object cbbPre_PortType: TComboBox
          Left = 112
          Top = 14
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          TextHint = 'Port Type'
          Items.Strings = (
            'EQ'
            'EQ_X'
            'Stocker'
            'Loader'
            'STB_Left'
            'STB_Right'
            'UTB')
        end
        object edtPre_Shift: TEdit
          Left = 112
          Top = 66
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 2
          TextHint = 'Shift'
        end
        object edtPre_Rotate: TEdit
          Left = 112
          Top = 89
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          TextHint = 'Rotate'
        end
        object edtPre_PIOTimeLevel: TEdit
          Left = 112
          Top = 135
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 5
          TextHint = 'PIO Timer Level'
        end
        object cbbPre_PIODirection: TComboBox
          Left = 112
          Top = 113
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 4
          TextHint = 'PIO Direction'
          Items.Strings = (
            'Left'
            'Right')
        end
        object edtPre_LookdownLevel: TEdit
          Left = 112
          Top = 158
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 6
          TextHint = 'LookDown Level'
        end
        object grpPre_TransOption: TGroupBox
          Left = 7
          Top = 235
          Width = 212
          Height = 59
          Caption = 'Trans Option'
          TabOrder = 7
          object chkPre_RFReader: TCheckBox
            Left = 4
            Top = 15
            Width = 97
            Height = 17
            Caption = 'RFReader'
            TabOrder = 1
          end
          object chkPre_TransSound: TCheckBox
            Left = 4
            Top = 32
            Width = 97
            Height = 17
            Caption = 'TransSound'
            TabOrder = 3
          end
          object chkPre_Oscillation: TCheckBox
            Left = 107
            Top = 15
            Width = 97
            Height = 17
            Caption = 'Oscillation'
            TabOrder = 0
          end
          object chkPre_HandDetectEQ: TCheckBox
            Left = 107
            Top = 32
            Width = 102
            Height = 17
            Caption = 'HandDetectEQ'
            TabOrder = 2
          end
        end
        object cbbPre_PIOType: TComboBox
          Left = 112
          Top = 183
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 9
          TextHint = 'PIO Type'
          Items.Strings = (
            'IR'
            'RF'
            'Hybrid')
        end
        object edtPre_PIOID: TEdit
          Left = 112
          Top = 203
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 8
          TextHint = 'PIO ID'
        end
        object edtPre_PIOChannel: TEdit
          Left = 112
          Top = 219
          Width = 105
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 10
          TextHint = 'PIO Channel'
        end
      end
    end
    object AutoTeahcingData: TTabSheet
      Caption = 'AutoTeahcingData'
      ImageIndex = 2
      object GroupBox1: TGroupBox
        Left = 3
        Top = 5
        Width = 238
        Height = 276
        Caption = 'AutoTeaching Driving Data'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        DesignSize = (
          238
          276)
        object Label15: TLabel
          Left = 24
          Top = 55
          Width = 53
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Port Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label16: TLabel
          Left = 23
          Top = 23
          Width = 54
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Station ID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label17: TLabel
          Left = 23
          Top = 170
          Width = 53
          Height = 15
          Anchors = [akTop, akRight]
          Caption = 'Port Type'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object ComboBox1: TComboBox
          Left = 104
          Top = 52
          Width = 121
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 0
          TextHint = 'Port Type'
          Items.Strings = (
            'EQ'
            'EQ_X'
            'Stocker'
            'Loader'
            'STB_Left'
            'STB_Right'
            'UTB')
        end
        object edtStationIDAT: TEdit
          Left = 104
          Top = 23
          Width = 121
          Height = 23
          ImeName = 'Microsoft IME 2010'
          NumbersOnly = True
          TabOrder = 1
          Text = '0'
        end
        object Button2: TButton
          Left = 23
          Top = 81
          Width = 201
          Height = 48
          Caption = 'Driving Data Read'
          TabOrder = 2
          OnClick = Button2Click
        end
        object ComboBox2: TComboBox
          Left = 103
          Top = 167
          Width = 121
          Height = 23
          ImeName = 'Microsoft IME 2010'
          TabOrder = 3
          TextHint = 'Port Type'
          Items.Strings = (
            'EQ'
            'EQ_X'
            'Stocker'
            'Loader'
            'STB_Left'
            'STB_Right'
            'UTB')
        end
        object btnNullType: TButton
          Left = 132
          Top = 200
          Width = 103
          Height = 49
          Caption = 'NULL Type'
          TabOrder = 4
          OnClick = btnNullTypeClick
        end
      end
      object memoAT: TMemo
        Left = 247
        Top = 16
        Width = 329
        Height = 201
        ImeName = 'Microsoft IME 2010'
        TabOrder = 1
      end
      object Button3: TButton
        Left = 424
        Top = 240
        Width = 156
        Height = 49
        Caption = 'Auto Teaching(File)'
        TabOrder = 2
        OnClick = Button3Click
      end
      object Button4: TButton
        Left = 247
        Top = 240
        Width = 158
        Height = 49
        Caption = 'Map Maker(File)'
        TabOrder = 3
        OnClick = Button4Click
      end
      object Button5: TButton
        Left = 26
        Top = 206
        Width = 103
        Height = 49
        Caption = 'Auto Teaching'
        TabOrder = 4
        OnClick = Button5Click
      end
    end
  end
  object timerUpdateDisplay: TTimer
    Interval = 200
    OnTimer = timerUpdateDisplayTimer
    Left = 359
    Top = 2
  end
  object ATTimer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = ATTimerTimer
    Left = 434
    Top = 6
  end
  object tmrJogMove: TTimer
    Enabled = False
    Interval = 300
    OnTimer = tmrJogMoveTimer
    Left = 248
  end
end
