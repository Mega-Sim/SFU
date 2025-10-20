//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FFTViewer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TFFTViewerForm *FFTViewerForm;

#define	CHART_BOTTOM_AXIS_RANGE	50
//---------------------------------------------------------------------------
__fastcall TFFTViewerForm::TFFTViewerForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TFFTViewerForm::initForm()
{
	memset(&m_FFTData, 0, sizeof(m_FFTData));
	this->SetStartTime(0);
	this->SetEndTime(0);
	SeriesFFTData->Clear();
	ScrollBarFFT->Position = 0;
	memset(m_TopAmpFreq, 0, sizeof(FREQ_INFO)*5);
	SeriesFFTDataPoint->Clear();
}

void TFFTViewerForm::SetStartTime(int t)
{
	ledtStartPosition->Text = (double)t / 1000.0;
	m_FFTData.start = t;
}

void TFFTViewerForm::SetEndTime(int t)
{
	ledtEndPosition->Text = (double)t / 1000.0;
	m_FFTData.end = t;
}

void TFFTViewerForm::SetSeriesData(TFastLineSeries* data)
{
	m_FFTData.data = data;
}
void __fastcall TFFTViewerForm::FormShow(TObject *Sender)
{
	initForm();
}
//---------------------------------------------------------------------------
void TFFTViewerForm::ExecuteFFT()
{
	double output[AVS_FFT_FREQ] = {0.0, };
	double input[AVS_FREQ] = {0.0,};
	int cnt = m_FFTData.end - m_FFTData.start;

	//Axis Data 생성
	for(int i=0; i<cnt; i++)
	{
		input[i] = m_FFTData.data->YValue[m_FFTData.start + i];
	}

	chtFFT->Title->Caption = m_FFTData.data->Title + " Axis FFT";

	//Execute FFT
	FFT::Execute(input, AVS_FREQ, output, AVS_FFT_FREQ);

	//Draw Chart
	for(int i=0; i<AVS_FFT_FREQ; i++)
	{
		CheckTopAmpFreq(i+1, output[i]);

		SeriesFFTData->AddXY(i, output[i], i+1);
	}

	chtFFT->LeftAxis->SetMinMax(0, m_TopAmpFreq[0].amp + 5);

	AnsiString strItem;

	//Display Top Freq
	rgTopFreqGroup->Items->Clear();
	for(int i=0; i<5; i++)
	{
		if(m_TopAmpFreq[i].amp == 0) break;

		strItem.sprintf("freq:%d amp:%.2f", m_TopAmpFreq[i].freq, m_TopAmpFreq[i].amp);
		rgTopFreqGroup->Items->Add(strItem);
	}

	//Move Top Amplitude position
	rgTopFreqGroup->ItemIndex = 0;
	rgTopFreqGroupClick(0);
}
void __fastcall TFFTViewerForm::ScrollBarFFTChange(TObject *Sender)
{
	float bottom_axis_min;
	float bottom_axis_max;

	bottom_axis_min = (double)(AVS_FFT_CHART_BOTTOM_MAX) * ((double)ScrollBarFFT->Position/100.0);
	if(bottom_axis_min > (AVS_FFT_CHART_BOTTOM_MAX - CHART_BOTTOM_AXIS_RANGE)) bottom_axis_min -= CHART_BOTTOM_AXIS_RANGE;
	bottom_axis_max = bottom_axis_min + CHART_BOTTOM_AXIS_RANGE;

	if(bottom_axis_min > bottom_axis_max) return;

	chtFFT->BottomAxis->SetMinMax(bottom_axis_min, bottom_axis_max);
}
//---------------------------------------------------------------------------

void TFFTViewerForm::CheckTopAmpFreq(int freq, double d)
{
	if(d < 0.5) return;

	for(int i=0; i<5; i++)
	{
		if(m_TopAmpFreq[i].amp < d)
		{
			FREQ_INFO fi;
			fi.freq = freq;
			fi.amp = d;

			for(int j=4; j>i; j--)
			{
				m_TopAmpFreq[j] = m_TopAmpFreq[j-1];
			}

			m_TopAmpFreq[i] = fi;

			break;
        }
    }
}

void __fastcall TFFTViewerForm::rgTopFreqGroupClick(TObject *Sender)
{
	int nSelIndex = rgTopFreqGroup->ItemIndex;
	FREQ_INFO fi = m_TopAmpFreq[nSelIndex];

	SeriesFFTDataPoint->Clear();

	SeriesFFTDataPoint->AddXY(fi.freq-1, fi.amp);

	//차트 범위밖일경우 재조정
	if(chtFFT->BottomAxis->Minimum > fi.freq
	|| chtFFT->BottomAxis->Maximum < fi.freq)
	{
		ScrollBarFFT->Position = (double)(fi.freq*100) /(double)AVS_FFT_FREQ;

		ScrollBarFFTChange(0);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFFTViewerForm::SeriesFFTDataClick(TChartSeries *Sender, int ValueIndex,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
	SeriesFFTDataPoint;
}
//---------------------------------------------------------------------------

void __fastcall TFFTViewerForm::btClearClick(TObject *Sender)
{
	initForm();
}
//---------------------------------------------------------------------------

