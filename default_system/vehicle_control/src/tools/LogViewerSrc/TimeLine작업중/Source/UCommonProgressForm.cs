using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace LogViewer
{
	// UCommonProgressForm
	// DProgress 컨트롤 클래스에서 생성되어질 공통 Progress UI Form
	public partial class UCommonProgressForm : Form
	{
		private DProgress parent = null;

		public UCommonProgressForm()
		{
			InitializeComponent();
		}

		public void SetParent(DProgress obj)
		{
			parent = obj;
			if(String.IsNullOrEmpty(obj.text) == false)
				textBox1.Text = obj.text;
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
	
		}
	}

	// DProgress
	// 일회성 스레드를 생성하고 
	// 그 스레드에서 UCommonProgressForm 을 생성하여 Main Job과는 별도로 Progress UI를 실현한다
	public class DProgress
	{

		public int ProgressVal_TPer = 0;		// TPercent 1000이 100%이다
		private bool bFinish = false;
		public string text = "";
		private int nAutoProgress_sec = 0;
		private Form parentForm = null;
		Thread th = null;
		public DProgress(Form parentForm = null)
		{
			this.parentForm = parentForm;
		}
		
		// Progress 시작
		// nAutoProgress_msec는, 1초로(1000ms) 세팅하면 자동으로 1초동안 균등하게 progress 진행되도록 한다
		public void StartProgress(int nAutoProgress_msec = 0, string text = "")
		{
			ProgressVal_TPer = 0;
			bFinish = false;
			this.text = text;
			this.nAutoProgress_sec = nAutoProgress_msec;
			// 일회성 Thread 생성
			th = new Thread(ExecuteProgress);
			th.Start(this);
		}

		public void EndProgress()
		{
			bFinish = true;
			if(th != null)
				th.Join();

			th = null;
		}

		// Thread Function
		private static void ExecuteProgress(object o)
		{
			// UI 생성, 이 객체를 사용하는 쪽에서 EndProgress를 호출할때까지 Progress UI를 보여줌
			DProgress obj = (DProgress)o;
			UCommonProgressForm dlg = new UCommonProgressForm();
			dlg.Show();

			int sleepTime_msec = 50;
			double dInterval = 0;
			int nAcc = 0;

			// 자동 Progress는, 1초로 세팅하면 자동으로 1초동안 균등하게 progress 진행되도록 한다
			if (obj.nAutoProgress_sec != 0)
			{
				dInterval = (double)obj.nAutoProgress_sec / sleepTime_msec;
				dInterval = Math.Max(dInterval, 1);
				nAcc = (int)(1000 / dInterval);
			}

			// 끝날때까지 무한루프
			while (obj.bFinish == false)
			{
				if (obj.nAutoProgress_sec != 0 )
				{
					obj.ProgressVal_TPer += nAcc;

					int nVal = obj.ProgressVal_TPer;
					nVal = Math.Min(Math.Max(0, nVal), 1000);
					dlg.progressBar1.Value = nVal;
					dlg.progressBar1.Refresh();
				}

				dlg.Refresh();
				Thread.Sleep(sleepTime_msec);
			}
			dlg.Close();
		}
	}
}
