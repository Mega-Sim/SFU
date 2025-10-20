using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
using LogViewer;
using MyTools;
using Excel = Microsoft.Office.Interop.Excel;
using System.Reflection;
using System.Collections;

namespace LogViewer_DataManager
{
    public partial class UDataManager : Form
    {

        // *********************************************************************************************
        // 업로드 그리드 Col 항목
        enum eUploadGridCol
        {
            로컬경로, 파일이름, Size, ____, OHT_Target_경로, CountOf
        }
		public enum eMonitorResultCol { OHT, 항목, 기준값, 누적_발생, 점검_사항, CountOf } // result 결과 창 col 정의
		public class DMonitorResultRow { public string []data = new string[(int)eMonitorResultCol.CountOf]; }		//result 결과 Row Class 정의

		public enum eResultType
        {
            Result, Version, Ping
        }
        // 화면 하단에 로그 나오는 부분 처리하는 메시지 큐
        public class JobLogQueue    // inner class
        {
            List<string> arrMsg = new List<string>();
            object lockVar = new object();
			public bool bFlushToFile = false;      // 파일로 로그 flush 하라는 flag
			public string flushFilePath = "";
            public void EnQ(string msg)
            {
                lock (lockVar)
                {
                    arrMsg.Add(msg);
                }
            }
            public int GetCount()
            {
                int nVal = 0;
                lock (lockVar)
                {
                    nVal = arrMsg.Count();
                }
                return nVal;
            }
            public bool DeQ(out string msg)
            {
                bool bResult = false;
                msg = "";
                lock (lockVar)
                {
                    if (arrMsg.Count > 0)
                    {
                        msg = arrMsg[0];
                        arrMsg.RemoveAt(0);
                        bResult = true;
                    }
                }
                return bResult;
            }
			public void Flush_toFile(string path)
			{
				bFlushToFile = true;
				flushFilePath = path;
			}
        }
        // ****************************************************************************************************************

        public UDataManager_MnOpt m_dlgMonitorOpt = new UDataManager_MnOpt();
		public DMonitorResult m_MonitorResult = new DMonitorResult();
        public JobLogQueue m_LogQ = new JobLogQueue();
        public List<sOHTList> m_OhtList = new List<sOHTList>();
        OpenFileDialog m_dlgFileOpenUpload = new OpenFileDialog();
        List<sUploadFile> m_ArrUploadList = new List<sUploadFile>();
        string m_OhtListPath = "";
		
		
        public DDataManager m_DataMan = new DDataManager();
        public delegate void OhtResult_UIDele(eResultType type, string IP, string msg);
        OhtResult_UIDele m_OhtResult_UIDel;
        public string m_strMonitorLocalPath = "";

        private delegate void DelegateMonitorResultUpdate();
		string[] m_arrFile_Oht = null;
		public bool m_bAvaliable = false;
        

		public UDataManager()
        {
            InitializeComponent();
			m_dlgMonitorOpt.m_Parent = this;

            TDrawingControl.SetDoubleBuffered(dataGridView_MonitorResult);
			dataGridView_MonitorResult.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;

			m_DataMan.m_Parent = this;
            m_OhtListPath = Application.StartupPath + "\\" + "OHTList.txt";
            m_dlgFileOpenUpload.Title = "업로드 할 파일을 선택하세요. 복수 선택 가능";
            m_dlgFileOpenUpload.Multiselect = true;

            listView_OhtList.Font = new Font("맑은 고딕", 10);
            //listView_OhtList.HeaderStyle = ColumnHeaderStyle.Nonclickable;
            
            m_OhtResult_UIDel = SetOhtResult; //delegate
            textBox_Status.Text = ">> Welecom to Data Manager~\r\n";
			tabPage_MonitorOpt.Controls.Add(m_dlgMonitorOpt.Controls[0]);

			m_arrFile_Oht = Directory.GetFiles(Application.StartupPath, "*.OHTList.txt", SearchOption.TopDirectoryOnly);
			if(m_arrFile_Oht.Length == 0)
			{
				MessageBox.Show("실행폴더 내에 OHT 리스트 파일이 없어서 데이터 메니저를 사용할 수 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				
				return;
			}

			foreach (string file in m_arrFile_Oht)
				comboBox_OhtList.Items.Add(Path.GetFileNameWithoutExtension(file).Replace(".OHTList", ""));

			{
				IniMan ini = new IniMan();
				ini.Path = UMain.INI_PATH;
				string str = "";

				// OHT List 파일 이름 ini 가져와서 세팅한다
				str = ini.Read(this.Name, comboBox_OhtList.Name, "");
				int nnidx = 0;
				if (String.IsNullOrEmpty(str) == false)
				{

					if (Int32.TryParse(str, out nnidx))
					{
						if (comboBox_OhtList.Items.Count <= nnidx)
							nnidx = 0;

					}
				}
				comboBox_OhtList.SelectedIndex = nnidx;
			}
			LoadOHTListFile();

			InitGrid();
			
			m_bAvaliable = true;
		}

        // 그리드 초기화
        void InitGrid()
        {
            TDrawingControl.SetDoubleBuffered(dataGridView_Upload);
			dataGridView_Upload.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;

			dataGridView_Upload.ColumnCount = (int)eUploadGridCol.CountOf;

            // Upload Grid Col 추가
            for (int i = 0; i < (int)eUploadGridCol.CountOf; i++)
            {
                eUploadGridCol e = (eUploadGridCol)i;

                dataGridView_Upload.Columns[i].HeaderText = e.ToString().Replace('_', ' ');
                dataGridView_Upload.Columns[i].Name = e.ToString().Replace('_', ' ');
                dataGridView_Upload.Columns[i].DefaultCellStyle.Font = new Font("맑은 고딕", 9);
                dataGridView_Upload.Columns[i].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView_Upload.Columns[i].SortMode = DataGridViewColumnSortMode.NotSortable;
            }
            dataGridView_Upload.Columns[(int)eUploadGridCol.로컬경로].Width = 300;
            dataGridView_Upload.Columns[(int)eUploadGridCol.파일이름].Width = 250;
            dataGridView_Upload.Columns[(int)eUploadGridCol.Size].Width = 80;
            dataGridView_Upload.Columns[(int)eUploadGridCol.____].Width = 30;
            dataGridView_Upload.Columns[(int)eUploadGridCol.OHT_Target_경로].Width = 300;

            dataGridView_Upload.ColumnHeadersDefaultCellStyle.Font = new Font("맑은 고딕", 8);
            dataGridView_Upload.RowPostPaint += DataGridView_RowPostPaint;
            //dataGridView_Upload.ColumnHeadersHeight = 30;



            // User Log event Grid 초기화 **********************************************
            TDrawingControl.SuspendDrawing(dataGridView_MonitorResult);

            dataGridView_MonitorResult.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
            dataGridView_MonitorResult.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView_MonitorResult.ColumnCount = (int)eMonitorResultCol.CountOf;

            for (eMonitorResultCol i = 0; i < eMonitorResultCol.CountOf; i++)
            {
                dataGridView_MonitorResult.Columns[(int)i].Name = i.ToString().Replace('_', ' ');
                dataGridView_MonitorResult.Columns[(int)i].HeaderText = i.ToString().Replace('_', ' ');

				dataGridView_MonitorResult.Columns[(int)i].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
                dataGridView_MonitorResult.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
            }

            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.OHT].Width = 60;
            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.항목].Width = 60;
            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.기준값].Width = 80;
            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.누적_발생].Width = 90;
            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.점검_사항].Width = 600;

            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.기준값].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            dataGridView_MonitorResult.Columns[(int)eMonitorResultCol.누적_발생].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;

            dataGridView_MonitorResult.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            dataGridView_MonitorResult.AllowUserToResizeRows = false;
            dataGridView_MonitorResult.RowTemplate.Height = 25;
			
            TDrawingControl.ResumeDrawing(dataGridView_MonitorResult);
        }


        // UI상 OHT 리스트에서 특정 Oht에 대한 수행 결과 업데이트. 외부 Thread에서 호출가능
        public void SetOhtResult_Extern(eResultType type, string name, string msg)
        {
            m_OhtResult_UIDel(type, name, msg);
        }

        // delegate로 실행되는 UI OHT 리스트의 결과 Update 부분
        void SetOhtResult(eResultType type, string name, string msg)
        {
            sOHTList obj = m_OhtList.Find(o => o.Name == name);

            if (obj == null)
                return;

            // m_OhtList 내용이 바뀌면 timer에서 자동으로 refresh 됨
            if (type == eResultType.Ping)
                obj.Ping = msg;
            else if (type == eResultType.Version)
                obj.Version = msg;
            else if (type == eResultType.Result)
                obj.Result = msg;
        }


        public void LoadINI()
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = UMain.INI_PATH;
                string str = "";

				
				// 로그 다운로드 탭의 정보 ************************

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
                if (Int32.TryParse(str, out n))
                {
                    radioButton_LogTime1_Range.Checked = n == 0;
                    radioButton_LogTime2_AfterAll.Checked = n == 1;
                    radioButton_LogTime3_All.Checked = n == 2;
                }

                str = ini.Read(this.Name, textBox_LogDnLocalDir.Name, "");
                if (Directory.Exists(str))
                    textBox_LogDnLocalDir.Text = str;

                checkBox_AllLogFile.Checked = ini.Read(this.Name, "LogDownAllLogChk", "") == "1";
                
                str = ini.Read(this.Name, "LogDownLogListCnt", "0");
                listview_LogSelect.Items.Clear();
                if (Int32.TryParse(str, out n))
                {
                    for (int i = 0; i < n; i++)
                    {
                        string strx = ini.Read(this.Name, String.Format("LogDownLogList_{0}", i), "");
                        if (String.IsNullOrEmpty(strx))
                            continue;

                        string[] arrx = new string[2];
                        arrx[1] = strx;
                        listview_LogSelect.Items.Add(new ListViewItem(arrx));
                    }                    
                    if(listview_LogSelect.Items.Count == n)
                    {
                        for (int i = 0; i < n; i++)
                            listview_LogSelect.Items[i].Checked = ini.Read(this.Name, String.Format("LogDownLogList_Chked_{0}", i), "") == "1";
                    }
                }

                // 일반파일 다운로드 탭의 정보 ************************
                //int nCnt = listView_DownloadOHTFile.Items.Count;
                str = ini.Read(this.Name, "FileDownListCnt", "0");
                listView_DownloadOHTFile.Items.Clear();
                if (Int32.TryParse(str, out n))
                {
                    for (int i = 0; i < n; i++)
                    {
                        string strx = ini.Read(this.Name, String.Format("FileDownList_{0}", i), "");
                        if (String.IsNullOrEmpty(strx))
                            continue;

                        string[] arrx = new string[2];
                        arrx[1] = strx;
                        listView_DownloadOHTFile.Items.Add(new ListViewItem(arrx));
                    }
                    if(listView_DownloadOHTFile.Items.Count == n)
                    {
                        for (int i = 0; i < n; i++)
                            listView_DownloadOHTFile.Items[i].Checked = ini.Read(this.Name, String.Format("FileDownList_Chk{0}", i), "0") == "1" ? true : false;
                    }
                }

                str = ini.Read(this.Name, textBox_downloadPath.Name, "");
                if (Directory.Exists(str))
                    textBox_downloadPath.Text = str;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        // 화면에 보이는 이런저런 값들 INI Save. 다음번 열때 똑같은 환경으로 만들어주기 위함
        public bool SaveINI()
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = UMain.INI_PATH;

				ini.WriteEx(this.Name, comboBox_OhtList.Name, comboBox_OhtList.SelectedIndex);

				// 로그 다운로드 탭의 정보 ************************
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
                else if (radioButton_LogTime2_AfterAll.Checked)
                    n = 1;
                else if (radioButton_LogTime3_All.Checked)
                    n = 2;

                ini.WriteEx(this.Name, "TimeZoneRadio", n);
                ini.Write(this.Name, textBox_LogDnLocalDir.Name, textBox_LogDnLocalDir.Text);
                ini.WriteEx(this.Name, "LogDownAllLogChk", checkBox_AllLogFile.Checked ? "1" : "0");

                int nCnt = listview_LogSelect.Items.Count;
                ini.WriteEx(this.Name, "LogDownLogListCnt", nCnt);
                for (int i = 0; i < nCnt; i++)
                   ini.Write(this.Name, String.Format("LogDownLogList_{0}", i), listview_LogSelect.Items[i].SubItems[1].Text);
                
                for (int i = 0; i < nCnt; i++)
                {
                    int nVal = listview_LogSelect.Items[i].Checked ? 1 : 0;
                    ini.WriteEx(this.Name, String.Format("LogDownLogList_Chked_{0}", i), nVal);
                }

                // 일반파일 다운로드 탭의 정보 ************************
                nCnt = listView_DownloadOHTFile.Items.Count;
                ini.WriteEx(this.Name, "FileDownListCnt", nCnt);
                for (int i = 0; i < nCnt; i++)
                    ini.Write(this.Name, String.Format("FileDownList_{0}", i), listView_DownloadOHTFile.Items[i].SubItems[1].Text);

                for (int i = 0; i < nCnt; i++)
                    ini.WriteEx(this.Name, String.Format("FileDownList_Chk{0}", i), listView_DownloadOHTFile.Items[i].Checked ? 1 : 0);

                ini.WriteEx(this.Name, textBox_downloadPath.Name, textBox_downloadPath.Text);
				return true;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
			return false;
        }


        void SaveDownloadLogList()
        {
            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;

            //

            



            //ini.Write(refGrid.Name, "FontStyle", Convert.ToString(fontStype));

        }
        // OHT 리스트 파일 저장
        void SaveOHTListFile()
        {
			try
			{
				File.Delete(m_OhtListPath);
				string txt = "";
				foreach (sOHTList item in m_OhtList)
				{
					txt += String.Format("{0}/{1}/{2}/{3}/{4}/{5}/{6}\n", item.Name, item.IP, item.Result, item.Version, item.Ping, item.bChecked ? "1" : "0", item.Memo);
				}

				File.WriteAllText(m_OhtListPath, txt);
			}
			catch (Exception ee)
			{
				MessageBox.Show("OHT List 파일 저장 실패\n\n" + ee.Message);
			}
        }

        // OHT 리스트 파일 로딩
        void LoadOHTListFile()
        {
            m_OhtList.Clear();

            try
            {
                if (File.Exists(m_OhtListPath) == false)
                {
                    File.Create(m_OhtListPath);
                }
                string[] lines = File.ReadAllLines(m_OhtListPath);

                // line by line 돌면서 각 호기 정보 수집해서 m_OhtList에 입력
                foreach (string line in lines)
                {
                    if (String.IsNullOrEmpty(line))
                        continue;

                    string[] arr = line.Split('/');    
                    if (arr.Length < 2) // V30001 / 192.168.100.100 들어가려면 최소 2개 이상 나와야함
                        continue;
                    if (arr[0].Length < 4)  // 호기명 최대 4자 이상 
                        continue;
                    if (arr[1].Split('.').Length != 4)  // IP 주소 validation
                        continue;

                    int nTmpHogi = 0;
                    string strTmpHogi = arr[0].Substring(arr[0].Length - 4, 4);
                    if (Int32.TryParse(strTmpHogi, out nTmpHogi) == false)  // 호기명은 뒤에 숫자여야함
                        continue;

                    sOHTList obj = new sOHTList();
                    obj.Name = arr[0];
                    obj.nOHTNo = nTmpHogi;
                    obj.IP = arr[1];

                    // 그외 다른 정보들도 존재하면 읽을 수 있다
                    if (arr.Length > 2) obj.Result = arr[2];
                    if (arr.Length > 3) obj.Version = arr[3];
                    if (arr.Length > 4) obj.Ping = arr[4];
                    if (arr.Length > 5) obj.bChecked = arr[5] == "1";
                    if (arr.Length > 6) obj.Memo = arr[6];

                    m_OhtList.Add(obj);
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show("DataManager에서 OHT List 파일 로딩 실패\n\n" + ee.Message);
            }
            // UI Refresh
            Refresh_OhtList(true);
        }

        void Refresh_OhtListColor()
        {
            // 필요 없을듯. NG 일때 빨간색 표현
            //foreach (ListViewItem lvi in listView_OhtList.Items)
            //{
            //    if (lvi.SubItems.Count >= 5)
            //    {

            //        bool bFail = lvi.SubItems[3].Text.IndexOf("NG", 0) >= 0 || lvi.SubItems[4].Text.IndexOf("NG", 0) >= 0 || lvi.SubItems[5].Text.IndexOf("NG", 0) >= 0;
            //        Color clr = bFail ? Color.LightPink : Color.White;

            //        if (lvi.BackColor != clr)
            //            lvi.BackColor = clr;
            //    }
            //}
        }
        // m_OhtList 리스트를 OHT ListView 리프래시
        void Refresh_OhtList(bool bRedrawAll)
        {
            listView_OhtList.SuspendLayout();   // layout suspend

            if (listView_OhtList.Items.Count != m_OhtList.Count)
                bRedrawAll = true;

            if (bRedrawAll)
                listView_OhtList.Items.Clear();

            listBox_SelectedOHT.Items.Clear();  // listBox_SelectedOHT 에 선택된 OHT 리스트도 업데이트
            int nCheckCnt = 0;
            for (int i = 0; i < m_OhtList.Count; i++)
            {
                sOHTList obj = m_OhtList[i];

                if (obj.bChecked)
                    listBox_SelectedOHT.Items.Add(obj.Name);    // listBox_SelectedOHT 에 선택된 OHT 리스트도 업데이트

                if (bRedrawAll)
                {
                    string[] arr = new string[listView_OhtList.Columns.Count];
					arr[0] = obj.bChecked ? "1" : "0";
					arr[1] = obj.Name;
                    arr[2] = obj.IP;
                    arr[3] = obj.Result;
                    arr[4] = obj.Version;
                    arr[5] = obj.Ping;
                    arr[6] = obj.Memo;

                    ListViewItem lvi = new ListViewItem(arr);
                    lvi.Tag = obj.Name;       // 테그에 OHT 이름을 넣는다
                    lvi.Checked = obj.bChecked;

                    lvi.ImageIndex = obj.bChecked ? 1 : 0;

                    if (obj.bChecked) // 체크된건 굵게 표시하기 위함
                        lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
                    else
                        lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));


                    if (obj.bChecked)
                        nCheckCnt++;

                    listView_OhtList.Items.Add(lvi);
                }
                else
                {
                    ListViewItem lvi = listView_OhtList.Items[i];
                    //if ((string)lvi.Tag != m_OhtList.Find(o=>o.Name i)
                    lvi.Tag = obj.Name;
                    if (lvi.Checked != obj.bChecked)
                    {
                        //lvi.Checked = obj.bChecked;
                        CheckOhtList(i, obj.bChecked);  // 체크하는 방식이 조금 다름
                    }

                    if (obj.bChecked)
                        nCheckCnt++;

					lvi.ImageIndex = obj.bChecked ? 1 : 0;
					if (obj.bChecked) // 체크된건 굵게 표시하기 위함
						lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
					else
						lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));

					lvi.SubItems[0].Text = obj.bChecked ? "1 " : "0";

					if (lvi.SubItems[1].Text != obj.Name)
                        lvi.SubItems[1].Text = obj.Name;

                    if (lvi.SubItems[2].Text != obj.IP)
                        lvi.SubItems[2].Text = obj.IP;

                    if (lvi.SubItems[3].Text != obj.Result)
                    {
                        lvi.SubItems[3].Text = obj.Result;
                        lvi.SubItems[3].BackColor = obj.Result.IndexOf("OK", 0) >= 0 ? Color.LightGreen : (obj.Result.IndexOf("NG", 0) >= 0 ? Color.LightPink : Color.Yellow);
                    }

                    if (lvi.SubItems[4].Text != obj.Version)
                    {
                        lvi.SubItems[4].Text = obj.Version;
                        lvi.SubItems[4].BackColor = obj.Result.IndexOf("NG", 0) >= 0 ? Color.LightPink : Color.White;
                    }

                    if (lvi.SubItems[5].Text != obj.Ping)
                    {
                        lvi.SubItems[5].Text = obj.Ping;
                        lvi.SubItems[5].BackColor = obj.Result.IndexOf("NG", 0) >= 0 ? Color.LightPink : Color.White;
                    }

                    if (lvi.SubItems[6].Text != obj.Memo)
                        lvi.SubItems[6].Text = obj.Memo;
                }
            }

            Refresh_OhtListColor();

            label_SelectedOHTCnt.Text = String.Format("선택된 항목: {0}", nCheckCnt);
            listView_OhtList.ResumeLayout();   // Resume suspend
        }

        // 업로드 항목 추가
        private void btn_UploadFileAdd_Click(object sender, EventArgs e)
        {

			btn_OhtList_SaveOhtListFile_Click(null, null);

			if (m_dlgFileOpenUpload.ShowDialog() != DialogResult.OK)
                return;

            foreach (string file in m_dlgFileOpenUpload.FileNames)
            {
                bool bExists = false;
                foreach (sUploadFile aa in m_ArrUploadList)
                {
                    if (aa.srcPath == file)
                    {
                        bExists = true;
                        break;
                    }
                }
                if (bExists)        // 중복추가 되는 것 방지
                    continue;

                sUploadFile item = new sUploadFile();
                item.srcPath = file;

                m_ArrUploadList.Add(item);
            }
            Refresh_UploadList();
        }

        //업로드 그리드 다시 그리기 - m_ArrUploadList 를 가지고 datagridview를 꾸민다
        public void Refresh_UploadList()
        {
            dataGridView_Upload.Rows.Clear();

            foreach (sUploadFile item in m_ArrUploadList)
            {
                //DataGridViewRow row = new DataGridViewRow();


                //int nIdx = dataGridView_Upload.Rows.Add();
                long length = new System.IO.FileInfo(item.srcPath).Length;

                string[] rowCells = new string[(int)eUploadGridCol.CountOf];

                rowCells[(int)eUploadGridCol.로컬경로] = Path.GetDirectoryName(item.srcPath);
                try
                {
                    string strTmp = rowCells[(int)eUploadGridCol.로컬경로].Substring(rowCells[(int)eUploadGridCol.로컬경로].Length - 1, 1) != @"\" ? @"\" : "";
                    rowCells[(int)eUploadGridCol.로컬경로] += strTmp;
                } catch { }

                rowCells[(int)eUploadGridCol.파일이름] = Path.GetFileName(item.srcPath);
                rowCells[(int)eUploadGridCol.Size] = String.Format("{0:#,###0}", length);
                rowCells[(int)eUploadGridCol.OHT_Target_경로] = item.targetPath == "" ? "<Target 지정>" : item.targetPath;
                rowCells[(int)eUploadGridCol.____] = "-->";

                int nIdx = dataGridView_Upload.Rows.Add(rowCells);
                dataGridView_Upload.Rows[nIdx].Cells[(int)eUploadGridCol.OHT_Target_경로].Style.BackColor = item.targetPath == "" ? Color.LightPink : Color.White;
                dataGridView_Upload.Rows[nIdx].Cells[(int)eUploadGridCol.____].Style.BackColor = Color.Black;
                dataGridView_Upload.Rows[nIdx].Cells[(int)eUploadGridCol.____].Style.ForeColor = Color.White;
            }
        }

		public void UpdateMonitorResultGrid(List<UDataManager.DMonitorResultRow> rowList)
		{

			dataGridView_MonitorResult.Rows.Clear();
			if(rowList.Count() == 0)
			{
				MessageBox.Show("기준값을 초과하는 항목이 없어요.");
				return;
			}


			TDrawingControl.SuspendDrawing(dataGridView_MonitorResult);
			try
			{
				foreach (var obj in rowList)
				{
					int nIdx = dataGridView_MonitorResult.Rows.Add(obj.data);
				}
			}
			catch (Exception eee)
			{
				MessageBox.Show("결과항목 표시 Grid Exception.\n\n", eee.Message);
				//throw;
			}

			TDrawingControl.ResumeDrawing(dataGridView_MonitorResult);


		}

        // Target폴더 셀을 더블클릭 했을 때 특정VHL 을 기준으로 Path를 얻어낸다
        private void dataGridView_Upload_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (dataGridView_Upload.SelectedCells.Count != 1)
                return;

            if (dataGridView_Upload.SelectedCells[0].ColumnIndex != (int)eUploadGridCol.OHT_Target_경로)
                return;

            btn_Upload_OhtTargetSet_Click(null, null);
        }

        // 업로드 항목 삭제
        private void btn_Delete_UploadItem_Click(object sender, EventArgs e)
        {

            List<int> rows = MyTools.GridHelper.GetSelectedRows(dataGridView_Upload);
            if (rows.Count < 1)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }

            if (MessageBox.Show(Convert.ToString(rows.Count) + "개의 선택된 항목 삭제하시겠습니까?", "항목 삭제", MessageBoxButtons.OKCancel) != DialogResult.OK)
                return;

            rows.Sort();
            for (int i = rows.Count - 1; i >= 0; i--)
            {
                int nIdx = rows[i];
                m_ArrUploadList.RemoveAt(nIdx);
            }
            Refresh_UploadList();
        }

        // 업로드 그리드 팝업 메뉴 - 선택된 항목 타겟 폴더 지정
        private void 선택된항목Target폴더지정ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGridView_Upload.SelectedCells.Count < 1)
                return;

            btn_Upload_OhtTargetSet_Click(null, null);
        }

        // 업로드 그리에서 우클릭 팝업 메뉴 처리
        private void dataGridView_Upload_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
                contextMenuStrip1.Show((Control)sender, mousePos);
            }
        }
        private void DataGridView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            var grid = sender as DataGridView;

            //찍을 로우 인덱스는 Zero-base 가 아닌 1부터
            var rowIdx = (e.RowIndex + 1).ToString();

            //폰트는 아무거나 지정 가능
            using (StringFormat rightFormat = new StringFormat())
            using (Font font = new System.Drawing.Font(this.Font, FontStyle.Regular))
            {
                //숫자 매길 거니까, 우측 정렬.
                rightFormat.Alignment = StringAlignment.Far;
                rightFormat.LineAlignment = StringAlignment.Center;

                //로우셀렉터에서 우측 정렬로 숫자를 찍으니,
                //바운드는 좀 줄여야 함.
                var headerBounds = new Rectangle(e.RowBounds.Left, e.RowBounds.Top, grid.RowHeadersWidth - 10, e.RowBounds.Height);

                //숫자를 쓴다.
                e.Graphics.DrawString(rowIdx, font, SystemBrushes.ControlText, headerBounds, rightFormat);
            }
        }

        // 업로드 시작 버튼 클릭
        private void btn_StartUpload_Click(object sender, EventArgs e)
        {
			btn_OhtList_SaveOhtListFile_Click(null, null);

			if (m_ArrUploadList.Count == 0)
            {
                MessageBox.Show("복사할 항목을 지정하세요.");
                return;
            }
            if (m_DataMan.GetCurTask() != eDataManTask.None)
            {
                MessageBox.Show("현재 다른 작업이 진행중이기 때문에 진행할수 없습니다..", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (m_OhtList.FindAll(o => o.bChecked).Count == 0)
            {
                MessageBox.Show("선택된 OHT가 업습니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 복사할 목록에 대해서 한번더 명시해준다
            string msg = "복사를 시작 하시겠습니까?\n\n";

            for (int i = 0; i < m_ArrUploadList.Count; i++)
            {
                if (String.IsNullOrEmpty(m_ArrUploadList[i].targetPath))
                {
                    MessageBox.Show("Target 폴더가 지정되지 않은 파일이 있습니다.\n Target 폴더를 지정해야 복사 할 수 있습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                if (File.Exists(m_ArrUploadList[i].srcPath) == false)
                {
                    MessageBox.Show("파일이 존재하지 않는 파일이 있습니다.\n\n" + m_ArrUploadList[i].srcPath, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                if (i > 10)
                {
                    msg += "외 " + Convert.ToString(m_ArrUploadList.Count - i) + " 개의 파일을 복사 하시겠습니까?";
                    break;
                }
                string fileName = Path.GetFileName(m_ArrUploadList[i].srcPath);
                msg += fileName + "\n";
            }

            if (MessageBox.Show(msg, "Double Check", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;


            foreach (sOHTList obj in m_OhtList)
                obj.Result = "";
            textBox_Status.Text = "";

			m_DataMan.m_UploadFileList.Clear();
			foreach(sUploadFile aa in m_ArrUploadList)
			{
				sUploadFile bb = new sUploadFile();
				bb.DeepCopy(aa);
				m_DataMan.m_UploadFileList.Add(bb);
			}
			
			m_DataMan.DoWork(ref m_OhtList, eDataManTask.File_Upload);
        }

        // 업로드탭 - OHT Target 폴더 지정
        private void btn_Upload_OhtTargetSet_Click(object sender, EventArgs e)
        {
            if (dataGridView_Upload.SelectedCells.Count < 1)
            {
                MessageBox.Show("항목을 먼저 선택 해 주세요.");
                return;
            }
            List<int> rows = MyTools.GridHelper.GetSelectedRows(dataGridView_Upload);
            rows.Sort();

            // 지정 다이얼로그 모달로 띄움
            USetUploadPath dlg = new USetUploadPath();
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            for (int i = 0; i < rows.Count; i++)
            {
                int nIdx = rows[i];
                m_ArrUploadList[nIdx].targetPath = dlg.path;
            }

            Refresh_UploadList();
        }
		
        //  OHT 리스트 파일 저장
        private void btn_OhtList_SaveOhtListFile_Click(object sender, EventArgs e)
        {
			SaveOHTListFile();  //파일에도 저장
			m_dlgMonitorOpt.SaveINI();
			bool bRet = SaveINI();

			if (sender != null)
			{
				if (bRet)
					MessageBox.Show("설정을 저장했습니다", "");
				else
					MessageBox.Show("설정을 저장 실패", "");
			}
		}

        // OHTList 버튼 -- 메모 추가하기
        private void btn_OhtList_SetMemo_Click(object sender, EventArgs e)
        {
            UCommonTypeIn dlg = new UCommonTypeIn();
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            foreach (ListViewItem lvi in listView_OhtList.SelectedItems)
            {
                string name = (string)lvi.Tag;
				sOHTList obj = m_OhtList.Find(o => o.Name == name);
				if(obj == null)
				{
					MessageBox.Show("Error!");
					Refresh_OhtList(true);
					return;
				}
				obj.Memo = dlg.txt;
            }

            Refresh_OhtList(false);
        }

        // OHTList 버튼 -- 선택 메모 삭제
        private void btn_OhtList_DeleteMemo_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("선택된 항목 메모를 삭제 하시겠습니까?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            foreach (ListViewItem lvi in listView_OhtList.SelectedItems)
            {
				string name = (string)lvi.Tag;
				sOHTList obj = m_OhtList.Find(o => o.Name == name);
				if (obj == null)
				{
					MessageBox.Show("Error!");
					Refresh_OhtList(true);
					return;
				}
				obj.Memo = "";
			}
            Refresh_OhtList(false);
		}

        // OHTList 버튼 -- 모든 메모 삭제
        private void btn_OhtList_DeleteMemoAll_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("모든 메모를 삭제 하시겠습니까?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;
            foreach (sOHTList obj in m_OhtList)
                obj.Memo = "";

            Refresh_OhtList(false);
        }
		
        // OHTList 버튼 -- 핑 시도
        private void btn_OhtList_DoPing_Click(object sender, EventArgs e)
        {
            if (m_DataMan.GetCurTask() != eDataManTask.None)
            {
                MessageBox.Show("현재 " + m_DataMan.GetCurTask().ToString().Replace('_', ' ') + " 작업이 수행중 입니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (m_OhtList.FindAll(o => o.bChecked).Count == 0)
            {
                MessageBox.Show("선택된 호기가 없습니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (MessageBox.Show("선택된 호기의 Ping을 시도 하시겠습니까?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            foreach (sOHTList obj in m_OhtList)
                obj.Ping = "";
            textBox_Status.Text = "";

            m_DataMan.DoWork(ref m_OhtList, eDataManTask.Ping); // task 시작
        }
		
        // OHTList 버튼 -- 버전 리딩
        private void btn_OhtList_ReadVersion_Click(object sender, EventArgs e)
        {
            if (m_DataMan.GetCurTask() != eDataManTask.None)
            {
                MessageBox.Show("현재 " + m_DataMan.GetCurTask().ToString().Replace('_', ' ') + " 작업이 수행중 입니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (m_OhtList.FindAll(o => o.bChecked).Count == 0)
            {
                MessageBox.Show("선택된 호기가 없습니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (MessageBox.Show("OHT Version Reading을 하시겠습니까?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            foreach (sOHTList obj in m_OhtList)
                obj.Version = "";
            textBox_Status.Text = "";

            m_DataMan.DoWork(ref m_OhtList, eDataManTask.Read_OHT_Version);// task 시작
        }

        // OHTList 버튼 -- 지정된 범위 OHT 선택 Check 하기
        private void btn_OhtList_DoSelect_Click(object sender, EventArgs e)
        {
            int nFrom = 0;
            int nTo = 0;
            if (Int32.TryParse(edt_OhtList_SelectOht_From.Text, out nFrom) == false || Int32.TryParse(edt_OhtList_SelectOht_To.Text, out nTo) == false)
            {
                MessageBox.Show("호기를 정수형으로 잘 입력해보세요", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (nTo <= nFrom)
            {
                MessageBox.Show("호기 범위를 작은 수에서 큰 수로 입력 해야 합니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

			int nCnt = 0;
            foreach (sOHTList obj in m_OhtList)
            {
				if (obj.nOHTNo >= nFrom && obj.nOHTNo <= nTo)
				{
					obj.bChecked = true;
					nCnt++;
				}				
            }

			int nCha = (nTo - nFrom) - nCnt;
			if (nCha > 0)
				MessageBox.Show(Convert.ToString(nCha) + "개 항목을 선택 할 수 없습니다.");
			
            Refresh_OhtList(false);
        }

		private void btn_OhtList_DoSelect2_Click(object sender, EventArgs e)
		{
			int nOHT = 0;
			if (Int32.TryParse(edt_OhtList_SelectOht.Text, out nOHT) == false)
			{
				MessageBox.Show("호기를 정수형으로 잘 입력해보세요", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return;
			}

			bool bFound = false;
			foreach (sOHTList obj in m_OhtList)
			{
				if (obj.nOHTNo == nOHT)
				{
					obj.bChecked = true;
					bFound = true;
					break;
				}
			}

			if (bFound == false)
				MessageBox.Show( Convert.ToString(nOHT) + "호기를 선택 할 수 없습니다.");

			Refresh_OhtList(false);
		}

		private bool bCritical = false;
        private System.Threading.ThreadState prevThreadState = System.Threading.ThreadState.Unstarted;
        private bool bOnceTimer = true;
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (bCritical || !m_bAvaliable)
                return;
            if (!this.splitContainer1.Visible)
                return;
            bCritical = true;

            if(bOnceTimer)  // 실행시 최초 한번 INI 로딩
            {
                bOnceTimer = false;
                LoadINI();
            }

            // OHT 리스트 UI와 데이터간 다른점이 생기면 리스트를 Refresh 한다
            bool bDoRefresh = false;
            do
            {
                if (listView_OhtList.Items.Count != m_OhtList.Count)
                {
                    bDoRefresh = true;
                    break;
                }

                for (int i = 0; i < listView_OhtList.Items.Count; i++)
                {
                    ListViewItem lvi = listView_OhtList.Items[i];

                    string name = (string)lvi.Tag;
					sOHTList obj = m_OhtList.Find(o => o.Name == name);
					if(obj == null)
					{
						continue;	// assertion
					}

                    if (lvi.Checked != obj.bChecked)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[1].Text != obj.Name)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[2].Text != obj.IP)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[3].Text != obj.Result)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[4].Text != obj.Version)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[5].Text != obj.Ping)
                    { bDoRefresh = true;
                        break;
                    }

                    if (lvi.SubItems[6].Text != obj.Memo)
                    { bDoRefresh = true;
                        break;
                    }
                }
            } while (false);

            // 다른점이 있다면 Refresh
            if (bDoRefresh)
            {
                Refresh_OhtList(false);
            }
			
            btn_StartUpload.Enabled = m_DataMan.GetCurTask() == eDataManTask.None;
            btn_UploadJobCancel.Enabled = m_DataMan.GetCurTask() != eDataManTask.None;
			btn_DownloadFileCancel.Enabled = m_DataMan.GetCurTask() != eDataManTask.None;
			
			btn_StartDownloadOHTFile.Enabled = m_DataMan.GetCurTask() == eDataManTask.None;
            btn_DownloadFileCancel.Enabled = m_DataMan.GetCurTask() != eDataManTask.None;
			

			btn_UploadJobCancel.Text = m_DataMan.m_bJobCancel ? "취소중" : "Cancel";
			btn_DownloadFileCancel.Text = m_DataMan.m_bJobCancel ? "취소중" : "Cancel";

			// 로그 다운로드 UI 갱신
			numeric_LTimeFrom_Year.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Month.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Day.Enabled = !radioButton_LogTime3_All.Checked;
            numeric_LTimeFrom_Hour.Enabled = !radioButton_LogTime3_All.Checked;

            numeric_LTimeTo_Year.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Month.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Day.Enabled = radioButton_LogTime1_Range.Checked;
            numeric_LTimeTo_Hour.Enabled = radioButton_LogTime1_Range.Checked;

			splitContainer12.Visible = !checkBox_AllLogFile.Checked;

			//bool bRefreshDonloadLogList = false;
			//if (listview_LogSelect.Items.Count != m_arrDownloadLogList.Count)
			//    bRefreshDonloadLogList = true;
			//else
			//{
			//    for (int i = 0; i < m_arrDownloadLogList.Count; i++)
			//    {
			//        string dnLog = m_arrDownloadLogList[i];
			//        if (dnLog != listview_LogSelect.Items[i].ToString())
			//        {
			//            bRefreshDonloadLogList = true;
			//            break;
			//        }
			//    }
			//}
			//if (bRefreshDonloadLogList)
			//{
			//    Refresh_DownloadLogList();
			//    SaveDownloadLogList();  //파일에도 저장
			//}

			button_Monitor_Start.Enabled = Thread_MonitorCheck_atLocal.ThreadState != System.Threading.ThreadState.Running;
			button_monitor_stop.Enabled = Thread_MonitorCheck_atLocal.ThreadState == System.Threading.ThreadState.Running;
			btn_ShowMonitorResultFile.Visible = File.Exists(m_MonitorResult.resultFilePath);

            // 모니터로그 계산 스레드가 막 끝났을때, 결과를 Update한다
            if (prevThreadState != Thread_MonitorCheck_atLocal.ThreadState)
            {
                if(prevThreadState == System.Threading.ThreadState.Running)
                {
                    
                }
            }
            prevThreadState = Thread_MonitorCheck_atLocal.ThreadState;

            // progres bar
            progressBar1.Visible = m_DataMan.nProgress_tPer > 0 || m_DataMan.GetCurTask() != eDataManTask.None;
            if (m_DataMan.nProgress_tPer > 0)
                progressBar1.Value = Math.Min(1000, m_DataMan.nProgress_tPer);
			
			// 화면 로그 찍기
			while (m_LogQ.GetCount() > 0)
			{
				string msg = "";
				if (m_LogQ.DeQ(out msg) == true)
				{
					int nLen = textBox_Status.Text.Length;
					textBox_Status.AppendText(msg + "\r\n");
					int nIdx = textBox_Status.GetFirstCharIndexFromLine(textBox_Status.Lines.Count() - 1);
					//textBox_Status.SelectionStart = nIdx;
					//textBox_Status.SelectionLength = 1;
					textBox_Status.ScrollToCaret();
				}
			}

			// Task 스레드에서 작업이 끝난 후 여태까지 찍은 작업 내역 로그를 file 로 내보내라는 시그널
			if(m_LogQ.bFlushToFile)
			{
				File.WriteAllText(m_LogQ.flushFilePath, textBox_Status.Text);
				m_LogQ.bFlushToFile = false;
			}

			bCritical = false;
        }
        bool bFlag = false;
        private void CheckOhtList(int nIdx, bool bVal)
        {
            bFlag = true;
            listView_OhtList.Items[nIdx].Checked = bVal;

            listView_OhtList.Items[nIdx].ImageIndex = bVal ? 1 : 0;
            if (bVal) // 체크된건 굵게 표시하기 위함
                listView_OhtList.Items[nIdx].Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            else
                listView_OhtList.Items[nIdx].Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));

            bFlag = false;
        }
        private void listView_OhtList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (bFlag == false)
            {
				string name = (string)listView_OhtList.Items[e.Index].Tag;
				sOHTList obj = m_OhtList.Find(o => o.Name == name);
				if(obj == null)
				{
					MessageBox.Show("Error!");
					Refresh_OhtList(true);
					return;
				}
				obj.bChecked = e.NewValue == CheckState.Checked;
				listView_OhtList.Items[e.Index].SubItems[0].Text = obj.bChecked ? "1" : "0";
                e.NewValue = e.CurrentValue;    // 체크해서 바뀌지 않고 Timer에서 m_OhtList하고 다를 경우 자연스럽게 Refresh 되도록 유도해야 함
            }
            else
            {
                //if (e.NewValue == CheckState.Checked) // 체크된건 굵게 표시하기 위함
                //    listView_OhtList.Items[e.Index].Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
                //else
                //    listView_OhtList.Items[e.Index].Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            }
        }

        // OHTList 전체 체크
        private void btn_OhtList_CheckAll_Click(object sender, EventArgs e)
        {
            foreach (sOHTList obj in m_OhtList)
                obj.bChecked = true;
        }

        // OHTList 전체 체크 해제
        private void btn_OhtList_UncheckAll_Click(object sender, EventArgs e)
        {
            foreach (sOHTList obj in m_OhtList)
                obj.bChecked = false;
        }

        private void listBox_SelectedOHT_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                string str = (string)listBox_SelectedOHT.SelectedItem;

                if (String.IsNullOrEmpty(str))
                    return;
                if (str.Length < 4)
                    return;
                string tmp = str.Substring(str.Length - 4, 4);
                int nHogi = 0;
                if (Int32.TryParse(tmp, out nHogi) == false)
                    return;

                sOHTList obj = m_OhtList.Find(o => o.nOHTNo == nHogi);
                obj.bChecked = false;       // 체크 해제
            }
        }

        private void btn_UploadJobCancel_Click(object sender, EventArgs e)
        {
            m_LogQ.EnQ(">> 취소중");
            m_DataMan.m_bJobCancel = true;
        }

        private void btn_ResetResult_Click(object sender, EventArgs e)
        {
            foreach (sOHTList obj in m_OhtList)
            {
                obj.Result = "";
                //obj.Ping = "";
                //obj.Version = "";
            }
			Refresh_OhtList(true);
        }

        private void UDataManager_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_DataMan.m_bKillThread = true;
            if (m_DataMan.m_taskThread.Join(1000) == false)
            {
                m_DataMan.m_taskThread.Abort();
            }

        }

        private void btn_LogDnFileBrowse_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();

            if (!String.IsNullOrEmpty(textBox_LogDnLocalDir.Text))
                dlg.SelectedPath = textBox_LogDnLocalDir.Text;

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            textBox_LogDnLocalDir.Text = dlg.SelectedPath;
        }
        
        // #### 로그 다운로드  - 버튼 - 리스트에 추가
        private void btn_AddNewLog_Click(object sender, EventArgs e)
        {
            string[] arrBadKeyword = { ".LOG", "\\", "/", "?", "*", "<", ">" };
            foreach (string s in arrBadKeyword)
            {
                if (textBox_NewLogName.Text.ToUpper().IndexOf(s) >= 0)
                {
                    MessageBox.Show(String.Format("키워드 [{0}]은 사용할 수 없습니다.", s), "불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            foreach(ListViewItem lvi in listview_LogSelect.Items)
            {
                if (lvi.SubItems[1].Text.ToUpper() == textBox_NewLogName.Text.ToUpper())
                {
                    MessageBox.Show("이미 있습니다.", "불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            string[] arrx = new string[2];
            arrx[1] = textBox_NewLogName.Text.ToUpper();

            listview_LogSelect.Items.Add(new ListViewItem(arrx));
			btn_OhtList_SaveOhtListFile_Click(null, null);
		}

        // #### 로그 다운로드 - 리스트에서 삭제
        private void btn_DeleteLog_Click(object sender, EventArgs e)
        {
            if (listview_LogSelect.SelectedItems.Count == 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }

            while (listview_LogSelect.SelectedItems.Count > 0)
            {
                int nIdx = listview_LogSelect.SelectedItems[0].Index;
                listview_LogSelect.Items.RemoveAt(nIdx);
            }
        }
		
        public Thread Thread_MonitorCheck_atLocal = new Thread(DDataManager.DoCheckMonitorThread_atLocal);

        // 모니터링 로그 검사시작
        private void button_Monitor_Start_Click(object sender, EventArgs e)
        {
			btn_OhtList_SaveOhtListFile_Click(null, null);
			m_dlgMonitorOpt.Apply();
			
            if (String.IsNullOrEmpty(m_dlgMonitorOpt.textBox_MonitorLogPath.Text))
            {
                MessageBox.Show("폴더에서 검사를 하려면, 로그폴더를 선택해야 합니다.");
                return;
            }
            if (Directory.Exists(m_dlgMonitorOpt.textBox_MonitorLogPath.Text) == false)
            {
                MessageBox.Show("폴더가 존재하지 않습니다.");
                return;
            }
            
            if(Thread_MonitorCheck_atLocal.ThreadState == System.Threading.ThreadState.Running)
            {
                MessageBox.Show("검사가 진행중입니다.");
                return;
            }
        
            if (MessageBox.Show("모니터링 로그 검사를 하시겠습니까?", "", MessageBoxButtons.OKCancel) != DialogResult.OK)
                return;

            Thread_MonitorCheck_atLocal.Abort();
            Thread_MonitorCheck_atLocal = null;

            // 결과 객체 초기화
            m_MonitorResult.Init();
            
			foreach (sOHTList obj in m_OhtList)
				obj.Result = "";
			textBox_Status.Text = "";
            m_LogQ.EnQ(String.Format("> 모니터링 로그 검사 시작됨 [{0}개 항목 검사]------------------- ", m_dlgMonitorOpt.CheckedItem.Count));

            // 체크된 항목 결과객체에 세팅
            foreach (var v in m_dlgMonitorOpt.CheckedItem)
                m_MonitorResult.AddMonitorItemDefine(v.name);

            m_strMonitorLocalPath = m_dlgMonitorOpt.textBox_MonitorLogPath.Text;
            m_DataMan.bThreadStop_CheckMonitor_atLocal = false;

            // 스레드로 수행
            Thread_MonitorCheck_atLocal = new Thread(DDataManager.DoCheckMonitorThread_atLocal);
            Thread_MonitorCheck_atLocal.Start(m_DataMan);
        }
        public void UpdateMonitorLogResult_Invoke(UDataManager obj)
        {
            
            //this.Invoke(obj._DelegateMonitorResultUpdate_);
        }
        // 모니터링 로그 결과 DataGridView에 표시
        private void UpdateMonitorLogResult()
        {
			tabControl_Monitor.SelectedIndex = 1;
        }

      

        private void btn_ShowMonitorResultFile_Click(object sender, EventArgs e)
        {
            if(File.Exists(m_MonitorResult.resultFilePath) == false)
            {
                MessageBox.Show("결과파일이 없습니다.");
                return;
            }
            string arg = "/select, \"" + m_MonitorResult.resultFilePath + "\"";
            System.Diagnostics.Process.Start("explorer.exe", arg);
        }
        double dOldSplitPos_per = 0;
        bool bExpanded = false;
        // Split 으로 구분된 Col 숨기기 보이기
     
        private void btn_ExpandOhtList_Click(object sender, EventArgs e)
        {
            if (bExpanded)
            {
                splitContainer1.SplitterDistance = (int)(dOldSplitPos_per * splitContainer1.Width); // 원복
                btn_ExpandOhtList.Image = imageList_Expand.Images[1];
            }
            else
            {
                
                dOldSplitPos_per = (double)splitContainer1.SplitterDistance / splitContainer1.Width;
                splitContainer1.SplitterDistance = splitContainer1.Width - 200;  // 줄이기
                btn_ExpandOhtList.Image = imageList_Expand.Images[0];
            }
            bExpanded = !bExpanded;
        }


        // 일반파일 다운로드 탭 --- 버튼 - 목록에 파일 추가
        private void btn_DownloadFileAddFile_Click(object sender, EventArgs e)
        {
            if(String.IsNullOrEmpty(textBox_AddDownloadFilePath.Text))
            {
                MessageBox.Show("파일 경로를 입력하세요");
                return;
            }

            foreach (ListViewItem item in listView_DownloadOHTFile.Items)
            {
                string str = item.SubItems[1].Text.ToUpper();
                if (str == textBox_AddDownloadFilePath.Text.ToUpper())
                {
                    MessageBox.Show("이미 목록에 있습니다.");
                    return;
                }
            }

            string[] arr = new string[2];
            arr[0] = "";
            arr[1] = textBox_AddDownloadFilePath.Text;

            listView_DownloadOHTFile.Items.Add(new ListViewItem(arr));
			btn_OhtList_SaveOhtListFile_Click(null, null);
		}

        // 일반파일 다운로드 탭 --- 버튼 - 로컬 Target 폴더지정 ***********
        string prevPath_DownloadFileFolderBrowse = "";
        private void btn_DownloadFileFolderBrowse_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = prevPath_DownloadFileFolderBrowse;

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            textBox_downloadPath.Text = dlg.SelectedPath;
        }

        // 일반파일 다운로드 탭 --- 버튼 - 목록에서 삭제 버튼
        private void btn_DownloadFileDeleteCurFile_Click(object sender, EventArgs e)
        {
            if(listView_DownloadOHTFile.SelectedItems.Count == 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }

            while(listView_DownloadOHTFile.SelectedItems.Count > 0)
            {
                int nIdx = listView_DownloadOHTFile.SelectedItems[0].Index;
                listView_DownloadOHTFile.Items.RemoveAt(nIdx);
            }
        }

        public void SaveDownloadFileList()
        {

        }
		// 버튼 --- 파일 다운로드탭의 시작 버튼(로그, 일반파일 둘다 해당) ###########
		private void btn_StartDownloadOHTFile_Click(object sender, EventArgs e)
        {
			btn_OhtList_SaveOhtListFile_Click(null, null);

            // 로그파일 다운로드 탭 선택된 상태 *******************
            if (tabControl_Log.SelectedIndex == 0)
            {
                if(Directory.Exists(textBox_LogDnLocalDir.Text) == false)
                {
                    MessageBox.Show("다운로드 받으려는 폴더를 찾을 수 없습니다.");
                    return;
                }
                //if(Directory.GetDirectories(textBox_LogDnLocalDir.Text).Length != 0)
                //{
                //    MessageBox.Show("다운로드 받으려는 폴더는 반드시 비어있어야 합니다.\n폴더가 뒤섞이는 것을 방지하기 위함입니다.");
                //    return;
                //}

                m_DataMan.m_DownloadLogList.Clear();

				// 파일에 관계없이 모든 로그. 로그 이름은 설정하지 않고 시간으로만 파일을 찾는다
				if(checkBox_AllLogFile.Checked)
				{
					sDownloadLog obj = new sDownloadLog();
					obj.bAllLog = true;
					obj.strLocalPos = textBox_LogDnLocalDir.Text;

					obj.logTime.From = new DateTime((int)numeric_LTimeFrom_Year.Value, (int)numeric_LTimeFrom_Month.Value, (int)numeric_LTimeFrom_Day.Value, (int)numeric_LTimeFrom_Hour.Value, 0, 0);
					obj.logTime.To = new DateTime((int)numeric_LTimeTo_Year.Value, (int)numeric_LTimeTo_Month.Value, (int)numeric_LTimeTo_Day.Value, (int)numeric_LTimeTo_Hour.Value, 59, 59);
					obj.logTime.opt = radioButton_LogTime1_Range.Checked ? DLogTime.eTimeZone.Range : (radioButton_LogTime2_AfterAll.Checked ? DLogTime.eTimeZone.AfterAll : DLogTime.eTimeZone.AllTime);

					m_DataMan.m_DownloadLogList.Add(obj);
				}
				else
				{
					// DDataManager 객체로 전달
					foreach (ListViewItem lvi in listview_LogSelect.Items)
					{
						if (lvi.Checked == false)
							continue;

						sDownloadLog obj = new sDownloadLog();
						obj.strLocalPos = textBox_LogDnLocalDir.Text;
						obj.strLogName = lvi.SubItems[1].Text;

						obj.logTime.From = new DateTime((int)numeric_LTimeFrom_Year.Value, (int)numeric_LTimeFrom_Month.Value, (int)numeric_LTimeFrom_Day.Value, (int)numeric_LTimeFrom_Hour.Value, 0, 0);
						obj.logTime.To = new DateTime((int)numeric_LTimeTo_Year.Value, (int)numeric_LTimeTo_Month.Value, (int)numeric_LTimeTo_Day.Value, (int)numeric_LTimeTo_Hour.Value, 59, 59);
						obj.logTime.opt = radioButton_LogTime1_Range.Checked ? DLogTime.eTimeZone.Range : (radioButton_LogTime2_AfterAll.Checked ? DLogTime.eTimeZone.AfterAll : DLogTime.eTimeZone.AllTime);

						m_DataMan.m_DownloadLogList.Add(obj);
					}
				}


				if (m_OhtList.Count == 0)
				{
					MessageBox.Show("선택된 OHT가 없습니다.");
					return;
				}
				if (m_DataMan.m_DownloadLogList.Count == 0)
                {
                    MessageBox.Show("선택된 로그파일이 없습니다.");
                    return;
                }

				if (MessageBox.Show("로그 다운 받기를  시작합니다.", "File Download", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
					return;
				
				textBox_Status.Text = "";
                btn_ResetResult_Click(null, null);
				m_DataMan.DoWork(ref m_OhtList, eDataManTask.Log_Download);               
            }

            // 일반파일 다운로드 탭 선택된 상태 *********************
            else
            {
                if (Directory.Exists(textBox_downloadPath.Text) == false)
                {
                    MessageBox.Show("다운로드 받으려는 폴더를 찾을 수 없습니다.");
                    return;
                }
                if (Directory.GetDirectories(textBox_downloadPath.Text).Length != 0)
                {
                    MessageBox.Show("다운로드 받으려는 폴더는 반드시 비어있어야 합니다.\n폴더가 뒤섞이는 것을 방지하기 위함입니다.");
                    return;
                }

                m_DataMan.m_DownloadFileList.Clear();

                // DDataManager 객체로 전달
                foreach (ListViewItem lvi in listView_DownloadOHTFile.Items)
                {
                    if (lvi.Checked == false)
                        continue;

                    sDownloadFile obj = new sDownloadFile();
                    obj.LocalPath = textBox_downloadPath.Text;
                    obj.OhtPath = lvi.SubItems[1].Text;
                    
                    m_DataMan.m_DownloadFileList.Add(obj);
                }

                if (m_DataMan.m_DownloadFileList.Count == 0)
                {
                    MessageBox.Show("선택된 파일이 없습니다.");
                    return;
                }

				if (MessageBox.Show("파일 다운 받기를  시작합니다.", "File Download", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
					return;
				
				textBox_Status.Text = "";
                btn_ResetResult_Click(null, null);
				m_DataMan.DoWork(ref m_OhtList, eDataManTask.File_Download);
			}
        }

        private void listView_DownloadOHTFile_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView_DownloadOHTFile.SelectedItems.Count == 0)
                textBox_DownloadFileCurFile.Text = "";
            else
            {
                textBox_DownloadFileCurFile.Text = listView_DownloadOHTFile.SelectedItems[0].SubItems[1].Text;
            }
        }
        
        private void btn_DownloadFileCancel_Click(object sender, EventArgs e)
        {
            m_DataMan.m_bJobCancel = true;
        }

		private void edt_OhtList_SelectOht_To_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
				btn_OhtList_DoSelect_Click(this, e);
		}
		private void edt_OhtList_SelectOht_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
				btn_OhtList_DoSelect2_Click(this, e);
		}

		private void textBox_NewLogName_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
				btn_AddNewLog_Click(this, e);
		}
		
		private void textBox_AddDownloadFilePath_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
				btn_DownloadFileAddFile_Click(this, e);
		}

		private void popupBtn_OpenOHT_Explorer(object sender, EventArgs e)
		{
			if (listView_OhtList.SelectedItems.Count != 1)
			{
				MessageBox.Show("한개의 항목만 선택하세요.");
				return;
			}
			try
			{
				

				string name = (string)listView_OhtList.SelectedItems[0].Tag;
				sOHTList obj = m_OhtList.Find(o => o.Name == name);
				if(obj == null)
				{
					MessageBox.Show("Error!");
					Refresh_OhtList(true);
					return;
				}

				if (MessageBox.Show(String.Format("이 호기를 탐색기로 열까요??\n\nOHT: {0}\nIP: {1}", obj.Name, obj.IP), "VNC 열기", MessageBoxButtons.OKCancel) != DialogResult.OK)
					return;

				string arg = String.Format("\\\\{0}", obj.IP);
				System.Diagnostics.Process.Start("explorer.exe", arg);
			}
			catch (Exception)
			{
			}
			
		}

		private void popupBtn_OpenOHT_VNC(object sender, EventArgs e)
		{
			if (listView_OhtList.SelectedItems.Count != 1)
			{
				MessageBox.Show("한개의 항목만 선택하세요.");
				return;
			}

			try
			{
				string name = (string)listView_OhtList.SelectedItems[0].Tag;
				sOHTList obj = m_OhtList.Find(o => o.Name == name);
				if (obj == null)
				{
					MessageBox.Show("Error!");
					Refresh_OhtList(true);
					return;
				}

				if (MessageBox.Show(String.Format("아래 호기에 VNC 접속 하시겠습니까?\n\nOHT: {0}\nIP: {1}", obj.Name, obj.IP), "VNC 열기", MessageBoxButtons.OKCancel) != DialogResult.OK)
					return;

				Process[] vncProcess = Process.GetProcessesByName("vncviewer");
				if (vncProcess.Length > 0)
				{
					if (MessageBox.Show("VNC 이미 실행중입니다.\n\n중복해서 실행할 경우 사용자가 실수 할 수 있습니다.\n기존 VNC를 모두 강제 종료하고 실행하겠습니까?", "경고", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning) == DialogResult.OK)
					{
						foreach (Process p in vncProcess)
							p.Kill();
					}
				}

				ProcessStartInfo startInfo = new ProcessStartInfo();
				startInfo.FileName = Application.StartupPath + @"\reference\vncviewer.exe";
				startInfo.Arguments = obj.IP;

				System.Diagnostics.Process.Start(startInfo);
			}
			catch (Exception)
			{
			}
			
		}

		private void popupBtn_WriteOhtMemo(object sender, EventArgs e)
		{
			btn_OhtList_SetMemo_Click(this, e);
		}

		private void popupBtn_DeleteOhtMemo(object sender, EventArgs e)
		{
			btn_OhtList_DeleteMemo_Click(this, e);
		}
		
		private void listView_OhtList_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)	// 우클릭 팝업
			{
				Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
				contextMenu_OhtList.Show((Control)sender, mousePos);
			}
		}
		private void listBox_SelectedOHT_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right) // 우클릭 팝업
			{
				Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
				contextMenu_CheckedOht.Show((Control)sender, mousePos);
			}
		}

		private void button_monitor_stop_Click(object sender, EventArgs e)
		{
			m_DataMan.bThreadStop_CheckMonitor_atLocal = true;
			MessageBox.Show("중단하였습니다.\n\n중단이 완료될 때가지 기다려주세요.");
			return;
		}

		private void comboBox_OhtList_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				int n = comboBox_OhtList.SelectedIndex;
				m_OhtListPath = m_arrFile_Oht[n];
				LoadOHTListFile();
			}
			catch (Exception ee)
			{
				MessageBox.Show("OHT List 변경 실패\n\n" + ee.Message, "Error");
				throw;
			}
		}

		private void textBox_Status_KeyDown(object sender, KeyEventArgs e)
		{
			if(e.Control && e.KeyCode == Keys.C)
			{
				textBox_Status.Copy();
			}
		}

		//string prevPath = "";
		private void btn_SaveMonitorResult_Click(object sender, EventArgs e)
		{
			SaveFileDialog dlg = new SaveFileDialog();
			if (String.IsNullOrEmpty(m_dlgMonitorOpt.prevPath_saveResult) == false)
				dlg.InitialDirectory = m_dlgMonitorOpt.prevPath_saveResult;

			dlg.FileName = m_dlgMonitorOpt.prevPath_lastOpenFileName + "_Result";

			dlg.Filter = "Text  file (*.csv)|*.csv|All files (*.*)|*.*";

			if (dlg.ShowDialog() != DialogResult.OK)
				return;
			try
			{
				// csv 파일로 저장하기 위해서 콤마로 하여 데이터 생성
				List<string> arrLines = new List<string>();
				string line = "";

				for (int i = 0; i < (int)eMonitorResultCol.CountOf; i++)
				{
					eMonitorResultCol ee = (eMonitorResultCol)i;
					line += ee.ToString().Replace('_', ' ') + ",";
				}
				arrLines.Add(line);

				foreach (var obj in m_dlgMonitorOpt.m_resultRowList)
				{
					line = "";
					foreach (var item in obj.data)
						line += item + ",";
					arrLines.Add(line);
				}
				// file writing
				File.WriteAllLines(dlg.FileName, arrLines, Encoding.Default);
			}
			catch (Exception eee)
			{
				MessageBox.Show("데이터 생성 중 에러 발생\n\n" + eee.Message);
				//throw;
				return;
			}
			string arg = "/select, \"" + dlg.FileName + "\"";
			Process.Start("explorer", arg);
		}

		private void btn_DCheck_Click(object sender, EventArgs e)
		{
			if (m_DataMan.GetCurTask() != eDataManTask.None)
			{
				MessageBox.Show("현재 " + m_DataMan.GetCurTask().ToString().Replace('_', ' ') + " 작업이 수행중 입니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return;
			}
			if (m_OhtList.FindAll(o => o.bChecked).Count == 0)
			{
				MessageBox.Show("선택된 호기가 없습니다.", "작업 불가", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return;
			}
			if (MessageBox.Show("D 드라이브 체크 하시겠습니까?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
				return;
			
			textBox_Status.Text = "";

			btn_ResetResult_Click(null, null);
			m_DataMan.DoWork(ref m_OhtList, eDataManTask.D_Drv_CHK);// task 시작
		}

		private void toolbtn_OhtExplorer_Click(object sender, EventArgs e)
		{
			if (listBox_SelectedOHT.SelectedItems.Count != 1)
			{
				MessageBox.Show("한개의 항목만 선택하세요.");
				return;
			}
			try
			{
				string oht = listBox_SelectedOHT.SelectedItems[0].ToString();
				sOHTList obj = m_OhtList.Find(o => o.Name == oht);
				if (obj == null)
				{
					MessageBox.Show(oht + ",목록에서 찾을 수 없습니다.");
					return;
				}

				if (MessageBox.Show(String.Format("이 호기를 탐색기로 열까요?\n\nOHT: {0}\nIP: {1}", obj.Name, obj.IP), "", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
					return;

				string arg = String.Format("\\\\{0}", obj.IP);
				System.Diagnostics.Process.Start("explorer.exe", arg);
			}
			catch (Exception)
			{
			}
		}

		private void toolbtn_OhtVNC_Click(object sender, EventArgs e)
		{

			if (listBox_SelectedOHT.SelectedItems.Count != 1)
			{
				MessageBox.Show("한개의 항목만 선택하세요.");
				return;
			}
			try
			{
				string oht = listBox_SelectedOHT.SelectedItems[0].ToString();
				sOHTList obj = m_OhtList.Find(o => o.Name == oht);
				if (obj == null)
				{
					MessageBox.Show(oht + ",목록에서 찾을 수 없습니다.");
					return;
				}

				if (MessageBox.Show(String.Format("아래 호기에 VNC 접속 하시겠습니까?\n\nOHT: {0}\nIP: {1}", obj.Name, obj.IP), "", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) != DialogResult.OK)
					return;

				ProcessStartInfo startInfo = new ProcessStartInfo();
				startInfo.FileName = Application.StartupPath + @"\reference\vncviewer.exe";
				startInfo.Arguments = obj.IP;

				System.Diagnostics.Process.Start(startInfo);
			}
			catch (Exception)
			{
			}
		}

		private void toolbtn_OhtUncheck_Click(object sender, EventArgs e)
		{
			var a = new KeyEventArgs(Keys.Delete);
			listBox_SelectedOHT_KeyDown(null, a);
		}

		SortOrder order = SortOrder.Descending;
		private void listView_OhtList_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			//if (e.Column == 0)
			//	return;
			listView_OhtList.Sort();
			listView_OhtList.ListViewItemSorter = new MyListViewComparer(e.Column, order);
			order = order == SortOrder.Descending ? SortOrder.Ascending : SortOrder.Descending;	// 토글로 바꾸자
		}

		string prev_savebrowse = "";
		private void oHTBrowser내용Excel저장ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SaveFileDialog dlg = new SaveFileDialog();

			try
			{ 
			
				if (String.IsNullOrEmpty(prev_savebrowse) == false)
					dlg.InitialDirectory = prev_savebrowse;

				//dlg.FileName = m_dlgMonitorOpt.prevPath_lastOpenFileName + "_Result";

				dlg.Filter = "Text  file (*.csv)|*.csv|All files (*.*)|*.*";

				if (dlg.ShowDialog() != DialogResult.OK)
					return;

				prev_savebrowse = Path.GetDirectoryName(dlg.FileName);

				int i = 1;
				List<string> lines = new List<string>();

				string line = "";
				for (int c = 0; c < listView_OhtList.Columns.Count; c++)
				{
					if (c == 0)
						continue;

					ColumnHeader col = listView_OhtList.Columns[c];
					line += col.Text + ",";
				}
				lines.Add(line);
				foreach (ListViewItem lvi in listView_OhtList.Items)
				{
					line = "";
					for (int c = 0; c < listView_OhtList.Columns.Count; c++)
					{
						if (c == 0)
							continue;

						ColumnHeader col = listView_OhtList.Columns[c];
						line += String.Format("{0},", lvi.SubItems[c].Text);
					}
					lines.Add(line);
					i++;
				}
				File.WriteAllLines(dlg.FileName, lines, Encoding.Default);
			}
			catch (Exception eee)
			{
				MessageBox.Show("데이터 생성 중 에러 발생\n\n" + eee.Message);
				//throw;
				return;
			}
			string arg = "/select, \"" + dlg.FileName + "\"";
			Process.Start("explorer", arg);

			
		}
	}
	class MyListViewComparer : IComparer
	{
		private int col;
		private SortOrder order;
		public MyListViewComparer(int column, SortOrder order)
		{
			col = column;
			this.order = order;
		}
		public int Compare(object x, object y)
		{
			int RetVal = -1;
			ListViewItem ox = (ListViewItem)x;
			ListViewItem oy = (ListViewItem)y;
			RetVal = String.Compare(ox.SubItems[col].Text, oy.SubItems[col].Text);
			if (this.order == SortOrder.Descending)
				RetVal *= -1;
			return RetVal;
		}
	}
    

  
}
