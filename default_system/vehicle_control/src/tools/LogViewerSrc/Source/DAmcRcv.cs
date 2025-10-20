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
using System.Drawing;


namespace LogViewer
{

    // 슬러시(/) 로 구분된 순서
    public enum eAmcRcvSplitedIdx
    {
        Time = 0
        , AmpEnable4Bit
        , Axis1_Following_Error
        , Axis2_Following_Error
        , Axis3_Following_Error
        , Axis4_Following_Error
        , Axis1_Position
        , Axis2_Position
        , Axis3_Position
        , Axis4_Position
        , Running4Bit
        , Axis1_Velocity
        , Axis2_Velocity
        , Axis3_Velocity
        , Axis4_Velocity
        , Output_1
        , Output_2
        , Output_3
        , Input_1
        , Input_2
        , Input_3
        , Node_Cnt
        , Node_Cnt_of_Pre_Check
        , Position_of_Driving_Tag
        , Position_of_Trans_Tag_First
        , Position_of_Trans_Tag_Final
        , Position_of_Driving_Tag_Pre_Check
        , Axis1_Voltage
        , Axis2_Voltage
        , Axis3_Voltage
        , Axis4_Voltage
        , PauseStatus
        , Source0
        , Source1
        , Source2
        , Source3
        , State0
        , State1
        , COUNTOF
    }


    public enum eAmcRcvIdx
    {
        // ##################################################################################################
        // ##################################################################################################
        //  ***** Naming 규칙 (중요) *****
        // 1. 언더바(_)로 Split 한다.
        // 2. 언더바(_)가 없다면 대분류와 소분류가 없는 값이다.
        // 3. 언더바(_)가 한개라면 [대분류_소분류] 로 된다.
        // 4. 언더바(_)가 두개 이상이라면 [대분류_소분류1_소분류2] 로 된다.

        // ex) [Time]  ==> Time  분류 없음
        // ex) [FollowingErr_Axis1]  ==> 대분류: FollowingErr,  소분류: Axis1
        // ex) [Input1_2]  ==> 대분류: In1,  소분류1: Hand,  소분류2: Alarm
        // ex) [Input1_3]  ==> 대분류: In1,  소분류1: Hand,  소분류2: R_Grip

           //  LineNo
            Time_Date
            , Time
            , Mode_Stat
			, Steer
			, Position      // dobule
            , Velocity      //double

            , AmpEnable_Axis1
            , AmpEnable_Axis2
            , AmpEnable_Axis3
            , AmpEnable_Axis4

            , FollowingErr_Axis1
            , FollowingErr_Axis2
            , FollowingErr_Axis3
            , FollowingErr_Axis4

            , Position_Axis1_pulse
            , Position_Axis2_pulse
            , Position_Axis3_pulse
            , Position_Axis4_pulse

			, Position_Axis1_mm
			, Position_Axis2_mm
			, Position_Axis3_mm
			, Position_Axis4_mm

			, Running_Axis1
            , Running_Axis2
            , Running_Axis3
            , Running_Axis4

            , Velocity_Axis1
            , Velocity_Axis2
            , Velocity_Axis3
            , Velocity_Axis4
            
            , NodeData_NodeCount_Cnt
            , NodeData_NodeCount_PreCheck
            , NodeData_Position_DrivingTag
            , NodeData_Position_TransTagFirst
            , NodeData_Position_TransTagFinal
            , NodeData_Position_DrivingTagPreCheck

            , Voltage_Axis1
            , Voltage_Axis2
            , Voltage_Axis3
            , Voltage_Axis4

			, OBS_Detect_Level		// obs IO 3개 조합해서 나타낸다
			, UBG_Detect_Level      // UBG IO 3개 조합해서 나타낸다

			, Output_OBS_PatternNo      // obs 출력 IO 3개 조합해서 나타낸다
			, Output_UBG_PatternNo      // UBG 출력 IO 3개 조합해서 나타낸다

			, AMC_Input1        // raw 값 그대로
            , AMC_Input2
            , AMC_Input3

            , AMC_Output1
            , AMC_Output2
            , AMC_Output3

            // Source0 값으로 AXIS 엠프 에러 정보 비트 분석
            , Axis1_Home_Switch
            , Axis1_Pos_Limit
            , Axis1_Neg_Limit
            , Axis1_Amp_Falut
            , Axis1_Acc_Limit
            , Axis1_Vel_Limit
            , Axis1_Reverse_Move
            , Axis1_X_NegLimit
            , Axis1_X_PosLimit
            , Axis1_Error_Limit
            , Axis1_PC_Command
            , Axis1_OutOf_Frames
            , Axis1_AmpPower_OnOff
            , Axis1_AbsComm_Error
            , Axis1_Inpos_Status
            , Axis1_RunStop_Command
            , Axis1_Collision_State

            , Axis2_Home_Switch
            , Axis2_Pos_Limit
            , Axis2_Neg_Limit
            , Axis2_Amp_Falut
            , Axis2_Acc_Limit
            , Axis2_Vel_Limit
            , Axis2_Reverse_Move
            , Axis2_X_NegLimit
            , Axis2_X_PosLimit
            , Axis2_Error_Limit
            , Axis2_PC_Command
            , Axis2_OutOf_Frames
            , Axis2_AmpPower_OnOff
            , Axis2_AbsComm_Error
            , Axis2_Inpos_Status
            , Axis2_RunStop_Command
            , Axis2_Collision_State

            , Axis3_Home_Switch
            , Axis3_Pos_Limit
            , Axis3_Neg_Limit
            , Axis3_Amp_Falut
            , Axis3_Acc_Limit
            , Axis3_Vel_Limit
            , Axis3_Reverse_Move
            , Axis3_X_NegLimit
            , Axis3_X_PosLimit
            , Axis3_Error_Limit
            , Axis3_PC_Command
            , Axis3_OutOf_Frames
            , Axis3_AmpPower_OnOff
            , Axis3_AbsComm_Error
            , Axis3_Inpos_Status
            , Axis3_RunStop_Command
            , Axis3_Collision_State

            , Axis4_Home_Switch
            , Axis4_Pos_Limit
            , Axis4_Neg_Limit
            , Axis4_Amp_Falut
            , Axis4_Acc_Limit
            , Axis4_Vel_Limit
            , Axis4_Reverse_Move
            , Axis4_X_NegLimit
            , Axis4_X_PosLimit
            , Axis4_Error_Limit
            , Axis4_PC_Command
            , Axis4_OutOf_Frames
            , Axis4_AmpPower_OnOff
            , Axis4_AbsComm_Error
            , Axis4_Inpos_Status
            , Axis4_RunStop_Command
            , Axis4_Collision_State

            //State0 값 보고 축별 EVENT 정보를 얻는다
            , AXIS1_EVENT_STOP
            , AXIS1_EVENT_ESTOP
            , AXIS1_EVENT_ABORT

            , AXIS2_EVENT_STOP
            , AXIS2_EVENT_ESTOP
            , AXIS2_EVENT_ABORT
            
            , PauseStatus
            , Source0
            , Source1
            , Source2
            , Source3
            , State0
            , State1

            // Input 1
            //, ___INPUT1___          // 구분자
            , Input1_1        // 나머지 다 boolean
            , Input1_2
            , Input1_3
            , Input1_4
            , Input1_5
            , Input1_6
            , Input1_7
            , Input1_8
            , Input1_9
            , Input1_10
            , Input1_11
            , Input1_12
            , Input1_13
            , Input1_14
            , Input1_15
            , Input1_16
            , Input1_17
            , Input1_18
            , Input1_19
            , Input1_20
            , Input1_21
            , Input1_22
            , Input1_23
            , Input1_24
            , Input1_25
            , Input1_26
            , Input1_27
            , Input1_28
            , Input1_29
            , Input1_30
            , Input1_31
            , Input1_32

            // In2
            //, ___INPUT2___ = 72   // 구분자
            , Input2_1
            , Input2_2
            , Input2_3
            , Input2_4
            , Input2_5
            , Input2_6
            , Input2_7
            , Input2_8
            , Input2_9
            , Input2_10
            , Input2_11
            , Input2_12
            , Input2_13
            , Input2_14
            , Input2_15
            , Input2_16
            , Input2_17
            , Input2_18
            , Input2_19
            , Input2_20
            , Input2_21
            , Input2_22
            , Input2_23
            , Input2_24
            , Input2_25
            , Input2_26
            , Input2_27
            , Input2_28
            , Input2_29
            , Input2_30
            , Input2_31
            , Input2_32

            // Input3
            //, ___INPUT3___ = 105  // 구분자
            , Input3_1
            , Input3_2
            , Input3_3
            , Input3_4
            , Input3_5
            , Input3_6
            , Input3_7
            , Input3_8
            , Input3_9
            , Input3_10
            , Input3_11
            , Input3_12
            , Input3_13
            , Input3_14
            , Input3_15
            , Input3_16 //기존 코드 위치.
            , Input3_17
            , Input3_18
            , Input3_19
            , Input3_20
            , Input3_21
            , Input3_22
            , Input3_23
            , Input3_24
            , Input3_25
            , Input3_26
            , Input3_27
            , Input3_28
            , Input3_29
            , Input3_30
            , Input3_31
            , Input3_32


            // Out1
            //, ___OUTPUT1___ = 138     // 구분자
            , Output1_1
            , Output1_2
            , Output1_3
            , Output1_4
            , Output1_5
            , Output1_6
            , Output1_7
            , Output1_8
            , Output1_9
            , Output1_10
            , Output1_11
            , Output1_12
            , Output1_13
            , Output1_14
            , Output1_15
            , Output1_16
            , Output1_17
            , Output1_18
            , Output1_19
            , Output1_20
            , Output1_21
            , Output1_22
            , Output1_23
            , Output1_24
            , Output1_25
            , Output1_26
            , Output1_27
            , Output1_28
            , Output1_29
            , Output1_30
            , Output1_31
            , Output1_32


            // Out2
            //, ___OUTPUT2___ = 171  // 구분자
            , Output2_1
            , Output2_2
            , Output2_3
            , Output2_4
            , Output2_5
            , Output2_6
            , Output2_7
            , Output2_8
            , Output2_9
            , Output2_10
            , Output2_11
            , Output2_12
            , Output2_13
            , Output2_14
            , Output2_15
            , Output2_16
            , Output2_17
            , Output2_18
            , Output2_19
            , Output2_20
            , Output2_21
            , Output2_22
            , Output2_23
            , Output2_24
            , Output2_25
            , Output2_26
            , Output2_27
            , Output2_28
            , Output2_29
            , Output2_30
            , Output2_31
            , Output2_32

            // Out3
            //, ___OUTPUT3___ = 204 // 구분자
            , Output3_1
            , Output3_2
            , Output3_3
            , Output3_4
            , Output3_5
            , Output3_6
            , Output3_7
            , Output3_8
            , Output3_9
            , Output3_10
            , Output3_11
            , Output3_12
            , Output3_13
            , Output3_14
            , Output3_15
            , Output3_16  //기존 코드 위치
            , Output3_17
            , Output3_18
            , Output3_19
            , Output3_20
            , Output3_21
            , Output3_22
            , Output3_23
            , Output3_24
            , Output3_25
            , Output3_26
            , Output3_27
            , Output3_28
            , Output3_29
            , Output3_30
            , Output3_31
            , Output3_32

            , COUNTOF   //
    }
    

    public class DAmcRcvRowItem
    {

        //public bool bValid = false;
        public SmallDateTime time = new SmallDateTime();
        //public int nLineNo = 0;
        //public string rawData = "";     // raw string data
        public dynamic[] subItems;   // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다

        public int DataCount = 0;
        public int InputCount = 0;
        public int OutputCount = 0;
        public int Input = 0;
        public int Output = 0;

        public DAmcRcvRowItem()
        {
            InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
            Input = (int)eAmcRcvIdx.Input1_1;
            Output = (int)eAmcRcvIdx.Output1_1;
            DataCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;
            subItems = new dynamic[DataCount];

        }
        public DAmcRcvRowItem(DAmcRcvRowItem row)
        {
            
            InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
            Input = (int)eAmcRcvIdx.Input1_1;
            Output = (int)eAmcRcvIdx.Output1_1;
            DataCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;
            subItems = new dynamic[DataCount];
            //this.bValid = row.bValid;
            this.time = row.time;
            //this.nLineNo = row.nLineNo;
            //this.rawData = row.rawData;
            this.subItems = row.subItems;
        }

        // ex) Output2_1  <= 이런식 으로 값이 넘어옴
        // 이걸 Out2.Shutter.R_Origin  <= 이렇게 '_'를 '.' 로 바꾸되 두개 이상 들어가지 않게 해야함. 
        // ex 대분류.소분류1.소분류2
        // bAsMultiLine 옵션 : 대/소 분류 사이에 . 대신 \n 넣어서 개행문자를 넣음. col 이름을 보기 편하게 하기 위한 옵션
        public static string GetHeaderName(eAmcRcvIdx idx, bool asMultiLine, string temp = "")
        {
            string strEnumName = idx.ToString();
            string[] arr = strEnumName.Split('_');
            char szSpliter = '.';

            if (asMultiLine)
            {
                szSpliter = '\n';
                strEnumName = temp;
            }
               

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

        public void Init()
        {
            time.SetAsToday();
            //nLineNo = 0;
            //bValid = false;
        }

        public dynamic GetData(eAmcRcvIdx idx)
        {
            int nIdx = (int)idx;
            return subItems[nIdx];
        }
    }
    

    public class DAmcRcv
    {
        public string filePath = "";
        public static char szSpliter = '/';
        public static double []WheelUnitPulse = { 114.7341619, 114.7341619, 114.7341619, 682.666667};     // 휠 변환값
        public static double[] Pulse2Unit = { 0.0063, -3.1771, 304.3292, -3 }; //Pulse to mm 변환에 필요한 값
        public static double[] SettingWheelPulse = { 1, 1, 1, 1 }; //셋팅 가능

        public void pulseSet(double[] source)
        {
            Array.Copy(source, SettingWheelPulse, 4);
        }

        public DataTable table = null;
        public bool bLoaded = false;

        public static int DataCount = 0;
        public static int InputCount = 0;
        public static int OutputCount = 0;
        public static int Input = 0;
        public static int Output = 0;

        public static int[] UBGSensorInput = new int[3];
        public static int[] OBSSensorInput = new int[3];
        public static int[] UBGSensorOutput = new int[4];
        public static int[] OBSSensorOutput = new int[4];

        // 각 Col 별로 색을 표시하는 DViewColor 객체 집합. DViewColor에서 셀 값 별로 어떤 색으로 보여줘야할지 결정한다
        static public DViewColor[] CellColor = new DViewColor[(int)eAmcRcvIdx.COUNTOF];
        public DAmcRcv()
        {
            //IO Parameter Load
            InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");

            UBGSensorInput = MyTools.GridHelper.LoadUBGSensorInput(UMain.INI_PATH, "IOSetting");
            OBSSensorInput = MyTools.GridHelper.LoadOBSSensorInput(UMain.INI_PATH, "IOSetting");

            UBGSensorOutput = MyTools.GridHelper.LoadUBSSensorOutput(UMain.INI_PATH, "IOSetting");
            OBSSensorOutput = MyTools.GridHelper.LoadOBSSensorOutput(UMain.INI_PATH, "IOSetting");

            Input = (int)eAmcRcvIdx.Input1_1;
            Output = (int)eAmcRcvIdx.Output1_1;
            DataCount = (int)eAmcRcvIdx.COUNTOF - 192 + InputCount + OutputCount;

            table = CreateTable();
			if (CellColor[0] == null)
			{
				for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
				{
					CellColor[i] = new DViewColor();
				}
			}
            
        }
        
        public void Clear()
        {
            filePath = "";
            if (table != null)
                table.Rows.Clear();
            bLoaded = false;
        }

        public DAmcRcvRowItem GetRowObj(int nRow)
        {
            DAmcRcvRowItem row = new DAmcRcvRowItem();

            if (table.Rows.Count <= nRow)
            {
                Debug.Assert(false, "Out of table range!");
                return row;
            }

            try
            {
                row.subItems = table.Rows[nRow].ItemArray;
            }
            catch (Exception)
            {
                Debug.Assert(false, "Getting table ItemArray failed!");
            }
            return row;
        }

        public object[] GetRow(int nRow)
        {
            if(table.Rows.Count <= nRow)
            {
                Debug.Assert(false, "Out of table range!");
                return null;
            }

            return table.Rows[nRow].ItemArray;            
        }
        public object GetCell(int col, int row)
        {
            if (table.Rows.Count <= row || table.Columns.Count <= col)
            {
                Debug.Assert(false, "Out of table range!");
                return null;
            }
            return table.Rows[row].ItemArray[col];
        }
    
        public void SetCell(int col, int row, object val)
        {
            if (table.Rows.Count <= row || table.Columns.Count <= col)
            {
                Debug.Assert(false, "Out of table range!");
                return;
            }
            table.Rows[row].ItemArray[col] = val;
        }

        // UserLog 모두 로딩 한 후에 시간대 별로 어떤 Mode, Stat 어떤 상태였는지 Amc 로그에도 같이 넣준다.
        // 테이블 볼 때 훨신 보기 편함
        public void SetUserModeStat(List<DUserLogEventChangeRowItem> eventChangeList)
        {
            if (eventChangeList.Count < 2)
                return;

            DUserLogEventChangeRowItem curUserLog = eventChangeList.ElementAt(0);
            DUserLogEventChangeRowItem nextUserLog = eventChangeList.ElementAt(1);

            DateTime curTime = curUserLog.subItems[(int)eEventChangeLogIdx.Time_Date];
            DateTime nextTime = nextUserLog.subItems[(int)eEventChangeLogIdx.Time_Date];

            string curStat = String.Format("{0}", curUserLog.subItems[(int)eEventChangeLogIdx.Mode]);
			string cCurSteer= String.Format("{0}", curUserLog.subItems[(int)eEventChangeLogIdx.Steer]);

			int nIdx = 0;

            for(int i=0; i<table.Rows.Count; i++)
            {
                DateTime amcTime = (DateTime)table.Rows[i].ItemArray[(int)eAmcRcvIdx.Time_Date];
                if(amcTime >= curTime && amcTime < nextTime)
                {
                    DataRow row = table.Rows[i];
                    
                    row[(int)eAmcRcvIdx.Mode_Stat] = curStat;
					row[(int)eAmcRcvIdx.Steer] = cCurSteer;
                    
                    //table.Rows[i].ItemArray[(int)eAmcRcvIdx.Mode_Stat] = curStat;
                }
                else if(amcTime < curTime)
                {
                    // 로그 시작부분이 user 로그보다 amc 로그가 먼저 기록됐을 경우, User로그 범위보다 빠른 시간대가 나올수 있는데
                    // 어느정도 차이 안나면 그냥 세팅 시킨다
                    TimeSpan span = amcTime - curTime;
                    if (span.TotalSeconds < 1)
                    {
                        DataRow row = table.Rows[i];
                        row[(int)eAmcRcvIdx.Mode_Stat] = curStat;
						row[(int)eAmcRcvIdx.Steer] = cCurSteer;

                        
                        //.ItemArray[2/*(int)eAmcRcvIdx.Mode_Stat*/] = curStat; // item array로 넣으니까 안들어가네 값이
                    }
                    else
                        continue;   // 너무 차이 많이나면 skip 시킴
                }
                else
                {
                   nIdx++;
                    if (nIdx + 1 >= eventChangeList.Count)
                        break;


                   curUserLog = eventChangeList.ElementAt(nIdx);
                   nextUserLog = eventChangeList.ElementAt(nIdx+1);
                    
                    curTime = curUserLog.subItems[(int)eEventChangeLogIdx.Time_Date];
                    nextTime = nextUserLog.subItems[(int)eEventChangeLogIdx.Time_Date];
                    
                    curStat = String.Format("{0}", curUserLog.subItems[(int)eEventChangeLogIdx.Mode]);
					cCurSteer = String.Format("{0}", curUserLog.subItems[(int)eEventChangeLogIdx.Steer]);					
					i--;
                }
            }
                
        }

        public bool LoadFile(string path)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf("AMC_RECV2") < 0)
                return false;
            
            // 한개 파일의 모든 라인 한번에 읽기
            string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

            if (allLines.Length == 0)
                return false;

            this.filePath = path;

            int nLineNo = 1;
            DAmcRcvRowItem row = new DAmcRcvRowItem();
            // 한줄 한줄 파싱하기
            for (int i = 0; i < allLines.Length; i++, nLineNo++)// (string line in allLines)
            {   
                //row.subItems[(int)eAmcRcvIdx.LineNo] = nLineNo;

                // 파싱
                if (!ParseLine(path, allLines[i], row, false))
                    continue;
                
                // table에 추가
                table.Rows.Add(row.subItems);
            }
            
            bLoaded = true;
            return true;
        }
        
        // 특정 시간부터 nSec 동안 데이터만 로딩한다
        public bool LoadFile_timeZone(string path, DateTime tmFrom, int nSec, bool bSetting)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf("AMC_RECV2") < 0)
                return false;

			if (File.Exists(path) == false)
				return false;

            // 한개 파일의 모든 라인 한번에 읽기
            string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

			DateTime tmTo = tmFrom.AddSeconds(nSec);
			

			double dGapsec = 0;
			int nStartLineNo = GMethod.FindLogTimeLineNo(allLines, tmFrom, out dGapsec);
			int nEndLineNo = GMethod.FindLogTimeLineNo(allLines, tmTo, out dGapsec);

			if (nStartLineNo < 0)
				return false;

			if (nEndLineNo < 0)
				nEndLineNo = allLines.Length - 1;

			if (nStartLineNo > nEndLineNo)
				return false;

			this.filePath = path;
			
            DAmcRcvRowItem row = new DAmcRcvRowItem();
            // 한줄 한줄 파싱하기
            for (int i = nStartLineNo; i <= nEndLineNo; i++)// (string line in allLines)
            {
                //row.subItems[(int)eAmcRcvIdx.LineNo] = nLineNo;

                // 파싱
                if (!ParseLine(path, allLines[i], row, bSetting))
                    continue;

                //DateTime curTime = row.subItems[(int)eAmcRcvIdx.Time_Date];

                //if (curTime > tmEnd)
                //{
                //    if (table.Rows.Count > 0) //    전날 밤12시, 이후날 새벽12시 이어질때 버그가 발생
                //        break;
                //    else
                //        continue;
                //}

                // 수집하는 시간 이내일 때만 수집
                //if (curTime > tmFrom)
                    table.Rows.Add(row.subItems);   // table에 추가
            }

			

			bLoaded = true;
            return true;
        }
        
        public static bool ParseLine(string filePath, string line, DAmcRcvRowItem row, bool bSetting)
        {
            // string line 으로 들어오는 값
            // ex) [03:59:59.343]/1111/107/-12709/0/-2/12719/12709/0/2/1000/55000/50000/0/0/408289280/5369104/1061/-268238563/-2005090177/12648641/0/0/0/0/0/0/330/512/1199/1199/0/0/0/0/0
            string[] arrSplit = line.Split(szSpliter);


            double[] WheelUnitCal = new double[4];

            if (!bSetting)
            {
                Array.Copy(WheelUnitPulse, WheelUnitCal, 4);               
            }
            else
            {          
                Array.Copy(SettingWheelPulse,  WheelUnitCal,  4);
            }


            // SplitedIdx 개수만큼 다 있지는 않더라도 최소한 volt 값까지는 있어야 온전한 line이라고 간주하자
            if (arrSplit.Length < (int)eAmcRcvSplitedIdx.Axis4_Voltage)
            {
                return false;
            }

            int nTmp = 0;

            // 구분자로 구분된 데이터 찾아서 변수에 저장
            for (int i = 0; i < (int)eAmcRcvSplitedIdx.COUNTOF; i++)
            {
                if (i >= arrSplit.Length)
                    continue;

                eAmcRcvSplitedIdx nIdx = (eAmcRcvSplitedIdx)i;
                string strVal = arrSplit[i];
                int nVal = 0;
                Int32.TryParse(strVal, out nVal);

                // 시간 이외의 모든 값들은 다 정수형이여야 한다. 정수값이 아니라면 pass 한다.
                // 그렇다고 해서 retrun false 하지 말고 해당 값만 skip 하는걸로
                if (nIdx != eAmcRcvSplitedIdx.Time && GMethod.IsDigit_Int(strVal) == false)
                    continue;

                

                switch (nIdx)
                {
                    
                    case eAmcRcvSplitedIdx.Time:
                        
						SmallDateTime smt = new SmallDateTime();
						// 시간 얻어오는게 실패하면 해당 line은 실패한것. 다른 값들 보지 않음.
						if (MyTools.GMethod.GetTimeFromLine(filePath, line, ref smt) == false)
								return false;

						DateTime tm = smt.GetDateTime();
						row.time.SetTime(tm);
                        
                        row.subItems[(int)eAmcRcvIdx.Time_Date] = tm;//row.time.GetTimeString();
                        row.subItems[(int)eAmcRcvIdx.Time] = smt.GetTimeString();
                        break;

                    case eAmcRcvSplitedIdx.AmpEnable4Bit:
                        {
                            if (strVal.Length != 4)
                                continue;

                            int n1 = 0, n2 = 0, n3 = 0, n4 = 0;

                            Int32.TryParse(strVal.Substring(0, 1), out n1);
                            Int32.TryParse(strVal.Substring(1, 1), out n2);
                            Int32.TryParse(strVal.Substring(2, 1), out n3);
                            Int32.TryParse(strVal.Substring(3, 1), out n4);

                            row.subItems[(int)eAmcRcvIdx.AmpEnable_Axis1] = n1;
                            row.subItems[(int)eAmcRcvIdx.AmpEnable_Axis2] = n2;
                            row.subItems[(int)eAmcRcvIdx.AmpEnable_Axis3] = n3;
                            row.subItems[(int)eAmcRcvIdx.AmpEnable_Axis4] = n4;
                            break;
                        }

                    case eAmcRcvSplitedIdx.Axis1_Following_Error:

                        row.subItems[(int)eAmcRcvIdx.FollowingErr_Axis1] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis2_Following_Error:

                        row.subItems[(int)eAmcRcvIdx.FollowingErr_Axis2] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis3_Following_Error:

                        row.subItems[(int)eAmcRcvIdx.FollowingErr_Axis3] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis4_Following_Error:

                        row.subItems[(int)eAmcRcvIdx.FollowingErr_Axis4] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis1_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis1_pulse] = nVal;
                        row.subItems[(int)eAmcRcvIdx.Position] = 0.0;

					    if (WheelUnitPulse[0] > 0) // zero div 방지
					    {
						    double dTmp_ = (int)Math.Round((double)nVal / WheelUnitPulse[0], 0);
						    row.subItems[(int)eAmcRcvIdx.Position] = dTmp_;
						    row.subItems[(int)eAmcRcvIdx.Position_Axis1_mm] = dTmp_;
					    }

                        break;

                    case eAmcRcvSplitedIdx.Axis2_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis2_pulse] = nVal;
						if (WheelUnitCal[1] > 0) // zero div 방지
						{
							double dTmp_ = (int)Math.Round((double)nVal / WheelUnitCal[1], 0);
							row.subItems[(int)eAmcRcvIdx.Position_Axis2_mm] = dTmp_;
						}
						break;

                    case eAmcRcvSplitedIdx.Axis3_Position:
                        //Pulse 값 계산식 적용
                        row.subItems[(int)eAmcRcvIdx.Position_Axis3_pulse] = nVal;
						if (WheelUnitCal[2] > 0) // zero div 방지
						{
                            //double dTmp_ = (int)Math.Round((double)nVal / WheelUnitPulse[2], 0);
                            double dVal = nVal/100000.0;
                            double dTmp_ = Pulse2Unit[0]*(double)Math.Pow(dVal, 3.0) + Pulse2Unit[1]*(double)Math.Pow(dVal, 2.0) + Pulse2Unit[2]*(double)dVal + Pulse2Unit[3];

                            //정규식에 의해 원점일때 -3까지 도출 가능 이 경우, 0으로 수정해서 출력
                            if (dTmp_ < 0) dTmp_ = 0;

                            row.subItems[(int)eAmcRcvIdx.Position_Axis3_mm] = Math.Round(dTmp_);
                        }
						break;

                    case eAmcRcvSplitedIdx.Axis4_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis4_pulse] = nVal;
						if (WheelUnitCal[3] > 0) // zero div 방지
						{
							double dTmp_ = (int)Math.Round((double)nVal / WheelUnitCal[3], 0);
							row.subItems[(int)eAmcRcvIdx.Position_Axis4_mm] = dTmp_;
						}
					break;
					
					case eAmcRcvSplitedIdx.Running4Bit:

                        {
                            if (strVal.Length != 4)
                                continue;

                            int n1 = 0, n2 = 0, n3 = 0, n4 = 0;

                            Int32.TryParse(strVal.Substring(0, 1), out n1);
                            Int32.TryParse(strVal.Substring(1, 1), out n2);
                            Int32.TryParse(strVal.Substring(2, 1), out n3);
                            Int32.TryParse(strVal.Substring(3, 1), out n4);

                            row.subItems[(int)eAmcRcvIdx.Running_Axis1] = n1;
                            row.subItems[(int)eAmcRcvIdx.Running_Axis2] = n2;
                            row.subItems[(int)eAmcRcvIdx.Running_Axis3] = n3;
                            row.subItems[(int)eAmcRcvIdx.Running_Axis4] = n4;
                            break;
                        }


                    case eAmcRcvSplitedIdx.Axis1_Velocity:

                       // row.subItems[(int)eAmcRcvIdx.Velocity_Axis1] = nVal;
                        row.subItems[(int)eAmcRcvIdx.Velocity] = 0.0;

						if (WheelUnitCal[0] > 0) // zero div 방지
						{
							row.subItems[(int)eAmcRcvIdx.Velocity_Axis1] = (int)Math.Round((double)nVal / WheelUnitCal[0], 0);
							row.subItems[(int)eAmcRcvIdx.Velocity] = (int)Math.Round((double)nVal / WheelUnitCal[0], 0);
						}


                        break;

                    case eAmcRcvSplitedIdx.Axis2_Velocity:
                        
                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis2] = 0.0;
                        if (WheelUnitCal[1] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis2] = (int)Math.Round((double)nVal / WheelUnitCal[1], 0);
                        break;

                    case eAmcRcvSplitedIdx.Axis3_Velocity:
                        
                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis3] = 0.0;
                        if (WheelUnitCal[2] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis3] = (int)Math.Round((double)nVal / WheelUnitCal[2], 0);
                        break;

                    case eAmcRcvSplitedIdx.Axis4_Velocity:

                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis4] = 0.0;
                        if (WheelUnitCal[3] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis4] = (int)Math.Round((double)nVal / WheelUnitCal[3], 0);
                        break;

                    case eAmcRcvSplitedIdx.Output_1:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output1] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = 32;
                        for (int bit = 0; bit < nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = Input + InputCount + bit;                            
                            row.subItems[e] = Convert.ToInt32(bVal);
                        }

                        break;

                    case eAmcRcvSplitedIdx.Output_2:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output2] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = 32;
                        for (int bit = 0; bit < nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = Input + InputCount+ 32 + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
						}

					break;

                    case eAmcRcvSplitedIdx.Output_3:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output3] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
                        nTmp -= 64;
                        for (int bit = 0; bit < nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = Input + InputCount + 64 + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					    }

                        break;

                    case eAmcRcvSplitedIdx.Input_1:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input1] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.Input1_32 - eAmcRcvIdx.Input1_1;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Input1_1 + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
						}

						//row.subItems[(int)eAmcRcvIdx.OBS_Detect_Level] = GetDetectLevel(row.subItems[(int)eAmcRcvIdx.Input1_29], row.subItems[(int)eAmcRcvIdx.Input1_30], row.subItems[(int)eAmcRcvIdx.Input1_31]);

                        break;

                    case eAmcRcvSplitedIdx.Input_2:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input2] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.Input2_32 - eAmcRcvIdx.Input2_1;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Input2_1 + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
						}

					
						break;

                    case eAmcRcvSplitedIdx.Input_3:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input3] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
                        nTmp -= 64;
                        for (int bit = 0; bit < nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Input3_1 + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					    }

                        break;

                    case eAmcRcvSplitedIdx.Node_Cnt:

                        row.subItems[(int)eAmcRcvIdx.NodeData_NodeCount_Cnt] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Node_Cnt_of_Pre_Check:

                        row.subItems[(int)eAmcRcvIdx.NodeData_NodeCount_PreCheck] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Position_of_Driving_Tag:

                        row.subItems[(int)eAmcRcvIdx.NodeData_Position_DrivingTag] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Position_of_Trans_Tag_First:

                        row.subItems[(int)eAmcRcvIdx.NodeData_Position_TransTagFirst] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Position_of_Trans_Tag_Final:

                        row.subItems[(int)eAmcRcvIdx.NodeData_Position_TransTagFinal] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Position_of_Driving_Tag_Pre_Check:

                        row.subItems[(int)eAmcRcvIdx.NodeData_Position_DrivingTagPreCheck] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis1_Voltage:

                        row.subItems[(int)eAmcRcvIdx.Voltage_Axis1] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis2_Voltage:

                        row.subItems[(int)eAmcRcvIdx.Voltage_Axis2] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis3_Voltage:

                        row.subItems[(int)eAmcRcvIdx.Voltage_Axis3] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis4_Voltage:

                        row.subItems[(int)eAmcRcvIdx.Voltage_Axis4] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.PauseStatus:

                        row.subItems[(int)eAmcRcvIdx.PauseStatus] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Source0:

                        row.subItems[(int)eAmcRcvIdx.Source0] = nVal;

                        // 모터에러를 GetAxisError() 함수로 얻어냄
                        nTmp = eAmcRcvIdx.Axis1_Collision_State - eAmcRcvIdx.Axis1_Home_Switch;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bBitVal = GetAxisError(nVal, bit);
                            int e = (int)eAmcRcvIdx.Axis1_Home_Switch + bit;
                            row.subItems[e] = bBitVal ? 1 : 0;
                        }
                        
                        break;

                    case eAmcRcvSplitedIdx.Source1:

                        row.subItems[(int)eAmcRcvIdx.Source1] = nVal;

                        // 모터에러를 GetAxisError() 함수로 얻어냄
                        nTmp = eAmcRcvIdx.Axis2_Collision_State - eAmcRcvIdx.Axis2_Home_Switch;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bBitVal = GetAxisError(nVal, bit);
                            int e = (int)eAmcRcvIdx.Axis2_Home_Switch + bit;
                            row.subItems[e] = bBitVal ? 1 : 0;
					}

                        break;

                    case eAmcRcvSplitedIdx.Source2:

                        row.subItems[(int)eAmcRcvIdx.Source2] = nVal;
                        // 모터에러를 GetAxisError() 함수로 얻어냄
                        nTmp = eAmcRcvIdx.Axis3_Collision_State - eAmcRcvIdx.Axis3_Home_Switch;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bBitVal = GetAxisError(nVal, bit);
                            int e = (int)eAmcRcvIdx.Axis3_Home_Switch + bit;
                            row.subItems[e] = bBitVal ? 1 : 0; ;
                        }
                        break;

                    case eAmcRcvSplitedIdx.Source3:

                        row.subItems[(int)eAmcRcvIdx.Source3] = nVal;
                        // 모터에러를 GetAxisError() 함수로 얻어냄
                        nTmp = eAmcRcvIdx.Axis4_Collision_State - eAmcRcvIdx.Axis4_Home_Switch;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bBitVal = GetAxisError(nVal, bit);
                            int e = (int)eAmcRcvIdx.Axis4_Home_Switch + bit;
                            row.subItems[e] = bBitVal ? 1 : 0; ;
                        }
                        break;

                    case eAmcRcvSplitedIdx.State0:

                        row.subItems[(int)eAmcRcvIdx.State0] = nVal;
                        
                        row.subItems[(int)eAmcRcvIdx.AXIS1_EVENT_STOP] = (nVal  == 1)  ? 1 : 0;
                        row.subItems[(int)eAmcRcvIdx.AXIS1_EVENT_ESTOP] = (nVal == 2) ? 1 : 0;
                        row.subItems[(int)eAmcRcvIdx.AXIS1_EVENT_ABORT] = (nVal == 3) ? 1 : 0;
                        
                        break;

                    case eAmcRcvSplitedIdx.State1:

                        row.subItems[(int)eAmcRcvIdx.State1] = nVal;

                        row.subItems[(int)eAmcRcvIdx.AXIS2_EVENT_STOP] = (nVal == 1) ? 1 : 0;
                        row.subItems[(int)eAmcRcvIdx.AXIS2_EVENT_ESTOP] = (nVal == 2) ? 1 : 0;
                        row.subItems[(int)eAmcRcvIdx.AXIS2_EVENT_ABORT] = (nVal == 3) ? 1 : 0;
                        break;
                }

            }
            int a = Input - 1  + InputCount;

            int OBSPatternNo = GetPatternNo(row.subItems[a + OBSSensorOutput[0]], row.subItems[a + OBSSensorOutput[1]],
                             row.subItems[a + OBSSensorOutput[2]], row.subItems[a + OBSSensorOutput[3]]);
            int UBGPatternNo = GetPatternNo(row.subItems[a + UBGSensorOutput[0]], row.subItems[a + UBGSensorOutput[1]],
                             row.subItems[a + UBGSensorOutput[2]], row.subItems[a + UBGSensorOutput[3]]);

            a = Input - 1;
            int OBSDetectLV = GetOBSDetectLevel(row.subItems[a + OBSSensorInput[0]], row.subItems[a + OBSSensorInput[1]], row.subItems[a + OBSSensorInput[2]]);
            int UBGDetectLV = GetUBGDetectLevel(row.subItems[a + UBGSensorInput[0]], row.subItems[a + UBGSensorInput[1]], row.subItems[a + UBGSensorInput[2]]);


            row.subItems[(int)eAmcRcvIdx.Output_OBS_PatternNo] = OBSPatternNo;   
            row.subItems[(int)eAmcRcvIdx.Output_UBG_PatternNo] = UBGPatternNo;
            row.subItems[(int)eAmcRcvIdx.OBS_Detect_Level] = OBSDetectLV;
            row.subItems[(int)eAmcRcvIdx.UBG_Detect_Level] = UBGDetectLV;

            return true;
        }
        
       
        // 대차센서, 장애물센서 감지상태에 따른 근접 값 리턴 Long Mid Short로 판단. 8 미감지 ~ 1바로정지
        public static int GetUBGDetectLevel(int nLong, int nMiddle, int nShort)
		{
			if (nLong == 1 && nMiddle == 1 && nShort == 1)
				return 8;
			if (nLong ==0 && nMiddle == 1 && nShort == 1)
				return 7;
			if (nLong == 0 && nMiddle == 0 && nShort == 1)
				return 6;
			if (nLong == 1 && nMiddle == 0 && nShort == 1)
				return 5;
			if (nLong == 1 && nMiddle == 0 && nShort == 0)
				return 4;
			if (nLong == 0 && nMiddle == 0 && nShort == 0)
				return 3;
			if (nLong == 0 && nMiddle == 1 && nShort == 0)
				return 1;

			return -1;
		}
        public static int GetOBSDetectLevel(int nLong, int nMiddle, int nShort)
        {
            if (nLong == 1 && nMiddle == 1 && nShort == 1)
                return 8;
            if (nLong == 0 && nMiddle == 1 && nShort == 1)
                return 7;
            if (nLong == 0 && nMiddle == 0 && nShort == 1)
                return 5;
            if (nLong == 0 && nMiddle == 0 && nShort == 0)
                return 3;

            return -1;
        }

        public static int GetPatternNo(int Sel1, int Sel2, int Sel3, int Sel4)
		{
			Sel1 = Sel1 == 0 ? 1 : 0;    //반전
			Sel2 = Sel2 == 0 ? 1 : 0;   
			Sel3 = Sel3 == 0 ? 1 : 0;
			Sel4 = Sel4 == 0 ? 1 : 0;

			int n1 = Sel4 * 8;
			int n2 = Sel3 * 4;
			int n3 = Sel2 * 2;
			int n4 = Sel1 * 1;
			return n1 + n2 + n3 + n4;	
		}
		private static bool GetAxisError(int sourceVal, int nPos)
        {
            if (nPos == 0) return (sourceVal & 0x0001) == 1 ? true :false;              //ST_HOME_SWITCH
            else if (nPos == 1) return (sourceVal & 0x0002) >= 1 ? true : false;             //ST_POS_LIMIT
            else if (nPos == 2) return (sourceVal & 0x0004) >= 1 ? true : false;             //ST_NEG_LIMIT
            else if (nPos == 3) return (sourceVal & 0x0008) >= 1 ? true : false;             //ST_AMP_FAULT
            else if (nPos == 4) return (sourceVal & 0x0010) >= 1 ? true : false;             //ST_A_LIMIT
            else if (nPos == 5) return (sourceVal & 0x0020) >= 1 ? true : false;             //ST_V_LIMIT
            else if (nPos == 6) return (sourceVal & 0x00C0) >= 1 ? true : false;             //ST_REVESE_MOVE
            else if (nPos == 7) return (sourceVal & 0x0040) >= 1 ? true : false;             //ST_X_NEG_LIMIT
            else if (nPos == 8) return (sourceVal & 0x0080) >= 1 ? true : false;             //ST_X_POS_LIMIT
            else if (nPos == 9) return (sourceVal & 0x0100) >= 1 ? true : false;             //ST_ERROR_LIMIT
            else if (nPos == 10) return (sourceVal & 0x0200) >= 1 ? true : false;             //ST_PC_COMMAND
            else if (nPos == 11) return (sourceVal & 0x0400) >= 1 ? true : false;             //ST_OUT_OF_FRAMES
            else if (nPos == 12) return (sourceVal & 0x0800) >= 1 ? true : false;             //ST_AMP_POWER_ONOFF
            else if (nPos == 13) return (sourceVal & 0x1000) >= 1 ? true : false;             //ST_ABS_COMM_ERROR
            else if (nPos == 14) return (sourceVal & 0x2000) >= 1 ? true : false;             //ST_INPOSITION_STATUS
            else if (nPos == 15) return (sourceVal & 0x4000) >= 1 ? true : false;             //ST_RUN_STOP_COMMAND
            else if (nPos == 16) return (sourceVal & 0x8000) >= 1 ? true : false;             //ST_COLLISION_STATE

            return false;
        }
        public static DataTable CreateTable()
        {
            DataTable theTable = new DataTable();
            int InputCount = MyTools.GridHelper.LoadInputNumber(UMain.INI_PATH, "IOSetting");
            int OutputCount = MyTools.GridHelper.LoadOutputNumber(UMain.INI_PATH, "IOSetting");
            // Column 생성
            int Input = (int)eAmcRcvIdx.Input1_1;
            int Output = (int)eAmcRcvIdx.Output1_1;
            string strColName="";
            for (eAmcRcvIdx i = 0; i < eAmcRcvIdx.COUNTOF; i++)
            {
                Type type = Type.GetType("System.Int32");
                
                if(i == eAmcRcvIdx.Time_Date)
                        type = Type.GetType("System.DateTime");
                else if (i == eAmcRcvIdx.Time)
                    type = Type.GetType("System.String");
                else if (i == eAmcRcvIdx.Mode_Stat)
                    type = Type.GetType("System.String");
				else if (i == eAmcRcvIdx.Steer)
					type = Type.GetType("System.Char");
				else if(i ==eAmcRcvIdx.Position || i == eAmcRcvIdx.Velocity)
                    type = Type.GetType("System.Double");
                else if( i >= eAmcRcvIdx.Input1_1 && i < eAmcRcvIdx.COUNTOF)
                    type = Type.GetType("System.Int32");
                else
                    type = Type.GetType("System.Int32");
                                

                if (i < eAmcRcvIdx.Input1_1)
                {
                    strColName = DAmcRcvRowItem.GetHeaderName(i, asMultiLine: false);
                    theTable.Columns.Add(strColName, type);
                }
                else if (i < eAmcRcvIdx.Output1_1)
                {
                    int n = (int)i;
                    if((n-Input)< InputCount)
                    {
                        strColName = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        string temp = strColName;
                        strColName = DAmcRcvRowItem.GetHeaderName(i, true, temp);
                        theTable.Columns.Add(strColName, type);
                    }
                }                    
                else
                {
                    int n = (int)i;
                    if ((n-Output) < OutputCount)
                    {
                        strColName = MyTools.GridHelper.LoadIOInfo(i, UMain.INI_PATH, "IOSetting");
                        string temp = strColName;
                        strColName = DAmcRcvRowItem.GetHeaderName(i, true, temp);
                        theTable.Columns.Add(strColName, type);
                    }
                }                   

                
            }
            return theTable;
        }

        /// <summary>
        ///  특정 시간에서 전, 후 몇초간의 데이터를 table로 리턴한다
        /// </summary>
        public DataTable GetTable_asTime(DateTime tm, int nBeforeSec, int nAfterSec, bool bTotalData = false)
        {
            // 전체 데이터를 다 달라
            if(bTotalData)
            {
                List<DataRow> rows = new List<DataRow>();
                DataTable newTable = CreateTable();

                foreach (DataRow row in table.Rows)
                {                    
                    var desRow = newTable.NewRow();
                    var sourceRow = row;
                    desRow.ItemArray = sourceRow.ItemArray.Clone() as object[];
                    newTable.Rows.Add(desRow);
                        
                }
                return newTable;
            }
            // 아니다. 원하는 시간대만 달라 
            else
            {
                DateTime tmFrom = tm.AddSeconds(nBeforeSec * (-1));
                DateTime tmTo = tm.AddSeconds(nAfterSec);

                List<DataRow> rows = new List<DataRow>();
                DataTable newTable = CreateTable();

                foreach (DataRow row in table.Rows)
                {
                    DateTime t = (DateTime)row[(int)eAmcRcvIdx.Time_Date];
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
           
        }
    }

}
