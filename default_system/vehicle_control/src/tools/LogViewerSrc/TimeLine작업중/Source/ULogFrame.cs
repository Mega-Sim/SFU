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
using System.Diagnostics;
using MyTools;

namespace LogViewer
{
    public partial class ULogFrame : Form
    {
        public DDataSet m_dataSet = null;
        public List<string> m_fileList = new List<string>();
        public DateTime m_CurTime = new DateTime();
        public string m_curFile = "";
        public UCommentMake m_dlgComment = new UCommentMake();

        Color defaultBoxColor = Color.Gray;
        
        // 코맨트 파일 관련 DEFINE ******
        const string CMNT_LINE_START = "<%NEWLINE%>  ";
        const string CMNT_KEY = "COMMENT: ";
        const string CMNT_FILE = "FILE: ";
        const string CMNT_RANGE = "RANGE: ";
        const string CMNT_AUTHOR = "AUTHOR: ";
        const string CMNT_AMCRCV_SHOW_COL = "AMCRECV_SHOW_COL: ";
        const string CMNT_AMCRCV_OPEN_RANGE = "AMCRECV_OPEN_RANGE: ";
        const string CMNT_ATTACHED_FILE = "ATTCHED_FILE: ";


        public ULogFrame()
        {
            InitializeComponent();

            TDrawingControl.SetDoubleBuffered(dataGridView_UserEvent);
			dataGridView_UserEvent.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			InitGrid();
            defaultBoxColor = textBox_Stat.BackColor;
            m_dlgComment.m_ParentGrid = dataGridView_Comment;
            dataGridView_Comment.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
        }

        private void InitGrid()
        {
            // User Log event Grid 초기화 **********************************************
            TDrawingControl.SuspendDrawing(dataGridView_UserEvent);

            dataGridView_UserEvent.DefaultCellStyle.Font = new Font("맑은 고딕", 8);
            dataGridView_UserEvent.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView_UserEvent.ColumnCount = (int)eEventChangeLogIdx.COUNTOF;

            for (eEventChangeLogIdx i = 0; i < eEventChangeLogIdx.COUNTOF; i++)
            {
                dataGridView_UserEvent.Columns[(int)i].Width = (i == eEventChangeLogIdx.Time) ? 110 : 60;

                dataGridView_UserEvent.Columns[(int)i].Name = i.ToString();
                dataGridView_UserEvent.Columns[(int)i].HeaderText = i.ToString();

                dataGridView_UserEvent.Columns[(int)i].DefaultCellStyle.Alignment = (i == eEventChangeLogIdx.Time) ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                dataGridView_UserEvent.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
            }

            dataGridView_UserEvent.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            dataGridView_UserEvent.Columns[0].Width = 40;
            dataGridView_UserEvent.AllowUserToResizeRows = false;
            dataGridView_UserEvent.RowTemplate.Height = 16;

            dataGridView_UserEvent.Columns[(int)eEventChangeLogIdx.Time_Date].Visible = false;

            foreach (DataGridViewColumn c in dataGridView_UserEvent.Columns)     // 정렬되면 안됨
                c.SortMode = DataGridViewColumnSortMode.NotSortable;

            MyTools.GridHelper.LoadGridInfo(dataGridView_UserEvent, UMain.INI_PATH, dataGridView_UserEvent.Name);

            TDrawingControl.ResumeDrawing(dataGridView_UserEvent);
        }

        public void SetUI(DDataSet set)
        {

            m_dataSet = set;
            m_curFile = set.UserLog.filePath;
             
            MakeEventLog(m_dataSet.UserLog.EventChangeList.ToArray());
            // Log File만 뽑기. 좀 장황하다..
            List<string> m_fileList = new List<string>(Directory.GetFiles(m_dataSet.folderPath)).FindAll(o=> Path.GetExtension(o).ToUpper() == ".LOG");

            foreach(string file in m_fileList)
            {
                try
                {
                    string[] arr = new string[2];
                    long length = new System.IO.FileInfo(file).Length;
                    
                    arr[0] = Path.GetFileNameWithoutExtension(file); 
                    arr[1] = String.Format("{0:#,###0}", length);

                    ListViewItem lvi = new ListViewItem(arr);
                    lvi.Tag = file;
                    listView_file.Items.Add(lvi);
                }
                catch
                {
                    continue;
                }
            }
            

        }

        public void MakeEventLog(DUserLogEventChangeRowItem[] rows)
        {
            dataGridView_UserEvent.Rows.Clear();

            // Elapse Time 구하기
            for (int r = 0; r < rows.Length - 1; r++)
            {
                DUserLogEventChangeRowItem item = rows[r];

                DateTime tm1 = rows[r + 1].subItems[(int)eEventChangeLogIdx.Time_Date];
                DateTime tm2 = rows[r].subItems[(int)eEventChangeLogIdx.Time_Date];
                TimeSpan span = tm1 - tm2;
                string strElapseTime = String.Format("{0:#0.#0}", span.TotalSeconds);
                rows[r].subItems[(int)eEventChangeLogIdx.Elapse_Sec] = strElapseTime;
            }

            int nFirstError = -1;

            TDrawingControl.SuspendDrawing(dataGridView_UserEvent);
            // gridview에 행 추가
            for (int r = 0; r < rows.Length; r++)
            {
                int n = dataGridView_UserEvent.Rows.Add(rows[r].subItems);   // ADD

                DataGridViewRow row = dataGridView_UserEvent.Rows[n];
                string strSpeed = (string)row.Cells[(int)eEventChangeLogIdx.Speed].Value;
                double dSpeed = 0;
                if (Double.TryParse(strSpeed, out dSpeed))
                {
                    strSpeed = String.Format("{0:0.00}", dSpeed); // 소수 두자리까지 모두 표시하기 위함
                    row.Cells[(int)eEventChangeLogIdx.Speed].Value = strSpeed;
                }

                string stat = rows[r].subItems[(int)eEventChangeLogIdx.Mode];

                // 상태별 색 표시
                if (stat.Length == 2)
                { 
                    // ME 셀 핑크색 표시
                    if (stat == "ME")
                    {
                        for (int i = 0; i < (int)eEventChangeLogIdx.COUNTOF; i++)
                            row.Cells[i].Style.BackColor = Color.HotPink;

                        if (nFirstError < 0)
                            nFirstError = r;
                    }
                    else if (stat.ElementAt(0) == 'M'  || stat.ElementAt(0) == 'E')      //ME 빼고 모든 에러 상태
                    {
                        for (int i = 0; i < (int)eEventChangeLogIdx.COUNTOF; i++)
                            row.Cells[i].Style.BackColor = Color.LightPink;
                    }
                    else if (stat.ElementAt(0) == 'A' && stat != "AG")      //AG 빼고 Auto 상태 모두
                    {
                        for (int i = 0; i < (int)eEventChangeLogIdx.COUNTOF; i++)
                            row.Cells[i].Style.BackColor = Color.Yellow;
                    }
                }

            }
            // 그리드에서 한화면에표시되는 줄 수를 알아내고, nFirstError 그리드 정중앙에 위치하도록 포커스를 맞춰줌

            try
            {
                int nDisplayedCnt = dataGridView_UserEvent.DisplayedRowCount(false);
                nDisplayedCnt /= 2;
                dataGridView_UserEvent.FirstDisplayedScrollingRowIndex = Math.Max(nFirstError - nDisplayedCnt, 0);

                // 해당 셀이 Visible이 아니면 에러나기 때문에 visible인 셀이 나올떄까지 찾는수밖에 없다
                for (int j = 0; j < dataGridView_UserEvent.ColumnCount; j++)
                {
                    DataGridViewCell cell = dataGridView_UserEvent.Rows[nFirstError].Cells[j];
                    if (cell.Visible)
                    {
                        dataGridView_UserEvent.CurrentCell = cell;
                        break;
                    }
                }
            }
            catch { }
            
            TDrawingControl.ResumeDrawing(dataGridView_UserEvent);
        }

        private void DataGridView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            try
            {
                var grid = sender as DataGridView;

                //찍을 로우 인덱스는 Zero-base 가 아닌 1부터
                var rowIdx = (e.RowIndex).ToString();

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
            catch (Exception) { }
        }
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (dataGridView_UserEvent != null)
                {
                    dataGridView_UserEvent.Dispose();
                    dataGridView_UserEvent = null;
                }
                if(dataGridView_Comment != null)
                {
                    dataGridView_Comment.Dispose();
                    dataGridView_Comment = null;
                }

                CloseAllTab();
                m_dataSet = null;

                if (components != null)
                {
                    components.Dispose();
                }
            }

            base.Dispose(disposing);
        }
        public void CloseAllTab()
        {
            for (int i = tabControl_Log.TabCount-1; i >= 0; i--)
                TabClose(i);
        }

        public void TabClose(int tabIndex)
        {
            if (tabIndex < 0 && tabIndex >= tabControl_Log.TabCount)
                return;

            TabPage page = tabControl_Log.TabPages[tabIndex];
            DLogForm logForm = (DLogForm)page.Tag;
            page.Tag = null;

            if (logForm != null)
            {
                if (logForm.form != null)
                {
                    logForm.form.Dispose();
                    logForm.form = null;
                }
            }
            tabControl_Log.TabPages.RemoveAt(tabIndex);
            page.Dispose();

            int nPrevPage = tabIndex - 1;
            if (nPrevPage >= 0 && nPrevPage < tabControl_Log.TabPages.Count)
                tabControl_Log.SelectedIndex = nPrevPage;

            //switch (logForm.type)
            //{
            //    case DLogForm.FormType.eAmcLog:
            //        {
            //            UAmcRecvTable dlg = (UAmcRecvTable)logForm.form;
            //            dlg.DestroySelf();
            //            break;
            //        }
            //    case DLogForm.FormType.eEventLog:
            //        {
            //            UEventLog dlg = (UEventLog)logForm.form;
            //            dlg.DestroySelf();
            //            break;
            //        }
            //    case DLogForm.FormType.eNotebook:
            //        {
            //            UNotebook dlg = (UNotebook)logForm.form;
            //            dlg.DestroySelf();
            //            break;
            //        }
            //}
        }
        
        public void AddTab(DLogForm logForm, string tabTitle, bool bAsSelectedTab = true)
        {
           

            // 탭 페이지 생성과 기본설정
            TabPage page = new TabPage();
            page.Location = new System.Drawing.Point(4, 27);
            page.Size = new System.Drawing.Size(889, 586);
            page.TabIndex = tabControl_Log.TabPages.Count;
            page.Padding = new System.Windows.Forms.Padding(3);
            page.UseVisualStyleBackColor = true;
            page.UseVisualStyleBackColor = true;

            // 탭페이지에 붙이고자 하는 폼을 붙인다
            page.Controls.Add(logForm.form.Controls[0]);
            page.Tag = logForm;
            page.Text = tabTitle;

            // 탭컨트롤에 탭 페이지를 붙임
            tabControl_Log.TabPages.Add(page);

            if (bAsSelectedTab)
                tabControl_Log.SelectTab(tabControl_Log.TabPages.Count - 1);    // 마지막 탭을 보여주는 것으로함
        }

        private void dataGridView_UserEvent_Leave(object sender, EventArgs e)
        {
            MyTools.GridHelper.SaveGridInfo(dataGridView_UserEvent, UMain.INI_PATH, dataGridView_UserEvent.Name);
        }

        // Event Grid --- Current Cell Changed ********************************
        private void dataGridView_UserEvent_CurrentCellChanged(object sender, EventArgs e)
        {
            if (dataGridView_UserEvent.SelectedCells.Count <= 0)
                return;

            int nRow = dataGridView_UserEvent.SelectedCells[0].RowIndex;
            string likeLine = "[" + (string)dataGridView_UserEvent.Rows[nRow].Cells[(int)eEventChangeLogIdx.Time].Value + "]";

            // 시간 갱신
            SetCurrentTime(likeLine, m_dataSet.UserLog.filePath);
        }

        // 현재 선택된 시간 갱신
        public bool SetCurrentTime(string logLine, string fileName)
        {
            DateTime tm;
            if (MyTools.GMethod.GetTimeFromLine(m_curFile, logLine, out tm) == false)
                return false;

            return SetCurrentTime(tm, fileName);
        }

        public bool SetCurrentTime(DateTime tm, string fileName)
        {
            m_CurTime = tm;
            m_curFile = fileName;
            string timeTxt = "";
            string curStat = "";
            string curSpeed = "";
            string node = "";
            bool bException = false;
            try
            {
                DataRow row = m_dataSet.UserLog.GetRow_ofTheTime(tm);
                timeTxt = String.Format("{0:00}:{1:00}:{2:00}.{3:000}", tm.Hour, tm.Minute, tm.Second, tm.Millisecond);
                curStat = (string)row.ItemArray[(int)eUserLogIdx.Mode];
                curSpeed = String.Format("{0} mm/s", row.ItemArray[(int)eUserLogIdx.Speed]);
                string curNode = Convert.ToString((int)row.ItemArray[(int)eUserLogIdx.Current_Node]);
                string curOffset = Convert.ToString((int)row.ItemArray[(int)eUserLogIdx.Current_NodeOffset]);
                string curStation = Convert.ToString((int)row.ItemArray[(int)eUserLogIdx.Current_Station]);
                node = String.Format("N:{0}({1}) S:{2}", curNode, curOffset, curStation);
            }
            catch
            {
                bException = true;
                //return false;
            }

            textBox_CurTime.Text = timeTxt;
            textBox_Stat.Text = curStat;
            textBox_Speed.Text = curSpeed;
            textBox_Node.Text = node;

            textBox_Stat.BackColor = textBox_Stat.Text == "ME" ? Color.HotPink : defaultBoxColor;
            

            return bException ? false : true;
        }
        private void listView_file_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Enter)
                OpenSelectedFiles(false);
        }

        private void listView_file_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            OpenSelectedFiles(false);
        }
        private DLogForm ShowAmcRecvTable(string logPath, DateTime time, int nBefore_sec, int nAfter_sec)
        {
            try
            {

				DateTime tmFrom = time.AddSeconds(nBefore_sec * (-1));
				DateTime tmTo = tmFrom.AddSeconds(nBefore_sec + nAfter_sec);


				m_dataSet.AmcRcv.Clear();	// AMCRECV 초기화

				int nCnt = tmTo.Hour - tmFrom.Hour;
				bool bExp = nCnt < 0;
				nCnt = Math.Max(nCnt, 0);

				// 파일에서 바로 로딩해서 데이터화 시킨다
				string folder = Path.GetDirectoryName(logPath);
				string fileName = Path.GetFileNameWithoutExtension(logPath);
				
				
				fileName = fileName.Substring(0, fileName.Length - 2);
				for (int h = 0; h <= nCnt; h++)
				{
					try
					{
						string fileName_ = String.Format("{0}\\{1}{2:00}.log", folder, fileName, bExp ? tmTo.Hour : tmFrom.Hour);
						m_dataSet.AmcRcv.LoadFile_timeZone(fileName_, tmFrom, nBefore_sec + nAfter_sec);
					}
					catch
					{
						continue;
					}
				}
				
	
				m_dataSet.AmcRcv.SetUserModeStat(m_dataSet.UserLog.EventChangeList);

				// AMC 데이터 테이블을 시간 기준으로 생성한다. UI 가 아니고 데이터 부분
				DataTable tmpTable = m_dataSet.AmcRcv.table;

                // AMC 테이블 UI 창 생성
                UAmcRecvTable uAmcRcv = new UAmcRecvTable();
                uAmcRcv.m_Parent = m_dataSet;

                // 데이터를 UI에 삽입니다. 그리드 생성하기 때문에 시간이 오래걸리는 작업
                uAmcRcv.ShowTable(tmpTable, time, m_dataSet.AmcRcv.filePath);

                string tabTitle = String.Format("AMCRecv{0:00}:{1:00}:{2:00}", time.Hour, time.Minute, time.Second);
                DLogForm logForm = new DLogForm();
                logForm.form = uAmcRcv;
                logForm.type = eLogType.AmcRcv;

                return logForm;
            }
            catch (Exception exp)
            {
                MessageBox.Show("테이블 생성중 에러 발생\n\n" + exp.Message);
                //throw;
            }
            return null;
        }
        public DLogForm CreateLogForm(string path)
        {
            if (String.IsNullOrEmpty(path))
                return null;

            if (File.Exists(path) == false)
                return null;
            
            // user log 인경우 table로 보여주기
            if (Path.GetFileNameWithoutExtension(path).ToUpper().IndexOf("USER2") >= 0)
            {
                // User Log 폼생성
                UUserLogTable form = new UUserLogTable();
                form.UserLogFileOpen(path, m_dataSet);
                form.GotoTime(m_CurTime);

                // DLogForm 형태로 변환
                DLogForm logForm = new DLogForm();
                logForm.form = form;
                logForm.type = eLogType.UserLog;
                logForm.path = path;
                return logForm;
            }
            else if (Path.GetFileNameWithoutExtension(path).ToUpper().IndexOf("AMC_RECV2") >= 0)
            {
                // 시간, 이전 이후 초 세팅하는 다이얼로그 모달로 띄운다
                UAmcRecvOpen dlg = new UAmcRecvOpen();
                dlg.m_SelectedTime = m_CurTime;
                dlg.ShowDialog();
                if (dlg.DialogResult != DialogResult.OK)
                    return null;

				DProgress prg = new DProgress();
				prg.StartProgress((dlg.nBefore_sec+dlg.nAfter_sec)*100);
				DLogForm logForm = ShowAmcRecvTable(path, dlg.m_SelectedTime, dlg.nBefore_sec, dlg.nAfter_sec);
				prg.EndProgress();

                logForm.path = path;
                return logForm;
            }
            else
            {
                // User Log 폼생성
                UNotebook form = new UNotebook();
                form.m_parent = m_dataSet;
                form.FileOpen(path);

                // DLogForm 형태로 변환
                DLogForm logForm = new DLogForm();
                logForm.form = form;
                logForm.type = eLogType.NONE;
                logForm.path = path;
                return logForm;
            }
        }

        // 현재 선택된 시간으로 이동
        private void ToolStripMenuItem_OpenCurTime_Click(object sender, EventArgs e)
        {
            OpenSelectedFiles(true);
        }

        // File List에서 현재 선택된 파일들 Tab으로 연다. 
        public void OpenSelectedFiles(bool bGoToCurTime)
        {
            if(listView_file.SelectedItems.Count != 1)
            {
                MessageBox.Show("한개의 파일을 선택해 주세요.");
                return;
            }
            
            ListViewItem lvi = listView_file.SelectedItems[0];
            try
            {
                string filePath = (string)lvi.Tag;
                if (String.IsNullOrEmpty(filePath))
                {
                    MessageBox.Show("알수 없는 파일 이름\n\n" + filePath);
                    return;
                }

                if (File.Exists(filePath) == false)
                {
                    MessageBox.Show("존재하지 않는 파일\n\n" + filePath);
                    return;
                }

                // 이미 열려있는 탭이지 확인
                for(int k=0 ; k<tabControl_Log.TabPages.Count ; k++)
                {
                    DLogForm pageForm = (DLogForm)tabControl_Log.TabPages[k].Tag;
                    if (pageForm == null)
                        continue;

                    // 열려고 하는 파일이 이미 열려있는 경우
                    if(pageForm.path == filePath)
                    {
                        tabControl_Log.SelectedIndex = k;
                        if(pageForm.type == eLogType.NONE)
                        {
                            UNotebook dlg = (UNotebook)pageForm.form;
                            if(bGoToCurTime)
                                dlg.GotoTime(m_CurTime);
                        }
                        else if(pageForm.type == eLogType.UserLog)
                        {
                            UUserLogTable dlg = (UUserLogTable)pageForm.form;
                            if (bGoToCurTime)
                                dlg.GotoTime(m_CurTime);
                        }
                        return;
                    }
                }
                
                // 폼 생성
                DLogForm form = CreateLogForm(filePath);
                if (form == null)
                {
                    MessageBox.Show("폼 생성 실패");
                    return;
                }

                // 폼 탭에 추가
                AddTab(form, Path.GetFileNameWithoutExtension(filePath), true);

                if (form.type == eLogType.NONE)
                {
                    UNotebook notebook = (UNotebook)form.form;
                    if (bGoToCurTime)
                        notebook.GotoTime(m_CurTime);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("파일 열기 실패\n\n" + ex.Message);
            }
        }

        // 이 파일 이름을 가진 탭이 이미 존재하는지 확인
        public bool IsThisTabOpen(string fileName, bool bSetAsSelectedTab = false)
        {
            string path = m_dataSet.folderPath + "\\" + fileName;
            if (File.Exists(path) == false)
                return false;

            // 이미 열려있는 탭이지 확인
            for (int k = 0; k < tabControl_Log.TabPages.Count; k++)
            {
                DLogForm pageForm = (DLogForm)tabControl_Log.TabPages[k].Tag;
                if (pageForm == null)
                    continue;

                if (pageForm.path == path)
                {
                    if(bSetAsSelectedTab)
                        tabControl_Log.SelectedIndex = k;

                    return true;
                }
            }
            return false;
        }
        // 특정 파일 열기함
        public bool OpenFile(string fileName, string strSelectRange)
        {
            string path = m_dataSet.folderPath + "\\" + fileName;
            if (File.Exists(path) == false)
                return false;
            
            // 열려있는 폼이라면 선택된 영역을 선택한다
            if(IsThisTabOpen(fileName, true))
            {
                SetSelecteRange(fileName, strSelectRange);
                return true;
            }

            // 열려있지 않다면 새로 생성
            // 폼 생성
            DLogForm form = CreateLogForm(path);
            if (form == null)
            {
                MessageBox.Show("폼 생성 실패");
                return false;
            }

            // 폼 탭에 추가
            AddTab(form, Path.GetFileNameWithoutExtension(path), true);
            if (IsThisTabOpen(fileName, true))
            {
                SetSelecteRange(fileName, strSelectRange);
                return true;
            }
            
            return false;
        }

        public bool SetSelecteRange(string fileName, string strSelectRange)
        {
            try
            {
                string path = m_dataSet.folderPath + "\\" + fileName;
                if (File.Exists(path) == false)
                    return false;

                // 이미 열려있는 탭이지 확인
                for (int k = 0; k < tabControl_Log.TabPages.Count; k++)
                {
                    DLogForm pageForm = (DLogForm)tabControl_Log.TabPages[k].Tag;
                    if (pageForm == null)
                        continue;
                    
                    if (pageForm.path == path)
                    {
                        tabControl_Log.SelectedIndex = k;
                        if (pageForm.type == eLogType.NONE)
                        {
                            UNotebook dlg = (UNotebook)pageForm.form;
                            if (String.IsNullOrEmpty(strSelectRange) == false)
                            {
                                double dIndex = 0;
                                double dLen = 0;
                                // ex) I10:L4  파싱하기 ===> text index 10번째에서 길이 4만큼 선택하기
                                GMethod.GetThisValue(strSelectRange, "I", out dIndex);
                                GMethod.GetThisValue(strSelectRange, "L", out dLen);
                                   dlg.SetSelection((int)dIndex, (int)dLen);
                                  //dlg.SetCurrentLine((int)dIndex);
                            }
                        }
                        else if (pageForm.type == eLogType.UserLog)
                        {
                            UUserLogTable dlg = (UUserLogTable)pageForm.form;
                            dlg.dataGridView_defUser.ClearSelection();
                            if (String.IsNullOrEmpty(strSelectRange) == false)
                            {
                                // ex) C10R20:C10R30, C15R20:C15R30 파싱하기
                                // col10, row20 ~ col 10, row30 과,  col15, row20 ~ col15, row30 파싱하기
                                strSelectRange = strSelectRange.Replace(" ", "");
                                string[] splitStr = strSelectRange.Split(',');
                                Point firstRange = new Point(-1, -1);

                                // 콤마(,) 단위로 나눈 후 반복문
                                foreach (string one in splitStr)
                                {
                                    string[] range = one.Split(':');    //C10R20:C10R30 에서 콜론(:) 앞뒤로 나눈다
                                    if (range.Length != 2)
                                        break;

                                    double dColFrom = 0;
                                    GMethod.GetThisValue(range[0], "C", out dColFrom);

                                    double dRowFrom = 0;
                                    GMethod.GetThisValue(range[0], "R", out dRowFrom);

                                    double dColTo = 0;
                                    GMethod.GetThisValue(range[1], "C", out dColTo);

                                    double dRowTo = 0;
                                    GMethod.GetThisValue(range[1], "R", out dRowTo);

                                    Point from = new Point((int)dColFrom, (int)dRowFrom);
                                    Point to = new Point((int)dColTo, (int)dRowTo);
                                    if (firstRange.X == -1)
                                        firstRange = from;

                                    // 영역을 각 셀별로 돌면서 선택함
                                    for (int col = from.X; col <= to.X; col++)
                                    {
                                        for (int row = from.Y; row <= to.Y; row++)
                                        {
                                            dlg.dataGridView_defUser.Rows[row].Cells[col].Selected = true;
                                        }
                                    }
                                }

                                // 선택된 영역이 스크롤되서 보이도록 한다
                                dlg.SetFocusThisLine(firstRange.Y);
                            }


                        }
                        return true;
                    }
                }
            }
            catch
            {
                MessageBox.Show("파일 열기 실패");
            }
            return false;
        }
        private void dataGridView_UserEvent_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            bool bSpace = (Control.ModifierKeys & Keys.Space) != 0;

            // 찾기
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                UMain.GetThis().m_CurrentGrid = (DataGridView)sender;
                UMain.GetThis().PopUpFindForm();
            }
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TabClose(tabControl_Log.SelectedIndex);
        }

        private void tabControl_Log_MouseClick(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
                contextMenuStrip_TabCtrl.Show((Control)sender, mousePos);
            }
        }

        private void 탐색기에서열기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(listView_file.SelectedItems.Count != 1)
            {
                MessageBox.Show("한개의 파일을 선택해 주세요.");
                return;
            }
            string arg = "/select, \"" + (string)listView_file.SelectedItems[0].Tag;
            System.Diagnostics.Process.Start("explorer.exe", arg);
        }

        private void 메모장에서열기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (listView_file.SelectedItems.Count != 1)
            {
                MessageBox.Show("한개의 파일을 선택해 주세요.");
                return;
            }

            Process.Start("notepad.exe", (string)listView_file.SelectedItems[0].Tag);
        }

        private void listView_file_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
                contextMenuStrip_Files.Show((Control)sender, mousePos);
            }
        }


        int nOldSplitPos = 0;
        bool bExpanded = false;

        private void btn_Expand_Click(object sender, EventArgs e)
        {
            if (bExpanded)
            {
                splitContainer1.SplitterDistance = nOldSplitPos; // 원복
                splitContainer2.Panel2.Enabled = true;
                btn_Expand.Image = imageList_Expand.Images[0];


            }
            else
            {
                splitContainer2.Panel2.Enabled = false;
                nOldSplitPos = splitContainer1.SplitterDistance;
                splitContainer1.SplitterDistance = 50;  // 줄이기
                btn_Expand.Image = imageList_Expand.Images[1];
            }
            bExpanded = !bExpanded;
        }

        // 노드, 스테이션 정보 Textbox를 더블 클릭했을 때
        // text box 내용을 파싱해서 map view 매니저 창으로 연결해준다
        private void textBox_Node_Click(object sender, EventArgs e)
        {
            double dStation = 0;
            //N:34002(2530) S:0 형식의 스트링에서 노드번호 얻어오기
            if (GMethod.GetThisValue(textBox_Node.Text, "S:", out dStation))
            {
                int nStation = (int)dStation;
                if (nStation > 0)
                {
                    // Find Station
                    UMapViewMan.GetThis().SetNodeID(nStation, true);
                    return;
                }
            }

            //N:34002(2530) S:1234 형식의 스트링에서 스테이션 번호 얻어오기
            double dNode = 0;
            if (GMethod.GetThisValue(textBox_Node.Text, "N:", out dNode))
            {
                int nNode = (int)dNode;
                if(nNode > 0)
                {
                    // Find Node
                    UMapViewMan.GetThis().SetNodeID(nNode, false);
                    return;
                }
            }
        }


        // Comment Save 버튼을 눌렀을 때
        private void btn_CommentSave_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Comment Save 할까요?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;
            if (m_dataSet == null)
            {
                MessageBox.Show("Dataset이 없습니다.");
                return;
            }

            SaveCommentFile();
        }

        // Comment 파일 저장
        public bool SaveCommentFile()
        {
            string strText = "";
            int nCnt = 0;
            foreach (DataGridViewRow r in dataGridView_Comment.Rows)
            {
                // Comment 셀 내용
                string str = (string)r.Cells["col_Comment"].Value;
                strText += CMNT_LINE_START + CMNT_KEY + str + "\n";

                // 파일명 셀
                str = (string)r.Cells["col_CommentLogFile"].Value;
                strText += CMNT_LINE_START + CMNT_FILE + str + "\n";

                // 선택범위
                str = (string)r.Cells["col_CommentRange"].Value;
                strText += CMNT_LINE_START + CMNT_RANGE + str + "\n";

                // 작성자
                str = (string)r.Cells["col_CommentAuthor"].Value;
                strText +=CMNT_LINE_START + CMNT_AUTHOR + str + "\n";

                // Amc Recv일 경우, Show, Hide된 Columns 정보.
                str = (string)r.Cells["col_AmcRcvShowCols"].Value;
                strText += CMNT_LINE_START + CMNT_AMCRCV_SHOW_COL + str + "\n";

                // Amc Recv일 경우, Show, Hide된 Columns 정보.
                str = (string)r.Cells["col_AmcRcvOpenRange"].Value;
                strText += CMNT_LINE_START + CMNT_AMCRCV_OPEN_RANGE + str + "\n";

                // Amc Recv일 경우, Show, Hide된 Columns 정보.
                str = (string)r.Cells["col_AttachedFile"].Value;
                strText += CMNT_LINE_START + CMNT_ATTACHED_FILE + str + "\n";
                
                nCnt++;
            }
            string strPath = m_dataSet.folderPath + "\\@Comment.txt";
            if (File.Exists(strPath))
                File.Delete(strPath);

            // 내용 저장
            File.WriteAllText(strPath, strText, Encoding.Default);
            return true;
        }

        // 코멘트 파일 오픈
        public bool LoadCommentFile(ref string strErrMsg)
        {
            List<string> listComment = new List<string>();
            List<string> listLogFile = new List<string>();
            List<string> listRange = new List<string>();
            List<string> listAuthor = new List<string>();
            List<string> listAmcRcvShowHideCol = new List<string>();
            List<string> listAmcRcvOpenRange = new List<string>();
            List<string> listAttachedFile = new List<string>();

            string strPath = m_dataSet.folderPath + "\\@Comment.txt";
            if (File.Exists(strPath) == false)
                return false;
            string []allLines = File.ReadAllLines(strPath, Encoding.Default);

            // 파일 모든 라인 돌면서 파싱하여 list에 저장
            foreach(string it in allLines)
            {
				string line = it;
                if (line.IndexOf(CMNT_LINE_START + CMNT_KEY) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_KEY, "");
                    listComment.Add(line);
                }
                else if(line.IndexOf(CMNT_LINE_START + CMNT_FILE) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_FILE, "");
                    listLogFile.Add(line);
                }
                else if (line.IndexOf(CMNT_LINE_START + CMNT_RANGE) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_RANGE, "");
                    listRange.Add(line);
                }
                else if (line.IndexOf(CMNT_LINE_START + CMNT_AUTHOR) == 0)
                {
                    line.Replace(CMNT_LINE_START + CMNT_AUTHOR, "");
                    listAuthor.Add(line);
                }
                else if (line.IndexOf(CMNT_LINE_START + CMNT_AMCRCV_SHOW_COL) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_AMCRCV_SHOW_COL, "");
                    listAmcRcvShowHideCol.Add(line);
                }
                else if (line.IndexOf(CMNT_LINE_START + CMNT_AMCRCV_OPEN_RANGE) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_AMCRCV_OPEN_RANGE, "");
                    listAmcRcvOpenRange.Add(line);
                }
                else if (line.IndexOf(CMNT_LINE_START + CMNT_ATTACHED_FILE) == 0)
                {
					line = line.Replace(CMNT_LINE_START + CMNT_ATTACHED_FILE, "");
                    listAttachedFile.Add(line);
                }
            }

            // 모든 항목 개수는 동일해야한다.
            // 다른개수가 있으면 파싱 실패
            if(listComment.Count != listLogFile.Count  || listComment.Count  != listRange.Count || listComment.Count != listAuthor.Count || listComment.Count != listAmcRcvShowHideCol.Count || listComment.Count != listAmcRcvOpenRange.Count || listComment.Count != listAttachedFile.Count)
            {
                return false;
            }

            // 항목별로 Comment 그리드에 추가
            for(int i=0; i< listComment.Count; i++)
            {
                int nRow = dataGridView_Comment.Rows.Add();
                dataGridView_Comment.Rows[nRow].Cells["col_Comment"].Value = listComment[i];
                dataGridView_Comment.Rows[nRow].Cells["col_CommentLogFile"].Value = listLogFile[i];
                dataGridView_Comment.Rows[nRow].Cells["col_CommentRange"].Value = listRange[i];
                dataGridView_Comment.Rows[nRow].Cells["col_CommentAuthor"].Value = listAuthor[i];
                dataGridView_Comment.Rows[nRow].Cells["col_AmcRcvShowCols"].Value = listAmcRcvShowHideCol[i];
                dataGridView_Comment.Rows[nRow].Cells["col_AmcRcvOpenRange"].Value = listAmcRcvOpenRange[i];
                dataGridView_Comment.Rows[nRow].Cells["col_AttachedFile"].Value = listAttachedFile[i];
            }
            
            return true;
        }

        // 항목 추가버튼 클릭. 코맨트 추가 창 띄움. 비모달
        private void btn_Add_Click(object sender, EventArgs e)
        {
            m_dlgComment.Init();
            m_dlgComment.Show();
        }

        // 선택된 행 지우기
        private void btn_Remove_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("선택된 Comment를 삭제 할까요?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            RemoveComment(false, true);
        }

        // 코멘트 삭제.
        // 모두 지울지, 선택한것만 지울지 택 가능
        public void RemoveComment(bool bAll = false, bool bSelectedOnly = true)
        {
            // 모든 Comment 행 삭제
            if(bAll)
            {
                for(int i = dataGridView_Comment.Rows.Count - 1; i>=0; i--)
                    dataGridView_Comment.Rows.RemoveAt(i);
                return;
            }

            if (bSelectedOnly == false)
                return;

            // 선택된 셀이 속한 행만 삭제
            List<int> selectedRow = GridHelper.GetSelectedRows(dataGridView_Comment);
            selectedRow.Sort();
            for (int i = selectedRow.Count - 1; i >= 0; i--)
            {
                int nIdx = selectedRow[i];
                dataGridView_Comment.Rows.RemoveAt(nIdx);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            // 현재 탭에서 Grid의 경우 선택된 cell 번호나, UNoteBook의 경우 선택된 라인번호를 Comment 다이얼로그에 입력한다
            // Comment Dialog가 활성화 되었을 때만 동작한다
            if (m_dlgComment.Visible)
                SetSelectedCommentCell();
        }

        // 현재 탭에서 Grid의 경우 선택된 cell 번호나, UNoteBook의 경우 선택된 라인번호를 Comment 다이얼로그에 입력한다
        // Comment Dialog가 활성화 되었을 때만 동작한다
        public void SetSelectedCommentCell()
        {
            if (tabControl_Log.SelectedIndex < 0)
                return;

            if (m_dlgComment.Visible == false)
                return;

            TabPage page = tabControl_Log.TabPages[tabControl_Log.SelectedIndex];

            DLogForm logForm = (DLogForm)page.Tag;

            m_dlgComment.m_folderPath = m_dataSet.folderPath;

            if (!m_dlgComment.checkBox_UseLogPos.Checked)
            {
                m_dlgComment.textBox_LogFile.Text = "";
                m_dlgComment.textBox_SelectedRange.Text = "";
                return;
            }

            m_dlgComment.textBox_LogFile.Text = Path.GetFileName(logForm.path);
            
            if (logForm.type == eLogType.AmcRcv)
            {
                UAmcRecvTable form = (UAmcRecvTable)logForm.form;
                List<int> cols = GridHelper.GetSelectedColumns(form.dataGridView1);
                List<int> rows = GridHelper.GetSelectedRows(form.dataGridView1);
                int nCnt = form.dataGridView1.Rows.Count ;
                string strFrom = (string)form.dataGridView1.Rows[1].Cells[(int)eAmcRcvIdx.Time].Value;
                string strTo = (string)form.dataGridView1.Rows[nCnt-1].Cells[(int)eAmcRcvIdx.Time].Value;

                m_dlgComment.textBox_AmcRecvTime.Text = strFrom + "~" + strTo;
                
                if (cols.Count == 0 || rows.Count == 0)
                {
                    m_dlgComment.textBox_SelectedRange.Text = "";   // 선택된 셀이 없는 경우 
                    return;
                }

                cols.Sort();
                rows.Sort();

                //C3R1:C3R10 형식으로 현재 선택된 그리드의 위치를 표기
                // col 3, row 1  ~ col3, row 10
                string str = "";
                int nRowMin = rows[0];
                int nRowMax = rows[rows.Count - 1];
                for (int i = 0; i < cols.Count; i++)
                {
                    str += String.Format("C{0}R{1}:C{2}R{3}", cols[i], nRowMin, cols[i], nRowMax);
                    if (i != cols.Count - 1)
                        str += ", ";
                }

                m_dlgComment.textBox_SelectedRange.Text = str;

                // Amc Recv 테이블의 경우 보여지는 Col 번호까지 기입해야한다
                str = "";
                for (int i = 0; i < form.dataGridView1.Columns.Count; i++)
                {
                    if (form.dataGridView1.Columns[i].Visible)
                        str += String.Format("{0}, ", i);
                }
                m_dlgComment.textBox_AmcRecvCols.Text = str;
            }

            else if (logForm.type == eLogType.UserLog)
            {
                UUserLogTable form = (UUserLogTable)logForm.form;
                List<int> cols = GridHelper.GetSelectedColumns(form.dataGridView_defUser);
                List<int> rows = GridHelper.GetSelectedRows(form.dataGridView_defUser);

                if (cols.Count == 0 || rows.Count == 0)
                {
                    m_dlgComment.textBox_SelectedRange.Text = "";   // 선택된 셀이 없는 경우
                    return;
                }

                cols.Sort();
                rows.Sort();

                //C3R1:C3R10 형식으로 현재 선택된 그리드의 위치를 표기
                // col 3, row 1  ~ col3, row 10
                string str = "";
                int nRowMin = rows[0];
                int nRowMax = rows[rows.Count - 1];
                for (int i = 0; i < cols.Count; i++)
                {
                    str += String.Format("C{0}R{1}:C{2}R{3}", cols[i], nRowMin, cols[i], nRowMax);
                    if (i != cols.Count - 1)
                        str += ", ";
                }

                m_dlgComment.textBox_SelectedRange.Text = str;
                m_dlgComment.textBox_AmcRecvCols.Text = "";
            }
            else if (logForm.type == eLogType.NONE)
            {
                UNotebook form = (UNotebook)logForm.form;

                // 현재 선택된 라인번호를 기입
                // L10 형식 . 10번쨰 line
                int nIndex = 0;
                int nLen = 0;
                form.GetCurrentSelection(out nIndex, out nLen);

                string str = String.Format("I{0}:L{1}", nIndex, nLen);

                m_dlgComment.textBox_SelectedRange.Text = str;
                m_dlgComment.textBox_AmcRecvCols.Text = "";
            }
        }

        private void dataGridView_Comment_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            List<int> row = GridHelper.GetSelectedRows(dataGridView_Comment);
            if (row.Count < 1)
                return;
            int nIdx = row[0];
            string filename = (string)dataGridView_Comment.Rows[nIdx].Cells["col_CommentLogFile"].Value;
            string range = (string)dataGridView_Comment.Rows[nIdx].Cells["col_CommentRange"].Value;
            string attachedFile = (string)dataGridView_Comment.Rows[nIdx].Cells["col_AttachedFile"].Value;

            if (String.IsNullOrEmpty(filename) == false)
                OpenFile(filename, range);
            
            if(String.IsNullOrEmpty(attachedFile) == false)
            {
                ProcessStartInfo stinfo = new ProcessStartInfo();
                stinfo.FileName = m_dataSet.folderPath + @"\" + attachedFile;
                stinfo.CreateNoWindow = true;
                stinfo.UseShellExecute = true;
                Process myProcess = Process.Start(stinfo);
                myProcess.Start();

            }
        }

        // 코멘트 파일 재오픈 버튼클릭
        private void btn_Reload_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Comment Reload 할까요?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            RemoveComment(true);
            string strErrMsg = "";
            if (LoadCommentFile(ref strErrMsg) == false)
            {
                MessageBox.Show("Comment 파일 열기 실패\n\n" + strErrMsg);
            }
        }

     
        // 코멘트 Clear 버튼 클릭
        private void btn_CommentClear_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Comment Clear 할까요?", "Question", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;
            RemoveComment(true);
        }

        // 선택된 노드 Map에서 찾기
        private void ToolStripMenuItem_FindInMapView_Node_Click(object sender, EventArgs e)
        {
            string nodes = "";
            int nCnt = 0;
            int nPrev = 0;
            foreach (DataGridViewCell cell in dataGridView_UserEvent.SelectedCells)
            {
                if (cell.ColumnIndex != (int)eEventChangeLogIdx.Node)
                    continue;
                if ((Int32)cell.Value == 0)
                    continue;
                if (nPrev == (Int32)cell.Value)
                    continue;
                nPrev = (Int32)cell.Value;
                nodes += String.Format("{0}, ", cell.Value);
                nCnt++;
            }
            if(nCnt == 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }
            UMapViewMan.GetThis().SetNodeID(nodes);
        }

        private void ToolStripMenuItem_FindInMapView_Station_Click(object sender, EventArgs e)
        {
            string station = "";
            int nCnt = 0;
            int nPrev = 0;
            foreach (DataGridViewCell cell in dataGridView_UserEvent.SelectedCells)
            {
                if (cell.ColumnIndex != (int)eEventChangeLogIdx.Station)
                    continue;
                if ((Int32)cell.Value == 0)
                    continue;
                if (nPrev == (Int32)cell.Value)
                    continue;
                nPrev = (Int32)cell.Value;
                station += String.Format("{0}, ", cell.Value);
                nCnt++;
            }
            if (nCnt == 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }
            UMapViewMan.GetThis().SetNodeID(station, true);
        }

        private void dataGridView_UserEvent_MouseClick(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
                contextMenu_Event.Show((Control)sender, mousePos);
            }
        }
    }
}
