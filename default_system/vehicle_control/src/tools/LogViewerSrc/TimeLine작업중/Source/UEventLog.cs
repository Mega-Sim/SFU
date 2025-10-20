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
using MyTools;
using System.Diagnostics;
using System.Threading;


namespace LogViewer
{
    // User로그의 이벤트가 변경되는 순간만 요약해서 보여주는 창
    public partial class UEventLog : Form 
    {

        static UEventLog m_this = null;
        public DataGridView m_CurrentGrid = null;
        public DateTime m_selectedTime = new DateTime();
        public int m_nShowFrom_Sec = 0;
        public int m_nShowTo_Sec = 0;
        public string filePath = "";
        public DDataSet dataSet = null;
        IniMan ini = new IniMan();

        public bool m_bUpdateRightGrid = true;      // 우측 그리드와 좌측 그리드 연계해서 보여줄지 여부
        public int m_nGridShowCnt_Before = 400; // 우측 그리드에 보여줄때, 좌측에 선택한 항목에서 이전 이후 몇개씩 보여줄지.
        public int m_nGridShowCnt_After = 100;

        private delegate void DelegateOpenAmcRecv();
        private DelegateOpenAmcRecv delegateOpenAmcRecv;

        public UEventLog()
        {
            InitializeComponent();
            delegateOpenAmcRecv = new DelegateOpenAmcRecv(ShowAmcRecvTable);

            // 그리드 포커스 됐을 때 이벤트 한개로 처리
            this.dataGridView_event.Enter += new System.EventHandler(this.dataGridView_Enter);
            this.dataGridView_user.Enter += new System.EventHandler(this.dataGridView_Enter);

            TDrawingControl.SetDoubleBuffered(dataGridView_event);
            TDrawingControl.SetDoubleBuffered(dataGridView_user);

			dataGridView_event.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			dataGridView_user.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;

		}
        public void DestroySelf()
        {
            this.dataGridView_event.Dispose();
            this.dataGridView_user.Dispose();
            this.Dispose();
            
            this.m_CurrentGrid = null;
            this.ini = null;
            this.dataSet = null;
            this.dataGridView_event.Enter -= this.dataGridView_Enter;
            this.dataGridView_user.Enter -= this.dataGridView_Enter;
            
            this.Close();
            this.Dispose();

           
        }

        // 화면에 보이는 이런저런 값들 INI Load. 다음번 열때 똑같은 환경으로 만들어주기 위함
        public void LoadINI()
        {
            try
            {
                ini.Path = UMain.INI_PATH;
                toolStripTextBox_From.Text = ini.Read(this.Name, toolStripTextBox_From.Name, "40");
                toolStripTextBox_To.Text = ini.Read(this.Name, toolStripTextBox_To.Name, "10");
                string str = ini.Read(this.ToString(), this.Name, Convert.ToString(this.splitContainer2.SplitterDistance));
                this.splitContainer2.SplitterDistance = Convert.ToInt32(str);

                str = ini.Read(this.Name, "GridShowCnt1", Convert.ToString(this.m_nGridShowCnt_Before));
                Int32.TryParse(str, out this.m_nGridShowCnt_Before);

                str = ini.Read(this.Name, "GridShowCnt2", Convert.ToString(this.m_nGridShowCnt_After));
                Int32.TryParse(str, out this.m_nGridShowCnt_After);
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
                ini.Path = UMain.INI_PATH;
                ini.Write(this.Name, toolStripTextBox_From.Name, toolStripTextBox_From.Text);
                ini.Write(this.Name, toolStripTextBox_To.Name, toolStripTextBox_To.Text);
                ini.Write(this.Name, this.splitContainer2.Name, Convert.ToString(this.splitContainer2.SplitterDistance));
                ini.Write(this.Name, "GridShowCnt1", Convert.ToString(this.m_nGridShowCnt_Before));
                ini.Write(this.Name, "GridShowCnt2", Convert.ToString(this.m_nGridShowCnt_After));
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        public void InitGrid(ref DDataSet dataSet)
        {
            this.dataSet = dataSet;
            filePath = this.dataSet.folderPath;

            //if (dataSet.AmcRcv.bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.AmcRcv.ToString());
            //if(dataSet.BcrLog.bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.BcrLog.ToString());
            //if(dataSet.SimpleLogs[(int)eSimpleLogType.DrivingCtrl].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.DrivingCtrl.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.DrivingError].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.DrivingError.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.EQPIOError].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.EQPIOError.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.ExtraJobThread].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.ExtraJobThread.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.TaskControl].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.TaskControl.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.PathSearch].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.PathSearch.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.Trans].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.Trans.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.BCRRawData].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.BCRRawData.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.CID_MONITORING].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.CID_MONITORING.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.CID].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.CID.ToString());

            //if (dataSet.SimpleLogs[(int)eSimpleLogType.DiagManager].bLoaded)
            //    comboBox_LogType.Items.Add(eLogType.DiagManager.ToString());

            //if (comboBox_LogType.Items.Count > 0)
            //    comboBox_LogType.SelectedIndex = 0;

            //          public DAmcRcv AmcRcv = new DAmcRcv();
            //public DUserLog UserLog = new DUserLog();
            //public DBcrLog BcrLog = new DBcrLog();
            //public DSimpleLog[] SimpleLogs = new DSimpleLog[(int)eSimpleLogType.COUNTOF];

            // User Log event Grid 초기화 **********************************************
            TDrawingControl.SuspendDrawing(dataGridView_event);

            dataGridView_event.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
            dataGridView_event.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView_event.ColumnCount = (int)eUserLogIdx.COUNTOF + 2;

            for (eEventChangeLogIdx i = 0; i < eEventChangeLogIdx.COUNTOF; i++)
            {
                dataGridView_event.Columns[(int)i].Width = i== eEventChangeLogIdx.Time ? 110 : 60;

                dataGridView_event.Columns[(int)i].Name = DUserLogEventChangeRowItem.GetHeaderName(i, true);
                dataGridView_event.Columns[(int)i].HeaderText = DUserLogEventChangeRowItem.GetHeaderName(i, true);

                dataGridView_event.Columns[(int)i].DefaultCellStyle.Alignment = i == eEventChangeLogIdx.Time ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                dataGridView_event.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
            }

            dataGridView_event.Columns[(int)eEventChangeLogIdx.Time_Date].Visible = false;

            
            dataGridView_event.Columns[2].Frozen = true;   // 열고정

            dataGridView_event.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            dataGridView_event.Columns[0].Width = 40;
            dataGridView_event.AllowUserToResizeRows = false;
            //dataGridView1.RowTemplate.Height = 20;
          
            TDrawingControl.ResumeDrawing(dataGridView_event);

            // User Grid 초기화 ****************************************************************
            TDrawingControl.SuspendDrawing(dataGridView_user);
            dataGridView_user.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
            dataGridView_user.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView_user.ColumnCount = (int)eUserLogIdx.COUNTOF + 2;

            for (eUserLogIdx i = 0; i < eUserLogIdx.COUNTOF; i++)
            {
                dataGridView_user.Columns[(int)i].Width = i == eUserLogIdx.Time ? 110 : 60;

                dataGridView_user.Columns[(int)i].Name = DUserLogRowItem.GetHeaderName(i, true);
                dataGridView_user.Columns[(int)i].HeaderText = DUserLogRowItem.GetHeaderName(i, true);

                dataGridView_user.Columns[(int)i].DefaultCellStyle.Alignment = i == eUserLogIdx.Time ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                dataGridView_user.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
            }

            dataGridView_user.Columns[(int)eEventChangeLogIdx.Time_Date].Visible = false;
            dataGridView_user.RowTemplate.Height = 20;

            UGridOption.LoadGridInfo(dataGridView_event);
            UGridOption.LoadGridInfo(dataGridView_user);

            foreach (DataGridViewColumn c in dataGridView_event.Columns)     // 정렬되면 안됨
                c.SortMode = DataGridViewColumnSortMode.NotSortable;

            TDrawingControl.ResumeDrawing(dataGridView_user);
            LoadINI();
        }
        
        // Event Change Log 데이터 그리드에 추가 *******************************
        public void MakeEventLog(DUserLogEventChangeRowItem []rows)
        {
            dataGridView_event.Rows.Clear();
            dataGridView_user.Rows.Clear();
            // Elapse Time 구하기
            for (int r = 0; r < rows.Length-1; r++)
            {
                DUserLogEventChangeRowItem item = rows[r];

                DateTime tm1 = rows[r + 1].subItems[(int)eEventChangeLogIdx.Time_Date];
                DateTime tm2 = rows[r].subItems[(int)eEventChangeLogIdx.Time_Date];
                TimeSpan span = tm1 - tm2;
                string strElapseTime = String.Format("{0:#0.#0}", span.TotalSeconds);
                rows[r].subItems[(int)eEventChangeLogIdx.Elapse_Sec] = strElapseTime;
            }

            int nFirstError = -1;

            TDrawingControl.SuspendDrawing(dataGridView_event);
            // gridview에 행 추가
            for (int r = 0; r < rows.Length; r++)
            {
                int n = dataGridView_event.Rows.Add(rows[r].subItems);   // ADD

                DataGridViewRow row = dataGridView_event.Rows[n];

                // ME 셀 핑크색 표시
                if (rows[r].subItems[(int)eEventChangeLogIdx.Mode] == "ME")
                {
                    for (int i = 0; i < (int)eEventChangeLogIdx.COUNTOF + 1; i++)
                        row.Cells[i].Style.BackColor = Color.Pink;

                    if (nFirstError < 0)
                        nFirstError = r;
                }
                
            }
            // 그리드에서 한화면에표시되는 줄 수를 알아내고, nFirstError 그리드 정중앙에 위치하도록 포커스를 맞춰줌

            try
            {
                int nDisplayedCnt = dataGridView_event.DisplayedRowCount(false);
                nDisplayedCnt /= 2;
                dataGridView_event.FirstDisplayedScrollingRowIndex = Math.Max(nFirstError - nDisplayedCnt, 0);

                // 해당 셀이 Visible이 아니면 에러나기 때문에 visible인 셀이 나올떄까지 찾는수밖에 없다
                for (int j = 0; j < dataGridView_event.ColumnCount; j++)
                {
                    DataGridViewCell cell = dataGridView_event.Rows[nFirstError].Cells[j];
                    if (cell.Visible)
                    {
                        dataGridView_event.CurrentCell = cell;
                        break;
                    }

                }
            }
            catch  { }

            foreach (DataGridViewColumn c in dataGridView_event.Columns)     // 정렬되면 안됨
                c.SortMode = DataGridViewColumnSortMode.NotSortable;

            TDrawingControl.ResumeDrawing(dataGridView_event);
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
            catch (Exception)            {           }
        }

        public void RefreshStatusbar(object sender)
        {
            DataGridView grid = null;
            if (sender == dataGridView_event)
                grid = dataGridView_event;
            else if (sender == dataGridView_user)
                grid = dataGridView_user;

            //toolStripStatusLabel_Count.Text = "Selected Cnt: " + Convert.ToString(grid.SelectedCells.Count);

            double dSum = 0;
            //bool bNoInt = false;
            for (int i = 0; i < grid.SelectedCells.Count; i++)
            {   try
                {
                    dSum += Convert.ToDouble(grid.SelectedCells[i].Value);
                }
                catch (Exception)
                {
                    //bNoInt = true;
                    break;
                }
            }

            //toolStripStatusLabel_Sum.Text =  "Selected Sum: " + (bNoInt ? "---" : Convert.ToString(Math.Round(dSum, 2)));
        }

        // Event Log에서 선택이 변경됐을때
        // 시간 표시 갱신하고 우측에 User Log 동기맞춰준다 *************************************************
        private void dataGridView1_CurrentCellChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == dataGridView_event)
                {
                    if (dataGridView_event.SelectedCells.Count <= 0)
                    {
                        RefreshStatusbar(dataGridView_event);
                        return;
                    }

                    // 시간 갱신
                    int nRow = dataGridView_event.SelectedCells[0].RowIndex;
                    m_selectedTime = (DateTime)dataGridView_event.Rows[nRow].Cells[(int)eEventChangeLogIdx.Time_Date].Value;
                    //SmallDateTime stm = new SmallDateTime(m_selectedTime);
                    //string strTime = stm.GetTimeString();
                    toolStripTextBox_StadardTime.Text = (string)dataGridView_event.Rows[nRow].Cells[(int)eEventChangeLogIdx.Time].Value;

                    // 우측 그리드 갱신
                    int nCurLine = 0;// (int)dataGridView_event.Rows[nRow].Cells[(int)eEventChangeLogIdx.UserLogNo].Value;
                    Update_RightUserLogGrid(nCurLine);  // 현재 선택된 기준으로 우측 그리드 갱신

                    RefreshStatusbar(dataGridView_event);
                }
                else if (sender == dataGridView_user)
                {

                    RefreshStatusbar(dataGridView_user);
                }
            }
            catch (Exception)            {                 }
        }
        
        // 우측 그리드 갱신
        public void Update_RightUserLogGrid(int nLineNo)
        {
            if (m_bUpdateRightGrid == false)
                return;

            TDrawingControl.SuspendDrawing(dataGridView_user);

            dataGridView_user.Rows.Clear();
            int nNumberOfLine = dataSet.UserLog.table.Rows.Count;
            //int nShowCnt = 500;
            int nMin = Math.Max(nLineNo - m_nGridShowCnt_Before, 0);
            int nMax = Math.Min(nLineNo + m_nGridShowCnt_After, nNumberOfLine);

            //int nMiddleVal = (nMin + nMax) / 2;
            int nSelectedLineNo = 0;
            
            for(int i=nMin, nIdx = 0 ; i<nMax ; i++, nIdx++)
            {
                if (i == nLineNo)
                    nSelectedLineNo = nIdx;
                
                DataRow row = dataSet.UserLog.table.Rows[i];
                int nCurRow = dataGridView_user.Rows.Add(row.ItemArray);

                string strMode = (string)row.ItemArray[(int)eUserLogIdx.Mode];
                // ME 셀 핑크색 표시
                if (strMode == "ME")
                {
                    foreach(DataGridViewCell cell in dataGridView_user.Rows[nCurRow].Cells)
                        cell.Style.BackColor = Color.Pink;
                }
            }

            // 그리드에서 한화면에표시되는 줄 수를 알아내고, nLineNo가 그리드 정중앙에 위치하도록 포커스를 맞춰줌
            int nDisplayedCnt = dataGridView_user.DisplayedRowCount(false);
            nDisplayedCnt /= 2;
            dataGridView_user.FirstDisplayedScrollingRowIndex = Math.Max(nSelectedLineNo - nDisplayedCnt, 0);
            dataGridView_user.CurrentCell = dataGridView_user.Rows[nSelectedLineNo].Cells[1];

            TDrawingControl.ResumeDrawing(dataGridView_user);
        }


        // 테이블 열기 버튼 이벤트
        private void button_show_Click(object sender, EventArgs e)
        {
            // 예외값 체크
            if (!Int32.TryParse(toolStripTextBox_From.Text, out m_nShowFrom_Sec) || !Int32.TryParse(toolStripTextBox_To.Text, out m_nShowTo_Sec))
            {
                MessageBox.Show("Check Second.");
                return;
            }

            if(m_nShowFrom_Sec < 0 || m_nShowFrom_Sec > 300 || m_nShowTo_Sec < 0 || m_nShowTo_Sec > 300)
            {
                MessageBox.Show("Check Second Range.");
                return;
            }

            // Thread로 AMC 테이블 Open 시작
            m_this = this;
            //splitContainer1.Enabled = false;
            progressBar1.Visible = true;
            
            
            ShowAmcRecvTable();


            //var th = new Thread(ShowTable_Thread);
            //th.Priority = ThreadPriority.Highest;
            //th.Start();

            //while(th.Join(1) == false)
            //{
            //    if (progressBar1.Value < 80)
            //        Thread.Sleep(50);
            //    else
            //        Thread.Sleep(200);

            //    if(progressBar1.Value < 100)
            //        progressBar1.Value++;
            //}

            progressBar1.Visible = false;
            splitContainer1.Enabled = true;

            toolStripButton_TableShow.Text = "Amc Recv 테이블 열기";
          

            m_this = null;
        }

        

        private void ShowTable_Thread()
        {
            m_this.Invoke(m_this.delegateOpenAmcRecv);  // Thread에서 UI 접근하려면 Invoke를 해야함
            // Invoke로 ShowAmcRecvTable() 함수가 호출됨
        }

        // Thread에서 Invoke로 호출되는 함수
        private void ShowAmcRecvTable()
        {
            try
            {
                // AMC 데이터 테이블을 시간 기준으로 생성한다. UI 가 아니고 데이터 부분
                DataTable tmpTable = dataSet.AmcRcv.GetTable_asTime(m_selectedTime, m_nShowFrom_Sec, m_nShowTo_Sec);

                // AMC 테이블 UI 창 생성
                UAmcRecvTable uAmcRcv = new UAmcRecvTable();
                uAmcRcv.m_Parent = dataSet;
                
                // 데이터를 UI에 삽입니다. 그리드 생성하기 때문에 시간이 오래걸리는 작업
                uAmcRcv.ShowTable(tmpTable, m_selectedTime, filePath);

                string tabTitle = String.Format("AMCRecv{0:00}:{1:00}:{2:00}", m_selectedTime.Hour, m_selectedTime.Minute, m_selectedTime.Second);
                DLogForm logForm = new DLogForm();
                logForm.form = uAmcRcv;
                logForm.type = eLogType.AmcRcv;

                dataSet.Frame.AddTab(logForm, tabTitle, true);
            }
            catch (Exception exp)
            {
                MessageBox.Show("테이블 생서중 에러 발생\n\n" + exp.Message);
                //throw;
            }
        }

        private void ULogEvent_FormClosing(object sender, FormClosingEventArgs e)
        {
            DestroySelf();
        }
        
        // 더블 클릭할 때
        private void dataGridView_CellMouseDblClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            bool bRBtn = e.Button == MouseButtons.Right;
            int nCol = e.ColumnIndex;
            int nRow = e.RowIndex;

            if (sender == dataGridView_event)
            {
                
                if(nCol == (int)eEventChangeLogIdx.Station)
                {
                    int nStation = (int)dataGridView_event.Rows[nRow].Cells[nCol].Value;
                    if (nStation <= 0)
                        return;
                    //Program.MainForm.m_mapView.ShowStation(nStation);
                }
                else if (nCol == (int)eEventChangeLogIdx.Node)
                {
                    int nNode = (int)dataGridView_event.Rows[nRow].Cells[nCol].Value;
                    if (nNode <= 0)
                        return;
                    //Program.MainForm.m_mapView.ShowNode(nNode);
                }
            }
            else if (sender == dataGridView_user)
            {
                if (nCol == (int)eUserLogIdx.Current_Station)
                {
                    int nStation = (int)dataGridView_user.Rows[nRow].Cells[nCol].Value;
                    if (nStation <= 0)
                        return;
                    //Program.MainForm.m_mapView.ShowStation(nStation);
                }
                else if (nCol == (int)eUserLogIdx.Current_Node)
                {
                    int nNode = (int)dataGridView_user.Rows[nRow].Cells[nCol].Value;
                    if (nNode <= 0)
                        return;
                    //Program.MainForm.m_mapView.ShowNode(nNode);
                }
            }
        }

        // 좌우측 두개의 Grid에 포커스가 잡힐 때 들어오는 이벤트
        // 현재 활성화된 그리드를 알기위함
        private void dataGridView_Enter(object sender, EventArgs e)
        {
            if (sender == dataGridView_event)
                m_CurrentGrid = dataGridView_event;
            else if(sender == dataGridView_user)
                m_CurrentGrid = dataGridView_user;
        }

        // 스플릿 위치바 이동할 때 마우스 Up 이벤트가 발생하기 때문에 이때 해당 위치 INI 저장하기 위함
        private void splitContainer2_MouseUp(object sender, MouseEventArgs e)
        {
            SaveINI();
        }
        
        // 우클릭 팝업 - 틀고정
        private void ToolStripMenuItem_FixCol_Click(object sender, EventArgs e)
        {
            if (m_CurrentGrid == null) {    MessageBox.Show("적용할 그리드를 활성화 시키세요");   return;}
            MyTools.GridHelper.FixCurrentCol(m_CurrentGrid); 
        }

        // 우클릭 팝업 - 틀고정 해제
        private void ToolStripMenuItem_FixColDismiss_Click(object sender, EventArgs e)
        {
            if (m_CurrentGrid == null) { MessageBox.Show("적용할 그리드를 활성화 시키세요"); return; }
            MyTools.GridHelper.FixCurrentCol(m_CurrentGrid, true); 
        }

        //  툴바 버튼 - 우측 테이블 연동할지 말지 여부
        private void ToolStripMenuItem_TableConnect_Click(object sender, EventArgs e)        {            m_bUpdateRightGrid = true;        }
        private void ToolStripMenuItem_TableDisconnect_Click(object sender, EventArgs e)        {            m_bUpdateRightGrid = false;        }
        

        // 툴바 버튼 - 엑셀 Export
        public void toolStripButton_ExportExcel_Click(object sender, EventArgs e)
        {
            if (m_CurrentGrid == null) { MessageBox.Show("적용할 그리드를 활성화 시키세요"); return; }
            lock (this)
            {
                DataGridView grid = m_CurrentGrid;
                bool bSelectedOnly = MessageBox.Show("선택된 셀만 할까요 아님 전체 셀을 할까요?\n선택된것만: Yes,  전체: No", "Export 영역", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes;
                string path = String.Format("{0}\\ExcelExport", Application.StartupPath);
                Directory.CreateDirectory(path);

                // 파일이름 날짜 섞어서 적절하게 자동으로 만들어주자
                DateTime tm = DateTime.Now;
                path += String.Format("\\EventLog{0:00}{1:00}{2:00}_{3:00}{4:00}{5:00}__{6}.csv", tm.Year-2000, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, MyTools.GMethod.GetLastDir(filePath));

                try
                {
                    if (m_CurrentGrid == dataGridView_event)
                        MyTools.GridHelper.ExportExcel_CSV(grid, path, bSelectedOnly, nTimeCol: 2, nSkipCol: 0, nSkipCol2: 1);
                    else if (m_CurrentGrid == dataGridView_user)
                        MyTools.GridHelper.ExportExcel_CSV(grid, path, bSelectedOnly, nTimeCol: 1, nSkipCol: 0);
                }
                catch (Exception exp)
                {
                    MessageBox.Show("엑셀 생성 실패\n\n", exp.Message);
                }
                // Excel 객체 쓰면 좋긴 한데,,, 느려도 너무 느려서 사용불가..
                //MyTool.GridHelper.ExportExcel(grid, bSelectedOnly, true, MyTool.GMethod.GetLastDir(dataSet.folderPath)); 
            }
        }

        // 툴바 버튼 - 그리드 옵션
        public void toolStripButton_GridOption_Click(object sender, EventArgs e)
        {
            DataGridView grid = m_CurrentGrid;
            UGridOption dlg = new UGridOption();
            dlg.m_nShowCntBefore = this.m_nGridShowCnt_Before;
            dlg.m_nShowCntAfter = this.m_nGridShowCnt_After;
            dlg.refParent = this;
            dlg.grid = grid;
            dlg.ShowDialog(); 
            
        }

        private void dataGridView_event_MouseDown(object sender, MouseEventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = (DataGridView)sender;

            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl(dataGridView_event);
                context_EventLog.Show(dataGridView_event, mousePos);
            }
        }

        private void dataGridView_user_MouseDown(object sender, MouseEventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = (DataGridView)sender;

            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl(dataGridView_user);
                context_EventLog.Show(dataGridView_user, mousePos);
            }
        }
        

        private void timer1_Tick(object sender, EventArgs e)
        {
            toolStripButton_TableShow.Enabled = m_this == null;     // thread로 table 여는 중
        }

        private void toolStripButton_OpenFolder_Click(object sender, EventArgs e)
        {
            try
            {
                string arg = "/select, \"" + dataSet.UserLog.filePath + "\"";
                System.Diagnostics.Process.Start("explorer.exe", arg);
            }
            catch (Exception)
            {
            }
        }

        private void dataGridView_event_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                UMain.GetThis().PopUpFindForm();
            }
        }

        private void dataGridView_user_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                UMain.GetThis().PopUpFindForm();
            }
        }
    }


}
