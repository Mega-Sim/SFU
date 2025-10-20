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
using System.Threading;
using MyTools;

namespace LogViewer
{
    public partial class UManualTest : Form
    {
        private string lastPath = "";
        private string main_path = "";
        private List<string> m_fileList = new List<string>();
        private DManualTest m_ManualTest = null;
        private UUserLogTable m_UserLogForm = null;
        private UAmcRecvTable m_AmcLogForm = null;


        private enum eImgList
        {
            OpenedFolder = 0
          , ClosedFolder
          , File
        }

        public UManualTest()
        {
            InitializeComponent();

            m_ManualTest = new DManualTest();
            m_UserLogForm = new UUserLogTable();
            m_AmcLogForm = new UAmcRecvTable();
            m_UserLogForm.InitGrid();
            
            CreateResultGrid();
            splitContainer_Amc_User.Panel1.Controls.Add(m_UserLogForm.Controls[0]);
            splitContainer_Amc_User.Panel2.Controls.Add(m_AmcLogForm.Controls[0]);

            this.treeView_FileList.ImageList = imageList1;
        }

        private void UManualTest_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_ManualTest.m_Thread != null)
                m_ManualTest.m_Thread.Abort();

            //e.Cancel = true;
            //this.Hide();
        }
        private void CreateResultGrid()
        {
            TDrawingControl.SetDoubleBuffered(dataGridView1);
			dataGridView1.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			// User Grid 초기화 ****************************************************************
			TDrawingControl.SuspendDrawing(dataGridView1);
            dataGridView1.DefaultCellStyle.Font = new Font("맑은 고딕", 9);
            dataGridView1.RowPostPaint += DataGridView_RowPostPaint;
            dataGridView1.ColumnCount = (int)eManualTest.COUNTOF;
            dataGridView1.ColumnHeadersHeight = 20;

            for (eManualTest i = 0; i < eManualTest.COUNTOF; i++)
            {
                dataGridView1.Columns[(int)i].Name = i.ToString();
                dataGridView1.Columns[(int)i].HeaderText = i.ToString();

                dataGridView1.Columns[(int)i].DefaultCellStyle.Alignment = i == eManualTest.Time ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
                //  dataGridView1.Columns[(int)i].DefaultCellStyle.WrapMode = DataGridViewTriState.True;    // 셀 안에 개행문자 가능하게 함
                //   dataGridView1.Columns[(int)i].SortMode = DataGridViewColumnSortMode.NotSortable; // 헤더로 정렬되면 큰일

                dataGridView1.Columns[(int)i].Width = 170;
            }

          

            dataGridView1.RowTemplate.Height = 20;

            //UGridOption.LoadGridInfo(dataGridView_defUser);
            TDrawingControl.ResumeDrawing(dataGridView1);
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
        private void btn_FolderLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            if (lastPath != "")
                dlg.InitialDirectory = lastPath;
            dlg.ValidateNames = false;
            dlg.CheckFileExists = false;
            dlg.CheckPathExists = true;
            dlg.FileName = "Folder_Select";

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            lastPath = Path.GetDirectoryName(dlg.FileName);

            main_path = Path.GetDirectoryName(dlg.FileName);
            m_fileList.Clear();

            GMethod.GetAllFiles_inDir(main_path, "AMC_Recv20", ref m_fileList);
            GMethod.GetAllFiles_inDir(main_path, "USER20", ref m_fileList);

            RefreshFileList();
        }

        void RefreshFileList()
        {
            treeView_FileList.BeginUpdate();
            treeView_FileList.Nodes.Clear();

            // 루드에서 모드 전체 파일&폴더 검색
            foreach (string fileFullPath in m_fileList)
            {
                MyTools.GMethod.GetLastDirAndFileName(fileFullPath);
                string strLastDir = MyTools.GMethod.GetLastDir(fileFullPath);
                string strFileName = Path.GetFileName(fileFullPath);

                try
                {
                    long length = new System.IO.FileInfo(fileFullPath).Length;
                    if (length > 0)
                    {
                        double dTotalFileSizeMB = Math.Max(Math.Round((double)length / 1000000.0, 1), 0.1);
                        strFileName += String.Format("  [{0:#,###0.0}Mb]", dTotalFileSizeMB);
                    }
                }
                catch (Exception)
                {
                }

                TreeNode nodeDir = new TreeNode(strLastDir);
                bool bFound = false;
                foreach (TreeNode node in treeView_FileList.Nodes)
                {
                    if (node.Text == nodeDir.Text)
                    {
                        bFound = true;

                        TreeNode nodeFile = new TreeNode(strFileName);
                        nodeFile.ImageIndex = (int)eImgList.File;
                        nodeFile.SelectedImageIndex = (int)eImgList.File;
                        nodeFile.Tag = fileFullPath;    // tag에 경로를 넣어두면 나중에 판별하기 쉬움
                        node.Nodes.Add(nodeFile);
                    }
                }
                // 해당하는 폴더를 못찾았으면 폴더 만든다. 폴더 시작
                if (!bFound)
                {
                    nodeDir.ImageIndex = (int)eImgList.ClosedFolder;
                    nodeDir.SelectedImageIndex = (int)eImgList.OpenedFolder;
                    nodeDir.Tag = Path.GetDirectoryName(fileFullPath);
                    int nPos = treeView_FileList.Nodes.Add(nodeDir);

                    TreeNode nodeFile = new TreeNode(strFileName);
                    nodeFile.ImageIndex = (int)eImgList.File;
                    nodeFile.SelectedImageIndex = (int)eImgList.File;
                    nodeFile.Tag = fileFullPath;
                    treeView_FileList.Nodes[nPos].Nodes.Add(nodeFile);
                }
            }


            treeView_FileList.EndUpdate();
            //labelTotalFileCnt.Text = String.Format("Total File : {0}", m_fileList.Count);
        }
        private void btn_Clear_Click(object sender, EventArgs e)
        {
            treeView_FileList.Nodes.Clear();
        }

        private bool bStarted = false;

        private void btn_Run_Click(object sender, EventArgs e)
        {
            if (bStarted)    // 취소하기
            {
                m_ManualTest.m_bCancel = true;
                return;
            }

            bStarted = true;
            if (String.IsNullOrEmpty(textBox_tolerance_msec.Text))
                textBox_tolerance_msec.Text = "300";
            m_ManualTest.m_nTolerance_msec = Convert.ToInt32(textBox_tolerance_msec.Text);
            m_ManualTest.Start(m_fileList, true);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (m_ManualTest.IsFinished() && bStarted)
            {
                bStarted = false;
                DrawResultTable();    // 결과 테이블 그리기
                progressBar1.Hide();
                btn_FolderLoad.Enabled = true;
                btn_Run.Text = "Run";
            }
            else if (bStarted)
            {
                btn_FolderLoad.Enabled = false;
                btn_Run.Text = "Cancel";
                progressBar1.Maximum = m_ManualTest.m_nTotal;
                progressBar1.Minimum = 0;
                progressBar1.Value = Math.Min(progressBar1.Maximum, m_ManualTest.m_nCurPos);
                progressBar1.Show();
            }
        }

        // 결과 테이블 그리기
        public void DrawResultTable()
        {
            TDrawingControl.SuspendDrawing(dataGridView1);

            dataGridView1.Rows.Clear();

            foreach (sManualRowList one in m_ManualTest.m_Result)
            {
                string[] strRow = new string[(int)eManualTest.COUNTOF];
                for (int i = 0; i < (int)eManualTest.COUNTOF; i++)
                {
                    strRow[i] = Convert.ToString(one.summary.subItems[i]);

                    if (i == (int)eManualTest.Folder)
                        strRow[i] = Path.GetFileName(strRow[i]);    // 최상위 폴더만 표시
                    //else if (i == (int)eManualTest.AmcFile || i == (int)eManualTest.UserFile)
                    //    strRow[i] = Path.GetFileNameWithoutExtension(strRow[i]);        //파일 이름만 표시
                    else if (i == (int)eManualTest.Time)
                    {
                        DateTime tm = one.summary.subItems[i];
                        SmallDateTime smallTm = new SmallDateTime(tm);
                        strRow[i] = smallTm.GetTimeString();
                    }
                    else if (i == (int)eManualTest.Elapse)
                    {
                        double dVal = one.summary.subItems[i];
                        strRow[i] = String.Format("{0:0.000}", dVal);   // elapse 소수점 양식 예쁘게
                    }
                }

                int nRowIdx = dataGridView1.Rows.Add(strRow);

                // 테그에는 원본 객체를 그대로 넣는다. 나중에 폴더 열거나 파일 열때, 편하게 하기 위함
                dataGridView1.Rows[nRowIdx].Cells[(int)eManualTest.Folder].Tag = one.summary.subItems[(int)eManualTest.Folder];
                //dataGridView1.Rows[nRowIdx].Cells[(int)eManualTest.AmcFile].Tag = one.summary.subItems[(int)eManualTest.AmcFile];
                //dataGridView1.Rows[nRowIdx].Cells[(int)eManualTest.UserFile].Tag = one.summary.subItems[(int)eManualTest.UserFile];
                dataGridView1.Rows[nRowIdx].Cells[(int)eManualTest.Time].Tag = one.summary.subItems[(int)eManualTest.Time];

            }
            TDrawingControl.ResumeDrawing(dataGridView1);
        }

        public void ShowAmcUserLog(sManualRowList item)
        {
            //m_UserLogForm.ShowTable_byDataRow(table.UserData.ToArray());
            //m_AmcLogForm.ShowTable_byRowArr(table.AmcData.ToArray(), table.amcFilePath);

            int nTimeGap_Before = 3;
            int nTimeGap_After = 3;

            Int32.TryParse(textBox_beforeMargin_sec.Text, out nTimeGap_Before);
            Int32.TryParse(textBox_AfterMargin_sec.Text, out nTimeGap_After);

            DateTime tmOrg = item.summary.subItems[(int)eManualTest.Time];
            DateTime tmFrom = tmOrg.AddSeconds((-1) * nTimeGap_Before);

            double dElpase = item.summary.subItems[(int)eManualTest.Elapse];
            dElpase += nTimeGap_Before + nTimeGap_After;
            dElpase = Math.Max(nTimeGap_Before + 3, dElpase);

            //string amcPath = item.summary.subItems[(int)eManualTest.AmcFile];
            //string userPath = item.summary.subItems[(int)eManualTest.UserFile];

            DDataSet dataSet = new DDataSet();
            //dataSet.UserLog.LoadFile_timeZone(userPath, tmFrom, (int)dElpase);
            //dataSet.AmcRcv.LoadFile_timeZone(amcPath, tmFrom, (int)dElpase);

            m_AmcLogForm.m_Parent = dataSet;
            m_UserLogForm.m_Parent = dataSet;
            m_AmcLogForm.InitGrid();
            m_UserLogForm.dataGridView_defUser.Rows.Clear();
            //m_AmcLogForm.ShowTable(dataSet.AmcRcv.table, tmOrg, amcPath);
            m_UserLogForm.ShowTable();

            dataSet.ClearAll();     // show 만하고 삭제
            tabControl1.SelectedIndex = 1;
        }

        private void dataGridView1_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (dataGridView1.SelectedCells.Count == 1)
            {
                int nRow = dataGridView1.SelectedCells[0].RowIndex;
                ShowAmcUserLog(m_ManualTest.m_Result[nRow]);
            }
        }

        private void dataGridView1_UserDeletingRow(object sender, DataGridViewRowCancelEventArgs e)
        {
            if (MessageBox.Show("선택한 행을 삭제할까요?", "", MessageBoxButtons.OKCancel) != DialogResult.OK)
                e.Cancel = true;
        }
    }
}
