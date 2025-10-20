using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MyTools;
using System.Runtime.CompilerServices;
using System.Diagnostics;

namespace LogViewer
{
    public partial class UUserLogTable : Form
    {
        public DDataSet m_Parent = null;
        public bool bLoaded = false;

        int m_nScrollFoundLine = -1;
        string m_strLineFoundMsg = "";
        public string m_filePath = "";
        

        public UUserLogTable()
        {
            InitializeComponent();
            TDrawingControl.SetDoubleBuffered(dataGridView_defUser);
			dataGridView_defUser.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
		}
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if(dataGridView_defUser != null)
                {
                    dataGridView_defUser.Dispose();
                    dataGridView_defUser = null;
                    m_Parent = null;
                }

                if (components != null)
                {
                    components.Dispose();
                }

                // Dispose stuff here
            }

            base.Dispose(disposing);
        }
        
        public void InitGrid()
        {

            // User Grid 초기화 ****************************************************************
            TDrawingControl.SuspendDrawing(dataGridView_defUser);
            dataGridView_defUser.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
            dataGridView_defUser.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView_defUser.ColumnCount = (int)eUserLogIdx.COUNTOF;

            for (eUserLogIdx i = 0; i < eUserLogIdx.COUNTOF; i++)
            {
                dataGridView_defUser.Columns[(int)i].Width = i == eUserLogIdx.Time ? 110 : 60;

                dataGridView_defUser.Columns[(int)i].Name = DUserLogRowItem.GetHeaderName(i, true);
                dataGridView_defUser.Columns[(int)i].HeaderText = DUserLogRowItem.GetHeaderName(i, true);

                dataGridView_defUser.Columns[(int)i].DefaultCellStyle.Alignment = i == eUserLogIdx.Time ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                dataGridView_defUser.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
                dataGridView_defUser.Columns[(int)i].SortMode = DataGridViewColumnSortMode.NotSortable; // 헤더로 정렬되면 큰일
            }

            dataGridView_defUser.Columns[(int)eEventChangeLogIdx.Time_Date].Visible = false;
            dataGridView_defUser.RowTemplate.Height = 17;
            
            //UGridOption.LoadGridInfo(dataGridView_defUser);
            TDrawingControl.ResumeDrawing(dataGridView_defUser);
        }

        public bool UserLogFileOpen(string path, DDataSet parent)
        {
            // dataset이 이미 있으면 삭제한다
            if(m_Parent != null)
            {
                m_Parent.DestroyFrame();
                m_Parent = null;
            }

            // dataset 생성
            if (parent == null)
                m_Parent = new DDataSet();
            else
                m_Parent = parent;

            if (m_Parent.UserLog.table.Rows.Count == 0)
            {// userlog 로딩
                if (m_Parent.UserLog.LoadFile(path) == false)
                    return false;
            }
            m_filePath = path;

            // 데이터 table에 뿌리기
            InitGrid();
            ShowTable();
            return true;
        }

        /// <summary>
        /// userlog 데이터를 테이블에 뿌린다
        /// </summary>
        public void ShowTable()
        {
            if(m_Parent == null)
            {
                MessageBox.Show("dataset이 없어서 user log 테이블을 만들 수 없습니다.");
                return;
            }
            if(m_Parent.UserLog.bLoaded == false)
            {
                MessageBox.Show("User로그 loading 되지 않았습니다.");
                return;
            }
            TDrawingControl.SuspendDrawing(dataGridView_defUser);

            foreach (DataRow row in m_Parent.UserLog.table.Rows)
            {
                int nCurRow = dataGridView_defUser.Rows.Add(row.ItemArray);

                string strSpeed = (string)row.ItemArray[(int)eUserLogIdx.Speed];
                double dSpeed = 0;
                if(Double.TryParse(strSpeed, out dSpeed))
                    dataGridView_defUser.Rows[nCurRow].Cells[(int)eUserLogIdx.Speed].Value = String.Format("{0:0.00}", dSpeed); ;
                

                string strMode = (string)row.ItemArray[(int)eUserLogIdx.Mode];
                
                // 상태별 색 표시
                if (strMode.Length == 2)
                {
                    // ME 셀 핑크색 표시
                    if (strMode == "ME")
                    {
                        dataGridView_defUser.Rows[nCurRow].Cells[(int)eUserLogIdx.Mode].Style.BackColor = Color.HotPink;
                    }
                    else if (strMode.ElementAt(0) == 'M' || strMode.ElementAt(0) == 'E')      //ME 빼고 모든 에러 상태
                    {
                        dataGridView_defUser.Rows[nCurRow].Cells[(int)eUserLogIdx.Mode].Style.BackColor = Color.LightPink;
                    }
                    else if (strMode.ElementAt(0) == 'A' && strMode != "AG")      //AG 빼고 Auto 상태 모두
                    {
                        dataGridView_defUser.Rows[nCurRow].Cells[(int)eUserLogIdx.Mode].Style.BackColor = Color.Yellow;
                    }
                }
            }

            TDrawingControl.ResumeDrawing(dataGridView_defUser);
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

        private void dataGridView_CurrentCellChanged(object sender, EventArgs e)
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

                // 선택된 셀들 순회하며 최소최대 row index를 찾는다
                foreach (DataGridViewCell cell in dataGridView_defUser.SelectedCells)
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
                    }
                    catch { }
                }

                label_selectedSum.Text = String.Format("Sum: {0}", dSum);

                DateTime tmFrom = new DateTime();
                DateTime tmTo = new DateTime();
                if (nMinRow < nMaxRow)
                {
                    tmFrom = (DateTime)dataGridView_defUser.Rows[nMinRow].Cells[(int)eUserLogIdx.Time_Date].Value;
                    tmTo = (DateTime)dataGridView_defUser.Rows[nMaxRow].Cells[(int)eUserLogIdx.Time_Date].Value;
                    //label_SelectedTime.Text = String.Format("Selected Time: {0:00}:{1:00}:{2:00} ~ {3:00}:{4:00}:{5:00}", tmFrom.Hour, tmFrom.Minute, tmFrom.Second, tmTo.Hour, tmTo.Minute, tmTo.Second);

                    TimeSpan span = tmTo - tmFrom;
                    label_spanTime.Text = String.Format("Selected Span Time: {0}", Math.Abs(span.TotalSeconds));
                    if(m_Parent != null)
                        m_Parent.Frame.SetCurrentTime(tmTo, m_filePath);
                }
                else if (nMinRow == nMaxRow)
                {
                    tmFrom = (DateTime)dataGridView_defUser.Rows[nMinRow].Cells[(int)eUserLogIdx.Time_Date].Value;
                    if (m_Parent != null)
                        m_Parent.Frame.SetCurrentTime(tmFrom, m_filePath);

                    label_spanTime.Text = String.Format("Selected Span Time: {0}", 0);
                }
                

            }
            catch (Exception) { }

            int nCnt = dataGridView_defUser.SelectedCells.Count;
            label_selectedItemCnt.Text = String.Format("Selected Item Count: {0}", nCnt);

           

            //;
            //label_Sum;

        }
        private void dataGridView_defUser_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            bool bSpace = (Control.ModifierKeys & Keys.Space) != 0;

            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                UMain.GetThis().PopUpFindForm();
            }
            // 해당 열 전체 선택
            else if (e.Control && e.KeyCode == Keys.Space)
            {
                List<int> selectedCols = new List<int>();
                // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
                foreach (DataGridViewCell cell in dataGridView_defUser.SelectedCells)
                {
                    if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
                        continue;
                    selectedCols.Add(cell.ColumnIndex);
                }

                foreach (int nCol in selectedCols)
                {
                    for (int i = 0; i < dataGridView_defUser.Rows.Count; i++)
                    {
                        DataGridViewCell cell = dataGridView_defUser.Rows[i].Cells[nCol];
                        if (cell != null)
                            cell.Selected = true;

                    }
                }
                UpdateStatusBar();
            }
        }

        private void dataGridView_defUser_MouseDown(object sender, MouseEventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = (DataGridView)sender;
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((DataGridView)sender);
                contextMenuStrip1.Show((DataGridView)sender, mousePos);
                
            }
        }

        private void 찾기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            UMain.GetThis().m_CurrentGrid = dataGridView_defUser;
            UMain.GetThis().PopUpFindForm();
        }

        private void 열선택ToolStripMenuItem_Click(object sender, EventArgs e)
        {
             // 해당 열 전체 선택
            //if (e.Control && e.KeyCode == Keys.Space)
            {
                List<int> selectedCols = new List<int>();
                // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
                foreach (DataGridViewCell cell in dataGridView_defUser.SelectedCells)
                {
                    if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
                        continue;
                    selectedCols.Add(cell.ColumnIndex);
                }

                foreach (int nCol in selectedCols)
                {
                    for (int i = 0; i < dataGridView_defUser.Rows.Count; i++)
                    {
                        DataGridViewCell cell = dataGridView_defUser.Rows[i].Cells[nCol];
                        if (cell != null)
                            cell.Selected = true;

                    }
                }
                UpdateStatusBar();
            }
        }
        // 특정 라인이 적절하게 화면에 보여주게 하기 위함
        public void SetFocusThisLine(int nRow)
        {
            int nDisplayedCnt = dataGridView_defUser.DisplayedRowCount(false);
            nDisplayedCnt /= 2;
            dataGridView_defUser.FirstDisplayedScrollingRowIndex = Math.Max(nRow - nDisplayedCnt, 0);
        }

        // 원하는 시간이 있는 라인으로 이동한다
        // 리턴값은, theTime과 이동하는 라인과의 차이값이다 second
        public double GotoTime(DateTime theTime)
        {
            int nLineCnt = dataGridView_defUser.Rows.Count;
            if (nLineCnt < 3)
                return 0;


            int nTotalTick = Environment.TickCount;

            // 날짜만 today로 바꿈
            //theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, theTime.Hour, theTime.Minute, theTime.Second, theTime.Millisecond);

            DateTime tmLine = new DateTime();

            int nFoundLine = -1;
            do
            {
                // 시작줄 확인       
                DateTime tmFirstLine = (DateTime)dataGridView_defUser.Rows[0].Cells[(int)eUserLogIdx.Time_Date].Value;
                
                if (theTime < tmFirstLine)   // 첫줄은 예외로 검사해봄
                {
                    nFoundLine = 0;
                    break;
                }

                // 마지막줄 확인
                DateTime tmLastLine = (DateTime)dataGridView_defUser.Rows[nLineCnt-1].Cells[(int)eUserLogIdx.Time_Date].Value;
                
                if (theTime > tmLastLine)   // 마지막줄 예외로 검사해봄
                {
                    nFoundLine = nLineCnt - 1;
                    break;
                }

            } while (false);

            // ##############################################
            // 분할정복 알고리즘 사용 Divide And Conqueror.
            // 분할정복을 사용하면 10만줄의 라인이 있어도, 원하는 시간을 최악의 경우에도 30번 이내의 검색으로 찾을 수 있다
            //
            int nPrevDivIdx = nLineCnt;
            int nDivIdx = nLineCnt / 2;
            int nCnt = 0;

            while (nFoundLine < 0)
            {
                tmLine = (DateTime)dataGridView_defUser.Rows[nDivIdx].Cells[(int)eUserLogIdx.Time_Date].Value;
              
                // Divied 된 인덱스가 5일경우 6번과 비교해서 찾았는지 확인한다
                if (nDivIdx < nLineCnt - 1)
                {
                    DateTime tm_p1 = (DateTime)dataGridView_defUser.Rows[nDivIdx+1].Cells[(int)eUserLogIdx.Time_Date].Value;
                    
                    bool bHubo = tm_p1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_p1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx + 1;
                        break;
                    }
                }

                // Divied 된 인덱스가 5일경우 4번과 비교해서 찾았는지 확인한다
                if (nDivIdx > 0)
                {
                    DateTime tm_s1 = (DateTime)dataGridView_defUser.Rows[nDivIdx - 1].Cells[(int)eUserLogIdx.Time_Date].Value;
                  
                    bool bHubo = tm_s1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_s1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx - 1;
                        break;
                    }
                }

                // Divided 된 인덱스와 인접하는 값들이 아니라면 다시 Divide한다
                int nGap = Math.Abs(nDivIdx - nPrevDivIdx);
                if (nGap <= 1)
                    break;

                // 분할정복 알고리즘. 반으로 나눈다
                int nDiv = nGap / 2;
                nPrevDivIdx = nDivIdx;
                nDivIdx = theTime > tmLine ? (nDivIdx + nDiv) : (nDivIdx - nDiv);
                nCnt++;
            }

            m_strLineFoundMsg = "해당하는 시간대 찾기 실패";

            if (nFoundLine < 0)
            {
                Trace.WriteLine("Fail to find line");
                return Double.MaxValue;
            }
            DateTime tmFound = (DateTime)dataGridView_defUser.Rows[nFoundLine].Cells[(int)eUserLogIdx.Time_Date].Value;
 
            TimeSpan span = theTime - tmFound;
            if (span.TotalSeconds > 5)
            {
                m_strLineFoundMsg = String.Format("찾는 시간:  {0:00}:{1:00}:{2:00}.{3:#0.##0}\n찾은 시간:  {4:00}:{5:00}:{6:00}.{7:#0.##0}\n\n {8}초의 오차가 있습니다.",
              theTime.Hour, theTime.Minute, theTime.Second, theTime.Millisecond,
              tmFound.Hour, tmFound.Minute, tmFound.Second, tmFound.Millisecond, span.TotalSeconds);
            }
            else
                m_strLineFoundMsg = "";

            Trace.WriteLine(String.Format("Find in at {0} times in {1}. Elapse: {2}", nCnt, nLineCnt, Environment.TickCount - nTotalTick));


            m_nScrollFoundLine = nFoundLine;
            
            return 0;
        }

        bool bAtOnce = true;
        private void timer1_ScrollTimer(object sender, EventArgs e)
        {
            if (bAtOnce == false)
                return;
            bAtOnce = false;

            if (m_nScrollFoundLine >= 0)
            {
                // 그리드에서 한화면에표시되는 줄 수를 알아내고,  그리드 정중앙에 위치하도록 포커스를 맞춰줌
                try
                {
                    SetFocusThisLine(m_nScrollFoundLine);
                    //int nDisplayedCnt = dataGridView_defUser.DisplayedRowCount(false);
                    //nDisplayedCnt /= 2;
                    //dataGridView_defUser.FirstDisplayedScrollingRowIndex = Math.Max(m_nScrollFoundLine - nDisplayedCnt, 0);

                    // 해당 셀이 Visible이 아니면 에러나기 때문에 visible인 셀이 나올떄까지 찾는수밖에 없다
                    for (int j = 0; j < dataGridView_defUser.ColumnCount; j++)
                    {
                        DataGridViewCell cell = dataGridView_defUser.Rows[m_nScrollFoundLine].Cells[j];
                        if (cell.Visible)
                        {
                            dataGridView_defUser.CurrentCell = cell;
                            break;
                        }
                    }
                }
                catch { }
                
                m_nScrollFoundLine = -1;
                if (String.IsNullOrEmpty(m_strLineFoundMsg) == false)
                {
                    MessageBox.Show(m_strLineFoundMsg);
                    m_strLineFoundMsg = "";
                }
            }

            bAtOnce = true;
        }

        private void dataGridView_defUser_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (dataGridView_defUser.SelectedCells.Count != 1)
                return;

            if(dataGridView_defUser.SelectedCells[0].ColumnIndex == (int)eUserLogIdx.Current_Node)
            {
                int nNode = (int)dataGridView_defUser.SelectedCells[0].Value;
                if(nNode > 0)
                    UMapViewMan.GetThis().SetNodeID(nNode, false);
            }
            else if(dataGridView_defUser.SelectedCells[0].ColumnIndex == (int)eUserLogIdx.Current_Station)
            {
                int nStation = (int)dataGridView_defUser.SelectedCells[0].Value;
                if(nStation > 0)
                    UMapViewMan.GetThis().SetNodeID(nStation, true);
            }
            
        }
    
        private void 선택된노드맵뷰에서찾기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string nodes = "";
            int nCnt = 0;
            int nPrev = 0;
            foreach (DataGridViewCell cell in dataGridView_defUser.SelectedCells)
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
            if (nCnt == 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.");
                return;
            }
            UMapViewMan.GetThis().SetNodeID(nodes);
        }

        private void 선택된스테이션맵뷰에서찾기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string station = "";
            int nCnt = 0;
            int nPrev = 0;
            foreach (DataGridViewCell cell in dataGridView_defUser.SelectedCells)
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
    }
}
