using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using MyTools;
namespace LogViewer
{
    public partial class UViewColor : Form
    {
        int nCurLogType = 0;    // 0=AmcRcv,  1=User,   2=Bcr
        public UViewColor()
        {
            try
            {
                InitializeComponent();
                TDrawingControl.SetDoubleBuffered(dataGridView_ColorAmcRcv);
				dataGridView_ColorAmcRcv.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithoutHeaderText;
				
				UViewColorOption_ManualLoad(this, null);
            }
            catch (Exception expt)
            {
                Debug.Assert(false, expt.Message);
                throw;
            }
        }

        // Load 함수. 테이블 초기화
        private void UViewColorOption_ManualLoad(object sender, EventArgs e)
        {
            try
            {
                dataGridView_ColorAmcRcv.RowPostPaint += DataGridView_RowPostPaint;
                dataGridView_ColorAmcRcv.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
                
                dataGridView_ColorAmcRcv.ColumnCount = DViewColor.COLOR_CNT + 2;

                dataGridView_ColorAmcRcv.Columns[0].HeaderText = "헤더 항목";
                dataGridView_ColorAmcRcv.Columns[0].Name = "헤더 항목";
                dataGridView_ColorAmcRcv.Columns[0].Width = 170;
                dataGridView_ColorAmcRcv.Columns[0].ReadOnly = true;
                dataGridView_ColorAmcRcv.Columns[0].SortMode = DataGridViewColumnSortMode.NotSortable;


                dataGridView_ColorAmcRcv.Columns[1].HeaderText = "조건식";
                dataGridView_ColorAmcRcv.Columns[1].Name = "조건식";
                dataGridView_ColorAmcRcv.Columns[1].Width = 100;
                dataGridView_ColorAmcRcv.Columns[1].ReadOnly = false;
                dataGridView_ColorAmcRcv.Columns[1].SortMode = DataGridViewColumnSortMode.NotSortable;

                for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                {
                    dataGridView_ColorAmcRcv.Columns[i+2].HeaderText = String.Format("색{0}", i + 1);
                    dataGridView_ColorAmcRcv.Columns[i+2].Name = String.Format("색{0}", i + 1);
                    dataGridView_ColorAmcRcv.Columns[i+2].Width = 30;
                    dataGridView_ColorAmcRcv.Columns[i+2].ReadOnly = true;
                    dataGridView_ColorAmcRcv.Columns[i+2].SortMode = DataGridViewColumnSortMode.NotSortable;
                }
            }
            catch (Exception expt)
            {
                Debug.Assert(false, expt.Message);
                throw;
            }

            LoadINI();
            //InitListView(nLogType : 0);
            UpdateData(false);
            UpdateData(true);
        }

        public void UpdateData(bool bUpdateValue)
        {
            ///////////////// Update 변수:         UI ===> 변수
            if(bUpdateValue)
            {
                string strErrSum = "";
                int nErrCnt = 0;

                // AMC Recv Update *******************************************************
                for (eAmcRcvIdx idx = 0; idx < eAmcRcvIdx.COUNTOF; idx++)
                {
                    int nIdx = (int)idx;

                    // 해당 AmcRecv 인덱스 항목의 Cell Color 객체 가져오기
                    DViewColor condColor = DAmcRcv.CellColor[nIdx];

                    // 조건 초기화
                    condColor.ClearCondition();

                    // 그리드에서 조건문 string 가져옴
                    string strCond = (string)dataGridView_ColorAmcRcv.Rows[nIdx].Cells[1].Value;
                    if (String.IsNullOrEmpty(strCond))
                        continue;

                    // 조건문을 Cell Color 객체에 추가. 이때 이러가 날 수도 있다.
                    string strErrMsg = "";
                    if (condColor.AddCondition(strCond, out strErrMsg) == false)
                    {
                        nErrCnt++;
                        strErrSum += strErrMsg + "\n";
                    }

                    // 입력한 조건문 개수. 콤마로 구분할 수 있다.
                    int nCondCnt = strCond.Split(',').Length;
                    nCondCnt = Math.Min(nCondCnt, DViewColor.COLOR_CNT);

                    // 입력한 조건문 개수만큼의 셀  Color만 가져와서 Cell Color 객체에 추가한다
                    for (int i = 0; i < nCondCnt; i++)
                        condColor.colorList[i] = dataGridView_ColorAmcRcv.Rows[nIdx].Cells[i + 2].Style.BackColor;
                }
                
                if(nErrCnt > 0)
                {
                    MessageBox.Show(String.Format("Color 조건 중 {0} 개의 에러 발생.\n\n", strErrSum));
                }
                //
                // TODO...

            }
            ////////////////// Update UI :          변수 ====> UI
            else
            {
                dataGridView_ColorAmcRcv.Rows.Clear();      // 다 지우고 다시 생성
                for (eAmcRcvIdx idx = 0; idx < eAmcRcvIdx.COUNTOF; idx++)
                {
                    int nIdx = (int)idx;
                    string[] strArr = new string[2 + DViewColor.COLOR_CNT]; // Color 색 + 2개 열 (헤더항목 + 조건식)
                    strArr[0] = DAmcRcvRowItem.GetHeaderName(idx, false);       // 헤더 이름 추가
                    strArr[1] = DAmcRcv.CellColor[nIdx].strCondition;       // 조건식 추가

                    // 열 삽입
                    int nPos = dataGridView_ColorAmcRcv.Rows.Add(strArr);
                    dataGridView_ColorAmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;

                    // DViewColor에서 값을 가져와서 그리드 색을 칠해준다
                    for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                        dataGridView_ColorAmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = DAmcRcv.CellColor[nIdx].colorList[i];
                }

                
                // 다른 탭... 나중에 주석 풀어야 함
                //for (eUserLogIdx idx = 0; idx < eUserLogIdx.COUNTOF; idx++)
                //{
                //    DViewColor[] objArr = DUserLog.CellColor;
                //    string[] strArr = new string[2 + DViewColor.COLOR_CNT];
                //    strArr[0] = DUserLogRowItem.GetHeaderName(idx, false);
                //    strArr[1] = objArr[(int)idx].strCondition;
                //    int nPos = dataGridView_AmcRcv.Rows.Add(strArr);

                //    dataGridView_AmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;
                //    for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                //    {
                //        dataGridView_AmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = objArr[(int)idx].colorList[i];
                //    }
                //}
                
                //for (eBcrLogIdx idx = 0; idx < eBcrLogIdx.COUNTOF; idx++)
                //{
                //    DViewColor[] objArr = DBcrLog.CellColor;
                //    string[] strArr = new string[2 + DViewColor.COLOR_CNT];
                //    strArr[0] = DBcrLog.GetHeaderName(idx, false);
                //    strArr[1] = objArr[(int)idx].strCondition;
                //    int nPos = dataGridView_AmcRcv.Rows.Add(strArr);

                //    dataGridView_AmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;

                //    for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                //    {
                //        dataGridView_AmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = objArr[(int)idx].colorList[i];
                //    }
                //}
            }
        }

        public static void SaveINI()
        {
            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;

            // AMC Recv
            for (eAmcRcvIdx idx = 0; idx < eAmcRcvIdx.COUNTOF; idx++)
            {
                int nIdx = (int)idx;
                string key = String.Format("Condition_{0}", nIdx);
                string val = DAmcRcv.CellColor[nIdx].strCondition;

                ini.Write("CellColor_AmcRcv", key, val);

                for(int i=0; i<DViewColor.COLOR_CNT; i++)
                {
                    Color clr = DAmcRcv.CellColor[nIdx].colorList[i];
                    int nColor = clr.ToArgb();
                    //if (nColor < 0)
                    //    continue;
                    key = String.Format("Color_{0}_{1}", nIdx, i);
                    val = Convert.ToString(nColor);
                    ini.Write("CellColor_AmcRcv", key, val);
                }
            }

            // User Log
            for (eUserLogIdx idx = 0; idx < eUserLogIdx.COUNTOF; idx++)
            {
                int nIdx = (int)idx;
                string key = String.Format("Condition_{0}", nIdx);
                string val = DAmcRcv.CellColor[nIdx].strCondition;

                ini.Write("CellColor_User", key, val);

                for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                {
                    int nColor = DAmcRcv.CellColor[nIdx].colorList[i].ToArgb();
                    //if (nColor < 0)
                    //    continue;
                    key = String.Format("Color_{0}_{1}", nIdx, i);
                    val = Convert.ToString(nColor);
                    ini.Write("CellColor_User", key, val);
                }
            }

            // BCR Log
            for (eBcrLogIdx idx = 0; idx < eBcrLogIdx.COUNTOF; idx++)
            {
                int nIdx = (int)idx;
                string key = String.Format("Condition_{0}", nIdx);
                string val = DAmcRcv.CellColor[nIdx].strCondition;

                ini.Write("CellColor_BCR", key, val);

                for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                {
                    int nColor = DAmcRcv.CellColor[nIdx].colorList[i].ToArgb();
                    //if (nColor < 0)
                    //    continue;
                    key = String.Format("Color_{0}_{1}", nIdx, i);
                    val = Convert.ToString(nColor);
                    ini.Write("CellColor_BCR", key, val);
                }
            }
        }
        static public void LoadINI()
        {
            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;

            // Static 변수 null 제거 용도로 한번만 생성해준다.
            if (DAmcRcv.CellColor[0] == null)
            {
                DAmcRcv tbl = new DAmcRcv();
            }
            if (DUserLog.CellColor[0] == null)
            {
                DUserLog tbl = new DUserLog();
            }
            if (DBcrLog.CellColor[0] == null)
            {
                DBcrLog tbl = new DBcrLog();
            }
            string strErrSum = "";
            int nErrCnt = 0;
            // AMC Recv
            for (eAmcRcvIdx idx = 0; idx < eAmcRcvIdx.COUNTOF; idx++)
            {
                int nIdx = (int)idx;
                string key = String.Format("Condition_{0}", nIdx);
                string val = "";

                string strrr = ini.Read("CellColor_AmcRcv", key, "");
                
                DAmcRcv.CellColor[nIdx].strCondition = strrr;
                string strErr = "";
                if (DAmcRcv.CellColor[nIdx].AddCondition(strrr, out strErr) == false)
                {
                    nErrCnt++;
                    strErrSum += strErr + "\n";
                }

                for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                {
                    int nColor = DAmcRcv.CellColor[nIdx].colorList[i].ToArgb();
                    key = String.Format("Color_{0}_{1}", nIdx, i);
                    val = ini.Read("CellColor_AmcRcv", key, "-1");
                    if (val != "-1")
                    {
                        int c = 0;
                        if(Int32.TryParse(val, out c))
                            DAmcRcv.CellColor[nIdx].colorList[i] = Color.FromArgb(c);
                    }
                }
            }

            if(nErrCnt > 0)
                MessageBox.Show(String.Format("Amc Color INI 로딩 중 {0} 개의 에러 발생\n\n" + strErrSum));
            nErrCnt = 0;
            // User Log
            for (eUserLogIdx idx = 0; idx < eUserLogIdx.COUNTOF; idx++)
            {
                try
                {

                    int nIdx = (int)idx;
                    string key = String.Format("Condition_{0}", nIdx);
                    string val = DUserLog.CellColor[nIdx].strCondition;

                    string strrr = ini.Read("CellColor_User", key, "");
                    DUserLog.CellColor[nIdx].strCondition = strrr;
                    string strErr = "";
                    if (DUserLog.CellColor[nIdx].AddCondition(strrr, out strErr) == false)
                    {
                        nErrCnt++;
                        strErrSum += strErr + "\n";
                    }

                    for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                    {
                        int nColor = DUserLog.CellColor[nIdx].colorList[i].ToArgb();
                        key = String.Format("Color_{0}_{1}", nIdx, i);
                        val = ini.Read("CellColor_User", key, "-1");
                        if (val != "-1")
                        {
                            int c = 0;
                            if (Int32.TryParse(val, out c))
                                DUserLog.CellColor[nIdx].colorList[i] = Color.FromArgb(c);
                        }
                    }
                }
                catch (Exception ex)
                {
                    continue;
                    //throw;
                }
            }
            if (nErrCnt > 0)
                MessageBox.Show(String.Format("UserLog Color INI 로딩 중 {0} 개의 에러 발생\n\n" + strErrSum));

            nErrCnt = 0;
            // BCR Log
            for (eBcrLogIdx idx = 0; idx < eBcrLogIdx.COUNTOF; idx++)
            {
                int nIdx = (int)idx;
                string key = String.Format("Condition_{0}", nIdx);
                string val = DBcrLog.CellColor[nIdx].strCondition;

                string strrr = ini.Read("CellColor_BCR", key, "");
                DBcrLog.CellColor[nIdx].strCondition = strrr;
                string strErr = "";
                if (DBcrLog.CellColor[nIdx].AddCondition(strrr, out strErr) == false)
                {
                    nErrCnt++;
                    strErrSum += strErr + "\n";
                }

                for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                {
                    int nColor = DBcrLog.CellColor[nIdx].colorList[i].ToArgb();
                    key = String.Format("Color_{0}_{1}", nIdx, i);
                    val = ini.Read("CellColor_BCR", key, "-1");
                    if (val != "-1")
                    {
                        int c = 0;
                        if (Int32.TryParse(val, out c))
                            DBcrLog.CellColor[nIdx].colorList[i] = Color.FromArgb(c);
                    }
                }
            }
            if (nErrCnt > 0)
                MessageBox.Show(String.Format("BCR Color INI 로딩 중 {0} 개의 에러 발생\n\n" + strErrSum));
        }

        private void DataGridView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
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
        public void buttonApply_Click(object sender, EventArgs e)
        {
            SaveINI();
            UpdateData(true);
            UpdateData(false);  // 데이터와 UI 일치하게 하기위해서 한번더 UI Update

            ApplyColor();
            Close();
            //Hide();
        }

        public void ApplyColor()
        {
            // 현재 열린 모든 탭 컨트롤 돌면서
            foreach(DDataSet ds in DLogMan.DataSetList)
            {
                foreach(UAmcRecvTable amc in ds.GetAmcRecvForm())
                {
                    amc.RefreshCellColor();
                }
            }
        }

        public void InitListView(int nLogType)
        {
            dataGridView_ColorAmcRcv.Rows.Clear();
            nCurLogType = nLogType;
            switch (nLogType)
            {
                case 0: // amc recv
                    {
                        for (eAmcRcvIdx idx = 0; idx < eAmcRcvIdx.COUNTOF; idx++)
                        {
                            int nIdx = (int)idx;
                            string[] strArr = new string[2 + DViewColor.COLOR_CNT]; // Color 색 + 2개 열 (헤더항목 + 조건식)
                            strArr[0] = DAmcRcvRowItem.GetHeaderName(idx, false);       // 헤더 이름 추가
                            strArr[1] = DAmcRcv.CellColor[nIdx].strCondition;       // 조건식 추가

                            // 열 삽입
                            int nPos = dataGridView_ColorAmcRcv.Rows.Add(strArr);
                            dataGridView_ColorAmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;

                            // DViewColor에서 값을 가져와서 그리드 색을 칠해준다
                            for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                                dataGridView_ColorAmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = DAmcRcv.CellColor[nIdx].colorList[i];
                        }
                        break;
                    }

                case 1: // user
                {
                    DViewColor[] objArr = DUserLog.CellColor;
                    for (eUserLogIdx idx = 0; idx < eUserLogIdx.COUNTOF; idx++)
                    {
                        string[] strArr = new string[2 + DViewColor.COLOR_CNT];
                        strArr[0] = DUserLogRowItem.GetHeaderName(idx, false);
                        strArr[1] = objArr[(int)idx].strCondition;
                        int nPos = dataGridView_ColorAmcRcv.Rows.Add(strArr);

                        dataGridView_ColorAmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;
                        for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                        {
                            dataGridView_ColorAmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = objArr[(int)idx].colorList[i];
                        }
                    }
                    break;
                }
                case 2: // BCR
                    {
                        DViewColor[] objArr = DBcrLog.CellColor;
                        for (eBcrLogIdx idx = 0; idx < eBcrLogIdx.COUNTOF; idx++)
                        {
                            string[] strArr = new string[2 + DViewColor.COLOR_CNT];
                            strArr[0] = DBcrLog.GetHeaderName(idx, false);
                            strArr[1] = objArr[(int)idx].strCondition;
                            int nPos = dataGridView_ColorAmcRcv.Rows.Add(strArr);

                            dataGridView_ColorAmcRcv.Rows[nPos].Cells[0].Style.BackColor = Color.LightGray;

                            for (int i = 0; i < DViewColor.COLOR_CNT; i++)
                            {
                                dataGridView_ColorAmcRcv.Rows[nPos].Cells[i + 2].Style.BackColor = objArr[(int)idx].colorList[i];
                            }
                        }
                        break;
                    }
            }
            
        }
        

        // 현재 탭을 기준으로 그리드와 DViewColor 얻어오기
        private bool GetCurrentGrid_Cell(out DataGridView grid, out DViewColor[] cellColorArr)
        {
            if (tabControl1.SelectedIndex == 0)
            {
                grid = dataGridView_ColorAmcRcv;
                cellColorArr = DAmcRcv.CellColor;
                return true;
            }
            else if (tabControl1.SelectedIndex == 1)
            {
                grid = null;
                cellColorArr = DUserLog.CellColor;
                return true;
            }
            else if (tabControl1.SelectedIndex == 2)
            {
                grid = null;
                cellColorArr = DBcrLog.CellColor;
                return true;
            }

            grid = null;
            cellColorArr = null;
            return false;
        }

        // Edit 버튼 눌렀을 때
        private void button_Edit_Click(object sender, EventArgs e)
        {
            toolStripMenu_Edit_Click(sender, e);
        }

        // Edit 버튼 눌렀을 때
        private void toolStripMenu_Edit_Click(object sender, EventArgs e)
        {
            DataGridView grid;
            DViewColor[] cellColorArr;
            if (GetCurrentGrid_Cell(out grid, out cellColorArr) == false)   // 현재 탭을 기준으로 그리드와 DViewColor 얻어오기
                return;

            if (grid.SelectedCells.Count < 1)
                return;

            for(int i=0; i< grid.SelectedCells.Count-1; i++)
            {
                if(grid.SelectedCells[i].ColumnIndex != grid.SelectedCells[i+1].ColumnIndex)
                {
                    MessageBox.Show("편집은 같은 속성끼리 가능합니다. 서로 다른 열을 선택할 수 없습니다.");
                    return;
                }
            }
            int nCol = grid.SelectedCells[0].ColumnIndex;
            
            switch(nCol)
            {
                case 0:         // 열 제목. 편집 불가
                    return;
                case 1:         // 조건문

                    UViewColor_Input dlg = new UViewColor_Input();
                    if (dlg.ShowDialog() != DialogResult.OK)
                        return;

                    string strCond = strCond = dlg.strCondition;

                    // 선택된 모든 cell을 순회하면서 적용한다
                    for (int i = 0; i < grid.SelectedCells.Count; i++)
                    {
                        int nRow = grid.SelectedCells[i].RowIndex;

                        cellColorArr[nRow].ClearCondition();
                        string strErrMsg = "";

                        // 조건문 삽입. 오류가 있을 경우 에러 메시지 보이기
                        if (cellColorArr[nRow].AddCondition(strCond, out strErrMsg) == false)
                        {
                            MessageBox.Show(strErrMsg, "조건문 에러", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }
                        grid.Rows[nRow].Cells[nCol].Value = strCond;    // 에러 없으면 grid에도 해당 조건 표시
                    }
                    break;

                default:        // 2~DViewColor.COLOR_CNT 까지 색정의.
                    
                    // 색 다이얼로그 보이고 선택받기
                    if (colorDialog1.ShowDialog() != DialogResult.OK)
                        return;
                    
                    // 선택된 셀들 모두 해당 색으로 세팅
                    for (int i = 0; i < grid.SelectedCells.Count; i++)
                    {
                        int nRow = grid.SelectedCells[i].RowIndex;

                        cellColorArr[nRow].colorList[nCol - 2] = colorDialog1.Color;
                        grid.Rows[nRow].Cells[nCol].Style.BackColor = colorDialog1.Color;
                        //grid.Rows[nRow].Cells[nCol].Selected = false;
                    }
                    
                    break;
            }
            
        }

        private void dataGridView_ColorAmcRcv_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl(dataGridView_ColorAmcRcv);
                context_Color.Show(dataGridView_ColorAmcRcv, mousePos);
            }
        }

        private void UViewColor_FormClosing(object sender, FormClosingEventArgs e)
        {
            //e.Cancel = true;
            //this.Visible = false;
        }
    }
}
