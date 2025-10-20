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
using System.IO.Compression;    //압축 해제용
using System.Threading;
using MyTools;

namespace LogViewer
{
    public partial class UMainFiles : Form
    {
        public UMain parent = null;

        private enum eImgList
        {
            OpenedFolder = 0
          , ClosedFolder
          , File
         , OpenedFolder_Comment
          , ClosedFolder_Comment
        }

        List<string> m_fileList = new List<string>();
        List<string> m_zipfileList = new List<string>();
        UNotebook m_notebook = new UNotebook();
        UUserLogTable m_userLog = new UUserLogTable();
        UAmcRecvTable m_amcLog = new UAmcRecvTable();
        string m_lastOpenFolderPath = "";

        public UMainFiles()
        {
            InitializeComponent();
            this.treeView_FileList.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_FileList_AfterSelect);
            this.treeView_FileList.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_FileList_NodeMouseDoubleClick);
            this.treeView_FileList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView_FileList_KeyDown);


            this.treeView_FileList.ImageList = imageList1;
            m_notebook.Dock = DockStyle.Fill;
            /// 몇개의 미리보기 화면을 화면에 미리 붙여놓는다
            /// 
            // notebook 화면 붙이기
            this.splitContainer2.Panel2.Controls.Add(m_notebook.Controls[0]);

            // userlog 붙이기
            this.splitContainer2.Panel2.Controls.Add(m_userLog.Controls[0]);

            // amclog 붙이기
            this.splitContainer2.Panel2.Controls.Add(m_amcLog.Controls[0]);

        }

        public void ChangePreviewForm(eLogType logType)
        {
            for (int i = 0; i < this.splitContainer2.Panel2.Controls.Count; i++)
            {
                if (logType == eLogType.UserLog)
                    this.splitContainer2.Panel2.Controls[i].Visible = (i == 1); // 1번째가 userlog preview화면
                else if (logType == eLogType.AmcRcv)
                    this.splitContainer2.Panel2.Controls[i].Visible = (i == 2); // 1번째가 userlog preview화면
                else
                    this.splitContainer2.Panel2.Controls[i].Visible = (i == 0); // 0번째가 notebook preview화면
            }


        }

        /// <summary>
        /// ZIP 파일 압축 해제하기
        /// </summary>
        /// <param name="zipFilePath">ZIP 파일 경로</param> // 얘도 노필요일듯
        /// <param name="extractFolder">압축 해제 저장 폴더</param>
        public void ExtractZIPFile()        //압축 해제       string zipFilePath, string extractFolder
        {
            m_zipfileList.Clear(); //사용전 초기화
            GMethod.GetAllFiles_inDir(main_path, ".ZIP", ref m_zipfileList);  //'.zip'이 들어간 파일만 모두 가져와서 저장

            // 폴더에 있는 압축 파일 전체 확인
            foreach (string fileFullPath in m_zipfileList)
            {
                try  //압축 풀다가 오류가 발생하면 catch문으로
                {
                    using (ZipArchive zip = ZipFile.OpenRead(fileFullPath))
                    {
                        //foreach (ZipArchiveEntry entry in zip.Entries) //앞축 파일 안에있는 파일개수만큼 확인
                        for (int i = 0; i <= zip.Entries.Count; i++)
                        {
                            ZipArchiveEntry entry;
                            entry = zip.Entries[i];

                            try
                            {
                                //string zipname = fileFullPath.Substring(0, fileFullPath.Length - 8);
                                //string filePath = MyTools.GMethod.GetLastDir(fileFullPath);  //Path.Combine(extractFolder, entry.FullName);
                                string folderPath = Path.GetDirectoryName(fileFullPath) + @"\" + entry.Name; //Path.Combine(zipname, entry.FullName)
                                                                                                             //string strLastDir = MyTools.GMethod.GetLastDir(fileFullPath);
                                FileInfo fileInfo = new FileInfo(folderPath);


                                if (!fileInfo.Exists)
                                    entry.ExtractToFile(folderPath); //같은 파일이 존재하지 않을때에만 압축파일 지정한 폴더에 풀기
                            }
                            catch (Exception) { }

                        }
                    }
                }
                catch (Exception) { }
                
            }
        }

        void RefreshFileList()
        {
            treeView_FileList.BeginUpdate();
            treeView_FileList.Nodes.Clear();
            //listView_LoadedFolder.Items.Clear();
            UMain.SetTitle(m_lastOpenFolderPath);

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
                    string dirPath = Path.GetDirectoryName(fileFullPath); ;
                    bool bCommentExists = File.Exists(dirPath + @"\@Comment.txt");
                    nodeDir.ImageIndex = bCommentExists ? (int)eImgList.ClosedFolder_Comment : (int)eImgList.ClosedFolder;
                    nodeDir.SelectedImageIndex = bCommentExists ? (int)eImgList.OpenedFolder_Comment : (int)eImgList.OpenedFolder;
                    nodeDir.Tag = dirPath;
                    int nPos = treeView_FileList.Nodes.Add(nodeDir);

                    TreeNode nodeFile = new TreeNode(strFileName);
                    nodeFile.ImageIndex = (int)eImgList.File;
                    nodeFile.SelectedImageIndex = (int)eImgList.File;
                    nodeFile.Tag = fileFullPath;
                    treeView_FileList.Nodes[nPos].Nodes.Add(nodeFile);
                }
            }


            //listView_LoadedFolder.Items.Clear();

            int nCnt = 0;
            // 다 추가하고 난 다음에, 
            //Loading 된 폴더 목로에서 검색해서 로딩 된 폴더면 삭제하고 로딩된 폴더 목록으로 옮긴다
            for (int i = treeView_FileList.Nodes.Count - 1; i >= 0; i--)
            {
                string path = (string)treeView_FileList.Nodes[i].Tag;
                // 로딩된 폴더가 아니면 Skip
                if (DLogMan.Find(path) == null)
                    continue;

                // 로딩된 폴더면 tree에서 삭제하고 아래 로딩 목록으로 가져온다
                treeView_FileList.Nodes.RemoveAt(i);
            }

            listView_LoadedFolder.Items.Clear();
            for (int i = 0; i < DLogMan.DataSetList.Count; i++)
            {
                string[] arr = new string[2];
                arr[0] = String.Format("{0}", nCnt + 1);
                arr[1] = String.Format("{0}", GMethod.GetLastDirAndFileName(DLogMan.DataSetList[i].folderPath));
                ListViewItem lvi = new ListViewItem(arr);
                lvi.Tag = DLogMan.DataSetList[i].folderPath;
                nCnt++;

                // 마지막에 열었던 폴더는 표시해주면 유용하다
                if (DLogMan.DataSetList[i].folderPath == m_lastOpenFolderPath)
                    lvi.BackColor = Color.GreenYellow;

                listView_LoadedFolder.Items.Add(lvi);
            }
            treeView_FileList.EndUpdate();
            //labelTotalFileCnt.Text = String.Format("Total File : {0}", m_fileList.Count);
        }

        string lastPath = "";
        private void btn_SetRoot_Click(object sender, EventArgs e)
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

            //파일 불러오기 전 압축을 먼저 풀기
            ExtractZIPFile();

            m_fileList.Clear();
            GMethod.GetAllFiles_inDir(main_path, ".LOG", ref m_fileList);

            DLogVerConv.init();
            // 로그 버전 변환. 구버전 일때만
            foreach (string file in m_fileList)
                DLogVerConv.LogVer2Conv(file);

            RefreshFileList();
        }

        /// <summary>
        /// 파일리스트에서 특정 파일 더블클릭 했을 때 해당 파일을 열수 있도록 유도해준다
        /// </summary>
        private void treeView_FileList_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode node = treeView_FileList.SelectedNode;
            if (node == null)
                return;

            string path = (string)node.Tag;
            if (String.IsNullOrEmpty(path))
                return;

            if (File.Exists(path) == false)
                return;

            this.label1.Text = "Preview: " + MyTools.GMethod.GetLastDirAndFileName(path);

            // user log 인경우 table로 보여주기
            if (Path.GetFileNameWithoutExtension(path).ToUpper().IndexOf("USER2") >= 0)
            {
                m_userLog.UserLogFileOpen(path, null);
                ChangePreviewForm(eLogType.UserLog);
            }
            //else if (Path.GetFileNameWithoutExtension(path).ToUpper().IndexOf("AMC_RECV2") >= 0)
            //{
            //    m_amcLog.InitGrid();
            //    m_amcLog.OpenFile(path);

            //    ChangePreviewForm(eLogType.UserLog);      // 시간 겁나 오래걸림
            //}
            else
            {
                m_notebook.FileOpen(path);
                ChangePreviewForm(eLogType.DiagManager);    // 아무거나 던지자..
            }

            return;

        }

        private void treeView_FileList_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode node = treeView_FileList.SelectedNode;

            if (node == null)
            {
                textBox_Path.Text = "";
                return;
            }

            if (String.IsNullOrEmpty((string)node.Tag))
            {
                textBox_Path.Text = "";
                return;
            }

            string path = (string)node.Tag;
            // This is a folder : 좌측 트리에서 폴더가 선택되었을 때
            if (!File.Exists(path))
            {
                textBox_Path.Text = path;
                return;
            }

            // 좌측 트리뷰에서 파일이 선택되었을 때
            textBox_Path.Text = Path.GetDirectoryName(path);

        }


        private void btn_openFolder_Click(object sender, EventArgs e)
        {
            string arg = "/select, \"" + textBox_Path.Text;

            if (MessageBox.Show("선택된 파일 경로를 탐색기에서 보시겠습니까?", "", MessageBoxButtons.OKCancel) == DialogResult.OK)
                System.Diagnostics.Process.Start("explorer.exe", arg);
        }


        //static DDataSet m_DatasetRef = null;
        //private static UMainFiles m_ThisRef = null;
        private string main_path = "";
        //private static void LoadLogFolder_Thread()
        //{
        //    if (m_ThisRef == null)
        //        return;
        //
        //    //m_ThisRef.Invoke(m_ThisRef.delegateLoad, m_ThisRef.textBox_Folder.Text);
        //    m_ThisRef.Load_Log();
        //}

        private DDataSet Load_Log(string folderPath)
        {
            DDataSet ds = DLogMan.CreateDataSet(folderPath);

            if (ds != null)
            {
                ds.ErrMsg = "";
                ds.Load();
            }

            return ds;

        }

        Thread m_Thread = null;
        /// 로그 로딩 버튼
        /// 
        private void btn_Load_Click(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(textBox_Path.Text))
            {
                MessageBox.Show("로딩할 로그가 없습니다.");
                return;
            }

            if (DLogMan.DataSetList.Count > 20)
            {
                MessageBox.Show("로딩 할 수 있는 최대 개수를 넘었습니다. 일부 폴더를 언로딩 해주세요.");
                return;
            }


            // 스레드로 로딩하기
            //m_ThisRef = this;
            //m_DatasetRef = null;

            //LogLoad_Thread();
            //if (m_Thread != null)
            //{
            //    if(m_Thread.Join(1) == false)
            //        m_Thread.Abort();
            //}

            DProgress myProgress = new DProgress(this);
            myProgress.StartProgress(5000);

            DDataSet ds = Load_Log(textBox_Path.Text);
            if (ds == null)
            {
                myProgress.EndProgress();
                MessageBox.Show("로딩실패");
                return;
            }

            ds.InitUI();
            myProgress.EndProgress();

            if (String.IsNullOrEmpty(ds.ErrMsg) == false)
                MessageBox.Show("로딩실패\n\n" + ds.ErrMsg);
            textBox_Path.Text = "";
            //m_Thread = new Thread(LoadLogFolder_Thread);
            //m_Thread.Priority = ThreadPriority.Highest;
            //m_Thread.Start();
            //progressBar1.Visible = true;
            //// 로딩 스레드 끝날 때 까지 기다린다
            //while (m_Thread.Join(1) == false)
            //{
            //    if (progressBar1.Value < 99)
            //        progressBar1.Value+=2;
            //}
            //m_Thread = null;
            //
            //progressBar1.Visible = false;
            //if (m_DatasetRef == null)
            //{
            //    MessageBox.Show("로딩실패");
            //    return;
            //}

            //UMain mainForm = UMain.GetThis();
            // 로딩 완료! ************************************

            //m_DatasetRef.InitUI();

            //m_DatasetRef = null;

            //// EventLog 폼 생성
            //UEventLog eventform = new UEventLog();
            //eventform.InitGrid(ref m_DatasetRef);
            //eventform.MakeEventLog(m_DatasetRef.UserLog.EventChangeList.ToArray());

            //// LogForm 양식으로 설정
            //DLogForm logForm = new DLogForm();
            //logForm.form = eventform;
            //logForm.type = DLogForm.FormType.eEventLog;


            //DLogMan.SetVisibleFrame(m_DatasetRef);

            // 툴바의 로그 리스트 콤보박스 Refresh


            RefreshFileList();
        }



        private void treeView_FileList_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                treeView_FileList_NodeMouseDoubleClick(null, null);
        }

        // 언로딩 버튼
        private void btn_Unloading_Click(object sender, EventArgs e)
        {
            //try
            //{
            UMain mainForm = UMain.GetThis();

            foreach (ListViewItem lvi in listView_LoadedFolder.SelectedItems)
            {
                string path = (string)lvi.Tag;
                DLogMan.DestroyLogFrame(path);

                for (int i = mainForm.panel1.Controls.Count - 1; i >= 0; i--)
                {
                    if ((string)mainForm.panel1.Controls[i].Tag == path)
                    {
                        mainForm.panel1.Controls[i].Tag = null;
                        mainForm.panel1.Controls.RemoveAt(i);
                    }
                }
            }



            //}
            //catch (Exception ee)
            //{
            //    MessageBox.Show("로그 제거중 에러 발생\n\n" + ee.Message);
            //}

            RefreshFileList();
        }

        // 로딩된 폴더 우클릭 처리
        private void listView_LoadedFolder_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl((Control)sender);
                contextMenuStrip_LoadedFolder.Show((Control)sender, mousePos);
            }
        }

        //
        private void 탐색기에서열기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (listView_LoadedFolder.SelectedItems.Count != 1)
            {
                MessageBox.Show("실패");
                return;
            }

            string path = (string)listView_LoadedFolder.SelectedItems[0].Tag;

            if (String.IsNullOrEmpty(path))
            {
                MessageBox.Show("실패");
                return;
            }

            string arg = "/select, \"" + path;
            System.Diagnostics.Process.Start("explorer.exe", arg);
        }

        private void listView_LoadedFolder_MouseDoubleClick(object sender, MouseEventArgs e)
        {

            if (listView_LoadedFolder.SelectedItems.Count != 1)
                return;

            string path = (string)listView_LoadedFolder.SelectedItems[0].Tag;

            if (ViewFolderForm(path) == false)
            {
                MessageBox.Show("폴더 열기 실패");
                return;
            }

            m_lastOpenFolderPath = path;
            RefreshFileList();
        }

        // 해당 폴더가 로딩 되어 있다면 해당 폴더가 열린 폼을 보여준다
        public bool ViewFolderForm(string path)
        {
            UMain mainForm = UMain.GetThis();

            int nFoundIdx = -1;
            for (int i = 1; i < mainForm.panel1.Controls.Count; i++)
            {
                string strFolderPath = (string)mainForm.panel1.Controls[i].Tag; // Control Tag에 폴더 Path가 있기를 기대
                if (String.IsNullOrEmpty(strFolderPath))
                    continue;

                if (strFolderPath == path)
                {
                    nFoundIdx = i;
                    break;
                }
            }

            // 해당 폴더가 안열려 있으면 false
            if (nFoundIdx < 0)
                return false;

            // 해당 폼만 Visible로 하고 나머진 다 hide
            for (int i = 0; i < mainForm.panel1.Controls.Count; i++)
            {
                mainForm.panel1.Controls[i].Visible = (i == nFoundIdx);
            }

            return true;
        }

        // 로딩된 폴더에서 엔터쳤을 때 폴더 창 보이기
        private void listView_LoadedFolder_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                listView_LoadedFolder_MouseDoubleClick(sender, null);
            }
        }

        private void UMainFiles_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_Thread != null)
                m_Thread.Abort();
        }
    }
}