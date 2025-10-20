using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LogViewer
{
    public partial class USetUploadPath : Form
    {
        public string m_strVhlIP = "";
        public string path = "";
        List<string> recentPathList = new List<string>();

        public USetUploadPath()
        {
            InitializeComponent();
        }
        private void USetUploadPath_Load(object sender, EventArgs e)
        {
            LoadINI();
        }
        public void LoadINI()
        {
            MyTools.IniMan ini = new MyTools.IniMan();
            ini.Path = UMain.INI_PATH;

            // 기본적으로 저장된 path
            string tmpVal = ini.Read("DataManager", "UploadPath_Def_Cnt", "0");
            int nDefCnt = 0;
            Int32.TryParse(tmpVal, out nDefCnt);
            for(int i=0; i<nDefCnt; i++)
            {
                tmpVal = ini.Read("DataManager", String.Format("UploadPath_Def_{0}", i), "");
                if(String.IsNullOrEmpty(tmpVal) == false)
                    comboBox1.Items.Add(tmpVal);
            }

            // 사용자가 한번 입력한 path. 나중에 몇개 개이상되면 사라짐, 오래된것부터 사라짐
            tmpVal = ini.Read("DataManager", "UploadPath_Recent_Cnt", "0");
            int nRecentCnt = 0;
            Int32.TryParse(tmpVal, out nRecentCnt);
            for (int i = 0; i < nRecentCnt; i++)
            {
                tmpVal = ini.Read("DataManager", String.Format("UploadPath_Recent_{0}", i), "");
                if (String.IsNullOrEmpty(tmpVal) == false)
                {
                    comboBox1.Items.Add(tmpVal);
                    recentPathList.Add(tmpVal);
                }

            }
        }
      
        private void button_Cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            Close();
        }

        private void btn_Apply_Click(object sender, EventArgs e)
        {
            if(String.IsNullOrEmpty(comboBox1.Text) || String.IsNullOrWhiteSpace(comboBox1.Text))
            {
                MessageBox.Show("경로를 입력해 주세요.");
                return;
            }
            if(comboBox1.Text.IndexOf('\\') < 0)
            {
                MessageBox.Show("잘못된 경로입니다.");
                return;
            }

			if(comboBox1.Text.Substring(comboBox1.Text.Length-1, 1) == "\\")
			{
				comboBox1.Text = comboBox1.Text.Substring(0, comboBox1.Text.Length-1);
			}

			AddPath(comboBox1.Text);
            path = comboBox1.Text;
            this.DialogResult = DialogResult.OK;
            Close();
        }

        bool AddPath(string path)
        {
            bool bExists = false;
            foreach (string item in comboBox1.Items)
            {
                if (item == comboBox1.Text)
                {
                    bExists = true;
                    break;
                }
            }

            if (bExists)
                return false;
       

            comboBox1.Items.Add(comboBox1.Text);

            recentPathList.Add(comboBox1.Text);
            if (recentPathList.Count > 10)  // 최대 10개 유지하면서 한개씩 땡기면서 먼저추가된 항목 지우기
            {
                for (int i = 1; i < recentPathList.Count; i++)
                    recentPathList[i - 1] = recentPathList[i];

                recentPathList.RemoveAt(recentPathList.Count - 1);
            }

            // INI save
            MyTools.IniMan ini = new MyTools.IniMan();
            ini.Path = UMain.INI_PATH;

            ini.Write("DataManager", "UploadPath_Recent_Cnt", Convert.ToString(recentPathList.Count));
            
            for(int i=0; i< recentPathList.Count; i++)
            {
                ini.Write("DataManager", String.Format("UploadPath_Recent_{0}", i), recentPathList[i]);
            }

            return true;
        }
       
    }
}
