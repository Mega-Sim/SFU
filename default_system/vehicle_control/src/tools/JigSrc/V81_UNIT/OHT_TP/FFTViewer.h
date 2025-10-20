//---------------------------------------------------------------------------

#ifndef FFTViewerH
#define FFTViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
struct FFT_DATA {
	int start;
	int end;
	TFastLineSeries* data;
};

#define _USE_MATH_DEFINES
#define AVS_FREQ	1000
#define AVS_FFT_FREQ (AVS_FREQ/2)
#define AVS_FFT_CHART_BOTTOM_MAX	100

#include <cmath>

class FFT
{
public:
	FFT() {}

	static int Execute(double* input, int size, double* output, int freq) {
		double an = 0.0f, bn = 0.0f;

		if ((input == NULL) || (output == NULL)) {
			return -1;
        }

        for (int i = 1; i <= freq; i++) {
			an = bn = 0.0f;

			for (int j = 0; j < size; j++) {
				an += std::sin((j * 2 * M_PI*i) / size)*input[j];
				bn += std::cos((j * 2 * M_PI*i) / size)*input[j];
			}

			an = 2.0*an / (double)size;
			bn = 2.0*bn / (double)size;

			output[i - 1] = std::sqrt(an*an + bn * bn);
		}

		return 0;
	}
};

struct FREQ_INFO
{
	int freq;
	double amp;
};

class TFFTViewerForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlTopMenu;
	TPanel *pnlChartArea;
	TChart *chtFFT;
	TBarSeries *SeriesFFTData;
	TLabeledEdit *ledtStartPosition;
	TLabeledEdit *ledtEndPosition;
	TPanel *pnlTopLeft;
	TPanel *Panel1;
	TScrollBar *ScrollBarFFT;
	TRadioGroup *rgTopFreqGroup;
	TButton *btClear;
	TPointSeries *SeriesFFTDataPoint;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ScrollBarFFTChange(TObject *Sender);
	void __fastcall rgTopFreqGroupClick(TObject *Sender);
	void __fastcall SeriesFFTDataClick(TChartSeries *Sender, int ValueIndex, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall btClearClick(TObject *Sender);
private:	// User declarations
	void CheckTopAmpFreq(int freq, double d);
	FREQ_INFO m_TopAmpFreq[5];
public:		// User declarations
	__fastcall TFFTViewerForm(TComponent* Owner);
    void initForm();
	void SetStartTime(int t);
	void SetEndTime(int t);
	FFT_DATA m_FFTData;
	void SetSeriesData(TFastLineSeries* data);
	void ExecuteFFT();
};
//---------------------------------------------------------------------------
extern PACKAGE TFFTViewerForm *FFTViewerForm;
//---------------------------------------------------------------------------
#endif
