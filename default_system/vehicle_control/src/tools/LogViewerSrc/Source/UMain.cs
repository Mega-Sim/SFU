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
using System.Runtime.InteropServices;
using LogViewer_DataManager;
using Diff;
using System.Diagnostics;
using System.Reflection;

namespace LogViewer
{
    public partial class UMain : Form
    {
        // 로그 테이블
        public enum TableName
        {
            Amc_Rcv
           ,User_Log
           ,Abnormal_Err
        };

        public enum eChildFormList
        {
            LogFile, Dignosis, DataManager, UBGPatternChk, Diff
        }


        public static string PROGRAM_NAME = "Log Viewer";
        //public static string PROGRAM_VER = "2.0.2.0";
        public string INI_NAME = "Settings.INI";
        public static string INI_PATH = "";
		public static string TMP_PATH = @"C:\.oht_easy_view_temporary";

		public UMainFiles m_formMainFiles;
        public UAmcRecvCols m_dlgAmcRecvColView;
        //public UTest m_dlgTest;
        public UFindGridColData m_dlgFind = new UFindGridColData();
        //public DTestList m_TestList;
        //public DDataSet dataSet;
        public DataGridView m_CurrentGrid = null;
        //public UEventLog m_LogEvent;
        //public UViewColor m_dlgColorOpt;
        //public UAmcRecvTable m_amcRecvTable = null;
        
        public UMapViewMan m_dlgMapMan = new UMapViewMan();
        public UCustomTest_UBG m_dlgCustomTest = null;
        public UManualTest m_dlgManaulTest = null;
        public UDataManager m_dlgDataManager = null;
        public Snake.USnakeGame m_dlgSnakeGame = null;
		public UDiff m_dlgDiff = null;

        public UMain()
        {
            InitializeComponent();
            INI_PATH = Application.StartupPath + "\\"+ INI_NAME;
            m_dlgCustomTest = new UCustomTest_UBG();
            m_dlgManaulTest = new UManualTest();
            m_dlgDataManager = new UDataManager();
            m_dlgSnakeGame = new Snake.USnakeGame();
			m_dlgDiff = new UDiff();
			UViewColor.LoadINI();

			btn_CustomTest1.Visible = false;

			if (m_dlgDataManager.m_bAvaliable == false)
				btn_DataManager.Enabled = false;    // 데이터메니저가 정상적으로 로딩 안되면 버튼 잠군다

			if (!Directory.Exists(TMP_PATH))
				Directory.CreateDirectory(TMP_PATH);

			// Process p = Process.Start("");
			//UMapView.OPEN_BY_OHTEASYVIEW = true;    //oht easy view에서 열린것을 알려줌
		}

        public static void SetTitle(string strCurPath)
        {
			try
			{
				if (String.IsNullOrEmpty(strCurPath))
					strCurPath = "";

				Version v = Assembly.GetExecutingAssembly().GetName().Version;
				DateTime fileTm = File.GetLastWriteTime(Application.ExecutablePath);
  				GetThis().Text = String.Format("{0}/{1}/{2}  ", PROGRAM_NAME, v.ToString(), fileTm.ToShortDateString());

				if (strCurPath.Length > 0)
				{
					strCurPath = strCurPath.Replace(@"\", "/");
					GetThis().Text += String.Format("  - {0}", strCurPath);
				}
			}
			catch
			{
			}
        }
        public static UMain GetThis()
        {
            return Program.MainForm;
        }

        private void Form_Main_Load(object sender, EventArgs e)
        {
            try
            {
                m_formMainFiles = new UMainFiles();

                // eChildFormList 순서대로 넣는다
                //LogFile, Dignosis, DataManager, UBGPatternChk, SnakeGamer

                panel1.Controls.Add(m_formMainFiles.Controls[0]);       //    LogFile
                panel1.Controls.Add(m_dlgManaulTest.Controls[0]);       //   Dignosis
                panel1.Controls.Add(m_dlgDataManager.Controls[0]);     //   DataManager
                panel1.Controls.Add(m_dlgCustomTest.Controls[0]);       //   UBGPatternChk
                //panel1.Controls.Add(m_dlgSnakeGame.Controls[0]);       //   SnakeGamer
				panel1.Controls.Add(m_dlgDiff.Controls[0]);       //   SnakeGamer


				//panel1.Controls.Add(new ULogFrame().Controls[0]);


				m_formMainFiles.parent = this;
                m_dlgAmcRecvColView = new UAmcRecvCols();
                
                //m_LogEvent = new UEventLog();

                //m_amcRecvTable = new UAmcRecvTable();

                //m_mapView = new UMapView();
                //panel1.Controls.Add(m_dlgMainTab.Controls[0]);
                //ViewTab_orFileTree(true);
                //m_dlgMainTab.tabPage_FileList.Controls.Add(m_dlgFileList.Controls[0]);
                //m_dlgMainTab.tabPage_LogTable.Controls.Add(m_LogEvent.Controls[0]);
                
                timer_refresh.Enabled = true;
                SetTitle("");
                //UViewColor dlg = new UViewColor();
                //dlg.buttonApply_Click(null, null);
                //UViewColor.LoadINI();   // Color 정보 로딩
            }
            catch (Exception exp)
            {
                Debug.Assert(false, exp.Message);
                throw;
            }
            //m_dlgAmcRecvColView.Show();
        }

        private eChildFormList m_LastPresedForm;
        public void ViewChildForm(eChildFormList n)
        {
            for (int i = 0; i < panel1.Controls.Count; i++)
                panel1.Controls[i].Visible = false;

            panel1.Controls[(int)n].Visible = true;
            m_LastPresedForm = n;
        }

        /// true: 전체 폴더리스트 창 보기
        /// false: 각 로그 Frame 창 보기
        public void ViewFileTree()
        {
            panel1.Controls[0].Visible = true;
            for (int i = 1; i < panel1.Controls.Count; i++)
                panel1.Controls[i].Visible = false;
        }
      
        public void ViewCustomTest()
        {
            panel1.Controls[0].Visible = false;
            panel1.Controls[1].Visible = true;
            for (int i = 2; i < panel1.Controls.Count; i++)
                panel1.Controls[i].Visible = false;
        }
        public void ViewTestForm()
        {
            panel1.Controls[0].Visible = false;
            panel1.Controls[1].Visible = false;
            panel1.Controls[2].Visible = true;
            for (int i = 3; i < panel1.Controls.Count; i++)
                panel1.Controls[i].Visible = false;
        }
        public void ViewDataManager()
        {
            panel1.Controls[0].Visible = false;
            panel1.Controls[1].Visible = false;
            panel1.Controls[2].Visible = false;
            panel1.Controls[3].Visible = true;
            for (int i = 4; i < panel1.Controls.Count; i++)
                panel1.Controls[i].Visible = false;
        }
        // 찾기 상자 팝업~
        public void PopUpFindForm()
        {
            if(m_CurrentGrid == null)
            {
                MessageBox.Show("선택된 그리드가 없습니다.");
                return;
            }

            m_dlgFind.SetCurrentGrid(m_CurrentGrid);
            m_dlgFind.Show();
            m_dlgFind.Visible = true;
        }

        private void ribbonButton_SetFolder_Click(object sender, EventArgs e)
        {
            //ULoad dlg = new ULoad();
            //dlg.ShowDialog();
            ViewChildForm(eChildFormList.LogFile);
        }
        
        private void ribbonBtn_Test_Click(object sender, EventArgs e)
        {
            if(Control.ModifierKeys == Keys.Control)
            {
                btn_CustomTest1.Visible = !btn_CustomTest1.Visible; // 숨김버튼 보이기
                return;
            }
            ViewChildForm(eChildFormList.Dignosis);
        }
        
        private void ribbonButton_GridColor_Click(object sender, EventArgs e)
        {
            UViewColor dlg = new UViewColor();
            //MyTool.GMethod.MoveForm_ParentCenter(dlg, this);
            dlg.ShowDialog();
            //dlg.BringToFront();
        }

        private void ribbonBtn_MapView_Click(object sender, EventArgs e)
        {
            //m_dlgMapMan.StartPosition = FormStartPosition.CenterParent;
            MyTools.GMethod.MoveForm_ParentCenter(m_dlgMapMan, this);
            m_dlgMapMan.Show();
            m_dlgMapMan.BringToFront();
        }
        
        bool bOnce = true;
        Color DefClr;
        Color SelectedClr = Color.LightGreen;

        private void timer_refresh_Tick(object sender, EventArgs e)
        {

            // 처음에 한번만 프로그램 켤 때 파일 로딩 창 자동으로 띄워주기
            if (bOnce)
            {
                bOnce = false;
                DefClr = btn_DataManager.BackColor;
                //m_dlgColor.buttonApply_Click(null, null);   // 처음에 무조건 한번은 색 정보 INI 로딩해야함
                //ribbonButton_SetFolder_Click(null, null);
            }

			if (!Directory.Exists(TMP_PATH))	// 임시폴더는 반드시 있어야함
				Directory.CreateDirectory(TMP_PATH);

			toolStripButton_Load.BackColor = m_LastPresedForm == eChildFormList.LogFile ? SelectedClr : DefClr;
            toolStripButton_Diagnose.BackColor = m_LastPresedForm == eChildFormList.Dignosis ? SelectedClr : DefClr;
            btn_DataManager.BackColor = m_LastPresedForm == eChildFormList.DataManager ? SelectedClr : DefClr;
            btn_CustomTest1.BackColor = m_LastPresedForm == eChildFormList.UBGPatternChk ? SelectedClr : DefClr;
			btn_Diff.BackColor = m_LastPresedForm == eChildFormList.Diff ? SelectedClr : DefClr;

		}

        private void UMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(MessageBox.Show("종료 하시겠습니까?", "", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                e.Cancel = true;
                return;
            }
            //m_dlgCustomTest.Close();
            //m_dlgManaulTest.Close();
            //m_dlgSnakeGame.Close();
            //m_dlgMapMan.Close();
            //m_dlgDataManager.Close();
            Environment.Exit(0);
            

        }
        
        private void btnMapView_Click(object sender, EventArgs e)
        {
            try
            {
                MyTools.GMethod.MoveForm_ParentCenter(m_dlgMapMan, this);
                Program.MainForm.m_dlgMapMan.Show();
                Program.MainForm.m_dlgMapMan.BringToFront();
            }
            catch (Exception)
            {
            }
        }
        

        private void btn_CustomTest1_Click(object sender, EventArgs e)
        {
            ViewChildForm(eChildFormList.UBGPatternChk);
        }

        private void btn_DataManager_Click(object sender, EventArgs e)
        {
            ViewChildForm(eChildFormList.DataManager);
        }
		private void btn_Diff_Click(object sender, EventArgs e)
		{
			ViewChildForm(eChildFormList.Diff);
		}
		private void toolStripButton_Load_MouseDown(object sender, MouseEventArgs e)
        {
            // 이스터에그: 로그 로딩버튼 우클릭+S버튼
            if(e.Button == MouseButtons.Right && Control.ModifierKeys == Keys.Control)
            {
                m_dlgSnakeGame.Show();
            }
        }

        private void UMain_FormClosed(object sender, FormClosedEventArgs e)
        {
        }

		USensorArea dlgSensorArea = new USensorArea();
		private void btn_LogConv_Click(object sender, EventArgs e)
		{

			UVersionConv dlg = new UVersionConv();
			dlg.ShowDialog();

			//dlgSensorArea.Show();
		}
	}

	public class LineTime
    {
        public DateTime tm = new DateTime();
        public int nLine = 0;
    }
}
