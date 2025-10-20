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
namespace LogViewer
{
    public partial class UAmcRecvCols : Form
    {
        public class DPreSet
        {
            public string name = "";
            public List<int> Items = new List<int>();
            public void Add(int n)
            {
                Items.Add(n);
            }
        }
        public class DShowItem
        {
            public string text = "";
            public object tag;
            public bool visible = true;
            public bool isChecked = false;
        }

        public List<DPreSet> presetItem = null;  
        public DShowItem[] listViewItems = null;

        IniMan ini = new IniMan();
        public UAmcRecvTable m_dlgParent;
		public int OrgWidth = 0;
        public int DataCount = 0;
        public List<string> StringArray = new List<string>();

		public UAmcRecvCols()
        {
            InitializeComponent();
            presetItem = new List<DPreSet>();
            string col;
            int InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            int OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
            int Input = (int)eAmcRcvIdx.Input1_1;
            int Output = (int)eAmcRcvIdx.Output1_1;
            DataCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;
            if(listViewItems == null)
                listViewItems = new DShowItem[DataCount];
            int count = 0;
            StringArray.Clear();
            for (eAmcRcvIdx i = 0; i< eAmcRcvIdx.COUNTOF; i++)
            {               

                string str = "";
                if (i < eAmcRcvIdx.Input1_1)
                {                        
                    col = DAmcRcvRowItem.GetHeaderName(i, asMultiLine: false);
                    col = col.Replace('\n', '.');                
                }
                else if (i < eAmcRcvIdx.Output1_1)
                {
                    int n = (int)i;
                    if ((n - Input) < InputCount)
                    {
                        col = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");                        
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
                        col = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");                        
                    }
                    else
                    {
                        continue;
                    }
                }
                
                //col = col.Replace('\n', '.');
                StringArray.Add(col);
                listViewItems[(int)count] = new DShowItem();
                listViewItems[(int)count].text = col;
                if (count == (int)eAmcRcvIdx.Time_Date)
                    listViewItems[(int)count].isChecked = false;
                else if (count >= (int)eAmcRcvIdx.AMC_Input1 && count <= (int)eAmcRcvIdx.State1)
                    listViewItems[(int)count].isChecked = false;
                else
                    listViewItems[(int)count].isChecked = true;
                //listViewItems[(int)count].isChecked = count < (int)eAmcRcvIdx.PauseStatus ? true : false;
                listViewItems[(int)count].tag = count;
                listViewItems[(int)count++].visible = true;                
            }
            
            RefreshListView();
            LoadINI(UMain.INI_PATH);
            RefreshPresetComboBox();
			OrgWidth = this.Width;
            this.TopMost = true;
        }
        public void DestroySelf()
        {
            this.textBox_Find.Dispose();
            this.m_dlgParent = null;
            this.presetItem.Clear();
            //this.Close();
            this.Dispose();
        }
        public void ShowHide(bool bVisible)
        {
            listView1.Visible = bVisible;
        }
        //bool bOnce = true;



        // 보여줄 아이템들 refresh
        void RefreshListView()
        {
            listView1.Items.Clear();

            foreach(DShowItem obj in listViewItems)
            {
                if (obj.visible == false)
                    continue;

                ListViewItem lvi = new ListViewItem(obj.text);
                lvi.Checked = obj.isChecked;
                lvi.Tag = obj.tag;
                listView1.Items.Add(lvi);
            }
        }

        void LoadINI(string path)
        {
            try
            {
                ini.Path = path;
                string strVal = ini.Read("AmcRecvColumnsPresets", "Count", "0");
                int nCnt = 0;
                if (!Int32.TryParse(strVal, out nCnt))
                    return;

                presetItem.Clear();

                for (int i = 0; i < nCnt; i++)
                {
                    string strName = ini.Read("AmcRecvColumnsPresets", String.Format("{0}_Name", i), "none");
                    string strS = ini.Read("AmcRecvColumnsPresets", String.Format("{0}_Items", i), "");
                    string[] strItems = strS.Split(',');


                    DPreSet item = new DPreSet();
                    item.name = strName.Replace("\0", "");
                    foreach (string col in strItems)
                    {
                        int n = int.Parse(col);        
                        item.Items.Add(n);

                    }
                    presetItem.Add(item);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show("INI 로딩 실패\n\n" + e.Message);
                //throw;
            }
        }

        void SaveINI(string path)
        {
            try
            {
                ini.Path = path;
                ini.Write("AmcRecvColumnsPresets", "Count", Convert.ToString(presetItem.Count));

                for (int i = 0; i < presetItem.Count; i++)
                {
                    ini.Write("AmcRecvColumnsPresets", String.Format("{0}_Name", i), presetItem[i].name);
                    string strItems = "";
                    int N = 0;
                    foreach (int col in presetItem[i].Items)
                    {
                        
                        if(presetItem[i].Items.Count-1 == N)
                        {
                            strItems += col.ToString();
                            break;
                        }
                        strItems += col.ToString() + ",";
                        N++;
                    }
                        

                    ini.Write("AmcRecvColumnsPresets", String.Format("{0}_Items", i), strItems);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show("INI 저장 실패\n\n" + e.Message);
                //throw;
            }
        }


        private int AddCurPreSet(string name)
        {
            if(String.IsNullOrEmpty(name))
            {
                MessageBox.Show("Preset 이름을 입력하세요.", "Preset 생성 실패", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return 0;
            }
            DPreSet oneset = new DPreSet();

            for (int i = 0; i < DataCount; i++)
            {
                if(listViewItems[(int)i].isChecked)
                    oneset.Add(i);
            }
            oneset.name = name;

			int nIdx = presetItem.FindIndex(o => o.name == name);
			bool b = false;
			if(nIdx >=0)	// 해당 프리셋이 이미 존재하는 경우 기존것을 삭제후 추가
			{
				presetItem.RemoveAt(nIdx);
				presetItem.Insert(nIdx, oneset);
				b = true;
			}
			else
			{	// 없는 경우 추가
				presetItem.Add(oneset);
			}
            SaveINI(UMain.INI_PATH);

			MessageBox.Show(String.Format("[{0}] 을 {1}하였습니다.", name, b ? "수정" : "추가"));
            return b ? 2 : 1;	// 수정되었으면 2, 추가되었으면 1
        }
        private void DeleteCurPreset(string str)
        {
            if (String.IsNullOrEmpty(str))
            {
                MessageBox.Show("Preset을 선택하세요.", "Preset 삭제 실패", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            bool bFound = false;
            for (int i = 0; i < presetItem.Count; i++)
            {
                if(presetItem[i].name == str)
                {
                    presetItem.RemoveAt(i);
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                MessageBox.Show("해당하는 Preset이 없습니다.", "Preset 삭제 실패", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
        }
        private void SetPreset(string str)
        {
            for (int i = 0; i < DataCount; i++)
            {
                listViewItems[(int)i].isChecked = false;
            }

            int nIdx = 0;
            bool bFound = false;
            foreach(DPreSet tmp in presetItem)
            {
                if (tmp.name == str)
                {
                    bFound = true;
                    break;
                }
                nIdx++;
            }
            if(!bFound)
            {
                MessageBox.Show(String.Format("[{0}]의 Preset 로딩 실패", str), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            int cnt = 0;
            foreach(int col in presetItem.ElementAt(nIdx).Items)
            {
                listViewItems[col].isChecked = true;
            }
			splitContainer1.Panel1.Focus();
            RefreshListView();
        }

        // 체크박스 체크했을 때
        public void listView1_ItemChecked(object sender, ItemCheckedEventArgs e)
        {
            //DoCheckToggle(e.Item.Index, e.Item.Checked);

            // OLD CODE
           e.Item.ImageIndex = e.Item.Checked ? 1 : 0;
           if (e.Item.Checked) // 체크된건 굵게 표시하기 위함
               e.Item.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
           else
               e.Item.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
           
           // checkedList 관리
           int theIdx = (int)e.Item.Tag;
           bool isChecked = e.Item.Checked;

           listViewItems[theIdx].isChecked = isChecked;

           // Gridview의 Column Show/Hide 하기
           int nMax = m_dlgParent.dataGridView1.Columns.Count;
           if(theIdx < nMax && theIdx >= 0)
               m_dlgParent.dataGridView1.Columns[theIdx].Visible = isChecked;   
        }

        public void DoCheckToggle(int nIdx, bool Checked)
        {
            listView1.Items[nIdx].Checked = Checked;
            //ListViewItem lvi = listView1.Items[nIdx];

            //lvi.ImageIndex = Checked ? 1 : 0;
            //if (Checked) // 체크된건 굵게 표시하기 위함
            //    lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            //else
            //    lvi.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));

            //// checkedList 관리
            //int theIdx = nIdx;// (int)lvi.Tag;
            //bool isChecked = Checked;

            //listViewItems[theIdx].isChecked = isChecked;

            //// Gridview의 Column Show/Hide 하기
            //int nMax = m_dlgParent.dataGridView1.Columns.Count;
            //if (theIdx < nMax && theIdx >= 0)
            //    m_dlgParent.dataGridView1.Columns[theIdx].Visible = isChecked;
        }

        //bool bOnce = true;
        private void timer1_Tick(object sender, EventArgs e)
        {
            // 로그테이블 탭을 클릭하면 show 하도록 한다
            //if(Program.MainForm.m_dlgMainTab.tabPage_LogTable.Controls.Count > 0)
            //{
            //    if (Program.MainForm.m_dlgMainTab.tabPage_LogTable.Controls[0].Visible && this.Visible == false)
            //    {
            //        // 첫 위치는 MainForm의 우상단에 표시하도록 한다
            //        if (bOnce)
            //        {
            //            bOnce = false;
            //            int nStartPosX = Program.MainForm.Location.X + Program.MainForm.Width - this.Width;
            //            this.Location = new Point(nStartPosX, Program.MainForm.Location.Y);
                        
            //        }

            //        //this.Show();
            //    }

            //}
         
            if (this.Visible == false)
                return;
            


            //string str = String.Format("Selected Items : {0} / {1}", checkedList.Count, (int)eAmcRcvIdx.COUNTOF);
            //if (toolStripStatusLabel1.Text != str)
            //    toolStripStatusLabel1.Text = str;
        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
			int nRet = AddCurPreSet(comboBox_Presets.Text);

			if (nRet > 0)
            {
				if (nRet == 1)	// 추가되면 리턴1. refesh 한다.
				{
					RefreshPresetComboBox();
					if (comboBox_Presets.Items.Count > 0)
						comboBox_Presets.SelectedIndex = comboBox_Presets.Items.Count - 1;
				}
            }
        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            DeleteCurPreset(comboBox_Presets.Text);
            RefreshPresetComboBox();
            comboBox_Presets.Text = "";
            
            ini.Write("AmcRecvColumnsPresets", "Count", "0");
            SaveINI(ini.Path);
        }
        private void RefreshPresetComboBox()
        {
			comboBox_Presets.SuspendLayout();

			comboBox_Presets.Items.Clear();
            foreach(DPreSet item in presetItem)
            {
                comboBox_Presets.Items.Add(item.name);
            }

			comboBox_Presets.ResumeLayout();
		}

        private void selectAllToolStripMenuItem_SelectAll_Click(object sender, EventArgs e)
        {
            //checkedList.Clear();
            //for (eAmcRcvIdx n = 0; n < eAmcRcvIdx.COUNTOF; n++)
            //    checkedList.Add(n);

            //RefreshShowItems();
            //comboBox_Presets.Text = "";
        }

        private void deselectAllToolStripMenuItem_DeselectAll_Click(object sender, EventArgs e)
        {
            //foreach (ListViewItem lvi in listView1.Items)
            //{
            //    lvi.Checked = false;
            //}
            //comboBox_Presets.Text = "";
        }

        // 리스트뷰에서 우클릭 팝업 리스트
        private void listView1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                contextMenuStrip_popup.Show(System.Windows.Forms.Cursor.Position);
            }
        }

        // 프리셋  변경했을 때 이벤트
        private void comboBox_Presets_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox_Find.Text = "";
            textBox1_TextChanged(sender, e);

            SetPreset(comboBox_Presets.Text);
        }
        

        // 비모달 형식으로 계속 메모리에 있도록 함
        private void ULogViewAmcRecvColumns_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
            
        }

        // find 창에 입력할때 발생됨
        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (textBox_Find.Text.Length == 0)
            {
                for (int i = 0; i < listViewItems.Length; i++)
                    listViewItems[i].visible = true;
            }
            else
            {
                for (int i = 0; i < listViewItems.Length; i++)
                {
                    string str = textBox_Find.Text.ToUpper();
                    string str2 = listViewItems[i].text.ToUpper();
                    listViewItems[i].visible = str2.Contains(str);
                }
            }
            RefreshListView();
        }
        

        // 체크된 항목만 리스트에 표시하기
        private void buttonShowOnly_Click(object sender, EventArgs e)
        {
            for(int i=0; i<(int)DataCount; i++)
            {
                listViewItems[i].visible = listViewItems[i].isChecked;
            }
            RefreshListView();
          
        }
        
        // 모두 표시
        public void buttonShowAll_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < (int)DataCount; i++)
            {
                listViewItems[i].visible = true;
            }
            RefreshListView();
        }
        

        // Key Down 이벤트 : Control A 전체선택 위함
        private void listView1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.A && e.Control)
            {
                foreach (ListViewItem lvi in listView1.Items)
                {
                    lvi.Selected = true;
                }
            }
        }

       
    }

  
}
