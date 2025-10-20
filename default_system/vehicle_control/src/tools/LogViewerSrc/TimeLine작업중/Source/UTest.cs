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
namespace LogViewer
{
    public partial class UTest : Form
    {
        List<int> m_SelectedItems = new List<int>();
        public UTestEdit m_dlgTestEdit = new UTestEdit();

        public static string m_strDefaultIconName = "DefualtIcon.png";
        public UTest()
        {
            InitializeComponent();
            LoadItems();


        }

        /// <summary>
        /// ListView 에항목들 m_TestList 에서 로딩해서 아이콘 항목들 표시
        /// </summary>
        public void LoadItems()
        {
            listView_testItem.Items.Clear();
            ImageList imgList = new ImageList();
            listView_testItem.LargeImageList = imgList;
            listView_testItem.SmallImageList = imgList;

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
            //        strIconPath = iconFolder + m_strDefaultIconName;
            //        if (File.Exists(strIconPath) == false)
            //            bmp = new Bitmap(30, 30);   
            //    }

            //    if(bmp == null)
            //        bmp = (Bitmap)Image.FromFile(strIconPath, true);

            //    // 어쨌든 아이콘은 item 한개당 하나씩 무조건 생성해서 item 번호를 image index로 만들어야 생각하기 쉽다
            //    imgList.Images.Add(bmp);
            //    tmp.ImageIndex = imgList.Images.Count - 1;
            //    tmp.Tag = item.Descript;    // 아이콘 테그에 해당 설명을 넣는다

            //    listView_testItem.Items.Add(tmp);
            //}
        }
        
        private void buttonEdit_Click(object sender, EventArgs e)
        {
            m_dlgTestEdit.ShowDialog();
        }

        private void button_Start_Click(object sender, EventArgs e)
        {
            //string msg = "";
            //int nFailCnt = 0;
            //foreach (ListViewItem item in listView_testItem.SelectedItems)
            //{
            //    m_SelectedItems.Add(item.Index);
            //    if(Program.MainForm.m_TestList.ItemList[item.Index].DoTest() == false)
            //    {
            //        nFailCnt++;
            //        msg += "검사 실패 : " + item.Text + "\n";
            //    }
            //}

            //if(nFailCnt == 0)
            //{
            //    MessageBox.Show("성공~!");
            //}
            //else if (m_SelectedItems.Count == nFailCnt)
            //{
            //    MessageBox.Show("모든 검사 실패!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            //    return;
            //}
            //else
            //{
            //    MessageBox.Show(String.Format("{0} 개 검사중 {1}개 완료, {2}개 실패.", m_SelectedItems.Count, m_SelectedItems.Count - nFailCnt, nFailCnt), "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            //}
            
            this.Close();    // 검사 끝났으니 창 닫는다
        }

        private void listView_testItem_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nCnt = listView_testItem.SelectedItems.Count;
            if (nCnt > 0)
                textBox_Description.Text = (string)listView_testItem.SelectedItems[nCnt - 1].Tag;
        }
    }


}
