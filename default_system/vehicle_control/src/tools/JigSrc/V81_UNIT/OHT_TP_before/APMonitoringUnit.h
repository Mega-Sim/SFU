// ---------------------------------------------------------------------------

#ifndef APMonitoringUnitH
#define APMonitoringUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Dialogs.hpp>

// ---------------------------------------------------------------------------
class TAPMonitoringForm : public TForm
{
__published: // IDE-managed Components

    TMainMenu *mmAPMonitoring;
    TMenuItem *mniStart;
    TMenuItem *mniStop;
    TMenuItem *mniCapture;
    TTimer *timerAPSignal;
    TOpenDialog *dlgOpenFile;
    TChart *chartAPSignal;
    TLineSeries *seriseAPSignal;
    TLabel *Label1;
    TPanel *AP_MAC;

    void __fastcall mniStartClick(TObject *Sender);
    void __fastcall mniStopClick(TObject *Sender);
    void __fastcall timerAPSignalTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall mniCaptureClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private: // User declarations
        public : // User declarations
    __fastcall TAPMonitoringForm(TComponent* Owner);

    float fTmpCount1;
    int nxTime1;

    void Graph_Start1(int);

};

// ---------------------------------------------------------------------------
extern PACKAGE TAPMonitoringForm *APMonitoringForm;
// ---------------------------------------------------------------------------
#endif
