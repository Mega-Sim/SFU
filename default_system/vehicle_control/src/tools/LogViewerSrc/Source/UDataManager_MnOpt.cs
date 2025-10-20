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
using System.Runtime.InteropServices;
using System.Diagnostics;
using LogViewer;
using MyTools;
using System.Threading;

namespace LogViewer_DataManager
{
    public partial class UDataManager_MnOpt : Form
    {
        bool bEditMode = false;
        public DLogTime timeOpt = new DLogTime();
        public List<DMonitorItems> CheckedItem = new List<DMonitorItems>();
		public List<DMonitorItems> AllStandardItem = new List<DMonitorItems>();
		public static string INI_PATH =  "";
		public UDataManager m_Parent = null;

        public UDataManager_MnOpt()
        {
            InitializeComponent();
			INI_PATH = Application.StartupPath + @"\MonitorSetting.ini";
			Init();
			

		}
        private void Init()
        {
            CheckedItem.Clear();
			AllStandardItem.Clear();

			LoadINI();
            //btn_Apply_Click(null, null);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            // 로그 다운로드 UI 갱신
            numeric_LTimeFrom_Year.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Month.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Day.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Hour.Enabled = !radioButton_LogTime3_All.Checked;

            numeric_LTimeTo_Year.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Month.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Day.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Hour.Enabled = radioButton_LogTime1_Range.Checked;
            
            btn_UpLevel.Enabled = bEditMode;
            btn_UpDown.Enabled = bEditMode;
            btn_Delete.Enabled = bEditMode;
            btn_Add.Enabled = bEditMode;
            btn_Edit.Enabled = bEditMode;
            textBox_Item.Enabled = bEditMode;
            textBox_Cond.Enabled = bEditMode;
            textBox_Desc.Enabled = bEditMode;
            textBox_ErrCnt.Enabled = bEditMode;

			btn_stop.Enabled = m_Parent.m_DataMan.GetCurTask() != eDataManTask.None;


		}

        // 화면에 보이는 이런저런 값들 INI Load. 다음번 열때 똑같은 환경으로 만들어주기 위함
        public void LoadINI()
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = INI_PATH;
                string str = "";

                str = ini.Read(this.Name, numeric_LTimeFrom_Year.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeFrom_Year.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeFrom_Month.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeFrom_Month.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeFrom_Day.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeFrom_Day.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeFrom_Hour.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeFrom_Hour.Value = Convert.ToInt32(str);


                str = ini.Read(this.Name, numeric_LTimeTo_Year.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeTo_Year.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeTo_Month.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeTo_Month.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeTo_Day.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeTo_Day.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, numeric_LTimeTo_Hour.Name, "");
                if (String.IsNullOrEmpty(str) == false)
                    numeric_LTimeTo_Hour.Value = Convert.ToInt32(str);

                str = ini.Read(this.Name, "TimeZoneRadio", "0");
                int n = 0;
                if(Int32.TryParse(str, out n))
                {
                    radioButton_LogTime1_Range.Checked = n == 0;
                    radioButton_LogTime2_AfterAll.Checked = n == 1;
                    radioButton_LogTime3_All.Checked = n == 2;
                }

                str = ini.Read(this.Name, "ItemCnt", "0");
                listView_MonitorItems.Items.Clear();
                if (Int32.TryParse(str, out n))
                {
                    for(int i=0; i<n; i++)
                    {
                        string chk = ini.Read(this.Name, String.Format("Item_{0}_Chk",i), "0");
                        string name = ini.Read(this.Name, String.Format("Item_{0}_Name", i), "");
                        string errCnt = ini.Read(this.Name, String.Format("Item_{0}_ErrCnt", i), "");
                        string cond = ini.Read(this.Name, String.Format("Item_{0}_Cond", i), "");
                        string desc = ini.Read(this.Name, String.Format("Item_{0}_Desc", i), "");

                        if (String.IsNullOrEmpty(name))
                            continue;
                        if (name.Length < 2)
                            continue;

                        string[] arr = new string[5];
                        arr[0] = "";
                        arr[1] = name;
                        arr[2] = errCnt;
                        arr[3] = cond;
                        arr[4] = desc;
                        ListViewItem lvi = new ListViewItem(arr);
                        lvi.Checked = chk == "1";

                        listView_MonitorItems.Items.Add(lvi);
                    }
                }

            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        // 화면에 보이는 이런저런 값들 INI Save. 다음번 열때 똑같은 환경으로 만들어주기 위함
        public void SaveINI()
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = INI_PATH;
                //string str = "";

                ini.WriteEx(this.Name, numeric_LTimeFrom_Year.Name, numeric_LTimeFrom_Year.Value);
                ini.WriteEx(this.Name, numeric_LTimeFrom_Month.Name, numeric_LTimeFrom_Month.Value);
                ini.WriteEx(this.Name, numeric_LTimeFrom_Day.Name, numeric_LTimeFrom_Day.Value);
                ini.WriteEx(this.Name, numeric_LTimeFrom_Hour.Name, numeric_LTimeFrom_Hour.Value);
                ini.WriteEx(this.Name, numeric_LTimeTo_Year.Name, numeric_LTimeTo_Year.Value);
                ini.WriteEx(this.Name, numeric_LTimeTo_Month.Name, numeric_LTimeTo_Month.Value);
                ini.WriteEx(this.Name, numeric_LTimeTo_Day.Name, numeric_LTimeTo_Day.Value);
                ini.WriteEx(this.Name, numeric_LTimeTo_Hour.Name, numeric_LTimeTo_Hour.Value);
                
                
                int n = 0;

                if (radioButton_LogTime1_Range.Checked)
                    n = 0;
                else if(radioButton_LogTime2_AfterAll.Checked)
                    n= 1;
                else if(radioButton_LogTime3_All.Checked)
                    n = 2;
                ini.WriteEx(this.Name, "TimeZoneRadio", n);


                ini.WriteEx(this.Name, "ItemCnt", listView_MonitorItems.Items.Count);
                
                for (int i = 0; i < listView_MonitorItems.Items.Count; i++)
                {
                    ini.WriteEx(this.Name, String.Format("Item_{0}_Chk", i), listView_MonitorItems.Items[i].Checked ? 1 : 0);
                    ini.WriteEx(this.Name, String.Format("Item_{0}_Name", i), listView_MonitorItems.Items[i].SubItems[1].Text);
                    ini.WriteEx(this.Name, String.Format("Item_{0}_ErrCnt", i), listView_MonitorItems.Items[i].SubItems[2].Text);
                    ini.WriteEx(this.Name, String.Format("Item_{0}_Cond", i), listView_MonitorItems.Items[i].SubItems[3].Text);
                    ini.WriteEx(this.Name, String.Format("Item_{0}_Desc", i), listView_MonitorItems.Items[i].SubItems[4].Text);
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        public void Apply()
        {
            timeOpt.From = new DateTime((int)numeric_LTimeFrom_Year.Value, (int)numeric_LTimeFrom_Month.Value, (int)numeric_LTimeFrom_Day.Value, (int)numeric_LTimeFrom_Hour.Value, 0, 0);
            timeOpt.To = new DateTime((int)numeric_LTimeTo_Year.Value, (int)numeric_LTimeTo_Month.Value, (int)numeric_LTimeTo_Day.Value, (int)numeric_LTimeTo_Hour.Value, 59, 59);
            timeOpt.opt = DLogTime.eTimeZone.Range;

            if (radioButton_LogTime1_Range.Checked)
                timeOpt.opt = DLogTime.eTimeZone.Range;
            else if (radioButton_LogTime2_AfterAll.Checked)
                timeOpt.opt = DLogTime.eTimeZone.AfterAll;
            else if (radioButton_LogTime3_All.Checked)
                timeOpt.opt = DLogTime.eTimeZone.AllTime;

            CheckedItem.Clear();
			AllStandardItem.Clear();

			foreach (ListViewItem lvi in listView_MonitorItems.Items)
            {
                if(lvi.Checked)
                {
                    DMonitorItems item = new DMonitorItems();
                    item.name = lvi.SubItems[1].Text;
                    item.ErrCnt = Convert.ToInt32(lvi.SubItems[2].Text);
                    item.cond = lvi.SubItems[3].Text;
                    item.desc = lvi.SubItems[4].Text;

                    CheckedItem.Add(item);
                }
            }
        }

        private void btn_DoEdit_Click(object sender, EventArgs e)
        {
            bEditMode = !bEditMode;
        }

        private void btn_UpLevel_Click(object sender, EventArgs e)
        {
            if (listView_MonitorItems.SelectedItems.Count != 1)
                return;
            int currentIndex = listView_MonitorItems.SelectedItems[0].Index;

            if (currentIndex > 0)
            {
                ListViewItem lvi = listView_MonitorItems.Items[currentIndex];
                listView_MonitorItems.Items.RemoveAt(currentIndex);
                listView_MonitorItems.Items.Insert(currentIndex - 1, lvi);
            }
            listView_MonitorItems.Focus();
        }

        private void btn_UpDown_Click(object sender, EventArgs e)
        {
            if (listView_MonitorItems.SelectedItems.Count != 1)
                return;
            int currentIndex = listView_MonitorItems.SelectedItems[0].Index;

            if (currentIndex >= 0 && currentIndex < listView_MonitorItems.Items.Count-1)
            {
                ListViewItem lvi = listView_MonitorItems.Items[currentIndex];
                listView_MonitorItems.Items.RemoveAt(currentIndex);
                listView_MonitorItems.Items.Insert(currentIndex + 1, lvi);
            }
            listView_MonitorItems.Focus();
        }

        private void btn_Delete_Click(object sender, EventArgs e)
        {
            if (listView_MonitorItems.SelectedItems.Count != 1)
                return;

			if (MessageBox.Show("삭제", String.Format("{0} 항목을 삭제하시겠습니까?", listView_MonitorItems.SelectedItems[0].SubItems[1].Text), MessageBoxButtons.OKCancel) != DialogResult.OK)
				return;

            int currentIndex = listView_MonitorItems.SelectedItems[0].Index;

            if (currentIndex >= 0)
            {
                listView_MonitorItems.Items.RemoveAt(currentIndex);
            }
            listView_MonitorItems.Focus();
        }

        private void btn_Add_Click(object sender, EventArgs e)
        {
            if(textBox_Item.Text.Length == 0 || textBox_Item.Text.Length > 5)
            {
                MessageBox.Show("Item 항목을 잘 적어주세요.");
                return;
            }
            if (GMethod.IsOnlyAZ09(textBox_Item.Text) == false)
            {
                MessageBox.Show("항목에 특수문자는 입력할 수 없습니다.");
                return;
            }

            int nTmp = 0;
            if(Int32.TryParse(textBox_ErrCnt.Text, out nTmp) == false)
            {
                MessageBox.Show("에러 기준값은 숫자만 입력하세요");
                return;
            }

            textBox_Item.Text = textBox_Item.Text.ToUpper();

            foreach (ListViewItem lvi in listView_MonitorItems.Items)
            {
                if(lvi.SubItems[1].Text == textBox_Item.Text)
                {
                    MessageBox.Show("이미 존재하는 항목");
                    return;
                }
            }

            string[] arr = new string[5];
            arr[0] = "";
            arr[1] = textBox_Item.Text;
            arr[2] = textBox_ErrCnt.Text;
            arr[3] = textBox_Cond.Text;
            arr[4] = textBox_Desc.Text;
            ListViewItem item = new ListViewItem(arr);
            item.Checked = true;

            listView_MonitorItems.Items.Add(item);
        }
		

        private void listView_MonitorItems_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView_MonitorItems.SelectedItems.Count != 1)
                return;

            ListViewItem lvi = listView_MonitorItems.SelectedItems[0];
            textBox_Item.Text = lvi.SubItems[1].Text;
            textBox_ErrCnt.Text = lvi.SubItems[2].Text;
            textBox_Cond.Text = lvi.SubItems[3].Text;
            textBox_Desc.Text = lvi.SubItems[4].Text;

            listView_MonitorItems.Focus();
        }

        private void button_Edit_Click(object sender, EventArgs e)
        {
            if (listView_MonitorItems.SelectedItems.Count != 1)
            {
                MessageBox.Show("한개의 항목을 선택해 주세요.");
                return;
            }

            ListViewItem lvi = listView_MonitorItems.SelectedItems[0];
            lvi.SubItems[1].Text = textBox_Item.Text;
            lvi.SubItems[2].Text = textBox_ErrCnt.Text;
            lvi.SubItems[3].Text = textBox_Cond.Text;
            lvi.SubItems[4].Text = textBox_Desc.Text;

            listView_MonitorItems.Focus();
        }

		private void textBox_Item_TextChanged(object sender, EventArgs e)
		{

		}

		private void label7_Click(object sender, EventArgs e)
		{

		}

		private void textBox_ErrCnt_TextChanged(object sender, EventArgs e)
		{

		}

		private void textBox_Desc_TextChanged(object sender, EventArgs e)
		{

		}

		private void textBox_Cond_TextChanged(object sender, EventArgs e)
		{

		}

		private void label1_Click(object sender, EventArgs e)
		{

		}

		private void label3_Click(object sender, EventArgs e)
		{

		}

		private void label2_Click(object sender, EventArgs e)
		{

		}

		//private void button_selectFolder_Click(object sender, EventArgs e)
		//{
		//	MessageBox.Show("호기별로 모니터링 로그가 있는 폴더를 지정해주세요. (상위폴더)");

		//	FolderBrowserDialog dlg = new FolderBrowserDialog();

		//	IniMan ini = new IniMan();
		//	ini.Path = UMain.INI_PATH;
		//	string defalutPath = ini.Read("DataManager_DownloadPage", textBox_MonitorLogPath.Name, "");

		//	if (Directory.Exists(defalutPath))
		//		dlg.SelectedPath = defalutPath;

		//	if (dlg.ShowDialog() != DialogResult.OK)
		//		return;

		//	textBox_MonitorLogPath.Text = dlg.SelectedPath;

		//	ini.Path = UMain.INI_PATH;
		//	ini.Write("DataManager_DownloadPage", textBox_MonitorLogPath.Name, textBox_MonitorLogPath.Text);
		//}

		string prevPath = "";
		private void btn_MonitorDownload_Click(object sender, EventArgs e)
		{
			Apply();

			if (m_Parent.m_OhtList.Count == 0)
			{
				MessageBox.Show("선택된 OHT가 없습니다.");
				return;
			}

			if (MessageBox.Show("우측 OHT List에서 선택한 호기의 모니터링 로그를 다운로드 합니다.\n\n로그파일은 *.mlog 파일 한개로 통합 저장됩니다.", "", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
				return;

			SaveFileDialog dlg = new SaveFileDialog();
			if(String.IsNullOrEmpty(prevPath) == false)
				dlg.InitialDirectory = prevPath;
			
			dlg.FileName = String.Format("{0}_{1}{2:00}{3:00}_{4:00}{5:00}{6:00}", m_Parent.comboBox_OhtList.Text, DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, DateTime.Now.Hour, DateTime.Now.Minute, DateTime.Now.Second);
			dlg.Filter = "Monitoring Log file (*.mlog)|*.mlog|All files (*.*)|*.*";
			if (dlg.ShowDialog() != DialogResult.OK)
				return;


			sDownloadLog obj = new sDownloadLog();

			obj.logTime.From = new DateTime((int)numeric_LTimeFrom_Year.Value, (int)numeric_LTimeFrom_Month.Value, (int)numeric_LTimeFrom_Day.Value, (int)numeric_LTimeFrom_Hour.Value, 0, 0);
			obj.logTime.To = new DateTime((int)numeric_LTimeTo_Year.Value, (int)numeric_LTimeTo_Month.Value, (int)numeric_LTimeTo_Day.Value, (int)numeric_LTimeTo_Hour.Value, 59, 59);
			obj.logTime.opt = radioButton_LogTime1_Range.Checked ? DLogTime.eTimeZone.Range : (radioButton_LogTime2_AfterAll.Checked ? DLogTime.eTimeZone.AfterAll : DLogTime.eTimeZone.AllTime);

			foreach (sOHTList aa in m_Parent.m_OhtList)
				aa.Result = "";

			m_Parent.textBox_Status.Text = "";
			m_Parent.m_DataMan.m_DownloadLogList.Clear();
			m_Parent.m_DataMan.m_DownloadLogList.Add(obj);
			m_Parent.m_DataMan.m_MonitoringSavePath = dlg.FileName;
			m_Parent.m_DataMan.DoWork(ref m_Parent.m_OhtList, eDataManTask.Monitor_Download);

		}

		private void button_selectFolder_Click(object sender, EventArgs e)
		{

		}

		private void btn_stop_Click(object sender, EventArgs e)
		{
			m_Parent.m_LogQ.EnQ(">> 취소중");
			m_Parent.m_DataMan.m_bJobCancel = true;
			m_Parent.m_DataMan.bThreadStop_CheckMonitor_atLocal = true;
		}
		
		// 모니터링 로그 분석 버튼
		private void btn_Analyze_Click(object sender, EventArgs e)
		{
			Apply();

			// 시간대를 잘 선택했는지 확인해주자
			if(timeOpt.opt != DLogTime.eTimeZone.AllTime)
			{
				if (MessageBox.Show("특정 시간대를 지정하셨습니다.\n\n이 경우 MLog 파일에서 해당하는 시간대만 검사하게 됩니다.\nMLog 전체 시간대에서 검사하고 싶다면 [전체 시간대] 를 선택해 주세요.", "", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning) != DialogResult.OK)
					return;
			}

			OpenFileDialog dlg = new OpenFileDialog();
			if(String.IsNullOrEmpty(prevPath_saveResult) == false)
				dlg.InitialDirectory = prevPath_saveResult;
			dlg.Filter = "Monitoring Log file (*.mlog)|*.mlog|All files (*.*)|*.*";
			if (dlg.ShowDialog() != DialogResult.OK)
				return;

			// 경로 정보 기억해 줘야 사용자가 편함
			prevPath_lastOpenFileName = Path.GetFileNameWithoutExtension(dlg.FileName);
			prevPath_saveResult = Path.GetDirectoryName(dlg.FileName);

			if (MessageBox.Show("모니터링 로그 검사를 하시겠습니까?\n이 작업은 OHT에 접속하지 않습니다.", "", MessageBoxButtons.OKCancel) != DialogResult.OK)
				return;

			//m_Parent.Thread_MonitorCheck_atLocal.Abort();
			//m_Parent.Thread_MonitorCheck_atLocal = null;

			// 결과 객체 초기화
			m_Parent.m_MonitorResult.Init();
			AnalyzeMonitorLog(dlg.FileName);
			
			//m_Parent.m_strMonitorLocalPath = prevPath22;
			//m_Parent.m_DataMan.bThreadStop_CheckMonitor_atLocal = false;
			//
			//// 스레드로 수행
			//m_Parent.Thread_MonitorCheck_atLocal = new Thread(DDataManager.DoCheckMonitorThread_atLocal);
			//m_Parent.Thread_MonitorCheck_atLocal.Start(m_Parent.m_DataMan);
		}

		// 결과 저장 객체 리스트
		public List<UDataManager.DMonitorResultRow> m_resultRowList = new List<UDataManager.DMonitorResultRow>();


		//2019-01-09,  00:02:29.156 <===  이 포멧에서 날짜 시간 추출하여 옵션에 선택된 시간대에 해당하는지 검사
		bool CheckDateTime_inMLog(string date, string time)
		{
			if (timeOpt.opt == DLogTime.eTimeZone.AllTime)
				return true;

			bool bRet = false;
			try
			{
				int year = Convert.ToInt32(date.Substring(0, 4));
				int mon = Convert.ToInt32(date.Substring(5, 2));
				int day = Convert.ToInt32(date.Substring(8, 2));
				int hour = Convert.ToInt32(time.Substring(0, 2));
				int min = Convert.ToInt32(time.Substring(3, 2));
				int sec = Convert.ToInt32(time.Substring(6, 2));

				bRet = timeOpt.IsIn(year, mon, day, hour);
			}
			catch
			{
				return false;
			}

			return bRet;
		}
		private void AnalyzeMonitorLog(string mlogPath)
		{
			string[] lines = File.ReadAllLines(mlogPath);
			
			string curOht = "";
			string prevOht = "";
			DMonitorResult.iRow rowObj = null;
			List<DMonitorResult.iRow> listRow = new List<DMonitorResult.iRow>();

			foreach (string line in lines)
			{
				try
				{
					string[] arr = line.Split('/');
					if (arr.Length < 3)
						continue;

					if (CheckDateTime_inMLog(arr[1], arr[2]) == false)		// 시간 체크
						continue;

					curOht = arr[0];

					// line by line 으로 읽다가 한개 OHT 내역이 끝남을 알림. row 항목 새로 생성
					if (prevOht != curOht)
					{
						rowObj = new DMonitorResult.iRow();
						rowObj.OHTName = curOht;
						
						listRow.Add(rowObj);
						prevOht = curOht;
					}

					// 카운트 누적시키기
					for (int it = 3; it < arr.Length; it += 2)
					{
						if (arr[it].Length < 2)
							continue;

						rowObj.AddCount(arr[it], Convert.ToInt32(arr[it + 1]));
					}
					rowObj.LogHours++;
				}
				catch (Exception eee)
				{
					continue;
				}
			}
			
			// 취합한 데이터로 기준값 초과하는게 있는지 찾는다
			foreach (DMonitorResult.iRow row in listRow)
			{
				int nItemCnt = CheckedItem.Count;
				// 모든 값을을 기준값과 비교하여 초과하는 항목은 ErrItemIdx에 기록해놓는다.
				for (int y = 0; y < nItemCnt; y++)
				{
					string name = CheckedItem[y].name;
					int nIdxPos = row.Items.FindIndex(o => o.name == name);
					if (nIdxPos < 0)
						continue;

					if (CheckedItem[y].ErrCnt < row.GetItemCount(nIdxPos))   // 기준값은 시간당 발생건과 비교한다 xxxx ==> 누적으로 비교
					{
						row.ErrItemIdx.Add(nIdxPos);
					}
				}
			}

			// *** CSV 저장
			List<string> saveLines = new List<string>();

			// csv header 헤더 입력
			string strRow = "OHT,측정시간";
			foreach (ListViewItem lvi in listView_MonitorItems.Items)
			{
				// 기준,최대,평균,누적
				string name = lvi.SubItems[1].Text;
				strRow += String.Format(",최대{0},누적{1}", name, name);
			}

			saveLines.Add(strRow);

			foreach (DMonitorResult.iRow row in listRow)
			{
				strRow = row.OHTName;
				strRow += String.Format(",{0}", row.LogHours);
				for (int n=0; n<row.Items.Count; n++)
				{
					DMonitorResult.iItem item = row.Items[n];
					strRow += String.Format(",{0},{1}", row.GetItemCount_inAnHour(n), row.GetItemCount(n));
				}
				saveLines.Add(strRow);
			}

			// 결과 Grid에 보여줄 Row 리스트
			m_resultRowList.Clear();

			// *** 에러 항목들 두번째 탭에 grid 표시
			//List<string> errOhtMsg = new List<string>();
			foreach (DMonitorResult.iRow row in listRow)
			{
				if (row.ErrItemIdx.Count > 0)
				{
					//errOhtMsg.Add(String.Format("{0} 호기 에러 항목 ***", row.OHTName));
					foreach (int nErrIdx in row.ErrItemIdx)
					{
						int nStVal = 0;
						string chkPoint = "";
						DMonitorItems itt = CheckedItem.Find(o => o.name == row.Items[nErrIdx].name);
						if (itt == null)
							continue;

						nStVal = itt.ErrCnt;
						chkPoint = itt.desc;

						UDataManager.DMonitorResultRow obj = new UDataManager.DMonitorResultRow();
						obj.data[(int)UDataManager.eMonitorResultCol.OHT] = row.OHTName;
						obj.data[(int)UDataManager.eMonitorResultCol.항목] = row.Items[nErrIdx].name;
						obj.data[(int)UDataManager.eMonitorResultCol.기준값] = Convert.ToString(nStVal);
						obj.data[(int)UDataManager.eMonitorResultCol.누적_발생] = Convert.ToString(row.GetItemCount(nErrIdx));
						obj.data[(int)UDataManager.eMonitorResultCol.점검_사항] = chkPoint;
						m_resultRowList.Add(obj);

						//string str = String.Format("\t{0}항목: 기준값 {1}, 누적 {2} 회 발생. {3} 점검 필요.", row.Items[nErrIdx].name, nStVal, row.GetItemCount(nErrIdx), chkPoint);
						//errOhtMsg.Add(str);
					}
					//errOhtMsg.Add("");
				}
			}

			m_Parent.UpdateMonitorResultGrid(m_resultRowList);	// 결과 그리드 표시

			//string totalLine = "";
			//foreach(var a in errOhtMsg)
			//{
			//	totalLine += a + "\r\n";
			//}
			//m_Parent.textBox_MonitorResult.Text = totalLine;

			SaveFileDialog dlg = new SaveFileDialog();
			if(String.IsNullOrEmpty(prevPath_saveResult) == false)
			{
				dlg.InitialDirectory = prevPath_saveResult;
			}
			dlg.Title = "분석 내용을 엑셀로 저장하세요.";
			dlg.Filter = "Excel CSV file (*.csv)|*.csv|All files (*.*)|*.*";
			dlg.FileName = Path.GetFileNameWithoutExtension(mlogPath) + "_Summary";
			if (dlg.ShowDialog() == DialogResult.OK)
			{
				prevPath_saveResult = Path.GetDirectoryName(dlg.FileName);
				File.WriteAllLines(dlg.FileName, saveLines, Encoding.Default);
				MessageBox.Show("엑셀파일로 저장되었습니다.");
			}
			m_Parent.tabControl_Monitor.SelectTab(1);	// 결과탭으로 이동
		}
		public string prevPath_saveResult = "";
		public string prevPath_lastOpenFileName = "";
		private void chk_opt_CheckedChanged(object sender, EventArgs e)
		{
			btn_Log2MLog.Visible = chk_opt.Checked;
		}

		private void btn_Log2MLog_Click(object sender, EventArgs e)
		{
			MessageBox.Show("모니터링 로그가 호기별로 모여있는 상위폴더를 찾아주세요.");

			FolderBrowserDialog dirDlg = new FolderBrowserDialog();
			if (dirDlg.ShowDialog() != DialogResult.OK)
				return;

			MessageBox.Show("MLog 파일 한개로 저장할 위치를 입력해 주세요.");

			SaveFileDialog dlg = new SaveFileDialog();
			if (String.IsNullOrEmpty(prevPath) == false)
				dlg.InitialDirectory = prevPath;

			dlg.Filter = "Monitoring Log file (*.mlog)|*.mlog|All files (*.*)|*.*";
			if (dlg.ShowDialog() != DialogResult.OK)
				return;

			// 바로 작업 ***
			List<string> lineList = new List<string>();

			string[] dirs = Directory.GetDirectories(dirDlg.SelectedPath);
			foreach(string dir in dirs)
			{
				string[] files = Directory.GetFiles(dir);
				string oht = GetOHTName_withFolderName(Path.GetFileName(dir));
				if (String.IsNullOrEmpty(oht))
					continue;
				
				foreach(string file in files)
				{
					DateTime tm = new DateTime();
					if (DDataManager.GetLogFileTime_byFileName(file, out tm) == false)
						continue;

					string[] lines = File.ReadAllLines(file);
					foreach(string line in lines)
					{
						if (line.Length < 10)
							continue;
						string ln = line;
						ln = ln.Replace('[', '/');
						ln = ln.Replace(']', '/');
						ln = String.Format("{0}/{1:00}-{2:00}-{3:00}{4}", oht, tm.Year, tm.Month, tm.Day, ln);
						lineList.Add(ln);
					}
				}
			}

			File.WriteAllLines(dlg.FileName, lineList);
			MessageBox.Show("완료되었습니다.");

		}
		string GetOHTName_withFolderName(string folderName)
		{
			string val = "";
			try
			{
				int pos = folderName.IndexOf('V');
				if (pos < 0 || pos + 6 > folderName.Length)
					return "";

				val = folderName.Substring(pos, 6);
				

			}
			catch (Exception)
			{
				return "";
			}
			return val;
		}
	}

	public class DMonitorItems
    {
        public string name = "";
        public string cond = "";
        public string desc = "";
        public int ErrCnt = 0;
    }
}
