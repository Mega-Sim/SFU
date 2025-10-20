object AMCParaForm: TAMCParaForm
  Left = 343
  Top = 132
  Caption = 'AMC Parameter '#54200#51665
  ClientHeight = 712
  ClientWidth = 935
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 585
    Height = 712
    Align = alLeft
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #44404#47548#52404
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitHeight = 715
    object GroupBox2: TGroupBox
      Left = 1
      Top = 1
      Width = 583
      Height = 40
      Align = alTop
      Caption = ' '#52629' '#49440#53469' '
      Color = 16767449
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #44404#47548#52404
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 0
      object ComboBox2: TComboBox
        Left = 8
        Top = 12
        Width = 145
        Height = 23
        Font.Charset = ANSI_CHARSET
        Font.Color = clTeal
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ImeName = 'Microsoft IME 2003'
        ParentFont = False
        TabOrder = 0
        OnChange = ComboBox2Change
        Items.Strings = (
          'Axis1'
          'Axis2'
          'Axis3'
          'Axis4')
      end
      object btnOpen: TButton
        Left = 168
        Top = 10
        Width = 75
        Height = 25
        Caption = 'Open'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        OnClick = btnOpenClick
      end
      object btnSave: TButton
        Left = 256
        Top = 10
        Width = 75
        Height = 25
        Caption = 'Save'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = btnSaveClick
      end
      object btnSaveAs: TButton
        Left = 344
        Top = 10
        Width = 75
        Height = 25
        Caption = 'Save As'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        OnClick = btnSaveAsClick
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 227
      Width = 583
      Height = 208
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel3'
      TabOrder = 1
      object GroupBox1: TGroupBox
        Left = 292
        Top = 0
        Width = 291
        Height = 208
        Align = alClient
        Caption = ' Gain ('#49549#46020#51228#50612','#53664#53356#47784#46300') '
        Color = 15198183
        Font.Charset = ANSI_CHARSET
        Font.Color = clFuchsia
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        TabOrder = 0
        object Label1: TLabel
          Left = 8
          Top = 158
          Width = 183
          Height = 15
          Caption = '1.'#49549#46020#51228#50612' PID '#47336#54532#50640#49436#51032' '#51228#50612#44050
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label2: TLabel
          Left = 8
          Top = 174
          Width = 238
          Height = 15
          Caption = '2. '#51228#50612#47784#46300#47484' '#53664#53356#47784#46300#47196' '#49444#51221#49884#50640#47564' '#46041#51089#54632
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label3: TLabel
          Left = 8
          Top = 190
          Width = 257
          Height = 15
          Caption = '3. Open Loop'#51032' '#44221#50864' '#48324#46020#51032' '#44172#51064#44050' '#49324#50857#54644#50556' '#54632
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Panel4: TPanel
          Left = 2
          Top = 16
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  VPGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          object edtVPGAIN: TEdit
            Left = 150
            Top = 4
            Width = 121
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel5: TPanel
          Left = 2
          Top = 44
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  VIGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          object edtVIGAIN: TEdit
            Left = 150
            Top = 4
            Width = 121
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel6: TPanel
          Left = 2
          Top = 72
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  VDGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
          object edtVDGAIN: TEdit
            Left = 150
            Top = 4
            Width = 121
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel7: TPanel
          Left = 2
          Top = 100
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  VFGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
          object edtVFGAIN: TEdit
            Left = 150
            Top = 4
            Width = 121
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel13: TPanel
          Left = 2
          Top = 128
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  VILIMIT'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
          object edtVILIMIT: TEdit
            Left = 150
            Top = 4
            Width = 121
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '1000.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
      end
      object GroupBox4: TGroupBox
        Left = 0
        Top = 0
        Width = 292
        Height = 208
        Align = alLeft
        Caption = ' Gain ('#50948#52824#51228#50612',Closed Loop) '
        Color = 15198183
        Font.Charset = ANSI_CHARSET
        Font.Color = clFuchsia
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        TabOrder = 1
        object Label4: TLabel
          Left = 8
          Top = 158
          Width = 186
          Height = 15
          Caption = '1. '#50948#52824#51228#50612' PID '#47336#54532#50640#49436#51032' '#51228#50612#44050
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label5: TLabel
          Left = 8
          Top = 174
          Width = 171
          Height = 15
          Caption = '2. Closed Loop'#51068#46412#51032' '#44172#51064#44050#51076
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label11: TLabel
          Left = 8
          Top = 190
          Width = 257
          Height = 15
          Caption = '3. Open Loop'#51032' '#44221#50864' '#48324#46020#51032' '#44172#51064#44050' '#49324#50857#54644#50556' '#54632
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Panel14: TPanel
          Left = 2
          Top = 16
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  PGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          object edtPGAIN: TEdit
            Left = 160
            Top = 4
            Width = 110
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '300.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel15: TPanel
          Left = 2
          Top = 44
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  IGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          object edtIGAIN: TEdit
            Left = 160
            Top = 4
            Width = 110
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '5.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel16: TPanel
          Left = 2
          Top = 72
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  DGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
          object edtDGAIN: TEdit
            Left = 160
            Top = 4
            Width = 110
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel17: TPanel
          Left = 2
          Top = 100
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  FGAIN'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
          object edtFGAIN: TEdit
            Left = 160
            Top = 4
            Width = 110
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '0.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
        object Panel18: TPanel
          Left = 2
          Top = 128
          Width = 280
          Height = 28
          Alignment = taLeftJustify
          BevelInner = bvRaised
          BevelOuter = bvLowered
          Caption = '  ILIMIT'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlue
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
          object edtILIMIT: TEdit
            Left = 160
            Top = 4
            Width = 110
            Height = 20
            BorderStyle = bsNone
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = #44404#47548#52404
            Font.Style = []
            ImeName = 'Microsoft IME 2003'
            ParentFont = False
            TabOrder = 0
            Text = '300.000000'
            OnChange = edtVEL_LIMITChange
          end
        end
      end
    end
    object GroupBox5: TGroupBox
      Left = 1
      Top = 41
      Width = 583
      Height = 186
      Align = alTop
      Caption = ' Limit '
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 2
      object Panel20: TPanel
        Left = 2
        Top = 16
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  VEL_LIMIT [cts/sec]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object Label13: TLabel
          Left = 274
          Top = 6
          Width = 230
          Height = 15
          Caption = #52572#45824' '#46041#51089' '#49549#46020' '#49444#51221'(v_move'#50640#49436#45716' '#48120#49324#50857')'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtVEL_LIMIT: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '409600.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel21: TPanel
        Left = 2
        Top = 44
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  ACC_LIMIT [msec]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object Label14: TLabel
          Left = 274
          Top = 6
          Width = 203
          Height = 15
          Caption = #52572#45824' '#44032#49549' '#49884#44036'(v_move'#50640#49436#45716' '#48120#49324#50857')'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtACC_LIMIT: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1000.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel22: TPanel
        Left = 2
        Top = 72
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  SWLOWER_LIMIT [cts]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object Label15: TLabel
          Left = 274
          Top = 6
          Width = 276
          Height = 15
          Caption = 'Profile'#49373#49457#49884' Limit'#48279#50612#45208#47732' Event'#52376#47532'(PTP Motion)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtSWLOWER_LIMIT: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '-2147483648'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel23: TPanel
        Left = 2
        Top = 100
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  SWUPPER_LIMIT [cts]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object Label16: TLabel
          Left = 274
          Top = 6
          Width = 276
          Height = 15
          Caption = 'Profile'#49373#49457#49884' Limit'#48279#50612#45208#47732' Event'#52376#47532'(PTP Motion)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtSWUPPER_LIMIT: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '2147483647'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel24: TPanel
        Left = 2
        Top = 128
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  IN_POSITION [cts]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        object Label17: TLabel
          Left = 274
          Top = 6
          Width = 217
          Height = 15
          Caption = #54788#51116#44050'-'#47785#54364#44050#51060' '#44050#48372#45796' '#51089#51004#47732' '#46020#52265#50756#47308
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtIN_POSITION: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '50.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel25: TPanel
        Left = 2
        Top = 156
        Width = 580
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = '  ERROR_LIMIT [cts]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
        object Label18: TLabel
          Left = 274
          Top = 6
          Width = 277
          Height = 15
          Caption = #47785#54364'-'#54788#51116#52264#44032' '#44050' '#51060#49345#51068' '#44221#50864' '#50640#47084#52376#47532'[PTP Motion)'
          Font.Charset = ANSI_CHARSET
          Font.Color = clBlack
          Font.Height = -12
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object edtERROR_LIMIT: TEdit
          Left = 160
          Top = 4
          Width = 110
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '350000.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
    object GroupBox3: TGroupBox
      Left = 1
      Top = 435
      Width = 583
      Height = 276
      Align = alClient
      Caption = ' Event '
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 3
      ExplicitHeight = 279
      object Label6: TLabel
        Left = 344
        Top = 14
        Width = 184
        Height = 15
        Caption = '> Limit'#44050#51012' '#48279#50612#45212' '#44221#50864' Event'#49440#53469
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 344
        Top = 30
        Width = 199
        Height = 15
        Caption = ' 0:NONE 1:STOP 2:ESTOP 3:ABORT'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label8: TLabel
        Left = 352
        Top = 48
        Width = 112
        Height = 15
        Caption = '>Limit '#49468#49436' '#46041#51089#47112#48296
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label9: TLabel
        Left = 352
        Top = 62
        Width = 140
        Height = 15
        Caption = '0:Active Low 1:Active High'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label10: TLabel
        Left = 112
        Top = 105
        Width = 113
        Height = 15
        Caption = '>Amp Fault '#47112#48296#51648#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label12: TLabel
        Left = 112
        Top = 119
        Width = 138
        Height = 15
        Caption = ' '#52992#51060#48660#51060' '#48736#51652#44221#50864#46020#54644#45817
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label19: TLabel
        Left = 408
        Top = 116
        Width = 154
        Height = 15
        Caption = '>'#49436#48372#46300#46972#51060#48652' '#46041#51089#47112#48296#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label20: TLabel
        Left = 168
        Top = 142
        Width = 215
        Height = 15
        Caption = '>Amp '#50640#47084#48156#49373#49884' '#47532#49483#51012' '#50948#54620' '#47112#48296' '#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label21: TLabel
        Left = 416
        Top = 84
        Width = 142
        Height = 15
        Caption = '>'#50948' '#47532#48139#49468#49436' '#46041#51089#49884' Event'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label22: TLabel
        Left = 168
        Top = 166
        Width = 253
        Height = 15
        Caption = '>'#50948' '#50640#47084#47532#48139' '#51312#44148#51060' '#46108#44221#50864' '#49688#54665#54624' '#51060#48292#53944' '#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label23: TLabel
        Left = 264
        Top = 198
        Width = 200
        Height = 15
        Caption = '>'#44033#51333' Event'#52376#47532#49884' '#44048#49549#44592#50872#44592'[msec]'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label24: TLabel
        Left = 432
        Top = 222
        Width = 139
        Height = 15
        Caption = '>Input '#54252#53944' '#51064#53552#47101#53944' '#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Panel8: TPanel
        Left = 2
        Top = 16
        Width = 167
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' SWUPPER_LIMIT_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object edtSWUPPER_LIMIT_ST: TEdit
          Left = 128
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel9: TPanel
        Left = 170
        Top = 16
        Width = 167
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' SWLOWER_LIMIT_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object edtSWLOWER_LIMIT_ST: TEdit
          Left = 128
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel10: TPanel
        Left = 2
        Top = 48
        Width = 110
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' POS_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object edtPOS_LEVEL: TEdit
          Left = 76
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel11: TPanel
        Left = 114
        Top = 48
        Width = 110
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' NEG_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object edtNEG_LEVEL: TEdit
          Left = 76
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel12: TPanel
        Left = 226
        Top = 48
        Width = 119
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' HOME_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        object edtHOME_LEVEL: TEdit
          Left = 86
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel19: TPanel
        Left = 2
        Top = 108
        Width = 110
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' AMP_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
        object edtAMP_LEVEL: TEdit
          Left = 76
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel26: TPanel
        Left = 2
        Top = 136
        Width = 159
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' AMP_RESET_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
        object edtAMP_RESET_LEVEL: TEdit
          Left = 124
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel30: TPanel
        Left = 2
        Top = 164
        Width = 159
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' ERROR_LIMIT_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 7
        object edtERROR_LIMIT_ST: TEdit
          Left = 124
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '3'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel31: TPanel
        Left = 2
        Top = 192
        Width = 127
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' STOP_RATE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 8
        object edtSTOP_RATE: TEdit
          Left = 80
          Top = 4
          Width = 37
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '100'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel32: TPanel
        Left = 130
        Top = 192
        Width = 127
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' ESTOP_RATE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 9
        object edtESTOP_RATE: TEdit
          Left = 86
          Top = 4
          Width = 37
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '-6'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel35: TPanel
        Left = 274
        Top = 108
        Width = 135
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' AMP_ON_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 10
        object edtAMP_ON_LEVEL: TEdit
          Left = 100
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel56: TPanel
        Left = 2
        Top = 220
        Width = 167
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' IO_INT_ENABLE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 11
        object edtIO_INT_ENABLE: TEdit
          Left = 96
          Top = 4
          Width = 65
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel57: TPanel
        Left = 170
        Top = 220
        Width = 127
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' INT_EVENT_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 12
        object edtINT_EVENT_ST: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel58: TPanel
        Left = 298
        Top = 220
        Width = 132
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' AMP_FAULT_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 13
        object edtAMP_FAULT_ST: TEdit
          Left = 100
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '3'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel60: TPanel
        Left = 2
        Top = 248
        Width = 159
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' USERIO_BOOTMODE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 14
        object edtUSERIO_BOOTMODE: TEdit
          Left = 128
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel61: TPanel
        Left = 162
        Top = 248
        Width = 159
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' USERIO_BOOTVALUE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 15
        object edtUSERIO_BOOTVALUE: TEdit
          Left = 128
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel27: TPanel
        Left = 2
        Top = 78
        Width = 135
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' POS_LEVEL_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 16
        object edtPOS_LEVEL_ST: TEdit
          Left = 100
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel28: TPanel
        Left = 138
        Top = 78
        Width = 135
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' NEG_LEVEL_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 17
        object edtNEG_LEVEL_ST: TEdit
          Left = 100
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel29: TPanel
        Left = 274
        Top = 78
        Width = 143
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' HOME_LEVEL_ST'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 18
        object edtHOME_LEVEL_ST: TEdit
          Left = 110
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 585
    Top = 0
    Width = 350
    Height = 712
    Align = alClient
    Caption = 'Panel2'
    TabOrder = 1
    ExplicitHeight = 715
    object GroupBox6: TGroupBox
      Left = 1
      Top = 257
      Width = 348
      Height = 104
      Align = alTop
      Caption = ' '#54924#51204#48169#54693#51060' '#45796#47484#44221#50864' '#49444#51221#54616#45716' '#44050' '
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 0
      object Label37: TLabel
        Left = 129
        Top = 17
        Width = 206
        Height = 15
        Caption = '>'#47784#53552#54924#51204#48169#54693' != '#47196#48391#54924#51204#48169#54693#51032' '#48512#54840
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label38: TLabel
        Left = 127
        Top = 33
        Width = 210
        Height = 15
        Caption = #54924#51204#48169#54693' '#48320#44221' '#54596#50836#49884' '#49324#50857'(DSP'#47532#48512#54021')'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label39: TLabel
        Left = 135
        Top = 49
        Width = 72
        Height = 15
        Caption = '0:CCW 1:CW'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Panel33: TPanel
        Left = 2
        Top = 16
        Width = 126
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' ENCODER_DIR'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object edtENCODER_DIR: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel34: TPanel
        Left = 2
        Top = 46
        Width = 126
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' MOTOR_PAUSE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object edtMOTOR_PAUSE: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel50: TPanel
        Left = 2
        Top = 74
        Width = 168
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' MOTOR_PAUSE_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object edtMOTOR_PAUSE_LEVEL: TEdit
          Left = 138
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel51: TPanel
        Left = 170
        Top = 74
        Width = 183
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' MOTOR_PAUSE_CHECKBIT'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object edtMOTOR_PAUSE_CHECKBIT: TEdit
          Left = 156
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
    object GroupBox7: TGroupBox
      Left = 1
      Top = 491
      Width = 348
      Height = 223
      Align = alClient
      Caption = ' '#48120#49324#50857', '#44256#51221#44050' '
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 1
      object Label42: TLabel
        Left = 169
        Top = 17
        Width = 142
        Height = 15
        Caption = '>'#51204#51088#44592#50612#48708#49444#51221'(1'#47196#44256#51221')'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label43: TLabel
        Left = 209
        Top = 65
        Width = 43
        Height = 15
        Caption = '>'#48120#49324#50857
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label44: TLabel
        Left = 121
        Top = 105
        Width = 198
        Height = 15
        Caption = '>0:servo, 1:stepper 2:micro_stepper'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label45: TLabel
        Left = 129
        Top = 137
        Width = 145
        Height = 15
        Caption = '>'#54588#46300#48177' '#51109#52824#49444#51221#44050'('#54637#49345'0)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label46: TLabel
        Left = 121
        Top = 161
        Width = 220
        Height = 15
        Caption = '>'#51064#53076#45908#47484' '#49324#50857#54616#50668' '#50896#51216#51012' '#52286#51012#44221#50864#49324#50857
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label47: TLabel
        Left = 177
        Top = 185
        Width = 43
        Height = 15
        Caption = '>'#48120#49324#50857
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Panel41: TPanel
        Left = 2
        Top = 16
        Width = 167
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' GEAR_RATIO'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object edtGEAR_RATIO: TEdit
          Left = 88
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel42: TPanel
        Left = 2
        Top = 44
        Width = 199
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' HWLOWER_LIMIT'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object edtHWLOWER_LIMIT: TEdit
          Left = 108
          Top = 4
          Width = 81
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '-2147483648'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel43: TPanel
        Left = 2
        Top = 72
        Width = 199
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' HWUPPER_LIMIT'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object edtHWUPPER_LIMIT: TEdit
          Left = 108
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '2147483647'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel44: TPanel
        Left = 2
        Top = 100
        Width = 119
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' MOTOR_TYPE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object edtMOTOR_TYPE: TEdit
          Left = 88
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel54: TPanel
        Left = 2
        Top = 128
        Width = 127
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' ENCODER_CFG'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        object edtENCODER_CFG: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel55: TPanel
        Left = 2
        Top = 156
        Width = 119
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' HOME_INDEX'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
        object edtHOME_INDEX: TEdit
          Left = 88
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel59: TPanel
        Left = 2
        Top = 184
        Width = 167
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' INPOS_LEVEL'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
        object edtINPOS_LEVEL: TEdit
          Left = 88
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
    object GroupBox8: TGroupBox
      Left = 1
      Top = 1
      Width = 348
      Height = 256
      Align = alTop
      Caption = ' Control Mode '
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 2
      object Label25: TLabel
        Left = 129
        Top = 22
        Width = 165
        Height = 15
        Caption = '> '#51228#50612#47784#46300'(0:velocity, 1:torque)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label26: TLabel
        Left = 129
        Top = 46
        Width = 204
        Height = 15
        Caption = '> '#51228#50612#47336#54532'(0:open loop, 1:close loop)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label27: TLabel
        Left = 129
        Top = 64
        Width = 133
        Height = 15
        Caption = '> '#50500#45216#47196#44536#52636#47141#51204#50517' '#44537#49457
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label28: TLabel
        Left = 121
        Top = 78
        Width = 112
        Height = 15
        Caption = '0:bioplar(-10v~+10v)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label29: TLabel
        Left = 121
        Top = 92
        Width = 201
        Height = 15
        Caption = '1:unipolar(0v~+10v):+-12v'#51204#50896#49324#50857#49884
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label30: TLabel
        Left = 113
        Top = 144
        Width = 137
        Height = 15
        Caption = ' 0:only standing 1:always'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label31: TLabel
        Left = 113
        Top = 112
        Width = 211
        Height = 15
        Caption = '>I'#44172#51064#51012' '#51204#44396#44036' '#46608#45716' '#51221#51648#51228#50612#49345#53468#50640#49436
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label32: TLabel
        Left = 113
        Top = 128
        Width = 78
        Height = 15
        Caption = ' '#49324#50857#54624#51648' '#44208#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label33: TLabel
        Left = 121
        Top = 168
        Width = 160
        Height = 15
        Caption = '>'#50948#52824#54805#49436#48372', '#49828#53597#47784#53552' '#49324#50857#49884
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label34: TLabel
        Left = 201
        Top = 192
        Width = 118
        Height = 15
        Caption = '>'#46160#48148#53300#49324#51060#51032' '#49549#46020#52264
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label35: TLabel
        Left = 209
        Top = 208
        Width = 77
        Height = 15
        Caption = '(mobile robot)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label36: TLabel
        Left = 185
        Top = 232
        Width = 67
        Height = 15
        Caption = '>'#44592#44396#51201#50896#51216
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Panel36: TPanel
        Left = 2
        Top = 16
        Width = 126
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' CONTROL_CFG'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object edtCONTROL_CFG: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel37: TPanel
        Left = 2
        Top = 46
        Width = 126
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' LOOP_CFG'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object edtLOOP_CFG: TEdit
          Left = 96
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel38: TPanel
        Left = 2
        Top = 76
        Width = 119
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' VOLTAGE_CFG'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object edtVOLTAGE_CFG: TEdit
          Left = 90
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel39: TPanel
        Left = 2
        Top = 106
        Width = 111
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' POS_IMODE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object edtPOS_IMODE: TEdit
          Left = 80
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel40: TPanel
        Left = 2
        Top = 136
        Width = 111
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' VEL_IMODE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        object edtVEL_IMODE: TEdit
          Left = 80
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel46: TPanel
        Left = 2
        Top = 166
        Width = 119
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' PULSE_MODE'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
        object edtPULSE_MODE: TEdit
          Left = 88
          Top = 4
          Width = 25
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel52: TPanel
        Left = 2
        Top = 196
        Width = 199
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' VTRACKING_FACTOR'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
        object edtVTRACKING_FACTOR: TEdit
          Left = 128
          Top = 4
          Width = 65
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '1.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel53: TPanel
        Left = 2
        Top = 226
        Width = 183
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' ENCODER_OFFSET'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 7
        object edtENCODER_OFFSET: TEdit
          Left = 112
          Top = 4
          Width = 65
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
    object GroupBox9: TGroupBox
      Left = 1
      Top = 361
      Width = 348
      Height = 130
      Align = alTop
      Caption = ' '#54596#53552' : 0'#51060#47732' '#44592#45733#51221#51648
      Color = 15198183
      Font.Charset = ANSI_CHARSET
      Font.Color = clFuchsia
      Font.Height = -12
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 3
      object Label40: TLabel
        Left = 209
        Top = 41
        Width = 130
        Height = 15
        Caption = '>'#50948#52824#51228#50612#50857' '#51452#54028#49688#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label41: TLabel
        Left = 209
        Top = 89
        Width = 130
        Height = 15
        Caption = '>'#53664#53356#51228#50612#50857' '#51452#54028#49688#49444#51221
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Panel45: TPanel
        Left = 2
        Top = 16
        Width = 207
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' POS_NOTCH_FREQ'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        object edtPOS_NOTCH_FREQ: TEdit
          Left = 120
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel47: TPanel
        Left = 2
        Top = 44
        Width = 207
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' POS_LPF_FREQ'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
        object edtPOS_LPF_FREQ: TEdit
          Left = 120
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel48: TPanel
        Left = 2
        Top = 72
        Width = 207
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' VEL_NOTCH_FREQ'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object edtVEL_NOTCH_FREQ: TEdit
          Left = 120
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
      object Panel49: TPanel
        Left = 2
        Top = 100
        Width = 207
        Height = 28
        Alignment = taLeftJustify
        BevelInner = bvRaised
        BevelOuter = bvLowered
        Caption = ' VEL_LPF_FREQ'
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlack
        Font.Height = -12
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object edtVEL_LPF_FREQ: TEdit
          Left = 120
          Top = 4
          Width = 73
          Height = 20
          BorderStyle = bsNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = #44404#47548#52404
          Font.Style = []
          ImeName = 'Microsoft IME 2003'
          ParentFont = False
          TabOrder = 0
          Text = '0.000000'
          OnChange = edtVEL_LIMITChange
        end
      end
    end
  end
  object SaveDialog1: TSaveDialog
    Left = 553
    Top = 9
  end
  object OpenDialog2: TOpenDialog
    Filter = 'Loc(*.loc)|*.loc'
    Left = 368
    Top = 40
  end
end
