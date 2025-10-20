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
using System.IO;
namespace LogViewer
{
    public partial class UTestEdit : Form
    {
        public UTestEdit()
        {
            InitializeComponent();
            RefreshList();
        }
        

        private void RefreshList()
        {
            listView1.Items.Clear();

            listView1.Items.Clear();
            ImageList imgList = new ImageList();
            listView1.LargeImageList = imgList;
            listView1.SmallImageList = imgList;

            string iconFolder = Application.StartupPath + @"\Pics\";

            // Test List INI에서 로딩된 정보대로 Listview 아이콘 꾸며준다
            //
            //foreach (DTestItem item in Program.MainForm.m_TestList.ItemList)
            //{
            //    ListViewItem tmp = new ListViewItem(item.Name);

            //    // 아이콘 설정된게 있으면 로딩
            //    string strIconPath = iconFolder + item.IconName;
            //    Bitmap bmp = null;
            //    if (File.Exists(strIconPath) == false)// 기본 아이콘은 무조건 있어야 되는건데 누가 지웠으면 그냥 new bitmap
            //    {
            //        strIconPath = iconFolder + UTest.m_strDefaultIconName;
            //        if (File.Exists(strIconPath) == false)
            //            bmp = new Bitmap(30, 30);
            //    }

            //    if (bmp == null)
            //        bmp = (Bitmap)Image.FromFile(strIconPath, true);

            //    // 어쨌든 아이콘은 item 한개당 하나씩 무조건 생성해서 item 번호를 image index로 만들어야 생각하기 쉽다
            //    imgList.Images.Add(bmp);
            //    tmp.ImageIndex = imgList.Images.Count - 1;
            //    tmp.Tag = item.Descript;    // 아이콘 테그에 해당 설명을 넣는다

            //    listView1.Items.Add(tmp);
            //}
            
        }
        // 새로 만들거나 수정할때 쿼리나 기타등등, 항목들 값에 문제 없는지 확인
        private bool ValidationCheck()
        {
            /// 쿼리 테스트******** 
            //DTableAmcRcv tmpTable = new DTableAmcRcv();
            //DRowItem_AmcRcv row = new DRowItem_AmcRcv();
            //tmpTable.OrgTable.Rows.Add(row.subItems);
            //if (String.IsNullOrEmpty(textBox_query.Text))
            //{
            //    MessageBox.Show("쿼리를 입력하세요.", "Validation Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return false;
            //}
            //if (textBox_query.Text.Contains('\n') && textBox_query.Text.Contains(','))
            //{
            //    MessageBox.Show("여러 쿼리를 입력할 때는 콤마 혹은 줄바꿈으로 구별하세요. 둘다 사용할수 없습니다.", "Validation Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return false;
            //}

            //string[] quries;

            //if (textBox_query.Text.Contains('\n'))
            //    quries = textBox_query.Text.Split('\n');
            //else if (textBox_query.Text.Contains(','))
            //    quries = textBox_query.Text.Split(',');
            //else
            //{
            //    return tmpTable.DoQuery(textBox_query.Text) == null ? false : true;
            //}

            //bool bRet = true;
            //foreach(string q in quries)
            //    bRet &= tmpTable.DoQuery(q) == null ? false : true;

            //if (!bRet)
            //    return false;


            //if(String.IsNullOrEmpty(textBox_name.Text))
            //{
            //    MessageBox.Show("이름을 입력하세요.", "Validation Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return false;
            //}

            //int nGrouppingCnt = 0;
            //int nIgnoreCnt = 0;

            //if (String.IsNullOrEmpty(textBox_grouppingCnt.Text) || Int32.TryParse(textBox_grouppingCnt.Text, out nGrouppingCnt) == false || nGrouppingCnt < 0)
            //{
            //    MessageBox.Show("Grouping Tolerance Cnt 값을 입력하세요.", "Validation Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return false;
            //}
            //if (String.IsNullOrEmpty(textBox_ignoreCnt.Text) || Int32.TryParse(textBox_ignoreCnt.Text, out nIgnoreCnt) == false || nIgnoreCnt < 0)
            //{
            //    MessageBox.Show("Ignore Cnt 값을 입력하세요.", "Validation Check", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return false;
            //}

            return true;
        }

        private void button_addnew_Click(object sender, EventArgs e)
        {
            if (ValidationCheck() == false)
            {
                MessageBox.Show("현재 설정된 값이 유효하지 않아 적용할 수 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            if (MessageBox.Show(String.Format("{0} 항목을 새로 추가 하시겠습니까?", textBox_name.Text), "Question", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
                return;
            
            // 새로 추가해서 리스트에 add
            //Program.MainForm.m_TestList.ItemList.Add(new DTestItem());
            //int nIdx = Program.MainForm.m_TestList.ItemList.Count - 1;

            //SetItem(nIdx);
            //RefreshList();
        }

        /// <summary>
        /// 현재 세팅 값을 TestItem List에 nIdx 값 위치에 넣는다
        /// </summary>
        private void SetItem(int nIdx)
        {
            //try
            //{
            //    DTestItem item = Program.MainForm.m_TestList.ItemList[nIdx];
            //    item.Name = textBox_name.Text;
            //    item.IconName = textBox_iconName.Text;
            //    item.Descript = textBox_desc.Text;
            //    item.bAutoModeOnly = checkBox_AutoMode.Checked;
            //    item.ErrGroupingToleranceLineCnt = Convert.ToInt32(textBox_grouppingCnt.Text);
            //    item.IgnoreCnt= Convert.ToInt32(textBox_ignoreCnt.Text);
            //    if (textBox_query.Text.Contains('\n'))
            //        item.Quries.AddRange(textBox_query.Text.Split('\n'));
            //    else if (textBox_query.Text.Contains(','))
            //        item.Quries.AddRange(textBox_query.Text.Split(','));
            //    else
            //    {
            //        item.Quries.Add(textBox_query.Text);
            //    }
                
            //}
            //catch (Exception e)
            //{
            //    MessageBox.Show("SetItem Failed\n\n" + e.Message, "Assert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //}
            //Program.MainForm.m_TestList.SaveINI(UMain.INI_PATH);
        }

        private void button_modify_Click(object sender, EventArgs e)
        {
            int nCnt = listView1.SelectedItems.Count;
            if (nCnt <= 0)
            {
                MessageBox.Show("선택된 항목이 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if(ValidationCheck() == false)
            {
                MessageBox.Show("현재 설정된 값이 유효하지 않아 적용할 수 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            int nIdx = listView1.SelectedItems[nCnt - 1].Index;
            ListViewItem item = listView1.Items[nIdx];
            
            if (MessageBox.Show(String.Format("{0} 항목을 수정 하시겠습니까?", item.Text), "Question", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
                return;
            
            SetItem(nIdx);
           
            RefreshList();
        }

        private void button_delete_Click(object sender, EventArgs e)
        {
            //int nCnt = listView1.SelectedItems.Count;
            //if (nCnt <= 0)
            //{
            //    MessageBox.Show("선택된 항목이 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return;
            //}
            //int nIdx = listView1.SelectedItems[nCnt - 1].Index;

            //DTestItem newItem = new DTestItem();
   

            //ListViewItem item = listView1.Items[nIdx];
            //if (MessageBox.Show(String.Format("{0} 항목을 삭제하시겠습니까?", item.Text), "Question", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            //    return;

            //try
            //{
            //    Program.MainForm.m_TestList.ItemList.RemoveAt(nIdx);
            //}
            //catch
            //{
            //    Debug.Assert(false, "Remove Failed!");
            //}

            RefreshList();
        }

        private void button_IconFileDialog_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            string iconFolder = Application.StartupPath + @"\Pics\";
            dlg.InitialDirectory = iconFolder;
            dlg.Multiselect = true;

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            if(Path.GetDirectoryName(dlg.FileName) + "\\" != iconFolder)
            {
                MessageBox.Show("Pics 폴더 안에 들어있는 이미지 파일만 로딩할수 있습니다.");
                return;
            }
            if(!File.Exists(dlg.FileName))
            {
                MessageBox.Show("파일을 찾을 수 없음.");
                return;
            }
            textBox_iconName.Text = Path.GetFileName(dlg.FileName);
        }
    }
}
 