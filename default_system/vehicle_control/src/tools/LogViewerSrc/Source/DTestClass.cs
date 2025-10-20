using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.Diagnostics;
using System.Windows.Forms;
using MyTools;

namespace LogViewer
{

    /// <summary>
    /// class ErrorRow ::: 에러 항목 한 줄에 대한 내용. 해당 시간, 줄번호, 얼마나 오랫동안 연속해서 났는지 
    /// </summary>
    public class DTestErrorRow
    {
        public int line = 0;
        public int Cnt = 0;
        public DateTime startTime = new DateTime();
        public DateTime finishedTime = new DateTime();
        public double timeSpan = 0.0;

        public DTestErrorRow(){}
        public void CalTimespan()
        {
            TimeSpan span = finishedTime - startTime;
            timeSpan = span.TotalSeconds;
        }
    }

    public class DTestList
    {
        public List<DTestItem> ItemList = new List<DTestItem>();
        IniMan ini = new IniMan();
        public DTestList()
        {
            LoadINI(UMain.INI_PATH);
        }
  

        public void LoadINI(string path)
        {
            ini.Path = path;
            string strVal = ini.Read("TestInfo", "Count", "0");
            int nCnt = 0;
            if (!Int32.TryParse(strVal, out nCnt))
                return;

            ItemList.Clear();

            for (int i = 0; i < nCnt; i++)
            {
                string strName = ini.Read("TestInfo", String.Format("{0}_Name", i), "TestNone");
                string strIcon = ini.Read("TestInfo", String.Format("{0}_IconName", i), "");
                string strDesc = ini.Read("TestInfo", String.Format("{0}_Descript", i), "");
                
                string strAutoModeOnly = ini.Read("TestInfo", String.Format("{0}_AutoModeOnly", i), "1");
                string strErrGrouping = ini.Read("TestInfo", String.Format("{0}_ErrGroupingToleranceLineCnt", i), "10");
                string strIgnoreCnt = ini.Read("TestInfo", String.Format("{0}_IgnoreCnt", i), "0");
                
                int nErrGrp = 20, nIgnoreCnt = 0;
                Int32.TryParse(strErrGrouping, out nErrGrp);
                Int32.TryParse(strIgnoreCnt, out nIgnoreCnt); 
                string[] strQuries = ini.Read("TestInfo", String.Format("{0}_Query", i), "").Split(',');

                DTestItem item = new DTestItem();
                item.Name = strName;
                item.IconName = strIcon;
                item.Descript = strDesc;
                item.bAutoModeOnly = strAutoModeOnly == "1" ? true : false;
                item.ErrGroupingToleranceLineCnt = Math.Max(0, nErrGrp);
                item.IgnoreCnt = Math.Max(0, nIgnoreCnt);
                item.Quries.AddRange(strQuries);

                ItemList.Add(item);
            }
        }

        public void SaveINI(string path)
        {
            ini.Path = path;
            ini.Write("TestInfo", "Count", Convert.ToString(ItemList.Count));

            for (int i = 0; i < ItemList.Count; i++)
            {
                ini.Write("TestInfo", String.Format("{0}_Name", i), ItemList[i].Name);
                ini.Write("TestInfo", String.Format("{0}_IconName", i), ItemList[i].IconName);
                ini.Write("TestInfo", String.Format("{0}_Descript", i), ItemList[i].Descript);
                ini.Write("TestInfo", String.Format("{0}_AutoModeOnly", i), Convert.ToString(ItemList[i].bAutoModeOnly));
                ini.Write("TestInfo", String.Format("{0}_ErrGroupingToleranceLineCnt", i), Convert.ToString(ItemList[i].ErrGroupingToleranceLineCnt));
                ini.Write("TestInfo", String.Format("{0}_IgnoreCnt", i), Convert.ToString(ItemList[i].IgnoreCnt));
                string strQuries = "";
                foreach (string q in ItemList[i].Quries)
                    strQuries += q + ",";

                ini.Write("TestInfo", String.Format("{0}_Query", i), strQuries);
            }
        }
    }

    /// <summary>
    ///  class Test_Base ::: 여러 테스트 클래스 항목들의 부모 순수가상 클래스. 테스트들을 하나로 묶어주기 위함
    /// </summary>
    public class DTestItem
    {
        public string Name = "";
        public string IconName = ""; // 실행파일 경로 Pics 폴더에 있는 이미지 이름을 설정하면 Icon으로 보여줄수 있다.
        public string Descript = ""; // 해당 테스트에 대한 설명
        public DAmcRcv AmcRecvTable = null;
        //public DTableUserLog UserLogTable = null;
        public List<DTestErrorRow> ErrList = new List<DTestErrorRow>();
        public bool bAutoModeOnly = false;     // UserLog까지 확인해서 FoupCover 열렸을 때가 Auto 모드 였을 때만으로 필터링
        public List<string> Quries = new List<string>();

        /// <summary>
        /// 에러Table을 ErrRow로 정리할 때 값이 헌팅하는 경우,
        /// 의미상 한개의 에러인데 깜박거린 횟수만큼 에러개수로 치면 결과가 복잡해 보이기 때문에 
        /// 연속되는 에러의 경우 ErrGroupingToleranceLineCnt 값의 라인수 이내에 있으면 한개의 에러로 간주한다
        /// </summary>
        public int ErrGroupingToleranceLineCnt = 5;
        public int IgnoreCnt = 0;

        public DTestItem() { }
        public void SetAmcTable(ref DAmcRcv table)
        {
            this.AmcRecvTable = table;
        }
        //public void SetUserTable(ref DTableUserLog table)
        //{
        //    this.UserLogTable = table;
        //}

        public void SetQuries(string[] queries)
        {
            Quries.Clear();
            Quries.AddRange(queries);
        }
        /// <summary>
        /// 쿼리대로 테스트 한다
        /// </summary>
        /// <returns></returns>
        public bool DoTest()
        {
            /*
            ErrList.Clear();

            if (bAutoModeOnly && this.UserLogTable == null) //UserLog 테이블 세팅 안해놨으면 테스트 할 수 없다
                bAutoModeOnly = false;


            DataTable resultTable = this.AmcRecvTable.DoQuery(Quries.ToArray());

            MakeErrorList(resultTable);
            */
            return true;
        }
        /// <summary>
        /// 에러Table을 ErrRow로 정리할 때 값이 헌팅하는 경우,
        /// 의미상 한개의 에러인데 깜박거린 횟수만큼 에러개수로 치면 결과가 복잡해 보이기 때문에 
        /// 연속되는 에러의 경우 ErrGroupingToleranceLineCnt 값의 라인수 이내에 있으면 한개의 에러로 간주해서
        /// ErrList 변수에 정리한다
        /// </summary>
        public bool MakeErrorList(DataTable resultTable)
        {
            if (resultTable.Rows.Count == 0)
                return false;

            try
            {
                DTestErrorRow CurData = null;    //처음에 bNewErrStart 가 true되고 바로 new 됨 
                bool bNewErrStart = false;
                int nLastLine = 0;

                // resultTable 돌면서 각각 에러 항목들을 덩어리 단위로 분리하여 ErrorRow로 만들고
                // ErrowRow 덩어리들을 ErrList에 저장한다
                for (int i = 0; i < resultTable.Rows.Count; i++)
                {
                    DataRow row = resultTable.Rows[i];
                    int nCurLine = i;       //(int)row[(int)eAmcRcvIdx.LineNo];
                    bNewErrStart = (i == 0) || (nCurLine - nLastLine > ErrGroupingToleranceLineCnt);  // ErrGroupingToleranceLineCnt 변수 주석 확인

                    if (bNewErrStart)
                    {
                        // 연속되는 에러가 끝나고 새로운 에러가 시작되면 
                        // 연속되는 에러의 끝지점에 시간을 기록한다. timespan을 구하기 위함
                        if(CurData != null && i>0)
                        {
                            DataRow prevRow = resultTable.Rows[i - 1];
                            CurData.finishedTime = (DateTime)prevRow[(int)eAmcRcvIdx.Time_Date];
                            CurData.CalTimespan();
                        }

                        // 새로운 에러 생성
                        CurData = new DTestErrorRow();
                        ErrList.Add(CurData);    //생성하자 마자 추가.

                        CurData.line = i; //(int)row[(int)eAmcRcvIdx.LineNo];
                        CurData.startTime = (DateTime)row[(int)eAmcRcvIdx.Time_Date];
                    }

                    // 마지막 데이터 처리
                    if(i == resultTable.Rows.Count-1)
                    {
                        int nIdx = Math.Max(0, i - 1);
                        DataRow prevRow = resultTable.Rows[nIdx];
                        CurData.finishedTime = (DateTime)prevRow[(int)eAmcRcvIdx.Time_Date];
                        CurData.CalTimespan();
                    }

                    nLastLine = nCurLine;
                    CurData.Cnt++;

                }
            }
            catch (Exception ex)
            {
                string logMessage = "Exception in MakeErrorList()" + ex.ToString();
                MessageBox.Show(logMessage);
                return false;
            }

           

            return true;
        }
        
    }
    
}
