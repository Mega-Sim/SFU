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

    public enum eCustomTest
    {
          Folder
        , AmcFile
        , UserFile
        , UBGLevel_Start
        , UBGLevel_End
        , Time
        , Elapse
        , NodeStart
        , NodeEnd
        , OffsetStart
        , OffsetEnd
        , MoveDist

        , COUNTOF
    }

    public class DCustomTestRowData
    {
        public dynamic[] subItems = new dynamic[(int)eCustomTest.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다
    }

    class DCustomTest_UBG
    {
        public int m_nCurPos;
        public int m_nTotal;
        public int m_nTolerance_msec;   // UBG, OBS 허용 시간차이
        public int m_nToleranceTo_msec;   // UBG, OBS 허용 시간차이

        public bool m_bCancel = false;
        private bool m_AllDone;
        private List<string> m_fileList = null;
        static private DCustomTest_UBG This = null;
        private List<DDataSet> m_ListDataSet = new List<DDataSet>();
        public List<int> m_JikJinNode = new List<int>();
        public int m_nUBGLevelRange_From = 0;
        public int m_nUBGLevelRange_To = 0;
        // 결과 저장 list, Table
        public List<sCustomRowList> m_Result= new List<sCustomRowList>();
        //private List<sCustomRowList> m_ResultB = new List<sCustomRowList>();
        public Thread m_workThread = null;
        public DCustomTest_UBG()
        {
            m_nCurPos = m_nTotal = 0;
            m_AllDone = false;
            This = this;
        }
        public void ClearResult()
        {
            foreach (sCustomRowList a in m_Result)
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
                if(m_workThread != null)
                {
                    m_workThread.Abort();
                }

                m_workThread = new Thread(RunThread);
                m_workThread.Priority = ThreadPriority.Highest;
                m_workThread.Start();
            }
            else
            {   RunThread();    // 동기 no thread
            }
        }

        public bool IsFinished()
        {   return m_AllDone;
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
                List<sCustomRowList> ret = This.DoFileCheck(one);
                This.m_Result.AddRange(ret);

                This.m_nCurPos++;
                if (This.m_bCancel)
                    break;
            }

            This.m_bCancel = false;
            This.m_AllDone = true;
            This.m_workThread = null;
        }

        // 한개의 파일에 대해서 필터를 검사하고 결과를 리턴한다
        private List<sCustomRowList> DoFileCheck(string amcFilePath)
        {
            // data set 한개 생성
            DDataSet dataSet = new DDataSet();
            dataSet.folderPath = Path.GetDirectoryName(amcFilePath);

            // amc recv 로딩
            dataSet.AmcRcv.LoadFile(amcFilePath);

            List<sCustomRowList> aFileResult = new List<sCustomRowList>();
            sCustomRowList rowList = null;

            // 모든 row 탐색
            foreach (DataRow row in dataSet.AmcRcv.table.Rows)
            {
                bool bOutSel1 = (bool)row.ItemArray[(int)eAmcRcvIdx.Out2_OHT_Detect_Sel1];
                bool bOutSel2 = (bool)row.ItemArray[(int)eAmcRcvIdx.Out2_OHT_Detect_Sel2];
                bool bOutSel3 = (bool)row.ItemArray[(int)eAmcRcvIdx.Out2_OHT_Detect_Sel3];

                bool bInUbgLong = (bool)row.ItemArray[(int)eAmcRcvIdx.In2_OHT_Detect_Long];
                bool bInUbgMid = (bool)row.ItemArray[(int)eAmcRcvIdx.In2_OHT_Detect_Middle];
                bool bInUbgShort = (bool)row.ItemArray[(int)eAmcRcvIdx.In2_OHT_Detect_Short];

                bool bInObsLong = (bool)row.ItemArray[(int)eAmcRcvIdx.In1_OBS_Long];
                bool bInObsMid = (bool)row.ItemArray[(int)eAmcRcvIdx.In1_OBS_Middle];
                bool bInObsShort = (bool)row.ItemArray[(int)eAmcRcvIdx.In1_OBS_Short];

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
                if (bOBSDetected == false && nUBGLevel >= m_nUBGLevelRange_From && nUBGLevel <= m_nUBGLevelRange_To)
                {
                    if (rowList == null)
                    {
                        rowList = new sCustomRowList();   // row 뭉치 생성
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
                sCustomRowList data = aFileResult.ElementAt(i);
                if (data.Validation(m_nTolerance_msec, m_nToleranceTo_msec) == false)    // validation check
                    aFileResult.RemoveAt(i);
            }

            if (aFileResult.Count > 0)
            {  
                // User 로그 수집
                dataSet.UserLog.LoadFile(aFileResult.ElementAt(0).userFilePath);

                foreach (sCustomRowList oneResult in aFileResult)     // 한개의 결과뭉치. 한개의 파일에는 여러개의 결과뭉치가 있을 수 있다
                {
                    DataRow amcRow_start = oneResult.AmcData.ElementAt(0);
                    DataRow amcRow_end = oneResult.AmcData.ElementAt(oneResult.AmcData.Count - 1);

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

                // user 로그 분석 후 Node가 스페셜 노드에 있는게 아니라면 삭제한다
                if(m_JikJinNode.Count > 0)
                {
                    for (int i = aFileResult.Count - 1; i >= 0; i--)
                    {
                        sCustomRowList data = aFileResult.ElementAt(i);

                        if (data.Validation_Node(m_JikJinNode) == false)    // validation check
                            aFileResult.RemoveAt(i);
                    }
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

    public class sCustomRowList
    {
        public string amcFilePath = "";
        public string userFilePath = "";
        public DCustomTestRowData summary = new DCustomTestRowData();
        public List<DataRow> AmcData = new List<DataRow>();
        public List<DataRow> UserData = new List<DataRow>();

        public bool Validation(int nTole_msec, int nTole2_msec)
        {
            if (AmcData.Count < 1)
                return false;
            
            DateTime first = (DateTime)AmcData.ElementAt(0).ItemArray[(int)eAmcRcvIdx.Time_Date];
            DateTime end = (DateTime)AmcData.ElementAt(AmcData.Count-1).ItemArray[(int)eAmcRcvIdx.Time_Date];
            
            TimeSpan span = end - first;
            if (span.TotalMilliseconds < nTole_msec)    // 너무 짧음
                return false;

            if (span.TotalMilliseconds > nTole2_msec)    // 너무 김
                return false;


            //OBS 켜진시점과 비교

            return true;
        }

        
        public bool Validation_Node(List<int>nodeList)
        {
            int nNode = 0;
            try
            {
                nNode = summary.subItems[(int)eCustomTest.NodeStart];
            }
            catch
            {
                return false;
            }

            if (nNode == 0)
                return false;
            int nnn = nodeList.Find(o => o == nNode);
            if (nnn <= 0)
                return false;

            return true;
        }
        // summary 배열 변수를 꾸며준다
        public void MakeSummary()
        {
            int nLastIdx = AmcData.Count - 1;

            Object[] startRow = AmcData[0].ItemArray;
            Object[] endRow = AmcData[nLastIdx].ItemArray;
            
            summary.subItems[(int)eCustomTest.Folder] = Path.GetDirectoryName(amcFilePath);
            summary.subItems[(int)eCustomTest.AmcFile] = amcFilePath;
            summary.subItems[(int)eCustomTest.UserFile] = userFilePath;

            
            bool bInUbgLong = (bool)startRow[(int)eAmcRcvIdx.In2_OHT_Detect_Long];
            bool bInUbgMid = (bool)startRow[(int)eAmcRcvIdx.In2_OHT_Detect_Middle];
            bool bInUbgShort = (bool)startRow[(int)eAmcRcvIdx.In2_OHT_Detect_Short];
            int nLevel = DCustomTest_UBG.GetUBGLevel(!bInUbgLong, !bInUbgMid, !bInUbgShort);

            summary.subItems[(int)eCustomTest.UBGLevel_Start] = nLevel;  // start ubg level
                        
            bInUbgLong = (bool)endRow[(int)eAmcRcvIdx.In2_OHT_Detect_Long];
            bInUbgMid = (bool)endRow[(int)eAmcRcvIdx.In2_OHT_Detect_Middle];
            bInUbgShort = (bool)endRow[(int)eAmcRcvIdx.In2_OHT_Detect_Short];
            nLevel = DCustomTest_UBG.GetUBGLevel(!bInUbgLong, !bInUbgMid, !bInUbgShort);

            summary.subItems[(int)eCustomTest.UBGLevel_End] = nLevel;   // end ubg level
            DateTime startTime = (DateTime)startRow[(int)eAmcRcvIdx.Time_Date];
            DateTime endTime = (DateTime)endRow[(int)eAmcRcvIdx.Time_Date];
            TimeSpan span = endTime - startTime;

            summary.subItems[(int)eCustomTest.Time] = startTime;
            summary.subItems[(int)eCustomTest.Elapse] = span.TotalSeconds;
            

            int nUserCnt = UserData.Count;
            if(nUserCnt == 1)
            {
                summary.subItems[(int)eCustomTest.NodeStart] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_Node];
                summary.subItems[(int)eCustomTest.NodeEnd] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_Node];

                summary.subItems[(int)eCustomTest.OffsetStart] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                summary.subItems[(int)eCustomTest.OffsetEnd] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                summary.subItems[(int)eCustomTest.MoveDist] = "";    // move dist는 맵을 로딩해야 가능
            }
            else if(nUserCnt > 1)
            {
                summary.subItems[(int)eCustomTest.NodeStart] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_Node];
                summary.subItems[(int)eCustomTest.NodeEnd] = (int)UserData[nUserCnt-1].ItemArray[(int)eUserLogIdx.Current_Node];

                summary.subItems[(int)eCustomTest.OffsetStart] = (int)UserData[0].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                summary.subItems[(int)eCustomTest.OffsetEnd] = (int)UserData[nUserCnt - 1].ItemArray[(int)eUserLogIdx.Current_NodeOffset];
                summary.subItems[(int)eCustomTest.MoveDist] = "";    // move dist는 맵을 로딩해야 가능
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
