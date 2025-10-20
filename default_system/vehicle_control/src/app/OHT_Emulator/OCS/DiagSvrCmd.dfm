object frmDiagSvrCmd: TfrmDiagSvrCmd
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = #47700#51064#53552#45324#49828' '#49436#48260#50640#49436' '#47749#47161' '#49569#49888
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
    Left = 6
    Top = 8
    Width = 696
    Height = 401
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'OHT Status '
      object btnConnectCheckRequest: TButton
        Left = 448
        Top = 32
        Width = 185
        Height = 25
        Caption = #50672#44208#54869#51064' '#50836#52397
        TabOrder = 0
        OnClick = btnConnectCheckRequestClick
      end
      object GroupBox1: TGroupBox
        Left = 32
        Top = 80
        Width = 633
        Height = 169
        Caption = 'GroupBox1'
        TabOrder = 1
        object btnMaintInfoRequest: TButton
          Left = 416
          Top = 19
          Width = 185
          Height = 25
          Caption = #53685#44228#51221#48372#48372#44256' '#50836#52397
          TabOrder = 0
          OnClick = btnMaintInfoRequestClick
        end
        object btnMaintInfoReset: TButton
          Left = 416
          Top = 50
          Width = 185
          Height = 25
          Caption = #53685#44228#51221#48372' '#52488#44592#54868' '#50836#52397
          TabOrder = 1
          OnClick = btnMaintInfoResetClick
        end
        object btnMaintInfoResponse: TButton
          Left = 416
          Top = 81
          Width = 185
          Height = 25
          Caption = #53685#44228#51221#48372' '#51025#45813
          TabOrder = 2
          OnClick = btnMaintInfoResponseClick
        end
        object cbMaint: TComboBox
          Left = 32
          Top = 21
          Width = 369
          Height = 21
          Style = csDropDownList
          ImeName = 'Microsoft IME 2010'
          ItemIndex = 0
          TabOrder = 3
          Text = #51452#54665' - '#51452#54665' '#44396#46041#48512#51032' '#45572#51201' '#51452#54665#44144#47532
          Items.Strings = (
            #51452#54665' - '#51452#54665' '#44396#46041#48512#51032' '#45572#51201' '#51452#54665#44144#47532
            #51312#54693' - Steering '#46041#51089' '#45572#51201' '#54943#49688
            #51060#51116#44592' '#54945#54665'(Shift) - Shift '#46041#51089' '#45572#51201' '#54943#49688
            #51060#51116#44592' '#54945#54665'(Shift) - Shift '#47784#53552' '#45572#51201' '#54924#51204#49688
            #49849#44053'(Hoist) - Hoist '#46041#51089' '#45572#51201' '#54943#49688
            #49849#44053'(Hoist) - Hoist '#47784#53552' '#45572#51201' '#54924#51204#49688
            #51060#51116#44592' '#49440#54924'(Rotate) - Rotate '#46041#51089' '#45572#51201' '#54943#49688
            #51060#51116#44592' '#44536#47532#54140'(Hand) - Hand '#46041#51089' '#45572#51201' '#54943#49688
            #45209#54616' '#48169#51648#48512'(Shutter) - Shutter '#46041#51089' '#45572#51201' '#54943#49688
            #49468#49436' - '#51452#54665#45432#46300' '#48148#53076#46300#47532#45908' '#46041#51089' '#45572#51201' '#54943#49688
            #49468#49436' - '#49828#53580#51060#49496#45432#46300' '#48148#53076#46300#47532#45908' '#46041#51089' '#45572#51201' '#54943#49688
            #49468#49436' - Look down '#49468#49436' '#44032#46041' '#45572#51201' '#49884#44036
            #49468#49436' - UBG '#49468#49436' '#46041#51089' '#45572#51201' '#49884#44036' (VHL '#44160#52636')'
            #49468#49436' - PBS '#49468#49436' '#46041#51089' '#45572#51201' '#49884#44036' ('#51109#50528#47932' '#44160#52636')')
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'BODY'
      ImageIndex = 1
    end
  end
  object btnClose: TButton
    Left = 566
    Top = 414
    Width = 136
    Height = 33
    Caption = 'Close'
    TabOrder = 1
    OnClick = btnCloseClick
  end
end
