using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace LogViewer
{
	public partial class UVersionConv : Form
	{
		public UVersionConv()
		{
			InitializeComponent();
			listBox_FileList.AllowDrop = true;
			listBox_FileList.DragEnter += new DragEventHandler(Form1_DragEnter);
			listBox_FileList.DragDrop += new DragEventHandler(Form1_DragDrop);
		}

		void Form1_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.Copy;
		}

		void Form1_DragDrop(object sender, DragEventArgs e)
		{
			string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
			foreach(string file in files)
			{
				if(Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("AMC_RECV") == 0 ||
					Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("AMC_SEND_PERIODIC") == 0||
                   Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("DRIVINGCTRL") == 0)
				{
					listBox_FileList.Items.Add(file);
				}
			}
			
			
		}

		private void btnClear_Click(object sender, EventArgs e)
		{
			listBox_FileList.Items.Clear();
		}

		private void btn_FileDlg_Click(object sender, EventArgs e)
		{
			OpenFileDialog dlg = new OpenFileDialog();
			dlg.Multiselect = true;
			dlg.Filter = "Text  file (*.log)|*.log|All files (*.*)|*.*";
			if (dlg.ShowDialog() != DialogResult.OK)
				return;

			listBox_FileList.Items.AddRange(dlg.FileNames);
		}

		private void btnConvert_Click(object sender, EventArgs e)
		{
			int nCnt = 0;
			int nCntFail = 0;
			DLogVerConv.init();
			foreach(var a in listBox_FileList.Items)
			{
				if (DLogVerConv.LogVer2Conv((string)a))
					nCnt++;
				else
					nCntFail++;
			}

			MessageBox.Show(String.Format("{0}개 항목 변환 성공.\n{1}개 항목 변환 실패.\n\n실패한 항목은 변환할 필요가 없는 로그.", nCnt, nCntFail));
			
		}

		private void btnClose_Click(object sender, EventArgs e)
		{
			Close();
		}
	}

}
