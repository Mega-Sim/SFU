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
using System.Reflection;


namespace VibrationManager
{
    public partial class UAnalysisInput : Form
    {
        private string CSV_path { get; set; }
        private string CSV_path2 { get; set; }
        private int Limit_Time { get; set; }
        private double Limit_Vib { get; set; }
        private double Limit_Vib_2 { get; set; }
        private double All_Average { get; set; }
        private double Differential_ { get; set; }


        public UAnalysisOutput MainForm = null;
        public UAnalysisOutput_Line MainForm_Line = null;

        public string Last_Path = "";
        public string Main_path = "C:\\";


        public UAnalysisInput()
        {
            Assembly assemObj = Assembly.GetExecutingAssembly();
            Version v = assemObj.GetName().Version; // 현재 실행되는 어셈블리..dll의 버전 가져오기

            int majorV = v.Major; // 주버전
            int minorV = v.Minor; // 부버전
            int buildV = v.Build; // 빌드번호
            int revisionV = v.Revision; // 수정번호
            string Version = "(Ver "+majorV.ToString() + '.' + minorV.ToString() + '.' + buildV.ToString() + '.' + revisionV.ToString()+')';
            this.Text = "Vibration Manager " + Version;
            InitializeComponent();
            LimitVib.Text = "0.6";            
            //cbMode.SelectedIndex = 0;
    


        }

        private void CSV_VIEW_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            int idx = CSV_VIEW.Items.Count;
            foreach (string file in files)
            {
                if (file.IndexOf(".csv") >= 0)
                {
                    CSV_VIEW.Items.Add(file);
                    FileInfo a = new FileInfo(file);
                    CSV_VIEW.Items[idx].Text = a.Name;
                    CSV_VIEW.Items[idx++].Name = file;
                }

            }

            foreach (ListViewItem listitem in CSV_VIEW.Items)
            {
                listitem.Checked = true;
            }
            

        }

        private void CSV_VIEW_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Copy;
      

        }
        private void Clear_Button(object sender, EventArgs e)
        { 
            foreach (ListViewItem listitem in CSV_VIEW.Items)
            {

                listitem.Checked = false;

            }
            CSV_VIEW.Items.Clear();            
        }

        private void Analysys_Button(object sender, EventArgs e)
        {
            //시간, 진동범위 입력         
            try
            {
                Limit_Vib = double.Parse(LimitVib.Text);
                //Limit_Vib_2 = double.Parse(LimitVib2.Text);
                string[] FileList = new string[100];
                FileList.Initialize();
                
                //if(cbMode.SelectedIndex == 0)
                //{
                    if (CSV_VIEW.CheckedItems.Count > 1)
                    {

                        for (int i = 0; i < CSV_VIEW.CheckedItems.Count; i++)
                        {
                            FileList[i] = CSV_VIEW.CheckedItems[i].Name;
                        }
                        MainForm = new UAnalysisOutput(FileList, Limit_Vib, Limit_Time);
                        MainForm.ShowDialog();
                    }
                    else
                    {
                        MessageBox.Show("데이터를 체크하시오.");
                        return;
                    }
                //}
                //else if(cbMode.SelectedIndex == 1)
                //{
                //    if (CSV_VIEW.CheckedItems.Count > 1)
                //    {

                //        for (int i = 0; i < CSV_VIEW.CheckedItems.Count; i++)
                //        {
                //            FileList[i] = CSV_VIEW.CheckedItems[i].Name;
                //        }
                //        MainForm_Line = new UAnalysisOutput_Line(FileList, Limit_Vib, Limit_Vib_2,  Limit_Time);
                //        MainForm_Line.ShowDialog();
                //    }
                //    else
                //    {
                //        MessageBox.Show("데이터를 체크하시오.");
                //        return;
                //    }
                //}
                              

            }
            catch (Exception)
            {
                MessageBox.Show("Exception");
            }



        }




        private void btn_Path_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            if (Last_Path != "")
                dlg.InitialDirectory = Last_Path;
            dlg.ValidateNames = false;
            dlg.CheckFileExists = false;
            dlg.CheckPathExists = true;
            dlg.FileName = "Folder_Select";

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            Last_Path = Path.GetDirectoryName(dlg.FileName);

            Main_path = Path.GetDirectoryName(dlg.FileName);


        }

        private void ModeSelect_SelectedIndexChanged(object sender, EventArgs e)
        {            
            foreach (ListViewItem listitem in CSV_VIEW.Items)
            {
                listitem.Checked = false;
            }  
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if(Check_All.Checked == true)
            {
                foreach (ListViewItem listitem in CSV_VIEW.Items)

                {
                    listitem.Checked = true;

                }
            }
            else
            {
                foreach (ListViewItem listitem in CSV_VIEW.Items)

                {

                    listitem.Checked = false;

                }
            }
        }
    }
}


