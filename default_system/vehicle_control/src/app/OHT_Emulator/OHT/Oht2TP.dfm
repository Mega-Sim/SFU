object frmOht2TP: TfrmOht2TP
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'OHT -> TP '#53685#49888' '#53580#49828#53944
  ClientHeight = 461
  ClientWidth = 712
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 696
    Height = 401
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'OHT -> PC Command'
      object Button1: TButton
        Left = 24
        Top = 16
        Width = 121
        Height = 41
        Caption = #51068#48152' '#54056#53431' '#51204#49569
        TabOrder = 0
        OnClick = Button1Click
      end
      object Button2: TButton
        Left = 24
        Top = 176
        Width = 121
        Height = 41
        Caption = #49345#53468' '#54056#53431' '#51204#49569
        TabOrder = 1
        OnClick = Button2Click
      end
      object Edit1: TEdit
        Left = 24
        Top = 128
        Width = 73
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 2
        Text = '12'
      end
      object Edit2: TEdit
        Left = 120
        Top = 128
        Width = 73
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 3
        Text = '13'
      end
      object Edit3: TEdit
        Left = 216
        Top = 128
        Width = 73
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 4
        Text = '14'
      end
      object Edit4: TEdit
        Left = 312
        Top = 128
        Width = 73
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 5
        Text = '15'
      end
      object Edit5: TEdit
        Left = 416
        Top = 128
        Width = 73
        Height = 21
        ImeName = 'Microsoft IME 2010'
        TabOrder = 6
        Text = '16'
      end
      object Button3: TButton
        Left = 24
        Top = 240
        Width = 121
        Height = 41
        Caption = 'init '#54056#53431' '#51204#49569
        TabOrder = 7
        OnClick = Button3Click
      end
      object chkStatusSendAuto: TCheckBox
        Left = 184
        Top = 184
        Width = 129
        Height = 17
        Caption = #49345#53468#54056#53431' '#51088#46041#51204#49569
        TabOrder = 8
        OnClick = chkStatusSendAutoClick
      end
      object Button4: TButton
        Left = 208
        Top = 16
        Width = 121
        Height = 41
        Caption = #51089#50629#51648#49884' '#51025#45813
        TabOrder = 9
        OnClick = Button4Click
      end
    end
  end
  object btnClose: TButton
    Left = 568
    Top = 415
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 1
    OnClick = btnCloseClick
  end
  object tmrAutoStatus: TTimer
    Enabled = False
    OnTimer = tmrAutoStatusTimer
    Left = 344
    Top = 208
  end
end
