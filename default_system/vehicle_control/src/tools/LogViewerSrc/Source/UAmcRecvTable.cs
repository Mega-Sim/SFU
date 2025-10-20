using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using LogViewer;
using MyTools;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;

namespace LogViewer
{
    public partial class UAmcRecvTable : Form
    {
        const bool USE_TABLE_BINDING = false;
        const int MAX_ROW_CNT = 50000;
        string folderPath = "";
        public UAmcRecvCols m_dlgCol = new UAmcRecvCols();
        public DDataSet m_Parent = null;
        public bool bLoaded = false;
        public int DataCount = 0;
        public int InputCount = 0;
        public int OutputCount = 0;
        public int Input = 0;
        public int Output = 0;

        public UAmcRecvTable()
        {
            InitializeComponent();
            m_dlgCol.m_dlgParent = this;
            dataGridView1.Tag = "AMCRECV";
            // UAmcRecvCols을 우측에 붙이기
            splitContainer1.Panel2.Controls.Add(m_dlgCol.Controls[0]);
            TDrawingControl.SetDoubleBuffered(dataGridView1);
			dataGridView1.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			//LoadForm(this, new EventArgs());
			InitForm();
			
        }

        public bool OpenFile(string path)
        {
            if(m_Parent != null)
            {
                m_Parent.DestroyFrame();
                m_Parent = null;
            }
            m_Parent = new DDataSet();
            if(m_Parent.AmcRcv.LoadFile(path) == false)
                return false;
            
            // AMC 데이터 테이블을 시간 기준으로 생성한다. UI 가 아니고 데이터 부분
            DataTable tmpTable = m_Parent.AmcRcv.GetTable_asTime(DateTime.Now, 0, 0, true); // 전체 데이터 다 얻기
			string errMsg = "";

			ShowTable(tmpTable, new DateTime(), path, ref errMsg);
			if (String.IsNullOrEmpty(errMsg) == false)
				MessageBox.Show(errMsg);

            return true;
        }

        public void DestroySelf()
        {
			try
			{
				this.m_Parent = null;
				m_dlgCol.DestroySelf();
				dataGridView1.Dispose();
			}
			catch (Exception)
			{

				//throw;
			}
        }
        private void InitForm()
        {
            dataGridView1.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView1.DefaultCellStyle.Font = new Font("맑은 고딕", 7);
			//dataGridView1.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;

			foreach (DataGridViewColumn c in dataGridView1.Columns)     // 정렬되면 안됨
                c.SortMode = DataGridViewColumnSortMode.NotSortable;
            

            // Columns Setting             
            InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
            Input = (int)eAmcRcvIdx.Input1_1;
            Output = (int)eAmcRcvIdx.Output1_1;

            dataGridView1.ColumnCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;
            DataCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;

            int count = 0;
            for (eAmcRcvIdx i = 0; i < eAmcRcvIdx.COUNTOF; i++)
            {
                string str = "";
                if(i< eAmcRcvIdx.Input1_1)
                {
                    str = DAmcRcvRowItem.GetHeaderName(i, asMultiLine: false);
                    str = str.Replace('\n', '.');
                }
                else if(i<eAmcRcvIdx.Output1_1)
                {
                    int n = (int)i;
                    if ((n - Input) < InputCount)
                    {
                        str = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        str = str.Replace('_', '.');
                        // string temp = str;
                        //str = DAmcRcvRowItem.GetHeaderName(i, true, temp);
                    }
                    else
                    {
                        continue;
                    }                                  
                }
                else
                {
                    int n = (int)i;
                    if ((n - Output) < OutputCount)
                    {
                        str = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        str = str.Replace('_', '.');
                        // string temp = str;
                        // str = DAmcRcvRowItem.GetHeaderName(i, true, temp);

                    }
                    else
                    {
                        continue;
                    }
                }               
                
                dataGridView1.Columns[count].Name = str;
				string[] cs = str.Split('.');
				if (cs.Length > 2)  // 두개 넘어가면 헤더에는 두개까지만 대문자로 크게
					dataGridView1.Columns[count].HeaderText = String.Format("{0}.{1}", cs[0].ToUpper(), cs[1].ToUpper());
				else
					dataGridView1.Columns[count].HeaderText = str;
				
				//Trace.WriteLine(str);

				//if(i > eColumn_AmcRcv.In1_Hand_Inpos)
				//    this.dataGridView1.Columns[(int)i].Visible = false;
				dataGridView1.Columns[count].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                dataGridView1.Columns[count++].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
            }
            dataGridView1.Columns[(int)eAmcRcvIdx.Time_Date].Visible = false;
            dataGridView1.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            dataGridView1.AllowUserToResizeRows = false;
            dataGridView1.RowTemplate.Height = 17;

            //row.DefaultCellStyle.BackColor = Color.Bisque;

            InitGrid();
        }
        
        public void InitGrid()
        {
            dataGridView1.Rows.Clear();
			//dataGridView1.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			string[] rowData = new string[(int)DataCount];


            // Row 첫번째는 Col Header와 같은 내용이다. Input Output 헤더 색을 다르게 표시하고싶은데 기본 Header에서는 지원안하기 땜에
            // 첫번째 행을 Header와 같이 쓰기 위함
            int count = 0;
            for (eAmcRcvIdx i = 0; i < eAmcRcvIdx.COUNTOF; i++)
            {
                string str = "";
                string strColName = "";                
                
                if (i < eAmcRcvIdx.Input1_1)
                {
                    str = DAmcRcvRowItem.GetHeaderName(i, asMultiLine: false);                    
                    rowData[count++] = str;
                }
                else if (i < eAmcRcvIdx.Output1_1)
                {
                    int n = (int)i;
                    if ((n - Input) < InputCount)
                    {
                        str = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        string temp = str;
                        str = DAmcRcvRowItem.GetHeaderName(i, true, temp);
                        rowData[count++] = str;
                    }
                    else
                    {
                        continue;
                    }

                }
                else
                {
                    int n = (int)i;
                    if ((n - Output) < OutputCount)
                    {
                        str = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        string temp = str;
                        str = DAmcRcvRowItem.GetHeaderName(i, true, temp);
                        rowData[count++] = str;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            DAmcRcvRowItem row = new DAmcRcvRowItem();
            row.subItems = rowData;
            AddRow(row);

			this.dataGridView1.ColumnHeadersVisible = true;
			this.dataGridView1.AllowUserToResizeColumns = true;
			this.dataGridView1.Rows[0].Height = 50;     // 첫째행을 헤더처럼 쓴다
            this.dataGridView1.Rows[0].Frozen = true;   // 첫째행 열고정

            // DataTable tmpTable = DAmcRcv.CreateBigTable();
            Font headerFont = new Font("맑은 고딕", 9);
            // 헤더 색구별로 Table 가독성 높이기
            for (int i = 0; i < DataCount; i++)
            {
                if (i < (int)eAmcRcvIdx.Input1_1)
                    this.dataGridView1.Rows[0].Cells[i].Style.BackColor = Color.LightGray;
                else if (i < (int)eAmcRcvIdx.Input1_1+InputCount)
                    this.dataGridView1.Rows[0].Cells[i].Style.BackColor = Color.LightBlue;
                else
                    this.dataGridView1.Rows[0].Cells[i].Style.BackColor = Color.LightGreen;

                this.dataGridView1.Rows[0].Cells[i].Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                this.dataGridView1.Rows[0].Cells[i].Style.Font = headerFont;
            }
            

        }

        public void AddRow(DAmcRcvRowItem row)
        {
            dataGridView1.Rows.Add(row.subItems);

        }

        private void DataGridView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            if (e.RowIndex == 0)    // 0번째는 헤더임
                return;
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

        // UEventLog에서 생성한 DataTable 기준으로 DataGridView를 만든다
        public void ShowTable(DataTable table, DateTime selectedTime, string filePath, ref string _out_ErrMsg)
        {

			//this.dataGridView1.Rows.Clear();
			this.dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None;
			foreach (DataGridViewColumn c in dataGridView1.Columns)     // 정렬되면 안됨
				c.SortMode = DataGridViewColumnSortMode.NotSortable;
			MyTools.GridHelper.LoadGridInfo(dataGridView1, UMain.INI_PATH, "AmcRcvGrid");            
            m_dlgCol.buttonShowAll_Click(null, null);   // col 다이얼로그에서 이거 한번 해줘야 현재 보이는 Col들이 Refresh 되면서 두 다이얼로그 상태가 맞아짐

														// 라벨들 세팅
			this.folderPath = m_Parent.AmcRcv.filePath;
            this.label_Path2.Text = m_Parent.AmcRcv.filePath;

            int nCnt = table.Rows.Count;
            if (nCnt > 1)
            {
                DateTime tmFrom = (DateTime)table.Rows[0].ItemArray[(int)eAmcRcvIdx.Time_Date];
                DateTime tmTo = (DateTime)table.Rows[nCnt-1].ItemArray[(int)eAmcRcvIdx.Time_Date];

                TimeSpan span = tmTo - tmFrom;
                this.label_OrgTime.Text = String.Format("{0:00}:{1:00}:{2:00} ~ {3:00}:{4:00}:{5:00} [{6:0.##0}sec]", tmFrom.Hour, tmFrom.Minute, tmFrom.Second, tmTo.Hour, tmTo.Minute, tmTo.Second, span.TotalSeconds);
            }

            //this.Text = Path.GetFileName(m_Parent.dataSet.AmcRcv.filePath) + "  " + this.label_time.Text;

            if (table.Columns.Count != dataGridView1.Columns.Count)
            {
				_out_ErrMsg = "뭔가 잘못됐습니다.. Column의 개수가 AmcRecv와 맞지 않습니다.";
				//MessageBox.Show("뭔가 잘못됐습니다.. Column의 개수가 AmcRecv와 맞지 않습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            if (nCnt == 0)
            {
				_out_ErrMsg = "데이터 개수가 0입니다. 보여줄 데이터가 없습니다.";

				//MessageBox.Show("데이터 개수가 0입니다. 보여줄 데이터가 없습니다.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            
            if (nCnt > MAX_ROW_CNT)
            {
				_out_ErrMsg = String.Format("최대 RowCount={0} 인데 이 값을 넘었습니다. CurrentRow={1}", MAX_ROW_CNT, nCnt);

				//MessageBox.Show(String.Format("최대 RowCount={0} 인데 이 값을 넘었습니다. CurrentRow={1}", MAX_ROW_CNT, nCnt), "Range Over", MessageBoxButtons.OK, MessageBoxIcon.Error);
                nCnt = MAX_ROW_CNT;
                return;

            }

            // selectedTime으로 받은 시간대와 가장 가까운 시간을 검색하기 위함
            double dMinSec = Double.MaxValue;
            int nMinIdx = -1;
            int nIdx = 0;

            TDrawingControl.SuspendDrawing(dataGridView1);

			//dataGridView1.DataSource = table.Rows.cop;
			// 모든 Row 순회하면서 테이블에 추가
			foreach (DataRow row in table.Rows)
            {
				//string[] strRow = new string[(int)eAmcRcvIdx.COUNTOF];
				//// 한개 Row의 모든 Column 순회하면서 데이터 쌓기
				//for (int i=0; i< (int)eAmcRcvIdx.COUNTOF; i++) 
				//{
				//    strRow[i] = Convert.ToString(row.ItemArray[i]);
				//    //// Boolean Type이 문자열 true false로 나와서 테이블 가독성이 떨어지기 때문
				//    //if (strRow[i] == "True")
				//    //    strRow[i] = "1";
				//    //else if (strRow[i] == "False")
				//    //    strRow[i] = "0";
				//}
			
				//List<DataGridViewRow> rows = new List<DataGridViewRow>();
				//rows.AddRange(table.Rows);
                // 그리드에 한 줄 추가
                int nRowIdx = dataGridView1.Rows.Add(row.ItemArray);
                
                // 선택된 시간하고 가장 가까운 시간 찾기
                DateTime tm = (DateTime)row.ItemArray[(int)eAmcRcvIdx.Time_Date];
                TimeSpan span = tm - selectedTime;
                double dSec = Math.Abs(span.TotalSeconds);
                if (dMinSec > dSec)
                {
                    dMinSec = dSec;
                    nMinIdx = nIdx;
                }
			
                nIdx++;
            }

            // 셀 Color 표시
			if(nCnt< 10000)
				RefreshCellColor();

            // 선택된 시간 선택해주기. 근데 해당 col이 visible이 아니면 에러나기 때문에 visible인지 보고 한개만 선택해준다
            for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
            {
                if (dataGridView1.Rows[nMinIdx].Cells[i].Visible)
                {
                    // 그리드에서 한화면에표시되는 줄 수를 알아내고, nLineNo가 그리드 정중앙에 위치하도록 포커스를 맞춰줌
                    int nDisplayedCnt = dataGridView1.DisplayedRowCount(false);
                    nDisplayedCnt /= 2;
                    try
                    {

                        dataGridView1.FirstDisplayedScrollingRowIndex = Math.Max(nMinIdx - nDisplayedCnt, 0);
                        dataGridView1.CurrentCell = dataGridView1.Rows[nMinIdx].Cells[1];
                    }
                    catch 
                    {

                        //throw;
                    }
                    
                    //dataGridView1.Rows[nMinIdx].Cells[i].Selected = true;
                    break;
                }
            }

            //this.dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None;
			
            //foreach (DataGridViewColumn c in dataGridView1.Columns)     // 정렬되면 안됨
            //    c.SortMode = DataGridViewColumnSortMode.NotSortable;

            this.dataGridView1.Columns[(int)eAmcRcvIdx.Steer].Frozen = true;    // stat 에서 열고정
			//dataGridView1.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			TDrawingControl.ResumeDrawing(dataGridView1);
            bLoaded = true;
        }


        // 단순히 보여줄때 row 만 입력하고 보여준다.
        public void ShowTable_byRowArr(DataRow []arrRow, string filePath)
        {
            this.dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells;

            m_Parent = new DDataSet();
            m_Parent.AmcRcv.filePath = filePath;
            m_Parent.folderPath = Path.GetDirectoryName(filePath);

            // 라벨들 세팅
            this.folderPath = filePath;
            this.label_Path2.Text = filePath;

            int nCnt = arrRow.Length;
            if (nCnt > 1)
            {
                DateTime tmFrom = (DateTime)arrRow[0].ItemArray[(int)eAmcRcvIdx.Time_Date];
                DateTime tmTo = (DateTime)arrRow[nCnt - 1].ItemArray[(int)eAmcRcvIdx.Time_Date];

                TimeSpan span = tmTo - tmFrom;
                this.label_OrgTime.Text = String.Format("{0:00}:{1:00}:{2:00} ~ {3:00}:{4:00}:{5:00} [{6:0.##0}sec]", tmFrom.Hour, tmFrom.Minute, tmFrom.Second, tmTo.Hour, tmTo.Minute, tmTo.Second, span.TotalSeconds);
            }

            //this.Text = Path.GetFileName(m_Parent.dataSet.AmcRcv.filePath) + "  " + this.label_time.Text;
            
            if (nCnt == 0)
            {
                MessageBox.Show("데이터 개수가 0입니다. 보여줄 데이터가 없습니다.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (nCnt > MAX_ROW_CNT)
            {
                MessageBox.Show(String.Format("최대 RowCount={0} 인데 이 값을 넘었습니다. CurrentRow={1}", MAX_ROW_CNT, nCnt), "Range Over", MessageBoxButtons.OK, MessageBoxIcon.Error);
                nCnt = MAX_ROW_CNT;
                return;
            }

            // selectedTime으로 받은 시간대와 가장 가까운 시간을 검색하기 위함
            //double dMinSec = Double.MaxValue;
            //int nMinIdx = -1;
            int nIdx = 0;

            TDrawingControl.SuspendDrawing(dataGridView1);
            dataGridView1.Rows.Clear();

            // 모든 Row 순회하면서 테이블에 추가
            foreach (DataRow row in arrRow)
            {
                string[] strRow = new string[(int)eAmcRcvIdx.COUNTOF];
                // 한개 Row의 모든 Column 순회하면서 데이터 쌓기
                for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
                {
                    strRow[i] = Convert.ToString(row.ItemArray[i]);
                    // Boolean Type이 문자열 true false로 나와서 테이블 가독성이 떨어지기 때문
                    if (strRow[i] == "True")
                        strRow[i] = "1";
                    else if (strRow[i] == "False")
                        strRow[i] = "0";
                }

                // 그리드에 한 줄 추가
                int nRowIdx = dataGridView1.Rows.Add(strRow);


                // 선택된 시간하고 가장 가까운 시간 찾기
                //DateTime tm = (DateTime)row.ItemArray[(int)eAmcRcvIdx.Time_Date];
                //TimeSpan span = tm - selectedTime;
                //double dSec = Math.Abs(span.TotalSeconds);
                //if (dMinSec > dSec)
                //{
                //    dMinSec = dSec;
                //    nMinIdx = nIdx;
                //}

                nIdx++;
            }

            // 셀 Color 표시
            RefreshCellColor();

            // 선택된 시간 선택해주기. 근데 해당 col이 visible이 아니면 에러나기 때문에 visible인지 보고 한개만 선택해준다
            //for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
            //{
            //    if (dataGridView1.Rows[nMinIdx].Cells[i].Visible)
            //    {
            //        // 그리드에서 한화면에표시되는 줄 수를 알아내고, nLineNo가 그리드 정중앙에 위치하도록 포커스를 맞춰줌
            //        int nDisplayedCnt = dataGridView1.DisplayedRowCount(false);
            //        nDisplayedCnt /= 2;
            //        dataGridView1.FirstDisplayedScrollingRowIndex = Math.Max(nMinIdx - nDisplayedCnt, 0);
            //        dataGridView1.CurrentCell = dataGridView1.Rows[nMinIdx].Cells[1];

            //        //dataGridView1.Rows[nMinIdx].Cells[i].Selected = true;
            //        break;
            //    }
            //}

            this.dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None;

            MyTools.GridHelper.LoadGridInfo(dataGridView1, UMain.INI_PATH, "AmcRcvGrid");
         
            m_dlgCol.buttonShowAll_Click(null, null);   // col 다이얼로그에서 이거 한번 해줘야 현재 보이는 Col들이 Refresh 되면서 두 다이얼로그 상태가 맞아짐
            foreach (DataGridViewColumn c in dataGridView1.Columns)     // 정렬되면 안됨
                c.SortMode = DataGridViewColumnSortMode.NotSortable;

            this.dataGridView1.Columns[(int)eAmcRcvIdx.Steer].Frozen = true;    // stat 에서 열고정

            TDrawingControl.ResumeDrawing(dataGridView1);
            bLoaded = true;
        }


        //DViewColor의 값으로 Cell 색 Refresh
        public void RefreshCellColor()
        {
			this.SuspendLayout();
            int nCnt = dataGridView1.Rows.Count;
            // 모든 Row를 순회하면서
            for (int r = 1; r < nCnt; r++)
            {
                try
                {
                    // 각 Col 마다 값을 DViewColor에 검사함
                    for (int i = 0; i < (int)DataCount; i++)
                    {
                       
                        if ( i == (int) eAmcRcvIdx.Mode_Stat)
                        {
                            string temp = dataGridView1.Rows[r].Cells[i].Value.ToString();
                            if(temp == "ME")
                            {
                                dataGridView1.Rows[r].Cells[i].Style.BackColor = Color.Pink;
                            }
                            else if (temp == "AG" || temp == "AA" || temp == "AI")
                            {
                                dataGridView1.Rows[r].Cells[i].Style.BackColor = Color.LightGreen;
                            }
                            else if (temp == "AL" || temp == "AU")
                            {
                                dataGridView1.Rows[r].Cells[i].Style.BackColor = Color.LightYellow;
                            }                           
                            else
                            {
                                dataGridView1.Rows[r].Cells[i].Style.BackColor = Color.White;
                            }
                        }
                        else
                        {
                            dataGridView1.Rows[r].Cells[i].Style.BackColor = Color.White;
                            dynamic val = dataGridView1.Rows[r].Cells[i].Value;
                            dataGridView1.Rows[r].Cells[i].Style.BackColor = DAmcRcv.CellColor[i].ContionCheck(val);
                        }                       	


                    }
                }
                catch (Exception expt)
                {
                    Debug.Assert(false, expt.Message);
                    throw;
                }
            }
			this.ResumeLayout();
		}

        public void toolStripBtn_TimeLine_Click(object sender, EventArgs e)
        {
            if(dataGridView1.SelectedCells.Count <= 2)
            {
                MessageBox.Show("데이터 범위를 선택해 주세요");
                return;
            }

            List<int> boolCols = new List<int>();       // bool 타입을 담은 열
            List<int> dblCols = new List<int>();        // 정수,실수형을 담은 데이터 열
            int nRowMin = Int32.MaxValue;
            int nRowMax = Int32.MinValue;

            for (int i=0;i< dataGridView1.SelectedCells.Count; i++)
            {
                int nCol = dataGridView1.SelectedCells[i].ColumnIndex;
                int nRow = dataGridView1.SelectedCells[i].RowIndex;
            
                if (nCol >= (int)eAmcRcvIdx.Input1_1)  // io 시작지점
                {
                    if (boolCols.Contains(nCol) == false)
                        boolCols.Add(nCol);
                }
                else
                {
                    if (dblCols.Contains(nCol) == false)
                        dblCols.Add(nCol);
                }
                if (nRow > nRowMax)
                    nRowMax = nRow;
                if (nRow < nRowMin)
                    nRowMin = nRow;
            }

            UTimeLine dlg = new UTimeLine();
            dlg.MakeTimeLine(folderPath, dataGridView1, nRowMin, nRowMax, (int)eAmcRcvIdx.Time, boolCols.ToArray(), dblCols.ToArray());
            dlg.Show();
        }
        
        bool bExpanded = false;
        // Split 으로 구분된 Col 숨기기 보이기
        public void button_ShowHideCols_Click(object sender, EventArgs e)
        {   
            if (bExpanded)
            {
				if (splitContainer1.Width < m_dlgCol.OrgWidth - 10)
					return;

				splitContainer1.SplitterDistance = splitContainer1.Width - m_dlgCol.OrgWidth;

				m_dlgCol.ShowHide(true);
                btn_Expand.Image = imageList_Expand.Images[1];
            }
            else
            {
                m_dlgCol.ShowHide(false);
                splitContainer1.SplitterDistance = splitContainer1.Width - 5;  // 줄이기
                btn_Expand.Image = imageList_Expand.Images[0];
            }
            bExpanded = !bExpanded;
        }

        public void toolStripBtn_SaveGridInfo_Click(object sender, EventArgs e)
        {
            //dataGridView1.ColumnHeadersVisible = !dataGridView1.ColumnHeadersVisible;
            //dataGridView1.AllowUserToResizeColumns = dataGridView1.ColumnHeadersVisible;
            MyTools.GridHelper.SaveGridInfo(dataGridView1, UMain.INI_PATH, "AmcRcvGrid");
        }

        public void toolStripBtn_ExportExcel_Click(object sender, EventArgs e)
        {
            try
            {
                bool bSelectedOnly = MessageBox.Show("선택된 셀만 할까요 아님 전체 셀을 할까요?\n선택된것만: Yes,  전체: No", "Export 영역", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes;
                string path = String.Format("{0}\\ExcelExport", Application.StartupPath);
                DateTime tm = DateTime.Now;
                path += String.Format("\\AmcRcv{0:00}{1:00}{2:00}_{3:00}{4:00}{5:00}__{6}.csv", tm.Year - 2000, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second, MyTools.GMethod.GetLastDir(this.folderPath));
                MyTools.GridHelper.ExportExcel_CSV(dataGridView1, path, bSelectedOnly, nTimeCol: 1, nSkipCol: -1, nSkipCol2: -1, nSkipRow: 0);
            }
            catch (Exception)
            {
            }
        }

        public void toolStripBtn_GridSetting_Click(object sender, EventArgs e)
        {
            try
            {
                UGridOption dlg = new UGridOption();
                dlg.grid = dataGridView1;
                dlg.ShowDialog();
            }
            catch (Exception)
            {
            }
        }

        private void btn_openPath_Click(object sender, EventArgs e)
        {
            try
            {
                string arg = "/select, \"" + this.folderPath + "\"";
                System.Diagnostics.Process.Start("explorer.exe", arg);
            }
            catch (Exception)
            {
            }
        }

        private void btn_MapView_Click(object sender, EventArgs e)
        {
            MyTools.GMethod.MoveForm_ParentCenter(Program.MainForm.m_dlgMapMan, this);
            Program.MainForm.m_dlgMapMan.Show();
            Program.MainForm.m_dlgMapMan.SetNodeID("");
        }


        private void UAmcRecvTable_FormClosing(object sender, FormClosingEventArgs e)
        {
            DestroySelf();
        }

        // 선택된 열에서 찾기
        //private int nCurRow = -1;
        //private void btn_FindInCol_Click(object sender, EventArgs e)
        //{
        //    if(dataGridView1.SelectedCells.Count < 1)
        //    {
        //        MessageBox.Show("선택된 Cell이 없습니다.");
        //        return;
        //    }

        //    string theVal = textbox_Find.Text;
        //    if(String.IsNullOrEmpty(theVal))
        //    {
        //        MessageBox.Show("입력한 값이 없습니다.");
        //        return;
        //    }

        //    List<int> selectedCols = new List<int>();
        //    // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
        //    foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
        //    {
        //        if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
        //            continue;
        //        selectedCols.Add(cell.ColumnIndex);
        //    }
          
        //    int nAddIdx = nCurRow;
        //    dataGridView1.ClearSelection();

        //    bool bFound = false;
        //    int nRowIdx = 0;

        //    // 선택된 열들 모두 순회하면서 검색 비교하는 부분 ********************
        //    foreach (int col in selectedCols)
        //    {
        //        for (int i = 0; i < dataGridView1.Rows.Count; i++)
        //        {
        //            nRowIdx = i + nAddIdx + 1;
        //            nRowIdx %= dataGridView1.Rows.Count;
        //            DataGridViewRow row = dataGridView1.Rows[nRowIdx];
        //            string str = String.Format("{0}", row.Cells[col].Value);
        //            bFound = str.ToUpper() == theVal.ToUpper();
        //            if (bFound)
        //            {   nCurRow = nRowIdx;
        //                row.Cells[col].Selected = true;
        //                dataGridView1.CurrentCell = row.Cells[col];
        //                dataGridView1.FirstDisplayedScrollingRowIndex = nRowIdx;
        //                return;
        //            }
        //        }
        //    }
          
        //    MessageBox.Show("해당 하는 값이 없습니다.");
        //}
        
        // 우클릭 팝업메뉴에서 맵뷰 검색 메뉴
        //private void ToolStripMenuItem_MapViewFind_Click(object sender, EventArgs e)
        //{
        //    try
        //    {
        //        MyTools.GMethod.MoveForm_ParentCenter(Program.MainForm.m_dlgMapMan, this);
        //        Program.MainForm.m_dlgMapMan.Show();
		//
        //        if (dataGridView1.SelectedCells.Count <= 0)
        //            return;
		//
        //        string val = Convert.ToString(dataGridView1.SelectedCells[0].Value);
        //        Program.MainForm.m_dlgMapMan.SetNodeID(val);
        //    }
        //    catch (Exception)
        //    {
        //    }
        //}

        private void ToolStripMenuItem_FixCol_Click(object sender, EventArgs e)
        {
			
			List<int> lc = GridHelper.GetSelectedColumns(dataGridView1);
            if (lc.Count != 1)
            {
                MessageBox.Show("한개의 열을 선택해 주세요.");
                return;
            }
			
			int col = lc[0];
			int nRow = dataGridView1.SelectedCells[0].RowIndex;

			MyTools.GridHelper.FixCol(dataGridView1, col, 0);

			// 그리드에서 한화면에표시되는 줄 수를 알아내고, nLineNo가 그리드 정중앙에 위치하도록 포커스를 맞춰줌
			int nDisplayedCnt = dataGridView1.DisplayedRowCount(false);
			nDisplayedCnt /= 2;
			try
			{

				dataGridView1.FirstDisplayedScrollingRowIndex = Math.Max(nRow - nDisplayedCnt, 0);
				dataGridView1.CurrentCell = dataGridView1.Rows[nRow].Cells[col];
			}
			catch
			{

				//throw;
			}
		}

        private void ToolStripMenuItem_UnfixCol_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedCells.Count <= 0)
            {
                MessageBox.Show("선택된 셀이 없습니다.");
                return;
            }
            
            MyTools.GridHelper.FixCol(dataGridView1, -1, 0);
        }

        private void dataGridView1_MouseClick(object sender, MouseEventArgs e)
        {
            
        }
        

        // 선택된 열 숨기기
        private void ToolStripMenuItem_HideCols_Click(object sender, EventArgs e)
        {
            List<int> selectedCols = new List<int>();
            // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
            foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
            {
                if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
                    continue;
                selectedCols.Add(cell.ColumnIndex);
            }

            // 우측 col UI에서 all 버튼 누른 이벤트 한번 발생.
            // 이전에 find한게 있어서 현재 모든 셀이 보이는 상탤가 아니라면 문제가 되기 때문에
            m_dlgCol.buttonShowAll_Click(null, null);

            foreach (int col in selectedCols)
                m_dlgCol.DoCheckToggle(col, false);
        }

        private void dataGridView1_CurrentCellChanged(object sender, EventArgs e)
        {
            UpdateStatusBar();
        }
        public void UpdateStatusBar()
        {
            //label_SelectedTime 꾸며주기
            try
            {
                int nMinRow = Int32.MaxValue;
                int nMaxRow = Int32.MinValue;
                double dSum = 0;

                foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
                {
                    if (nMinRow > cell.RowIndex)
                        nMinRow = cell.RowIndex;

                    if (nMaxRow < cell.RowIndex)
                        nMaxRow = cell.RowIndex;

                    try
                    {
                        string strTmp = Convert.ToString(cell.Value);
                        double dVal = 0;
                        if (Double.TryParse(strTmp, out dVal))
                            dSum += dVal;
                    } catch{}
                }

                label_Sum.Text = String.Format("Sum: {0}", dSum);

                if (nMinRow < nMaxRow)
                {
                    DateTime tmFrom = DateTime.Parse((string)dataGridView1.Rows[nMinRow].Cells[(int)eAmcRcvIdx.Time].Value);
                    DateTime tmTo = DateTime.Parse((string)dataGridView1.Rows[nMaxRow].Cells[(int)eAmcRcvIdx.Time].Value);
                    label_SelectedTime.Text = String.Format("Selected Time: {0:00}:{1:00}:{2:00} ~ {3:00}:{4:00}:{5:00}", tmFrom.Hour, tmFrom.Minute, tmFrom.Second, tmTo.Hour, tmTo.Minute, tmTo.Second);
                    TimeSpan span = tmTo - tmFrom;
                    label_SpanTime.Text = String.Format("Selected Span Time: {0}", span.TotalSeconds);
                    m_Parent.Frame.SetCurrentTime("[" + (string)dataGridView1.Rows[nMinRow].Cells[(int)eAmcRcvIdx.Time].Value + "]", m_Parent.AmcRcv.filePath);
                }
                else if(nMinRow == nMaxRow)
                {
                    int nEndRow = dataGridView1.Rows.Count-1;
                    int nRow1 = nMinRow == nEndRow ? nMinRow - 1 : nMinRow;
                    int nRow2 = nMinRow == nEndRow ? nMinRow : nMinRow+1;
                    DateTime tmFrom = DateTime.Parse((string)dataGridView1.Rows[nRow1].Cells[(int)eAmcRcvIdx.Time].Value);
                    DateTime tmTo = DateTime.Parse((string)dataGridView1.Rows[nRow2].Cells[(int)eAmcRcvIdx.Time].Value);
                    label_SelectedTime.Text = String.Format("Selected Time: {0:00}:{1:00}:{2:00}", tmFrom.Hour, tmFrom.Minute, tmFrom.Second);
                    TimeSpan span = tmTo - tmFrom;
                    label_SpanTime.Text = String.Format("Selected Span Time: {0}", span.TotalSeconds);
                    m_Parent.Frame.SetCurrentTime( "[" + (string)dataGridView1.Rows[nRow1].Cells[(int)eAmcRcvIdx.Time].Value + "]", m_Parent.AmcRcv.filePath);
                }
            }
            catch (Exception)            {            }

            int nCnt = dataGridView1.SelectedCells.Count;
            label_SelectedItemCnt.Text = String.Format("Selected Item Count: {0}", nCnt);

            
            //;
            //label_Sum;

        }

        private void dataGridView1_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            bool bSpace = (Control.ModifierKeys & Keys.Space) != 0;
            
            // 찾기
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                UMain.GetThis().PopUpFindForm();
            }
            // 화면 늘이기 줄이기
            else if(e.KeyCode == Keys.F11)
            {
                button_ShowHideCols_Click(null, null);
            }
            // 해당 열 전체 선택
            else if (e.Control && e.KeyCode == Keys.Space)
            {
                List<int> selectedCols = new List<int>();
                // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
                foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
                {
                    if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
                        continue;
                    selectedCols.Add(cell.ColumnIndex);
                }

                foreach(int nCol in selectedCols)
                {
                    for (int i = 0; i < dataGridView1.Rows.Count; i++)
                    {
                        DataGridViewCell cell = dataGridView1.Rows[i].Cells[nCol];
                        if(cell != null)
                            cell.Selected = true;

                    }
                }
                UpdateStatusBar();
            }
        }

        private void dataGridView1_MouseDown(object sender, MouseEventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = (DataGridView)sender;

            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl(dataGridView1);
                contextMenuStrip1.Show(dataGridView1, mousePos);
            }
        }

        private void label_Path2_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                if(File.Exists(m_Parent.AmcRcv.filePath))
                    Process.Start("notepad.exe", m_Parent.AmcRcv.filePath);
            }
            catch (Exception ee) { MessageBox.Show("열기 실패\n\n" + ee.Message); }
        }

        private void splitContainer1_Panel2_Paint(object sender, PaintEventArgs e)
        {

        }

        private void dataGridView1_ColumnDividerWidthChanged(object sender, DataGridViewColumnEventArgs e)
        {

        }

        private void btn_Find_Click(object sender, EventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = dataGridView1;
            UMain.GetThis().PopUpFindForm();
        }

        private void 찾기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btn_Find_Click(sender, e);
        }

		public void SetCellColor(Color clr)
		{
			foreach (DataGridViewCell cell in dataGridView1.SelectedCells)
				cell.Style.BackColor = clr;
		}

		private void menu_cellColor_Click(object sender, EventArgs e)
		{
			ToolStripMenuItem menu = (ToolStripMenuItem)sender;
			SetCellColor(menu.BackColor);
		}
		
		private void menu_cellInit_Click(object sender, EventArgs e)
		{
			RefreshCellColor();
		}
	}

	public static class TDrawingControl

    {

        [DllImport("user32.dll")]

        public static extern int SendMessage(IntPtr hWnd, Int32 wMsg, bool wParam, Int32 lParam);



        private const int WM_SETREDRAW = 11;



        /// <summary>
        /// Some controls, such as the DataGridView, do not allow setting the DoubleBuffered property.
        /// It is set as a protected property. This method is a work-around to allow setting it.
        /// Call this in the constructor just after InitializeComponent().
        /// </summary>
        /// <param name="control">The Control on which to set DoubleBuffered to true.</param>
        public static void SetDoubleBuffered(Control control)
        {
            // if not remote desktop session then enable double-buffering optimization
            if (!System.Windows.Forms.SystemInformation.TerminalServerSession)
            {

                // set instance non-public property with name "DoubleBuffered" to true
                typeof(Control).InvokeMember("DoubleBuffered",
                                             System.Reflection.BindingFlags.SetProperty |
                                                System.Reflection.BindingFlags.Instance |
                                                System.Reflection.BindingFlags.NonPublic,
                                             null,
                                             control,
                                             new object[] { true });
            }
        }

        /// <summary>
        /// Suspend drawing updates for the specified control. After the control has been updated
        /// call DrawingControl.ResumeDrawing(Control control).
        /// </summary>
        /// <param name="control">The control to suspend draw updates on.</param>
        public static void SuspendDrawing(Control control)
        {
            SendMessage(control.Handle, WM_SETREDRAW, false, 0);
        }

        /// <summary>
        /// Resume drawing updates for the specified control.
        /// </summary>
        /// <param name="control">The control to resume draw updates on.</param>
        public static void ResumeDrawing(Control control)
        {
            SendMessage(control.Handle, WM_SETREDRAW, true, 0);
            control.Refresh();
        }
    }
}
