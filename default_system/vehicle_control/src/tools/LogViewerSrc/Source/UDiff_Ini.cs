using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Diff
{
    public partial class UDiffIni : Form
    {
        public string strPath = "";
        public UDiffIni()
        {
            InitializeComponent();

           
       }

        private void UIni_Load(object sender, EventArgs e)
        {
            if (DIniCompResult.iniA.LoadINI(strPath) == false)
            {
                MessageBox.Show("기준 파일 로딩 실패");
                Close();
                return;
            }

            listView1.Items.Clear();

            foreach (IniSection sec in DIniCompResult.iniA.sectionList)
            {
                foreach (IniElem elem in sec.elemList)
                {
                    string[] arr = new string[3];
                    arr[0] = sec.name;
                    arr[1] = elem.name;
                    arr[2] = elem.val;

                    ListViewItem lvi = new ListViewItem(arr);
                    lvi.Checked = true;
                    listView1.Items.Add(lvi);
                }
            }
        }
        private void btn_apply_Click(object sender, EventArgs e)
        {
            foreach(ListViewItem lvi in listView1.Items)
            {
                IniElem elem = DIniCompResult.iniA.GetElement(lvi.SubItems[0].Text, lvi.SubItems[1].Text);
                if (elem == null)
                    continue;

                elem.bSkipItem = lvi.Checked == false;
            }

            this.DialogResult = DialogResult.OK;
            Close();
        }

    }
}
