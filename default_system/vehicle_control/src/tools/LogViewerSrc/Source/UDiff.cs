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
using System.Runtime.CompilerServices;
using System.Diagnostics;
namespace Diff
{
    public partial class UDiff : Form
    {
        public List<string> m_FileList = new List<string>();
        public List<DDiffResult> m_ResultList = new List<DDiffResult>();
        public List<DIniCompResult> m_IniResultList = new List<DIniCompResult>();
		public string WinmergePath = "";

		public int m_nGiJunNo = -1;
        public bool m_bInitilized = false;
		public static UDiff thisObj = null;
		public UDiff()
        {
            InitializeComponent();
			WinmergePath = Application.StartupPath + @"\reference\Winmerge\winmergeu.exe";

			this.listView_fileList.DragEnter += new DragEventHandler(Form1_DragEnter);
            this.listView_fileList.DragDrop += new DragEventHandler(Form1_DragDrop);
            progressBar1.Visible = false;
			thisObj = this;

			Reset();
        }

        // Filename15.ini 일 경우 15라는 숫자를 추출해냄
        public static bool GetFileNameLastNum(string fileNameWoExt, out int nVal)
        {
            string strVal = "";
            for (int i = 0; i < fileNameWoExt.Length; i++)
            {
                string sz = fileNameWoExt.Substring(i, 1);
                int n = 0;
                if (Int32.TryParse(sz, out n))
                {
                    strVal += sz;
                }
            }

            if (String.IsNullOrEmpty(fileNameWoExt))
            {
                nVal = 0;
                return false;
            }

            return Int32.TryParse(strVal, out nVal);
        }
        public void ListSort_FileName(ref List<DIniCompResult> list)
        {
            for(int k=0; k<list.Count; k++)
            {
                for (int r = k+1; r < list.Count; r++)
                {
                    
                    string str1 = Path.GetFileNameWithoutExtension(list[k].strTarget);
                    string str2 = Path.GetFileNameWithoutExtension(list[r].strTarget);
                    int n1 = 0;
                    int n2 = 0;
                    if (GetFileNameLastNum(str1, out n1) == false)
                        continue;
                    if (GetFileNameLastNum(str2, out n2) == false)
                        continue;

                    if (n1 > n2)
                    {
                        DIniCompResult tmp = list[k];
                        list[k] = list[r];
                        list[r] = tmp;
                    }
                }
            }
            
        }

        public List<DIniCompResult> ListSort(List<DIniCompResult> list)
        {
            List<DIniCompResult> diffList = new List<DIniCompResult>();
            List<DIniCompResult> sameList = new List<DIniCompResult>();
            
            foreach (DIniCompResult obj in list)
            {
                if (obj.bDiff)
                    diffList.Add(obj);
                else
                    sameList.Add(obj);
            }

            ListSort_FileName(ref diffList);
            ListSort_FileName(ref sameList); 

            List<DIniCompResult> outList = new List<DIniCompResult>();
            foreach(DIniCompResult obj in diffList)
                outList.Add(obj);

            foreach (DIniCompResult obj in sameList)
                outList.Add(obj);

            return outList;
        }
        public void ListSort_FileName(ref List<DDiffResult> list)
        {
            for (int k = 0; k < list.Count; k++)
            {
                for (int r = k + 1; r < list.Count; r++)
                {

                    string str1 = Path.GetFileNameWithoutExtension(list[k].strTarget);
                    string str2 = Path.GetFileNameWithoutExtension(list[r].strTarget);
                    int n1 = 0;
                    int n2 = 0;
                    if (GetFileNameLastNum(str1, out n1) == false)
                        continue;
                    if (GetFileNameLastNum(str2, out n2) == false)
                        continue;

                    if (n1 > n2)
                    {
                        DDiffResult tmp = list[k];
                        list[k] = list[r];
                        list[r] = tmp;
                    }
                }
            }

        }

        public List<DDiffResult> ListSort(List<DDiffResult> list)
        {
            List<DDiffResult> diffList = new List<DDiffResult>();
            List<DDiffResult> sameList = new List<DDiffResult>();

            foreach (DDiffResult obj in list)
            {
                if (obj.bDiff)
                    diffList.Add(obj);
                else
                    sameList.Add(obj);
            }

            ListSort_FileName(ref diffList);
            ListSort_FileName(ref sameList);

            List<DDiffResult> outList = new List<DDiffResult>();
            foreach (DDiffResult obj in diffList)
                outList.Add(obj);

            foreach (DDiffResult obj in sameList)
                outList.Add(obj);

            return outList;
        }
        void Form1_DragEnter(object sender, DragEventArgs e) { if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.Copy; }

        // file 끌어다 놨을 때 이벤트
        void Form1_DragDrop(object sender, DragEventArgs e)
        {
            if (m_bInitilized == false)
                return;

            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            Array.Sort(files, StringComparer.InvariantCultureIgnoreCase);

            AddFileList(files);
        }
        public void AddFileList(string []files)
        {

            foreach (string file in files)
            {
                if (Directory.Exists(file))  // this is a folder
                {
                    // 폴더를 만나면 내부 파일 목록을 끌어간다. 하위 폴더까지 신경쓰지 않음
                    string[] fileEntries = Directory.GetFiles(file);
                    Array.Sort(fileEntries, StringComparer.InvariantCultureIgnoreCase);

                    foreach (string file_inDir in fileEntries)
                        AddFile_Validation(file_inDir);
                }
                else if (File.Exists(file))             // this is a file
                {
                    string strTmp = Path.GetFileName(file).ToUpper();
                    AddFile_Validation(file);
                }
                else
                    continue;  // exception 케이스..
            }

            RefreshFileList();
        }
        public void AddFile_Validation(string path)
        {
            // filter check
            if (String.IsNullOrEmpty(textBox_FileFilter.Text) == false)
            { 
                string fileFileter = textBox_FileFilter.Text.ToUpper();
                string fileName = Path.GetFileName(path);
                fileName = fileName.ToUpper();
                string strTmp = path.ToUpper();

                if (!fileName.Contains(fileFileter))       //file filter 처리,  대소문자 구별 안함.
                    return;
            }
            // duplication check
            int nIdx = m_FileList.FindIndex(o => o == path);
            if (nIdx >= 0)
                return;
            
            m_FileList.Add(path);
        }
        int m_nDiffCnt = 0;
        public void RefreshFileList(bool bAsResult = false)
        {
            listView_fileList.Items.Clear();
            m_nDiffCnt = 0;
            // 단순 파일 목록 보여줄 때######################################################################
            if (bAsResult == false)
            {
                for (int i = 0; i < m_FileList.Count; i++)
                {
                    string[] arr = new string[4];
                    arr[0] = String.Format("{0}", i + 1);       // No
                    arr[1] = i == m_nGiJunNo  ? "기준파일" : "";    // Diff 결과
                    arr[2] = m_FileList[i]; // Path

                    ListViewItem lvi = new ListViewItem(arr);
                    if (i == m_nGiJunNo)
                        lvi.BackColor = Color.YellowGreen;

                    listView_fileList.Items.Add(lvi);
                }
                return;
            }

            // INI 의미 비교 옵션 결과 일 때 ###################################
            if(checkBox_INIComp.Checked)
            {                
                m_IniResultList = ListSort(m_IniResultList);
                
                for (int i = 0; i < m_IniResultList.Count; i++)
                {
                    DIniCompResult diffObj = m_IniResultList[i];

                    string[] arr = new string[4];
                    arr[0] = String.Format("{0}", i + 1);   // No
                    arr[1] = diffObj.bGijun ? "기준파일" : (diffObj.bDiff ? "다름" : "같음");    // Diff 결과
                    arr[2] = diffObj.strTarget;     // Path

                    if(diffObj.bDiff)
                        m_nDiffCnt++;

                    ListViewItem lvi = new ListViewItem(arr);
                    if (diffObj.bGijun)
                        lvi.BackColor = Color.YellowGreen;
                    if (diffObj.bDiff)
                        lvi.BackColor = Color.LightPink;

                    listView_fileList.Items.Add(lvi);
                }
                
                return;
            }

            m_ResultList = ListSort(m_ResultList);

            //m_ResultList.Sort(delegate (DDiffResult a, DDiffResult b)
            //{
            //    if (a.bDiff && b.bDiff) return 0;
            //    else if (a.bDiff && !b.bDiff) return -1;
            //    else if (!a.bDiff && b.bDiff) return 1;
            //    else return 0;
            //});

            // 단순 검사
            // 검사 결과 보여줄 때###################################
            for (int i = 0; i < m_ResultList.Count; i++)
            {
                DDiffResult diffObj = m_ResultList[i];

                string[] arr = new string[4];
                arr[0] = String.Format("{0}", i + 1);   // No
                arr[1] = diffObj.bGiJun ? "기준파일" : (diffObj.bDiff ? "다름" : "같음");    // Diff 결과
                arr[2] = diffObj.strTarget;     // Path

                if (diffObj.bDiff)
                    m_nDiffCnt++;

                ListViewItem lvi = new ListViewItem(arr);
                if (diffObj.bGiJun)
                    lvi.BackColor = Color.YellowGreen;
                if(diffObj.bDiff)
                    lvi.BackColor = Color.LightPink;

                listView_fileList.Items.Add(lvi);
            }
        }

        private void btn_load_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.ValidateNames = false;
            dlg.CheckFileExists = false;
            dlg.CheckPathExists = true;
            dlg.Multiselect = true;
            
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            AddFileList(dlg.FileNames);
            
        }


        private int m_nProgressPos = 0;
        private void btn_run_Click(object sender, EventArgs e)
        {

            if(m_nGiJunNo < 0 || m_nGiJunNo >= m_FileList.Count)
            {
                MessageBox.Show("기준 파일을 선택해야 합니다.\n\n우클릭으로 기준 선택 가능.");
                return;
            }

            
            m_bInitilized = false;
            progressBar1.Visible = true;
            progressBar1.Value = 0;
            progressBar1.Minimum = 0;
            progressBar1.Maximum = m_FileList.Count;
            splitContainer2.Enabled = false;
           // do
           // {

                // 스레드로 로딩하기
                var th = new Thread(Thread_DoDiff);
                th.Priority = ThreadPriority.Highest;
                th.Start();

                textBox_org.Text = "";
                textBox_target.Text = "";

                listView_org.Items.Clear();
                listView_target.Items.Clear();
                m_ResultList.Clear();

                // 로딩 스레드 끝날 때 까지 기다린다
                while (th.Join(1) == false)
                {
                    progressBar1.Value = m_nProgressPos;
                }

                // 이유는 모르지만 결과가 0나올떄가 있을수있다?
           // } while (m_FileList.Count != m_ResultList.Count);
            // Diff 끝남 ************

            progressBar1.Visible = false;
            splitContainer2.Enabled = true;
            RefreshFileList(true);
            MessageBox.Show(String.Format("다른 파일 {0}개" ,m_nDiffCnt));
        }

        private static void Thread_DoDiff()
        {
			thisObj.DoDiff();
        }

        private void DoDiff()
        {
            if(checkBox_INIComp.Checked)
            {
                m_IniResultList.Clear();
                string org = m_FileList[m_nGiJunNo];
                
                for (int i = 0; i < m_FileList.Count(); i++)
                {
                    DIniCompResult diff = new DIniCompResult();

                    // Diff 실행
                    diff.DoDiff(m_FileList[i]);
                    m_IniResultList.Add(diff);
                    m_nProgressPos = i;
                }
            }
            else
            {
                m_ResultList.Clear();
                string org = m_FileList[m_nGiJunNo];

                // 기준파일 세팅
                if (DDiffResult.SetGiJun(org) == false)
                    return;

                for (int i = 0; i < m_FileList.Count(); i++)
                {
                    DDiffResult diff = new DDiffResult();

                    // Diff 실행
                    diff.DoDiff(m_FileList[i]);
                    m_ResultList.Add(diff);
                    m_nProgressPos = i;
                }
            }
            
        }
        

        // 좌측 파일리스트에서 특정 항목 더블클릭
        private void listView_fileList_DoubleClick(object sender, EventArgs e)
        {
            btn_CompWinmerge_Click(null, null);
        }

        private void 탐색기에서해당위치열기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (listView_fileList.SelectedItems.Count != 1)
                    return;

                string arg = "/select, \"" + listView_fileList.SelectedItems[0].SubItems[2].Text + "\"";
                System.Diagnostics.Process.Start("explorer.exe", arg);
            }
            catch (Exception) { }
        }

        private void 메모장에서열기ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (listView_fileList.SelectedItems.Count != 1)
                    return;
                
                System.Diagnostics.Process.Start("notepad.exe", listView_fileList.SelectedItems[0].SubItems[2].Text);
            }
            catch (Exception) { }
        }
        // 좌측 파일리스트에서 우클릭 팝업 처리
        private void listView_fileList_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = GetMousePos_onControl(listView_fileList);
                contextMenuStrip1.Show(listView_fileList, mousePos);
            }
        }
        public static Point GetMousePos_onControl(Control ctrl)
        {
            Rectangle rt = ctrl.RectangleToScreen(ctrl.ClientRectangle);
            return new Point(Control.MousePosition.X - rt.X, Control.MousePosition.Y - rt.Y);
        }
        private void btn_Reset_Click(object sender, EventArgs e)
        {
            Reset();
        }
        private void Reset()
        {
            m_bInitilized = true;
            DDiffResult.strOrg = "";
            DIniCompResult.iniA.sectionList.Clear();
            DIniCompResult.iniA.filePath = "";
            DIniCompResult.iniA.bLoaded = false;
            //m_FileList.Clear();
            m_nGiJunNo = -1;
            m_nProgressPos = 0;
            m_ResultList.Clear();
            listView_fileList.Items.Clear();
            listView_org.Items.Clear();
            listView_target.Items.Clear();
            textBox_org.Text = "";
            textBox_target.Text = "";
            textBox_FileFilter.Text = "";
            progressBar1.Visible = false;
            progressBar1.Value = 0;
            m_ResultList.Clear();
            RefreshFileList();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            btn_load.Enabled = m_bInitilized;
            btn_run.Enabled = m_FileList.Count > 1;
            btn_Gijun.Enabled = m_bInitilized && listView_fileList.SelectedItems.Count == 1;
            //btn_CompWinmerge.Enabled = listView_fileList.SelectedItems.Count == 1 && String.IsNullOrEmpty(DDiffResult.strOrg) == false;
        }

        private void listView_fileList_SelectedIndexChanged(object sender, EventArgs e)
        {
            listView_org.Items.Clear();
            listView_target.Items.Clear();
            textBox_org.Text = "[기준파일] ";
            textBox_target.Text = "[비교파일] ";
            if (m_bInitilized)
                return;

            if (listView_fileList.SelectedItems.Count != 1)
                return;

            int nIdx = listView_fileList.SelectedItems[0].Index;

            // INI 의미 비교 결과인 경우 **********************************************************************
            if (checkBox_INIComp.Checked)
            {
                DIniCompResult diffObj = null;
                try
                {
                    diffObj = m_IniResultList[nIdx];
                    if (diffObj == null)
                        return;

                    if (diffObj.bDiff == false)
                        return;
                }
                catch (Exception) { return; }

                textBox_org.Text = "[기준파일] " + DIniCompResult.iniA.filePath;
                textBox_target.Text = "[비교파일] " + diffObj.strTarget;
                
                
                
                foreach(string str in diffObj.strDiffLineA)
                {
                    string[] arr = new string[2];
                    arr[0] = "";
                    arr[1] = str;

                    ListViewItem lvi = new ListViewItem(arr);
                    listView_org.Items.Add(lvi);
                }
                foreach (string str in diffObj.strDiffLineB)
                {
                    string[] arr = new string[2];
                    arr[0] = "";
                    arr[1] = str;

                    ListViewItem lvi = new ListViewItem(arr);
                    listView_target.Items.Add(lvi);
                }
                return;
            }
            else // 일반 비교인 경우 ***********************************************************
            {   
                DDiffResult diffObj = null;
                try
                {
                    diffObj = m_ResultList[nIdx];
                    if (diffObj == null)
                        return;

                    if (diffObj.bDiff == false)
                        return;
                }
                catch (Exception) { return; }

                textBox_org.Text = "[기준파일] " + DDiffResult.strOrg;
                textBox_target.Text = "[비교파일] " + diffObj.strTarget;
                listView_org.Items.Clear();
                listView_target.Items.Clear();

                if (diffObj.bToMuchDiff)
                {
                    string[] arr = new string[2];
                    arr[1] = diffObj.diffMsg;
                    ListViewItem lvi = new ListViewItem(arr);
                    listView_org.Items.Add(lvi);

                    arr = new string[2];
                    arr[1] = diffObj.diffMsg;
                    lvi = new ListViewItem(arr);
                    listView_target.Items.Add(lvi);
                    return;
                }

                int nCnt = diffObj.DiffLineList.Count;
                if (nCnt > 0)
                {
                    string[] arrTarget = null;

                    try
                    {
                        arrTarget = File.ReadAllLines(diffObj.strTarget);
                    }
                    catch (Exception exp)
                    {
                        string[] arr = new string[2];
                        arr[1] = exp.Message;
                        ListViewItem lvi = new ListViewItem(arr);
                        listView_org.Items.Add(lvi);

                        arr = new string[2];
                        arr[1] = exp.Message;
                        lvi = new ListViewItem(arr);
                        listView_target.Items.Add(lvi);
                        return;
                    }

                    foreach (int nLine in diffObj.DiffLineList)
                    {
                        if (DDiffResult.linesOrg == null)
                        {
                            DDiffResult.SetGiJun(DDiffResult.strOrg);
                            if (DDiffResult.linesOrg == null)
                                return;
                        }
                        if (nLine < DDiffResult.linesOrg.Length)
                        {
                            string[] arr = new string[2];
                            arr[0] = String.Format("{0}", nLine + 1);
                            arr[1] = DDiffResult.linesOrg[nLine];

                            ListViewItem lvi = new ListViewItem(arr);
                            listView_org.Items.Add(lvi);
                        }

                        if (nLine < arrTarget.Length)
                        {
                            string[] arr = new string[2];
                            arr[0] = String.Format("{0}", nLine + 1);
                            arr[1] = arrTarget[nLine];

                            ListViewItem lvi = new ListViewItem(arr);
                            listView_target.Items.Add(lvi);
                        }
                    }
                }
                // 특별한 메시지가 있으면 해당 메시지만 뿌린다
                if (String.IsNullOrEmpty(diffObj.diffMsg) == false)
                {
                    string[] arr = new string[2];
                    arr[0] = "";
                    arr[1] = diffObj.diffMsg;
                    ListViewItem lvi = new ListViewItem(arr);
                    lvi.BackColor = Color.YellowGreen;

                    ListViewItem lvi2 = new ListViewItem(arr);
                    lvi2.BackColor = Color.YellowGreen;

                    listView_org.Items.Add(lvi);
                    listView_target.Items.Add(lvi2);
                    return;
                }
            }
        }

        private void 기준지정ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if(listView_fileList.SelectedItems.Count != 1)
            {
                MessageBox.Show("한개의 파일을 선택한 후 기준을 지정해 주세요.");
                return;
            }

            if(checkBox_INIComp.Checked)
            {
                DIniCompResult.iniA.bLoaded = false;
                DIniCompResult.iniA.sectionList.Clear();
                DIniCompResult.iniA.filePath = "";

                UDiffIni dlg = new UDiffIni();
                int nIdx = listView_fileList.SelectedItems[0].Index;
                dlg.strPath = listView_fileList.Items[nIdx].SubItems[2].Text;
                dlg.ShowDialog();
                if (dlg.DialogResult != DialogResult.OK)
                    return;
                m_nGiJunNo = nIdx;
                RefreshFileList();
            }
            else
            {
                int nIdx = listView_fileList.SelectedItems[0].Index;
                m_nGiJunNo = nIdx;
                RefreshFileList();
            }
            
        }
        
        private void 삭제ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            List<int> selectedArr = new List<int>();
            foreach(ListViewItem lv in listView_fileList.SelectedItems)
            {
                selectedArr.Add(lv.Index);
            }

            selectedArr.Sort();
            
            for(int i=selectedArr.Count()-1; i>=0; i--)
            {
                int nIdx = selectedArr[i];
                m_FileList.RemoveAt(nIdx);
            }

            RefreshFileList();
        }
        

        private void listView_fileList_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;

            if (e.KeyCode == Keys.Delete)
            {
                삭제ToolStripMenuItem_Click(null, null);
            }
            else if(e.KeyCode == Keys.C && bControl)
            {

              
                string str = "";
                foreach (ListViewItem lvi in listView_fileList.SelectedItems)
                {
                    str += lvi.SubItems[2].Text + "\n";
                }
                if (String.IsNullOrEmpty(str) == false)
                    Clipboard.SetText(str);
                
            }
        }

        private void btn_Gijun_Click(object sender, EventArgs e)
        {
            기준지정ToolStripMenuItem_Click(null, null);
        }

        private void btn_usage_Click(object sender, EventArgs e)
        {
            UDiff_Help dlg = new UDiff_Help();
            dlg.ShowDialog();
        }

        private void 모두삭제ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_FileList.Clear();
            RefreshFileList();
        }

        private void UMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(MessageBox.Show("DIFF를 종료할까요?", "Quit", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                e.Cancel = true;
                return;
            }
        }

        private void btn_CompWinmerge_Click(object sender, EventArgs e)
        {
            try
            {
                string strWinmergepath = Application.StartupPath+ WinmergePath;
                string strRight = listView_fileList.SelectedItems[0].SubItems[2].Text;
                string strLeft = checkBox_INIComp.Checked ? DIniCompResult.iniA.filePath :  DDiffResult.strOrg;

                if(File.Exists(strWinmergepath) == false)
                {
                    MessageBox.Show("Winmerge 모듈이 없어서 실행할 수 없습니다.");
                    return;
                }
                if(File.Exists(strRight) == false)
                {
                    MessageBox.Show("대상 파일이 없음.\n\n" + strRight);
                    return;
                }
                if (File.Exists(strLeft) == false)
                {
                    MessageBox.Show("기준 파일이 없음.\n\n" + strLeft);
                    return;
                }
                string arg = String.Format("\"{0}\" \"{1}\"", strLeft, strRight);
                Process wm = Process.Start(strWinmergepath, arg);
            }
            catch (Exception ex)
            {
                MessageBox.Show("대상 파일을 지정해야합니다.\n\n");
            }
        }
        
        private void listView_org_KeyDown(object sender, KeyEventArgs e)
        {
            try
            {

                ListView listView = (ListView)sender;

                bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
                if (e.KeyCode == Keys.C && bControl)
                {
                    string str = "";
                    foreach (ListViewItem lvi in listView.SelectedItems)
                    {
                        str += lvi.SubItems[1].Text + "\n";
                    }
                    if (String.IsNullOrEmpty(str) == false)
                        Clipboard.SetText(str);
                }
            }
            catch (Exception)
            {

                //throw;
            }
        }
    }

    // 비교결과 리포팅 데이터
    public class DDiffResult
    {
        
        public string strTarget = "";
        public bool bDiff = false;
        public string diffMsg = "";
        public List<int> DiffLineList = new List<int>();
        public bool bGiJun = false;
        public bool bToMuchDiff = false;        // 달라도 너무 다른경우. 1Kb 이상 다르다거나.

        public static string strOrg = "";
        public static byte[] byteOrg = null;
        public static string[] linesOrg = null;
        public static bool IsOrgBinary = false;
        public static FileInfo OrgInfo = null;
        public DDiffResult() { }

        public static bool SetGiJun(string org)
        {
            strOrg = "";
            byteOrg = null;
            IsOrgBinary = false;
            linesOrg = null;

            try
            {
                strOrg = org;
                OrgInfo = new FileInfo(org);
                byteOrg = File.ReadAllBytes(org);
                IsOrgBinary = false;//IsBinary(byteOrg, byteOrg.Length);
                linesOrg = File.ReadAllLines(org);
            }
            catch (Exception exp)
            {
                MessageBox.Show("기준파일 읽기 실패.\n\n", exp.Message);
                return false;
            }
            return true;
        }

        // 비교 알고리즘 실행
        public void DoDiff(string target)
        {
            strTarget = target;

            if(strOrg == strTarget)
            {
                bGiJun = true;
                return;
            }

            byte[] byte_Target = null;
            bool bBinary_Target = false;

            try
            {
                FileInfo info = new FileInfo(target);
                long nDiffSize = OrgInfo.Length - info.Length;
                if (Math.Abs(nDiffSize) > 1024)
                {
                    bDiff = true;       // 1Kb 이상 다를 경우 별다른 비교 하지 않는다.
                    bToMuchDiff = true;
                    diffMsg = "1KB 이상 파일 크기가 다릅니다.";
                    return;
                }
                byte_Target = File.ReadAllBytes(target);
                bBinary_Target = false;//IsBinary(byte_Target, byte_Target.Length);
            }
            catch (Exception exp)
            {
                bDiff = true;       // 파일 읽기 실패나는 경우. 의외로 간간히 생김
                diffMsg = exp.Message;
                return;
            }

            // 바이너리가 안맞을 경우 바로 Diff
            if(IsOrgBinary != bBinary_Target)
            {
                bDiff = true;
                diffMsg = bBinary_Target ? "Target이 Text 파일이 아닙니다." : "Target이 Binary 파일이 아닙니다.";
                return;
            }
            
            // text file 일 경우 ***********
            string[] lines_target = null;

            try
            {   lines_target = File.ReadAllLines(target, Encoding.UTF8);
            }
            catch (Exception exp2)
            {
                bDiff = true;
                diffMsg = exp2.Message;
                return;
            }
            bool bLineDiff = linesOrg.Length != lines_target.Length;
            if (bLineDiff)
            {
                bDiff = true;
                diffMsg = "전체 라인 수가 다름.";
            }

            int nCnt = Math.Min(linesOrg.Length, lines_target.Length);

            for(int i=0; i< nCnt; i++)
            {
                // 해당 줄 비교 이렇게 해야지 그냥 스트링으로 비교하면 한글의 경우 비교가 안됨
                bool bDiffLine = linesOrg[i] != lines_target[i];
                
                if(bDiffLine)
                {
                    if(DiffLineList.Count() > 10)
                    {
                        diffMsg = String.Format("Diff Line ({0})개 넘음", DiffLineList.Count());
                        return;
                    }
                    DiffLineList.Add(i);
                    bDiff = true;
                }
            }

            if(bDiff == false)
            {
                if (byteOrg.Length != byte_Target.Length)
                {
                    bDiff = true;
                    diffMsg = "Binary 파일 용량이 다릅니다.";
                    return;
                }

                // 바이너리 비교
                int nLen = byteOrg.Length;
                for (int i = 0; i < nLen; i++)
                {
                    if (byteOrg[i] != byte_Target[i])
                    {
                        bDiff = true;
                        diffMsg = "서로 다른 Binary 파일입니다.";
                        return;
                    }
                }
            }

        }
        private static bool IsBinary(byte[] bytes, int maxLength)
        {
            int len = maxLength > 1024 ? 1024 : maxLength;

            int nonASCIIcount = 0;

            for (int i = 0; i < len; ++i)
                if (bytes[i] > 127)
                    ++nonASCIIcount;

            // if the number of non ASCII is more than a 50%
            // then is a binary file. 
            return (nonASCIIcount / len) > 0.3;
        }
    }


    public class DIniCompResult
    {
        public static DIni iniA = new DIni();     // 기준 ini

        public DIni iniB = new DIni();  // 비교 target ini
        public string strErrMsg = "";
        public bool bDiff = false;
        public bool bGijun = false;
        public string strTarget = "";
        public List<string> strDiffLineA = new List<string>();
        public List<string> strDiffLineB = new List<string>();
        public void Init()
        {
            iniA = new DIni();
            iniB = new DIni();
        }
        public bool SetGijun(string path)
        {
            bool bRet = iniA.LoadINI(path);
            if(bRet == false)
                strErrMsg = "기준 INI 로딩 실패";

            return bRet;
        }
        public bool DoDiff(string target)
        {
            strTarget = target;
            if (iniA.bLoaded == false)
            {
                strErrMsg = "기준 INI 로딩 실패";
                return false;
            }

            if (DIniCompResult.iniA.filePath == strTarget)
            {
                bGijun = true;
                return true;
            }

            if (iniB.LoadINI(target) == false)
            {
                strErrMsg = "INI 로딩 실패.  " + target;
                return false;
            }

            bDiff = false;
            // elem flag 초기화
            iniA.ElemFlagInit();
            iniB.ElemFlagInit();

            // CompIni하고 나면 element flag에 결과가 남는다
            iniA.CompIni(iniB);
            
            // strDiffLineA 에 기준 INI에서 다른점들 기록
            foreach (IniSection sec in iniA.sectionList)
            {
                foreach(IniElem elem in sec.elemList)
                {
                    if(elem.bFlag == false)
                    {
                        string diffMsg = String.Format("[{0}] {1} = {2}", sec.name, elem.name, elem.val);
                        strDiffLineA.Add(diffMsg);
                    }
                }
            }
            
            // strDiffLineB 에 Target INI에서 다른점들 기록
            foreach (IniSection sec in iniB.sectionList)
            {
                foreach (IniElem elem in sec.elemList)
                {
                    if (elem.bFlag == false)
                    {
                        string diffMsg = String.Format("[{0}] {1} = {2}", sec.name, elem.name, elem.val);
                        strDiffLineB.Add(diffMsg);
                    }
                }
            }

            bDiff = strDiffLineA.Count + strDiffLineB.Count > 0;
            return bDiff;
        }
    }
}
