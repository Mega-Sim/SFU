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
    // UserLog 슬러시로 구분된 인자들
    public enum eUserLogSplittedIdx
    {
        Time = 0
       , Current_Node
       , Current_Station
       , Current_NodeOffset
       , Speed
       , Steetring
       , AP_Siglevel
       , HID_Edc
       , HID_Ecap
       , HID_Eout
       , HID_Iout
       , HID_Iac
       , HID_ErrCode
       , HID_PickupTemper
       , HID_IGBTTemper
       , HID_ECap1Temper
       , ChkDivInfo
       , Turn_Left
       , Turn_Right
       , Carriage_Load1
       , Carriage_LoadunloadHP
       , Comm_FailCnt
       , CPU
       , Mem
       , RunStat_SlowDownStop
       , RunStat_CorrectPosStop

       , COUNTOF
    }

    // mode 같은 경우는 슬러시로 구분도 안되고 Time 도 있는 그대로 보여주면 가독성이 안좋기 때문에
    // eUserLogSplittedIdx 와는 별개로 Table에 보여질 Hader 인덱스이다
    public enum eUserLogIdx
    {
        Time_Date = 0
        , Time
        , Gap
       , Mode
       , Current_Node
       , Current_Station
       , Current_NodeOffset
       , Speed
       , Accel          // 가속도. 파싱된 데이터가 아니고 코드상에서 계산해서 가공된 데이터.
       , Steetring
       , AP_Siglevel
       , HID_Edc
       , HID_Ecap
       , HID_Eout
       , HID_Iout
       , HID_Iac
       , HID_ErrCode
       , HID_PickupTemper
       , HID_IGBTTemper
       , HID_ECap1Temper
       , ChkDivInfo
       , Turn_Left
       , Turn_Right
       , Carriage_Load1
       , Carriage_LoadunloadHP
       , Comm_FailCnt
       , CPU
       , Mem
       , RunStat_SlowDownStop
       , RunStat_CorrectPosStop

       , COUNTOF
    }

    // 이벤트 변환 창에 대한 열정보 인덱스
    // 중간에 Elpase time이 들어가기 때문에 새로 만듬
    public enum eEventChangeLogIdx
    {
        Time_Date
        , Time
       , Mode
       , Node
       , Station
       , Speed
      , Elapse_Sec
       , COUNTOF
    }

    public class DUserLogEventChangeRowItem
    {
        public dynamic[] subItems = new dynamic[(int)eEventChangeLogIdx.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다
        public DUserLogEventChangeRowItem() { }
        public void SetData(DUserLogRowItem val, int nLineNo)
        {
            subItems[(int)eEventChangeLogIdx.Time_Date]                             =   val.subItems[ (int)eUserLogIdx.Time_Date ]       ;
           subItems[(int)eEventChangeLogIdx.Time]                             =   val.subItems[ (int)eUserLogIdx.Time ]       ;
           subItems[(int)eEventChangeLogIdx.Mode]                            =   val.subItems[ (int)eUserLogIdx.Mode  ]       ;
            //subItems[(int)eEventChangeLogIdx.Elapse_Sec] = 0;
           subItems[(int)eEventChangeLogIdx.Node]                 =   val.subItems[ (int)eUserLogIdx.Current_Node]       ;
           subItems[(int)eEventChangeLogIdx.Station]               =   val.subItems[  (int)eUserLogIdx.Current_Station]       ;
           subItems[(int)eEventChangeLogIdx.Speed]                           =   val.subItems[ (int)eUserLogIdx.Speed]       ;           
        }
        // ex) Out2_Shutter_R_Origin  <= 이런식 으로 값이 넘어옴
        // 이걸 Out2.Shutter.R_Origin  <= 이렇게 '_'를 '.' 로 바꾸되 두개 이상 들어가지 않게 해야함. 
        // ex 대분류.소분류1.소분류2
        // bAsMultiLine 옵션 : 대/소 분류 사이에 . 대신 \n 넣어서 개행문자를 넣음. col 이름을 보기 편하게 하기 위한 옵션
        public static string GetHeaderName(eEventChangeLogIdx idx, bool asMultiLine)
        {
            string strEnumName = idx.ToString();
            string[] arr = strEnumName.Split('_');
            char szSpliter = '.';

            if (asMultiLine)
                szSpliter = '\n';

            string ret = "";
            // 3개 이하일때는 그냥 replace하면 됨
            if (arr.Length <= 3)
            {
                ret = strEnumName.Replace('_', szSpliter);
            }
            else
            {
                // 3개가 넘어가면 앞에서부터 3개까지만 replace하고 그뒤는 그냥 둬야함
                int nPos = 0;
                for (int nCnt = 0; nCnt < 2; nCnt++)
                    nPos = strEnumName.IndexOf('_', nPos + 1);

                string strLeft = strEnumName.Substring(0, nPos + 1);
                string strRight = strEnumName.Substring(nPos + 1, (strEnumName.Length) - (nPos + 1));
                strLeft = strLeft.Replace('_', szSpliter);

                ret = strLeft + strRight;
            }
            return ret;
        }
    }

    public class DUserLogRowItem
    {
        public dynamic[] subItems = new dynamic[(int)eUserLogIdx.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다
        public SmallDateTime time = new SmallDateTime();
        public char mode1 = ' ';
        public char mode2 = ' ';

        public DUserLogRowItem() { }
        public DUserLogRowItem(DUserLogRowItem row)
        {
            this.time = row.time;
            this.subItems = row.subItems;
            this.mode1 = row.mode1;
            this.mode2 = row.mode2;
        }
        // ex) Out2_Shutter_R_Origin  <= 이런식 으로 값이 넘어옴
        // 이걸 Out2.Shutter.R_Origin  <= 이렇게 '_'를 '.' 로 바꾸되 두개 이상 들어가지 않게 해야함. 
        // ex 대분류.소분류1.소분류2
        // bAsMultiLine 옵션 : 대/소 분류 사이에 . 대신 \n 넣어서 개행문자를 넣음. col 이름을 보기 편하게 하기 위한 옵션
        public static string GetHeaderName(eUserLogIdx idx, bool asMultiLine)
        {
            string strEnumName = idx.ToString();
            string[] arr = strEnumName.Split('_');
            char szSpliter = '.';

            if (asMultiLine)
                szSpliter = '\n';

            string ret = "";
            // 3개 이하일때는 그냥 replace하면 됨
            if (arr.Length <= 3)
            {
                ret = strEnumName.Replace('_', szSpliter);
            }
            else
            {
                // 3개가 넘어가면 앞에서부터 3개까지만 replace하고 그뒤는 그냥 둬야함
                int nPos = 0;
                for (int nCnt = 0; nCnt < 2; nCnt++)
                    nPos = strEnumName.IndexOf('_', nPos + 1);
                
                string strLeft = strEnumName.Substring(0, nPos + 1);
                string strRight = strEnumName.Substring(nPos + 1, (strEnumName.Length) - (nPos + 1));
                strLeft = strLeft.Replace('_', szSpliter);

                ret = strLeft + strRight;
            }
            return ret;
        }
    }

    // 특정 노드 방문 시간
    public class DNodeVisite
    {
        public int ID = 0;
        public DateTime tm = new DateTime();
    }

    
    public class DUserLog
    {
        public string filePath = "";
        public static char szSpliter = '/';
        //public List<DUserRowItem> RowList = new List<DUserRowItem>();

        // mode 변경되는 시점만 기준으로 가지고 있는 리스트. 변경시점만 관리하면 몇개 안된다
        public List<DUserLogEventChangeRowItem> EventChangeList = new List<DUserLogEventChangeRowItem>();
        public DataTable table = null;
        public bool bLoaded = false;
        static public DViewColor[] CellColor = new DViewColor[(int)eUserLogIdx.COUNTOF];
        public List<DNodeVisite> passedNode = new List<DNodeVisite>();
        public List<DNodeVisite> passedStation = new List<DNodeVisite>();

        public DUserLog()
        {
            table = CreateTable();
            for (int i = 0; i < (int)eUserLogIdx.COUNTOF; i++)
            {
                CellColor[i] = new DViewColor();
            }
        }
        public void Clear()
        {
            filePath = "";
            if(table != null)
                table.Rows.Clear();
            EventChangeList.Clear();
            passedNode.Clear();
            passedStation.Clear();
            bLoaded = false;
        }

        public bool LoadFile(string path)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf("USER2") < 0)
                return false;
            
            // 한개 파일의 모든 라인 한번에 읽기
            string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

            if (allLines.Length == 0)
                return false;

            this.filePath = path;
            
            DUserLogRowItem row = new DUserLogRowItem();
            // 한줄 한줄 파싱하기
            for (int i = 0; i < allLines.Length; i++)// (string line in allLines)
            {
                if (!ParseLine(path, allLines[i], row, table.Rows.Count == 0 && bLoaded == false))
                    continue;

                table.Rows.Add(row.subItems);
            }
            bLoaded = true;
            return true;
        }
		// 폴더 로딩할때 다 로딩하지않고 Event Change 로그만 로딩한다
		public bool LoadOnlyEventChange(string []arrUserLogPath)
		{
			bool bFirst = true;
			for(int t=0; t<arrUserLogPath.Length; t++)
			{
				string path = arrUserLogPath[t];
				if (Path.GetFileName(path).ToUpper().IndexOf("USER2") < 0)
					continue;

				// 한개 파일의 모든 라인 한번에 읽기
				string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

				if (allLines.Length == 0)
					continue;

				this.filePath = path;

				DUserLogRowItem row = new DUserLogRowItem();
				// 한줄 한줄 파싱하기
				for (int i = 0; i < allLines.Length; i++)// (string line in allLines)
				{
					if (!ParseLine(path, allLines[i], row, bFirst))
						continue;

					bFirst = false;
					table.Rows.Add(row.subItems);
				}
				
			}
			bLoaded = true;
			return true;
		}
        // 특정 노드 위치를 얼마나 많이 돌아 다녔는지
        public static int HowManyTimePassed(List<int> thisNodes, string path)
        {
            DUserLog userLog = new DUserLog();
            if (userLog.LoadFile(path) == false)
                return 0;

            int nCnt = 0;
            foreach (int nNode in thisNodes)
            {
                foreach (DNodeVisite node in userLog.passedNode)
                {
                    if (node.ID == nNode)
                        nCnt++;
                }
            }
            return nCnt;
        }
        // 특정 시간부터 nSec 동안 데이터만 로딩한다
        public bool LoadFile_timeZone(string path, DateTime tmFrom, int nSec)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf("USER2") < 0)
                return false;

            // 한개 파일의 모든 라인 한번에 읽기
            string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

            if (allLines.Length == 0)
                return false;

            this.filePath = path;

            DateTime tmEnd = tmFrom;
            tmEnd = tmEnd.AddSeconds(nSec);

            DUserLogRowItem row = new DUserLogRowItem();
            // 한줄 한줄 파싱하기
            for (int i = 0; i < allLines.Length; i++)// (string line in allLines)
            {
                if (!ParseLine(path, allLines[i], row, table.Rows.Count == 0 && bLoaded == false))
                    continue;

                DateTime curTime = row.subItems[(int)eUserLogIdx.Time_Date];

                if (curTime > tmEnd)
                {
                    if (table.Rows.Count > 0) //    전날 밤12시, 이후날 새벽12시 이어질때 버그가 발생
                        break;
                    else
                        continue;
                }

                // 수집하는 시간 이내일 때만 수집
                if(curTime > tmFrom)
                    table.Rows.Add(row.subItems);
            }
            bLoaded = true;
            return true;
        }
        string strPrevStat = "";
        int nPrevNode = -1;
        int nPrevStation = -1;

        public bool ParseLine(string filePath, string line, DUserLogRowItem row, bool bFirstLine)
        {
            // string line 으로 들어오는 값
            // ex) [20:59:57.875]AG/691012/000000/190/915.16/L/6/300/299/300/18/47/0/40/42/30/1/1/0/0/1/1/0/27/0/0
            string[] arrSplit = line.Split(szSpliter);

            // SplitedIdx 개수만큼 다 있지는 않더라도 최소한  몇개는 있어야 온전한 line이라고 간주하자
            if (arrSplit.Length < 5)
            {
                return false;
            }

            // 구분자로 구분된 데이터 찾아서 변수에 저장
            for (int i = 0; i < (int)eUserLogSplittedIdx.COUNTOF; i++)
            {
                eUserLogSplittedIdx nIdx = (eUserLogSplittedIdx)i;
                string strVal = arrSplit[i];
                int nVal = 0;
                //UInt32 uVal = 0;
                double dVal = 0;
                Int32.TryParse(strVal, out nVal);
                Double.TryParse(strVal, out dVal);
                //UInt32.TryParse(strVal, out uVal);

                switch (nIdx)
                {
                    case eUserLogSplittedIdx.Time:
                        {
                            SmallDateTime tm = new SmallDateTime();
                            if (GMethod.GetTimeFromLine(filePath, line, ref tm) == false)
                            {
                                return false;
                            }

                            row.time = tm;
                            row.subItems[(int)eUserLogIdx.Time_Date] = tm.GetDateTime();    // DateTime
                            row.subItems[(int)eUserLogIdx.Time] = tm.GetTimeString();       // string

                            if (bFirstLine)
                                row.subItems[(int)eUserLogIdx.Gap] = 0;
                            else
                            {
                                DateTime tm1 = (DateTime)table.Rows[table.Rows.Count - 1].ItemArray[(int)eUserLogIdx.Time_Date];
                                DateTime tm2 = tm.GetDateTime();
                                TimeSpan span = tm2 - tm1;

                                row.subItems[(int)eUserLogIdx.Gap] = (int)span.TotalMilliseconds;
                            }
                            string[] arrSzModeSplit = strVal.Split(']');    // 왜 모드는 슬러시로 구분하지 않았을까 ㅠㅠ
                            if (arrSzModeSplit.Length != 2)
                                return false;

                            // 주행모드
                            row.mode1 = arrSzModeSplit[1].ElementAt(0);     // AG  or ME or Etc...
                            row.mode2 = arrSzModeSplit[1].ElementAt(1);
                            row.subItems[(int)eUserLogIdx.Mode] = String.Format("{0}{1}", row.mode1, row.mode2);
                            break;
                        }

                    // Uint32
                    case eUserLogSplittedIdx.Current_Node:
                        row.subItems[(int)eUserLogIdx.Current_Node] = nVal;
                        break;

                    case eUserLogSplittedIdx.Current_Station:
                        row.subItems[(int)eUserLogIdx.Current_Station] = nVal;
                        break;

                    // 정수
                    case eUserLogSplittedIdx.Current_NodeOffset:
                        row.subItems[(int)eUserLogIdx.Current_NodeOffset] = nVal;
                        break;

                    // 실수형
                    case eUserLogSplittedIdx.Speed:
                        row.subItems[(int)eUserLogIdx.Speed] = strVal;  // dVal;
                        break;

                    //char
                    case eUserLogSplittedIdx.Steetring:

                        if (strVal.Length == 1)
                        {
                            char streering = strVal.ElementAt(0);
                            row.subItems[(int)eUserLogIdx.Steetring] = streering;
                        }

                        break;

                    // 나머진 다 정수형
                    case eUserLogSplittedIdx.AP_Siglevel:
                        row.subItems[(int)eUserLogIdx.AP_Siglevel] = nVal; break;
                    case eUserLogSplittedIdx.HID_Edc:
                        row.subItems[(int)eUserLogIdx.HID_Edc] = nVal; break;
                    case eUserLogSplittedIdx.HID_Ecap:
                        row.subItems[(int)eUserLogIdx.HID_Ecap] = nVal; break;
                    case eUserLogSplittedIdx.HID_Eout:
                        row.subItems[(int)eUserLogIdx.HID_Eout] = nVal; break;
                    case eUserLogSplittedIdx.HID_Iout:
                        row.subItems[(int)eUserLogIdx.HID_Iout] = nVal; break;
                    case eUserLogSplittedIdx.HID_Iac:
                        row.subItems[(int)eUserLogIdx.HID_Iac] = nVal; break;
                    case eUserLogSplittedIdx.HID_ErrCode:
                        row.subItems[(int)eUserLogIdx.HID_ErrCode] = nVal; break;
                    case eUserLogSplittedIdx.HID_PickupTemper:
                        row.subItems[(int)eUserLogIdx.HID_PickupTemper] = nVal; break;
                    case eUserLogSplittedIdx.HID_IGBTTemper:
                        row.subItems[(int)eUserLogIdx.HID_IGBTTemper] = nVal; break;
                    case eUserLogSplittedIdx.HID_ECap1Temper:
                        row.subItems[(int)eUserLogIdx.HID_ECap1Temper] = nVal; break;
                    case eUserLogSplittedIdx.ChkDivInfo:
                        row.subItems[(int)eUserLogIdx.ChkDivInfo] = nVal; break;
                    case eUserLogSplittedIdx.Turn_Left:
                        row.subItems[(int)eUserLogIdx.Turn_Left] = nVal; break;
                    case eUserLogSplittedIdx.Turn_Right:
                        row.subItems[(int)eUserLogIdx.Turn_Right] = nVal; break;
                    case eUserLogSplittedIdx.Carriage_Load1:
                        row.subItems[(int)eUserLogIdx.Carriage_Load1] = nVal; break;
                    case eUserLogSplittedIdx.Carriage_LoadunloadHP:
                        row.subItems[(int)eUserLogIdx.Carriage_LoadunloadHP] = nVal; break;
                    case eUserLogSplittedIdx.Comm_FailCnt:
                        row.subItems[(int)eUserLogIdx.Comm_FailCnt] = nVal; break;
                    case eUserLogSplittedIdx.CPU:
                        row.subItems[(int)eUserLogIdx.CPU] = nVal; break;
                    case eUserLogSplittedIdx.Mem:
                        row.subItems[(int)eUserLogIdx.Mem] = nVal; break;
                    case eUserLogSplittedIdx.RunStat_SlowDownStop:
                        row.subItems[(int)eUserLogIdx.RunStat_SlowDownStop] = nVal; break;
                    case eUserLogSplittedIdx.RunStat_CorrectPosStop:
                        row.subItems[(int)eUserLogIdx.RunStat_CorrectPosStop] = nVal; break;


                }
                
            }

            // 모드가 바뀌면 EventChangeList에 추가
            if (bFirstLine)
            {
                DUserLogEventChangeRowItem eventRow = new DUserLogEventChangeRowItem();
                eventRow.SetData(row, table.Rows.Count);
                EventChangeList.Add(eventRow);

            }
            else
            {
                //Debug.WriteLine((string)row.subItems[(int)eUserLogIdx.Mode] + ", " + strPrevMode);
                if (row.subItems[(int)eUserLogIdx.Mode] != strPrevStat || row.subItems[(int)eUserLogIdx.Current_Node] != nPrevNode || row.subItems[(int)eUserLogIdx.Current_Station] != nPrevStation)
                {
                    DUserLogEventChangeRowItem eventRow = new DUserLogEventChangeRowItem();
                    eventRow.SetData(row, table.Rows.Count);
                    EventChangeList.Add(eventRow);
                }
            }

            int nNode = row.subItems[(int)eUserLogIdx.Current_Node];
            int nStation = row.subItems[(int)eUserLogIdx.Current_Station];

            if (nPrevNode != nNode && nNode > 0)
            {
                DNodeVisite visited = new DNodeVisite();
                visited.ID = nNode;
                visited.tm = row.subItems[(int)eUserLogIdx.Time_Date];
                passedNode.Add(visited);
            }
            if (nPrevStation != nStation && nStation > 0)
            {
                DNodeVisite visited = new DNodeVisite();
                visited.ID = nStation;
                visited.tm = row.subItems[(int)eUserLogIdx.Time_Date];
                passedStation.Add(visited);
            }
            
            strPrevStat = row.subItems[(int)eUserLogIdx.Mode];
            nPrevNode = nNode;  // row.subItems[(int)eUserLogIdx.Current_Node];
            nPrevStation = nStation;    // row.subItems[(int)eUserLogIdx.Current_Station];


            return true;

        } //end of function

        // 테이블을 자료형에 맞게 생성한다
        public static DataTable CreateTable()
        {
            DataTable theTable = new DataTable();

            // Column 생성
            for (eUserLogIdx i = 0; i < eUserLogIdx.COUNTOF; i++)
            {
                string strColName = DUserLogRowItem.GetHeaderName(i, asMultiLine : false);

                Type type = Type.GetType("System.Int32");

                if (i == eUserLogIdx.Time_Date)
                    type = Type.GetType("System.DateTime");
                else if (i == eUserLogIdx.Time)
                    type = Type.GetType("System.String");
                else if (i == eUserLogIdx.Mode)
                    type = Type.GetType("System.String");
                else if (i == eUserLogIdx.Speed)
                    type = Type.GetType("System.String");
                else if (i == eUserLogIdx.Steetring)
                    type = Type.GetType("System.Char");
                else
                    type = Type.GetType("System.Int32");

                strColName = DUserLogRowItem.GetHeaderName(i, asMultiLine: false);
                
                theTable.Columns.Add(strColName, type);
            }

            return theTable;
        }
        /// <summary>
        ///  특정 시간에서 전, 후 몇초간의 데이터를 table로 리턴한다
        /// </summary>
        public DataTable GetTable_asTime(DateTime tm, int nBeforeSec, int nAfterSec)
        {
            DateTime tmFrom = tm.AddSeconds(nBeforeSec * (-1));
            DateTime tmTo = tm.AddSeconds(nAfterSec);

            List<DataRow> rows = new List<DataRow>();
            DataTable newTable = CreateTable();

            foreach (DataRow row in table.Rows)
            {
                DateTime t = (DateTime)row[(int)eUserLogIdx.Time_Date];
                if (t > tmFrom && t <= tmTo)    // from ~ to 시간대 이내의 데이터들을 newTable로 Clone 시킨다
                {
                    var desRow = newTable.NewRow();
                    var sourceRow = row;
                    desRow.ItemArray = sourceRow.ItemArray.Clone() as object[];
                    newTable.Rows.Add(desRow);

                }
            }
            return newTable;
        }

        // 특정 시간대의 UserLog의 내용을 찾기
        // 모든 UserLog를 0~끝까지 찾으면 시간이 너무 오래걸림
        // 알고리즘 구현
        public DataRow GetRow_ofTheTime(DateTime theTime)
        {
            if (table.Rows.Count < 3)
                return null;

            DataRow drFirst = table.Rows[0];
            DataRow drLast = table.Rows[table.Rows.Count-1];
            DateTime tmFirst = (DateTime)drFirst[(int)eUserLogIdx.Time_Date];
            DateTime tmLast = (DateTime)drLast[(int)eUserLogIdx.Time_Date];

			// 00 시대의 로그 로딩시 전날 밤12:59~당일날 0시59분까지 로그가 쌓이기 때문에 시간만 놓고 보면 <미래~과거> 역전 현상이 생긴다
			// 그래서 이부분은 예외로 빼서 간주해야 한다
			if(tmFirst.Hour  == 23 && tmLast.Hour == 0)
			{
				tmFirst = tmFirst.AddDays(-1);	// 시작일을 하루 빼주면 역전현상을 막을 수 있다
			}

            TimeSpan spanTotal = tmLast - tmFirst;
            TimeSpan spanFromFirst = theTime - tmFirst;
            
            if (spanTotal.TotalSeconds < 1)
                return null;    //exception case
            
            // 찾고자 하는시간대가 UserLog 전체시간대에서 몇% 위치쯤에 있을지 대략적으로 알아내기
            double dPercent = spanFromFirst.TotalSeconds / spanTotal.TotalSeconds;      // 1 = 100%

            // percent range must be 0.0 ~ 1.0  
            if (dPercent < 0)
                dPercent = 0;
            if (dPercent > 1)
                dPercent = 1;

            // 대략적인 시작 인덱스를 결적한다. 무턱대고 0부터 검색할수 없어서
            int nStartIdx = (int)((table.Rows.Count-1) * dPercent);    //10000 * 0.1 = 1000
            DateTime tmStart = (DateTime)table.Rows[nStartIdx].ItemArray[(int)eUserLogIdx.Time_Date];
            
            bool bDir = theTime > tmStart;  // dir=true면 증가하면서 검색, false면 감소하면서 검색
            int nTotalCnt = table.Rows.Count;
            
            int nFoundIdx = -1;
            int nCycle = 0;

            // 검색 방행에 따라 검색
            if (bDir)
            {
                for(int i=nStartIdx ; i< nTotalCnt ; i++)
                {
                    DateTime tmCur = (DateTime)table.Rows[i].ItemArray[(int)eUserLogIdx.Time_Date];

                    // 검색하다가 시간차 부호가 바뀌는 순간
                    if(theTime < tmCur)
                    {
                        TimeSpan spanCur = theTime - tmCur;
                        int nPrevIdx = Math.Max(i - 1, 0);
                        TimeSpan spanPrev = (DateTime)table.Rows[nPrevIdx].ItemArray[(int)eUserLogIdx.Time_Date] - theTime;
                        
                        // 부호바뀐 순간 이전 상태가 현재 상태로 여기자. 
                        if (spanPrev.TotalSeconds < 3)  // 차이가 너무 심하지만 않다면
                            nFoundIdx = nPrevIdx;
                        
                        else if(spanCur.TotalSeconds < 3)   // 이전상태가 우선하지만, 이전상태가 안맞을 경우 현재상태와 시간차가 크지 않다면
                            nFoundIdx = i;
                        
                        break;
                    }
                    nCycle++;
                }
            }
            else
            {
                for (int i = nStartIdx ; i >= 0 ; i--)
                {
                    DateTime tmCur = (DateTime)table.Rows[i].ItemArray[(int)eUserLogIdx.Time_Date];

                    // 검색하다가 시간차 부호가 바뀌는 순간
                    if (theTime >= tmCur)
                    {
                        TimeSpan spanCur = theTime - tmCur;
                        int nNextIdx = Math.Max(i - 1, 0);
                        TimeSpan spanNext = (DateTime)table.Rows[nNextIdx].ItemArray[(int)eUserLogIdx.Time_Date] - theTime;

                        // 검색방향이 거꾸로기 때문에 현재상태가 우선함
                        if (spanCur.TotalSeconds < 3)   
                            nFoundIdx = i;

                        // 현재 상태가 너무 차이가 크다면 차선책으로 다음상태를 넣어준다
                        else if (spanNext.TotalSeconds < 3) 
                            nFoundIdx = nNextIdx;

                        break;
                    }
                    nCycle++;
                }
            }

            if (nFoundIdx < 0 || nFoundIdx >= table.Rows.Count)
                return null;
            
            return table.Rows[nFoundIdx];
        }
            
    }   //end of class

}// end of namespace
