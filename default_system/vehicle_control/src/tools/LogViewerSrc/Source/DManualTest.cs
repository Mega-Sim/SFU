using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MyTools;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;
using System.Data;

namespace LogViewer
{

    public enum eManualTest // 매뉴얼 테스트 결과 기본항목.  이 항목에 추가로 사용자가 보고자 하는 항목이 보여야함
    {
        Folder
        , Time
        , Node
        , Speed
        , Elapse
            
        , COUNTOF
    }

    // 검사하는 항목 정보.
    // 어떤 로그의 몇번째 Col 을 조사할지
    class DManualTestItem
    {
        eLogType type;  // 어떤 로그파일
        int nCheckIdx;      // 몇번재 인덱스
        DCondition cond;    // 조건
    }

    public class DManualTestRowData
    {
        public dynamic[] subItems = new dynamic[(int)eManualTest.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다
    }


    class DManualTest
    {
        public int m_nCurPos;
        public int m_nTotal;
        public int m_nTolerance_msec;   // UBG, OBS 허용 시간차이
        public bool m_bCancel = false;
        private bool m_AllDone;
        private List<string> m_fileList = null;
        static private DManualTest This = null;
        private List<DDataSet> m_ListDataSet = new List<DDataSet>();

        // 결과 저장 list, Table
        public List<sManualRowList> m_Result= new List<sManualRowList>();
        //private List<sManualRowList> m_ResultB = new List<sManualRowList>();
        public Thread m_Thread = null;
        public DManualTest()
        {
            m_nCurPos = m_nTotal = 0;
            m_AllDone = false;
            This = this;
        }
        public void ClearResult()
        {
            foreach (sManualRowList a in m_Result)
                a.AmcData.Clear();
            m_Result.Clear();
            m_bCancel = false;
        }

        public void Start(List<string> fileList, bool bUseThread = true)
        {
            This = this;
            m_fileList = fileList;
            m_bCancel = false;

            if (bUseThread) // using thread. 비동기
            {
                m_AllDone = false;
                if (m_Thread != null)
                    m_Thread.Abort();
                m_Thread = new Thread(RunThread);
                m_Thread.Priority = ThreadPriority.Highest;
                m_Thread.Start();
            }
            else
            {   RunThread();    // 동기 no thread
            }
        }

        public bool IsFinished()
        {
            return m_AllDone;
        }

        // 작업 수행 스레드 **************
        private static void RunThread()
        {
            This.m_AllDone = false;
            List<string> amcList = This.m_fileList.FindAll(o => Path.GetFileName(o).ToUpper().IndexOf("AMC_RECV20") == 0);
            This.m_nTotal = amcList.Count;
            This.m_nCurPos = 1;
            This.ClearResult();

            // 한개한개 amc에 대해서 수행
            foreach (string one in amcList)
            {
                List<sManualRowList> ret = This.DoFileCheck(one);
                This.m_Result.AddRange(ret);

                This.m_nCurPos++;
                if (This.m_bCancel)
                    break;
            }

            This.m_bCancel = false;
            This.m_AllDone = true;
            This.m_Thread = null;
        }

        // 한개의 파일에 대해서 필터를 검사하고 결과를 리턴한다
        private List<sManualRowList> DoFileCheck(string amcFilePath)
        {
            // data set 한개 생성
            DDataSet dataSet = new DDataSet();
            dataSet.folderPath = Path.GetDirectoryName(amcFilePath);

            // amc recv 로딩
            dataSet.AmcRcv.LoadFile(amcFilePath);

            List<sManualRowList> aFileResult = new List<sManualRowList>();
            sManualRowList rowList = null;

            // 모든 row 탐색
            foreach (DataRow row in dataSet.AmcRcv.table.Rows)
            {
                bool bOutSel1 = false;
                bool bOutSel2 = false;
                bool bOutSel3 = false;

                bool bInUbgLong = false;
                bool bInUbgMid = false;
                bool bInUbgShort = false;

                bool bInObsLong = false;
                bool bInObsMid = false;
                bool bInObsShort = false;

                /////////////

                bool bJiksunPettern = !bOutSel1 && bOutSel2 && bOutSel3;    // 직선패턴 : 011

                if (bJiksunPettern == false)    // 직선 아니면 건너뜀. row 뭉치도 초기화
                {
                    if (rowList != null)
                    {
                        aFileResult.Add(rowList); // resultA에 추가
                        rowList = null;
                    }
                    continue;
                }
                
                bool bOBSDetected = !bInObsLong || !bInObsMid || !bInObsShort; // OBS 감지됨. 0=감지, 1=미감지
                int nUBGLevel = GetUBGLevel(!bInUbgLong, !bInUbgMid, !bInUbgShort);     // 신호가 반전되어 있음
                
                // obs는 감지 안됐는데 ubg는 감지 된경우
                if (bOBSDetected == false && nUBGLevel > 0)
                {
                    if (rowList == null)
                    {
                        rowList = new sManualRowList();   // row 뭉치 생성
                        rowList.amcFilePath = amcFilePath;  // 해당 파일 명도 세팅

                        //같은 시간대의 user로그가 없을 수도 있지만 user로그 도 넣는다.
                        string userFileName = Path.GetFileName(amcFilePath).ToUpper().Replace("AMC_RECV", "User");
                        rowList.userFilePath = Path.GetDirectoryName(amcFilePath)+ @"\" + userFileName;   
                    }

                    rowList.AmcData.Add(row);  // 조건에 부합하는 log row를 그대로 수집한다.  tolerance sec 처리는 일단 다 추가하고 나중에 한번에 처리
                }
                else
                {
                    if (rowList != null)
                    {
                        aFileResult.Add(rowList); // resultA에 추가
                        rowList = null;
                    }
                }
            }

            // 마지막에 추가 안됐으면 추가
            if (rowList != null)
            {
                if(aFileResult.Count == 0)
                    aFileResult.Add(rowList);

                if (aFileResult.ElementAt(aFileResult.Count - 1) != rowList)
                    aFileResult.Add(rowList);
            }

            // result table에 걸려있는 row들을 검사하면서 tolerance 시간 이하의 데이터는
            // 삭제한다
            for(int i= aFileResult.Count -1 ; i >= 0 ; i--)
            {
                sManualRowList data = aFileResult.ElementAt(i);
                if (data.Validation(m_nTolerance_msec) == false)    // validation check
                    aFileResult.RemoveAt(i);
            }

            // User 로그 수집
            if(aFileResult.Count > 0)
            {
                dataSet.UserLog.LoadFile(aFileResult.ElementAt(0).userFilePath);

                foreach (sManualRowList oneResult in aFileResult)     // 한개의 결과뭉치. 한개의 파일에는 여러개의 결과뭉치가 있을 수 있다
                {
                    DataRow amcRow_start = oneResult.AmcData.ElementAt(0);
                    DataRow amcRow_end = oneResult.AmcData.ElementAt(oneResult.AmcData.Count-1);

                    DateTime tmStart = (DateTime)amcRow_start.ItemArray[(int)eAmcRcvIdx.Time_Date];
                    DateTime tmEnd = (DateTime)amcRow_end.ItemArray[(int)eAmcRcvIdx.Time_Date];

                    bool bMeetStart = false;

                    // 모든 user 로그의 row들을 검사
                    foreach (DataRow userRow in dataSet.UserLog.table.Rows)
                    {
                        DateTime tmCurTime = (DateTime)userRow.ItemArray[(int)eUserLogIdx.Time_Date];
                        TimeSpan span = tmStart - tmCurTime;
                        //if (Math.Abs(span.TotalMilliseconds) < 500)   // 500 msec 이내를 user <-> amc 로그 허용범위
                        if (span.TotalMilliseconds < 500 && bMeetStart == false)   // 500 msec 이내를 user <-> amc 로그 허용범위
                            bMeetStart = true;

                        TimeSpan span2 = tmCurTime - tmEnd;
                        if (span2.TotalMilliseconds > 0 && span2.TotalMilliseconds < 500)  // 뭉치의 끝
                            break;

                        if (bMeetStart)    // amc 로그뭉치 시작과 끝 사이 시간대의 user로그를 수집한다
                            oneResult.UserData.Add(userRow);
                    }

                    oneResult.MakeSummary();    // 시작은 언제고 끝은 언제고 elapse는 얼마고, 지나온 노드는 어디어디인지 정리
                }
            }

            //dataSet.ClearAll();
            return aFileResult;
        }

        public static int GetUBGLevel(bool bLong, bool bMid, bool bShort)
        {
            if (bLong && !bMid && !bShort)  //100
                return 7;

            if (bLong && bMid && !bShort)   //110
                return 6;

            if (!bLong && bMid && !bShort)    //010
                return 5;

            if (!bLong && bMid && bShort)     //011
                return 4;

            if (bLong && bMid && bShort)       //111
                return 3;

            if (bLong && !bMid && bShort)     //101
                return 2;

            //if (!bLong && !bMiddle && !bShort)  //000
                return 0;
        }
    }

    public class sManualRowList
    {
        public string amcFilePath = "";
        public string userFilePath = "";
        public DManualTestRowData summary = new DManualTestRowData();
        public List<DataRow> AmcData = new List<DataRow>();
        public List<DataRow> UserData = new List<DataRow>();

        public bool Validation(int nTole_msec)
        {
            if (AmcData.Count < 1)
                return false;

            DateTime first = (DateTime)AmcData.ElementAt(0).ItemArray[(int)eAmcRcvIdx.Time_Date];
            DateTime end = (DateTime)AmcData.ElementAt(AmcData.Count-1).ItemArray[(int)eAmcRcvIdx.Time_Date];

            TimeSpan span = end - first;
            if (span.TotalMilliseconds < nTole_msec)    // 너무 짧음
                return false;

            //OBS 켜진시점과 비교

            return true;
        }

        // summary 배열 변수를 꾸며준다
        public void MakeSummary()
        {
            int nLastIdx = AmcData.Count - 1;

            Object[] startRow = AmcData[0].ItemArray;
            Object[] endRow = AmcData[nLastIdx].ItemArray;
            
            summary.subItems[(int)eManualTest.Folder] = Path.GetDirectoryName(amcFilePath);
            //summary.subItems[(int)eManualTest.AmcFile] = amcFilePath;
            //summary.subItems[(int)eManualTest.UserFile] = userFilePath;

            
            bool bInUbgLong = false;
            bool bInUbgMid = false;
            bool bInUbgShort = false;
            int nLevel = DManualTest.GetUBGLevel(!bInUbgLong, !bInUbgMid, !bInUbgShort);

            //summary.subItems[(int)eManualTest.UBGLevel_Start] = nLevel;  // start ubg level
                        
            bInUbgLong = false;
            bInUbgMid = false;
            bInUbgShort = false;
            nLevel = DManualTest.GetUBGLevel(!bInUbgLong, !bInUbgMid, !bInUbgShort);

            //summary.subItems[(int)eManualTest.UBGLevel_End] = nLevel;   // end ubg level
            DateTime startTime = (DateTime)startRow[(int)eAmcRcvIdx.Time_Date];
            DateTime endTime = (DateTime)endRow[(int)eAmcRcvIdx.Time_Date];
            TimeSpan span = endTime - startTime;

            summary.subItems[(int)eManualTest.Time] = startTime;
            summary.subItems[(int)eManualTest.Elapse] = span.TotalSeconds;
            

            int nUserCnt = UserData.Count;
            if(nUserCnt > 0)
            {
                summary.subItems[(int)eManualTest.Node] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_Node];
                //summary.subItems[(int)eManualTest.NodeEnd] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_Node];

                //summary.subItems[(int)eManualTest.OffsetStart] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                //summary.subItems[(int)eManualTest.OffsetEnd] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                //summary.subItems[(int)eManualTest.MoveDist] = "";    // move dist는 맵을 로딩해야 가능
            }
      
            // summary 끝나면 데이터 지워버리자
            ClearTable();
        }

        void ClearTable()
        {
            foreach (DataRow row in AmcData)
                row.Delete();
            foreach (DataRow row in UserData)
                row.Delete();

            AmcData.Clear();
            UserData.Clear();
        }
        
    }

}
