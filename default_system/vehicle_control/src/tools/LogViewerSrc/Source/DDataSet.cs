using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.IO;
using MyTools;
using System.Windows.Forms;
using System.Diagnostics;

namespace LogViewer
{
    public enum eLogType
    {
        NONE
       , AmcRcv
       , UserLog
	   , SensorArea
       , BcrLog
       , DrivingCtrl
       , DrivingError
       , EQPIOError
       , ExtraJobThread
       , TaskControl
       , PathSearch
       , Trans
       , BCRRawData
       , CID_MONITORING
       , CID
       , DiagManager

       , COUNTOF
    }

    /// 로그 탭에는 Event로그, Amc로그 등 다영한 폼이 붙여지기 때문에 탭의 Tag에 DLogForm 형태로
    /// 해당 탭을 포인터로 가리킨다
    public class DLogForm
    {
        public DLogForm()
        {
            type = eLogType.NONE;
            form = null;
            path = "";
        }

        public enum FormType { eNone, eEventLog, eAmcLog, eNotebook, eFileList }
        public eLogType type;   // 폼 타입
        public Form form;       // 폼
        public string path = "";
    }

   // 모든 로그 파일을 통합해서 하나의 테이블로 관리하는 클래스
    public class DDataSet
    {
        public string folderPath = "";      // 로그폴더 경로
        public List<string> fileList = new List<string>();  // 해당 폴더 안의 파일 리스트
        // 로그들 객체
        public DAmcRcv AmcRcv = new DAmcRcv();
        public DUserLog UserLog = new DUserLog();
        public DBcrLog BcrLog = new DBcrLog();
        //public DSimpleLog[] SimpleLogs = new DSimpleLog[(int)eSimpleLogType.COUNTOF];

        // TabControl과 전체적인 Frame이 들어간 UI Form
        public ULogFrame Frame = null;
        
        public string ErrMsg = "";
        public string DirPath = "";

        //public List<Form> LogFormList = new List<Form>();   // 로그 분석 창들 띄운 것들을 가지고 있다


        public DDataSet()
        {
          
        }
        public bool IsThisFrame(ULogFrame f)
        {
            return Frame == f;
        }

        public bool IsVisibleTabControl()
        {
            return Frame.Visible;
        }
    
        public void SetVisibleFrame(bool bVisible)
        {
            // 현재 탭컨트롤이 보이게한다
            if(Frame != null)
                Frame.Visible = bVisible;
        }

        /// <summary>
        /// AMC Recv 로그 폼을 리턴한다
        /// 단 AmcRecv 로그 폼이 여러개 열려있을 수 있기 떄문에 리스트로 리턴한다
        /// </summary>
        public List<UAmcRecvTable> GetAmcRecvForm()
        {
            List<UAmcRecvTable> list = new List<UAmcRecvTable>();
            if (Frame == null)
                return list;

            // 모든 탭을 돌면서 amclog가 있나 본다
            foreach (TabPage page in Frame.tabControl_Log.TabPages)
            {
                DLogForm logForm = (DLogForm)page.Tag;
                if (logForm.type == eLogType.AmcRcv)
                    list.Add((UAmcRecvTable)logForm.form);
            }
            return list;
        }

        /// <summary>
        /// AMC Recv 로그 폼을 리턴한다
        /// 단 AmcRecv 로그 폼이 여러개 열려있을 수 있기 떄문에 리스트로 리턴한다
        /// </summary>
        public List<UNotebook> GetGetNoteBookForm()
        {
            List<UNotebook> list = new List<UNotebook>();
            if (Frame == null)
                return list;

            // 모든 탭을 돌면서 amclog가 있나 본다
            foreach (TabPage page in Frame.tabControl_Log.TabPages)
            {
                DLogForm logForm = (DLogForm)page.Tag;
                if (logForm.type == eLogType.NONE)
                    list.Add((UNotebook)logForm.form);
            }

            return list;
        }
        
        /// <summary>
        /// 탭 컨트롤 자체를 삭제. 모든 탭 삭제
        /// </summary>
        public void DestroyFrame()
        {
            if (AmcRcv != null)
            {
                if (AmcRcv.table != null)
                {
                    AmcRcv.table.Dispose();
                    AmcRcv.table = null;
                }
            }
            if (UserLog != null)
            {
                if (UserLog.table != null)
                {
                    UserLog.table.Dispose();
                    UserLog.table = null;
                }
            }
            if (BcrLog != null)
            {
                if (BcrLog.table != null)
                {
                    BcrLog.table.Dispose();
                    BcrLog.table = null;
                }
            }

            ClearAll();


            if (Frame != null)
            {
                Frame.Dispose();
                Frame = null;
            }

            //foreach(DSimpleLog d in SimpleLogs)
            //{
            //    if (d.table != null)
            //        d.table.Dispose();
            //}
        }
        

        public void ClearAll()
        {
            //for (int i = 0; i < (int)eSimpleLogType.COUNTOF; i++)
            //{
            //    //if(SimpleLogs[i] != null)
            //    //    SimpleLogs[i].Clear();
            //}

            if (AmcRcv != null)
            {
                AmcRcv.Clear();
                AmcRcv = null;
            }
            if (UserLog != null)
            {
                UserLog.Clear();
                UserLog = null;
            }
            if (BcrLog != null)
            {
                BcrLog.Clear();
                BcrLog = null;
            }
        }
        
        public bool Load()
        {
            this.ErrMsg = "";
            this.DirPath = folderPath;
            if (!Directory.Exists(folderPath))
            {
                ErrMsg = folderPath + "  No Exists Directory";
                return false;
            }

            string[] files = Directory.GetFiles(DirPath);
            List<string> fileList = new List<string>();

            foreach (string check in files)
            {
                if (!check.Contains(".zip")) fileList.Add(check);
            }
            //List<string> fileList = new List<string>(files);
            List <string> userFiles = fileList.FindAll(o => Path.GetFileNameWithoutExtension(o).ToUpper().Contains("USER2"));


			// User로그가 여러개 있을수이다
			if(UserLog.LoadOnlyEventChange(userFiles.ToArray()) == false)
			{
                ErrMsg = " User Log 파일을 로딩하지 못했습니다.\n\n";
            }

            // 아직 Amc Recv 파일이 열리지 않았다면 open한다
            //if (AmcRcv == null)
            //    AmcRcv = new DAmcRcv();
			//
            //List<string> amcFile = fileList.FindAll(o => Path.GetFileNameWithoutExtension(o).ToUpper().Contains("AMC_RECV2"));
			//
            //// 현재 폴더에 모든 amc recv 파일들을 오픈다. 보통 한두개
            //List<string> openFail = new List<string>();
            //foreach (string file in amcFile)
            //{
            //    if (AmcRcv.LoadFile(file) == false)
            //        openFail.Add(file);
            //}
            //if(openFail.Count != 0)
            //{
            //    ErrMsg += "Amc Recv 파일 로딩 실패\n";
            //    foreach (string s in openFail)
            //        ErrMsg += s + "\n";
            //}
			//
            return ErrMsg == "";
        }
        public void InitUI()
        {
            UMain form = UMain.GetThis();

            Frame = new ULogFrame();

            Frame.SetUI(this);
            Frame.Visible = false;
            form.panel1.Controls.Add(Frame.Controls[0]);  // 스레드에서 호출되면 붙일 수 없음

            int nCurIdx = form.panel1.Controls.Count - 1;
            form.panel1.Controls[nCurIdx].Visible = false;
            form.panel1.Controls[nCurIdx].Tag = folderPath;

			if(File.Exists(folderPath + @"\@Comment.txt"))
			{
				if(Frame.tabControl_Info.TabCount >= 3)
					Frame.tabControl_Info.SelectedIndex = 2;

				string strErrMsg = "";
				if (Frame.LoadCommentFile(ref strErrMsg) == false)
				{
					MessageBox.Show("Comment 파일 열기 실패\n\n" + strErrMsg);
				}
			}
            //// Main폼 Panel1에 붙인다
            //form.panel1.Controls.Add(Frame.Controls[0]);  // 스레드에서 호출되면 붙일 수 없음

            // 나중에 AmcRcv.SetUserModeStat(UserLog.EventChangeList); 해줘야함
        }

        /*     SQL Query example
      DataTable table = new DataTable("Players");
      table.Columns.Add(new DataColumn("Size", typeof(int)));
      table.Columns.Add(new DataColumn("Sex", typeof(char)));

      table.Rows.Add(100, 'f');
      table.Rows.Add(235, 'f');
      table.Rows.Add(250, 'm');
      table.Rows.Add(310, 'm');
      table.Rows.Add(150, 'm');

      // Search for people above a certain size.
      // ... Require certain sex.
      DataRow[] result = table.Select("Size >= 230 AND Sex = 'm'");       //  <=== QUERY
      foreach (DataRow row in result)
      {
          Console.WriteLine("{0}, {1}", row[0], row[1]);
      }
      */

        // (다중)쿼리 처리하기 ****************************
        // 쿼리를 여러개 날릴 수 있다. 첫번째 쿼리 결과에서 두번재를 실행하고 세번째를 실행하는 방식으로
        // 결과는 resultTable 에 저장됨
        //public DataTable DoQuery(string[] queries)
        //{
        //    DataTable resultTable = CreateBigTable();

        //    resultTable.Rows.Clear();

        //    if (queries.Length <= 0 || BigTable.Rows.Count <= 0)
        //        return resultTable; // wrong case..


        //    // 만약 쿼리가 한개라면 한번에 끝난다
        //    if (queries.Length == 1)
        //    {
        //        DataRow[] rows = BigTable.Select(queries[0]);   // SQL Query 처리
        //        resultTable.Rows.Add(rows);
        //        return resultTable;
        //    }

        //    // 다중 쿼리를 날린 경우 처리
        //    for (int i = 0; i < queries.Length; i++)
        //    {
        //        string query = queries[i];
        //        DataRow[] rows;
        //        if (i == 0) // 첫번째는 raw table에서 query 실행
        //        {
        //            rows = BigTable.Select(query);   // SQL Query 처리
        //            resultTable.Rows.Clear();

        //            resultTable.Rows.Add(rows);
        //        }
        //        else
        //        {
        //            // 이후부터는 resultable에서 qeruy 실행
        //            rows = resultTable.Select(query);   // SQL Query 처리
        //            resultTable.Rows.Clear();

        //            resultTable.Rows.Add(rows);
        //        }
        //    }

        //    return resultTable;
        //}

        //// 쿼리 처리하기 ****************************
        //// 쿼리를 여러개 날릴 수 있다. 첫번째 쿼리 결과에서 두번재를 실행하고 세번째를 실행하는 방식으로
        //// 결과는 resultTable 에 저장됨
        //public DataTable DoQuery(string query)
        //{
        //    //resultTable.Rows.Clear();
        //    DataTable resultTable = CreateBigTable();

        //    if (query.Length <= 0)
        //        return resultTable; // wrong case..

        //    bool bQuerySuccess = false;
        //    try
        //    {
        //        var filteredDataRows = BigTable.Select(query);   // SQL Query 처리
        //        bQuerySuccess = true;
        //        resultTable = filteredDataRows.CopyToDataTable();
        //    }
        //    catch (Exception e)
        //    {
        //        if (bQuerySuccess)
        //            return resultTable;
        //        MessageBox.Show(String.Format("Query Failed : {0}\n\n{1}", query, e.Message), "Query Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        //        return resultTable;
        //    }

        //    return resultTable;
        //}

        //AMC_Send_Periodic2018050713.log  <<== 이런식으로 있을 때 13시 로그라는게 파일이름에 있다. 이것을 리턴
        // 만약 시간대가 안써있는 로그라면 -1 리턴
        //public static int GetLogFileEvent(string filename)
        //{
        //    if (filename.Contains("_TRACE_"))
        //        return -1;  // trace로그는 skip

        //    int nPos = filename.ToUpper().IndexOf(".LOG") - 2;
        //    if (nPos < 8)
        //        return -1;
        //    string strHour = filename.Substring(nPos, 2);
        //    int nHour = -1;

        //    if(!Int32.TryParse(strHour, out nHour))
        //        return -1;
        //    if (nHour < 0 || nHour > 23)
        //        return -1;

        //    return nHour;
        //}
    }
	
}
