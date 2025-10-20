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
        // ex) [In1_Hand_Alarm]  ==> 대분류: In1,  소분류1: Hand,  소분류2: Alarm
        // ex) [In1_Hand_R_Grip]  ==> 대분류: In1,  소분류1: Hand,  소분류2: R_Grip

           //  LineNo
            Time_Date
            , Time
            , Mode_Stat
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

            , Position_Axis1
            , Position_Axis2
            , Position_Axis3
            , Position_Axis4

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
            , In1_Hand_Inpos        // 나머지 다 boolean
            , In1_Hand_Alarm
            , In1_Hand_R_Grip
            , In1_Hand_L_Grip
            , In1_Hand_Close
            , In1_Hand_Open
            , In1_Hand_Push
            , In1_Hoist_Home
            , In1_Hand_PIOGo
            , In1_EQPIO_LoadReq
            , In1_EQPIO_UnloadReq
            , In1_EQPIO_Ready
            , In1_EQPIO_HO_AVBL
            , In1_EQPIO_ES
            , In1_EQPIO_R_Go
            , In1_EQPIO_L_Go
            , In1_Steering_Front_L
            , In1_Steering_Rear_L
            , In1_Steering_Front_R
            , In1_Steering_Rear_R
            , In1_Sutter_F_Close
            , In1_Sutter_R_Close
            , In1_Sutter_F_Open
            , In1_Sutter_R_Open
            , In1_Sutter_F_Alarm
            , In1_Sutter_R_Alarm
            , In1_Sutter_F_Inpo
            , In1_Sutter_R_Inpo
            , In1_OBS_Long
            , In1_OBS_Middle
            , In1_OBS_Short
            , In1_OBS_Fail

            // In2
            //, ___INPUT2___ = 72   // 구분자
            , In2_OHT_Detect_Long
            , In2_OHT_Detect_Middle
            , In2_OHT_Detect_Short
            , In2_OHT_Detect_Fail
            , In2_LookDown_Detec
            , In2_LookDown_Fail
            , In2_Shift_Home
            , In2_STB_L_Detect
            , In2_STB_R_Detect
            , In2_UTB_Detect
            , In2_FOUP_Detect
            , In2_Oscillation_Osclill
            , In2_EMO_Press
            , In2_Bumper_Press
            , In2_Fan_Fail
            , In2_Hoist_Belt2
            , In2_TP_Enable
            , In2_HandTP_Connect
            , In2_Foup_Cover
            , In2_PositionSensor_Drv_Front
            , In2_PositionSensor_Drv_Pre
            , In2_PositionSensor_Trans_F
            , In2_PositionSensor_Trans_R
            , In2_BCR_Drv_OK
            , In2_BCR_Drv_NG
            , In2_BCR_Trans_OK
            , In2_BCR_Trans_NG
            , In2_HoistBelt_Val
            , In2_RailSupport_Home
            , In2_RailSupport_Work
            , In2_None_Revese1
            , In2_SMPS_Short

            // Input3
            //, ___INPUT3___ = 105  // 구분자
            , In3_CID_Link
            , In3_CID_Interlock
            , In3_CID_Completion
            , In3_CID_Status
            , In3_CID_Reserve1
            , In3_CID_Reserve2
            , In3_OutTrigger_F_Alarm
            , In3_OutTrigger_F_Home
            , In3_OutTrigger_F_Work
            , In3_OutTrigger_R_Alarm
            , In3_OutTrigger_R_Home
            , In3_OutTrigger_R_Work
            , In3_None_Reserved3
            , In3_None_Reserved4
            , In3_None_Reserved5
            , In3_None_Reserved6


            // Out1
            //, ___OUTPUT1___ = 138     // 구분자
            , Out1_Hand_Close
            , Out1_Hand_Open
            , Out1_Hand_Origin
            , Out1_Hand_Reset
            , Out1_Hand_Free
            , Out1_Hand_Reserve1
            , Out1_Hand_Reserve2
            , Out1_Hand_Reserve3
            , Out1_Hand_PIO_Sel
            , Out1_EQPIO_Valid
            , Out1_EQPIO_CS_0
            , Out1_EQPIO_CS_1
            , Out1_EQPIO_Reserve1
            , Out1_EQPIO_Reserve2
            , Out1_EQPIO_TR_Req
            , Out1_EQPIO_Busy
            , Out1_EQPIO_Complete
            , Out1_EQPIO_L_Sel
            , Out1_EQPIO_R_Sel
            , Out1_Steering_F_TQ0
            , Out1_Steering_F_TQ1
            , Out1_Steering_R_TQ0
            , Out1_Steering_R_TQ1
            , Out1_Steering_F_Dir
            , Out1_Steering_R_Dir
            , Out1_Shutter_F_Reset
            , Out1_Shutter_R_Reset
            , Out1_Shutter_F_Close
            , Out1_Shutter_R_Close
            , Out1_Shutter_F_Open
            , Out1_Shutter_R_Open
            , Out1_Shutter_F_Origin


            // Out2
            //, ___OUTPUT2___ = 171  // 구분자
            , Out2_Shutter_R_Origin
            , Out2_Shutter_F_Free
            , Out2_Shutter_R_Free
            , Out2_OBS_Sel1
            , Out2_OBS_Sel2
            , Out2_OBS_Sel3
            , Out2_OBS_Sel4
            , Out2_OBS_Power
            , Out2_OHT_Detect_Sel1
            , Out2_OHT_Detect_Sel2
            , Out2_OHT_Detect_Sel3
            , Out2_OHT_Detect_Sel4
            , Out2_OHT_Detect_Power
            , Out2_LookDown_Sel1
            , Out2_LookDown_Sel2
            , Out2_LookDown_Sel3
            , Out2_LookDown_Sel4
            , Out2_LookDown_Power
            , Out2_Lamp_Red
            , Out2_Lamp_Yellow
            , Out2_Lamp_Green
            , Out2_Hand_Power
            , Out2_Driving_Break
            , Out2_Safety_Clear
            , Out2_Safety_Reset
            , Out2_Hoist_Break
            , Out2_None_Reserve1
            , Out2_None_Reserve2
            , Out2_None_Reserve3
            , Out2_None_Reserve4
            , Out2_None_Reserve5
            , Out2_None_Reserve6

            // Out3
            //, ___OUTPUT3___ = 204 // 구분자
            , Out3_CID_L_Sel
            , Out3_CID_R_Sel
            , Out3_CID_Occupancy
            , Out3_CID_Pass_Com
            , Out3_CID_Abnormal
            , Out3_CID_Operation
            , Out3_CID_Common
            , Out3_CID_Reserve
            , Out3_OutTrigger_F_Start
            , Out3_OutTrigger_F_DIR
            , Out3_OutTrigger_F_Brake
            , Out3_OutTrigger_F_Speed
            , Out3_OutTrigger_R_Start
            , Out3_OutTrigger_R_DIR
            , Out3_OutTrigger_R_Brake
            , Out3_OutTrigger_R_Speed


            , COUNTOF   //= 237
    }
    

    public class DAmcRcvRowItem
    {

        //public bool bValid = false;
        public SmallDateTime time = new SmallDateTime();
        //public int nLineNo = 0;
        //public string rawData = "";     // raw string data
        public dynamic[] subItems = new dynamic[(int)eAmcRcvIdx.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다


        public DAmcRcvRowItem() { }
        public DAmcRcvRowItem(DAmcRcvRowItem row)
        {
            //this.bValid = row.bValid;
            this.time = row.time;
            //this.nLineNo = row.nLineNo;
            //this.rawData = row.rawData;
            this.subItems = row.subItems;
        }

        // ex) Out2_Shutter_R_Origin  <= 이런식 으로 값이 넘어옴
        // 이걸 Out2.Shutter.R_Origin  <= 이렇게 '_'를 '.' 로 바꾸되 두개 이상 들어가지 않게 해야함. 
        // ex 대분류.소분류1.소분류2
        // bAsMultiLine 옵션 : 대/소 분류 사이에 . 대신 \n 넣어서 개행문자를 넣음. col 이름을 보기 편하게 하기 위한 옵션
        public static string GetHeaderName(eAmcRcvIdx idx, bool asMultiLine)
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
        public DataTable table = null;
        public bool bLoaded = false;
		public DAmcRecvTimeLineAtt []ArrTimeLineAtt = new DAmcRecvTimeLineAtt[(int)eAmcRcvIdx.COUNTOF];

		// 각 Col 별로 색을 표시하는 DViewColor 객체 집합. DViewColor에서 셀 값 별로 어떤 색으로 보여줘야할지 결정한다
		static public DViewColor[] CellColor = new DViewColor[(int)eAmcRcvIdx.COUNTOF];
        public DAmcRcv()
        {
			for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
			{
				ArrTimeLineAtt[i] = new DAmcRecvTimeLineAtt();
			}
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
            string nextStat = String.Format("{0}", nextUserLog.subItems[(int)eEventChangeLogIdx.Mode]);
            int nIdx = 0;

            for(int i=0; i<table.Rows.Count; i++)
            {
                DateTime amcTime = (DateTime)table.Rows[i].ItemArray[(int)eAmcRcvIdx.Time_Date];
                if(amcTime >= curTime && amcTime < nextTime)
                {
                    DataRow row = table.Rows[i];
                    row[(int)eAmcRcvIdx.Mode_Stat] = curStat;

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
                    nextStat = String.Format("{0}", nextUserLog.subItems[(int)eEventChangeLogIdx.Mode]);

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
                if (!ParseLine(path, allLines[i], row))
                    continue;
                
                // table에 추가
                table.Rows.Add(row.subItems);
            }
            
            bLoaded = true;
            return true;
        }
        
        // 특정 시간부터 nSec 동안 데이터만 로딩한다
        public bool LoadFile_timeZone(string path, DateTime tmFrom, int nSec)
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
                if (!ParseLine(path, allLines[i], row))
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

        public static bool ParseLine(string filePath, string line, DAmcRcvRowItem row)
        {
            // string line 으로 들어오는 값
            // ex) [03:59:59.343]/1111/107/-12709/0/-2/12719/12709/0/2/1000/55000/50000/0/0/408289280/5369104/1061/-268238563/-2005090177/12648641/0/0/0/0/0/0/330/512/1199/1199/0/0/0/0/0
            string[] arrSplit = line.Split(szSpliter);

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

                        DateTime tm = new DateTime();
                        // 시간 얻어오는게 실패하면 해당 line은 실패한것. 다른 값들 보지 않음.
                        if (MyTools.SmallDateTime.GetTimeFromLine(filePath, line, out tm) == false)
                            return false;

                        row.time.SetTime(tm);
                        SmallDateTime smt = new SmallDateTime(tm);
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

                        row.subItems[(int)eAmcRcvIdx.Position_Axis1] = nVal;
                        row.subItems[(int)eAmcRcvIdx.Position] = 0.0;

                        if (WheelUnitPulse[0] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Position] = (int)Math.Round((double)nVal / WheelUnitPulse[0], 0);

                        break;

                    case eAmcRcvSplitedIdx.Axis2_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis2] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis3_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis3] = nVal;
                        break;

                    case eAmcRcvSplitedIdx.Axis4_Position:

                        row.subItems[(int)eAmcRcvIdx.Position_Axis4] = nVal;
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

						if (WheelUnitPulse[0] > 0) // zero div 방지
						{
							row.subItems[(int)eAmcRcvIdx.Velocity_Axis1] = (int)Math.Round((double)nVal / WheelUnitPulse[0], 0);
							row.subItems[(int)eAmcRcvIdx.Velocity] = (int)Math.Round((double)nVal / WheelUnitPulse[0], 0);
						}


                        break;

                    case eAmcRcvSplitedIdx.Axis2_Velocity:
                        
                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis2] = 0.0;
                        if (WheelUnitPulse[1] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis2] = (int)Math.Round((double)nVal / WheelUnitPulse[1], 0);
                        break;

                    case eAmcRcvSplitedIdx.Axis3_Velocity:
                        
                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis3] = 0.0;
                        if (WheelUnitPulse[2] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis3] = (int)Math.Round((double)nVal / WheelUnitPulse[2], 0);
                        break;

                    case eAmcRcvSplitedIdx.Axis4_Velocity:

                        row.subItems[(int)eAmcRcvIdx.Velocity_Axis4] = 0.0;
                        if (WheelUnitPulse[3] > 0) // zero div 방지
                            row.subItems[(int)eAmcRcvIdx.Velocity_Axis4] = (int)Math.Round((double)nVal / WheelUnitPulse[3], 0);
                        break;

                    case eAmcRcvSplitedIdx.Output_1:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output1] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.Out1_Shutter_F_Origin - eAmcRcvIdx.Out1_Hand_Close;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Out1_Hand_Close + bit;
							row.subItems[e] = Convert.ToInt32(bVal);
                        }

                        break;

                    case eAmcRcvSplitedIdx.Output_2:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output2] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.Out2_None_Reserve6 - eAmcRcvIdx.Out2_Shutter_R_Origin;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Out2_Shutter_R_Origin + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					}

                        break;

                    case eAmcRcvSplitedIdx.Output_3:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Output3] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.Out3_OutTrigger_R_Speed - eAmcRcvIdx.Out3_CID_L_Sel;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.Out3_CID_L_Sel + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					}

                        break;

                    case eAmcRcvSplitedIdx.Input_1:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input1] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.In1_OBS_Fail - eAmcRcvIdx.In1_Hand_Inpos;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.In1_Hand_Inpos + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					}

                        break;

                    case eAmcRcvSplitedIdx.Input_2:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input2] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.In2_SMPS_Short - eAmcRcvIdx.In2_OHT_Detect_Long;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.In2_OHT_Detect_Long + bit;
                            row.subItems[e] = Convert.ToInt32(bVal);
					}

                        break;

                    case eAmcRcvSplitedIdx.Input_3:

                        // raw 값 저장
                        row.subItems[(int)eAmcRcvIdx.AMC_Input3] = nVal;

                        // Bit Shift 해서 eColumn_AmcRcv 분류대로 값 얻어냄
                        nTmp = eAmcRcvIdx.In3_None_Reserved6 - eAmcRcvIdx.In3_CID_Link;
                        for (int bit = 0; bit <= nTmp; bit++)
                        {
                            bool bVal = (nVal >> bit & 0x1) == 1 ? true : false;
                            int e = (int)eAmcRcvIdx.In3_CID_Link + bit;
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

            return true;
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

            // Column 생성
            for (eAmcRcvIdx i = 0; i < eAmcRcvIdx.COUNTOF; i++)
            {
                Type type = Type.GetType("System.Int32");
                
                if(i == eAmcRcvIdx.Time_Date)
                        type = Type.GetType("System.DateTime");
                else if (i == eAmcRcvIdx.Time)
                    type = Type.GetType("System.String");
                else if (i == eAmcRcvIdx.Mode_Stat)
                    type = Type.GetType("System.String");
                else if(i ==eAmcRcvIdx.Position || i == eAmcRcvIdx.Velocity)
                    type = Type.GetType("System.Double");
                else if( i >= eAmcRcvIdx.In1_Hand_Inpos && i < eAmcRcvIdx.COUNTOF)
                    type = Type.GetType("System.Int32");
                else
                    type = Type.GetType("System.Int32");

                string strColName = DAmcRcvRowItem.GetHeaderName(i, asMultiLine: false);
                
                
                theTable.Columns.Add(strColName, type);
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
	public class DAmcRecvTimeLineAtt
	{
		public class GuideLine
		{
			public bool Use = false;
			public Color color = Color.Black;
			public double Val = -1;
			public string text = "";
			

		}
		public DAmcRecvTimeLineAtt()
		{
			for (int i = 0; i < guideLines.Length; i++)
				guideLines[i] = new GuideLine();
		}
		public bool bBoolType = true;
		public bool bReverse = false;

		public double dMin = -1;
		public double dMax = -1;
		public double dHeight = 100;
		public GuideLine[] guideLines = new GuideLine[3];
		
	}
}
