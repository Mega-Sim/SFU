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
    public partial class UCommentAttach : Form
    {
        public string m_SelectedFileName = "";
        public UCommentAttach()
        {
            InitializeComponent();
        }

        private void textBox1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            string arg = "/select, \"" + this.textBox_Path + "\"";
            System.Diagnostics.Process.Start("explorer.exe", arg);
        }

        // 창 열릴대. folder path 대로 파일 리스트 뽑고, 파일 사이즈 계산
        private void UCommentAttach_Load(object sender, EventArgs e)
        {
            if(Directory.Exists(textBox_Path.Text) == false)
            {
                MessageBox.Show("지정된 로그 폴더가 없습니다.");
                return;
            }

            string []files = Directory.GetFiles(textBox_Path.Text);
            foreach(string file in files)
            {
                string[] arr = new string[2];
                arr[0] = Path.GetFileName(file);
                long length = new System.IO.FileInfo(file).Length;
                arr[1] = String.Format("{0:#,###0}", length);
                
                ListViewItem lvi = new ListViewItem(arr);
                lvi.Tag = file;
                listView1.Items.Add(lvi);
            }
        }

        // 파일을 리스트에서 더블클릭 하면 해당 파일로 결정된 것으로 알고 창을 닫는다
        private void listView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (listView1.SelectedItems.Count != 1)
                return;

            m_SelectedFileName = listView1.SelectedItems[0].Text;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
        
        // Key Down 이벤트 처리
        private void UCommentAttach_KeyDown(object sender, KeyEventArgs e)
        {
            // ESC
            if(e.KeyCode == Keys.Escape)
            {
                this.DialogResult = DialogResult.Cancel;
                this.Close();
            }
            // Enter
            else if (e.KeyCode == Keys.Enter)
            {
                listView1_MouseDoubleClick(sender, null);
            }
        }
    }
}
