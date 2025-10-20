using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace LogViewer
{
	class DLogVerConv
	{
        public static void init()
        {
            cid_contents_log.Clear();
            cid_numeric_log.Clear();
            driving_contents_log.Clear();
            driving_numeric_log.Clear();

            foreach(var a in TABLE_CID_STRING)
            {
                cid_contents_log.Add(ccn++, a); 
            }
            foreach(var a in TABLE_CID_V_STRING)
            {
                cid_numeric_log.Add(cnn++, a); 
            }
            foreach(var a in TABLE_DRIVING_STRING)
            {
                driving_contents_log.Add(dcn++, a); 
            }
            foreach(var a in TABLE_DRIVING_V_STRING)
            {
                driving_numeric_log.Add(dnn++, a); 
            }

        }
		private static char []TABLE_CHAR90 = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', '`', '~', '!', '@', '#', '$', '^', '&', '*', '(', ')', '_', '=', '+', '\\', '|', '[', '{', ']', '}', ';', ':', '\'', '\"', ',', '<', '.', '>', '?' };

        private static Dictionary<int, string> cid_contents_log = new Dictionary<int, string>();
        private static Dictionary<int, string> cid_numeric_log = new Dictionary<int, string>();
        private static Dictionary<int, string> driving_contents_log = new Dictionary<int, string>();
        private static Dictionary<int, string> driving_numeric_log = new Dictionary<int, string>();
        private static int ccn = 0, cnn = 0, dcn = 0, dnn = 0;

        private static string[] TABLE_CID_STRING = {
            "NONE",
            "[CID] Reset!!",
            "[CID] Jog Stop Key - CID occup On",
            "[CID] Jog Stop Key - CID occup Off",
            "[CID] MAN - R Link Off",
            "[CID] MAN - L Link Off",
            "[CID] (NORMAL RETURN) ELSE",
            "[CID] NormalCid_Return() ELSE",
            "[CID] CommonCid_Return() ELSE",
            "[CID] DD Command Received ",
            "[CID] S Command Data Receive Err",
            "[CID] M Command Data Receive Err",
            "[CID] Save_CID_O_Data",
            "[CID] Monitoring Parsing Error",
            "[CID] CID_Status REQ",
            "[CID] CID_Status Exception!!",
            "[CID] CID_Find_Stop_Reset exception 1",
            "[CID] CID_Find_Stop_Reset exception 2",
            "[CID] CID_Find_Stop_Reset exception 3",
            "[CID] CID_Find_Stop_Reset exception 1",
            "[CID] CID_Find_Stop_Reset exception 2",
            "[CID] CID_Find_Stop_Reset exception 3",
            "[CID] Manual -> Auto S/M to CID_STEP_CONNECTING",
            "[CID] Check CID Start. Abnormal Mode Off",
            "[CID] Now Common Reset Tag. O7 = 0 clear",
            "[CID] Common Reset Tag O7 current => not 1. wait",
            "[CID] Output Reset...",
            "[CID] OCS Pause Released.. MoveRestart",
            "[CID] Exception...",
            "[CID] CID_DIRECTION_RIGHT",
            "[CID] CID_Direction_Chk, [RIGHT] else case",
            "[CID] CID_DIRECTION_LEFT",
            "[CID] CID_Direction_Chk, [LEFT] else case",
            "[CID] CID_DIRECTION_COMMON_RIGHT",
            "[CID] CID_Direction_Chk, [RIGHT COMMON] else case",
            "[CID] CID_DIRECTION_COMMON_LEFT",
            "[CID] CID_Direction_Chk, [LEFT COMMON] else case",
            "[CID] CID_Direction_Chk ",
            "[CID] 작업변경 무시 m_pExecuteInfo->bChangeResult = false..",
            "[CID] 작업변경1 Pause Released",
            "[CID] CID_Occup_compt(5).. Completion_Not_ON",
            "[CID] CID_Occup_compt(5).. ACK delay",
            "[CID] 추가 필요 ??  m_pExecuteInfo->FlagCIDReport.bCIDStop = false;..",
            "[CID] Direction changed..",
            "[CID] Path changed..",
            "[CID] 연속 출력시, 작업변경 조건 안 맞아 대기 중..",
            "[CID] Direction .....Disconnect..",
            "[CID] Path is same before/after re-routing..",
            "[CID] 작업변경0_3 Pause Released",
            "[CID] CID_Occup_compt(13).. Completion_Not_ON",
            "[CID] CID_Occup_compt(13).. ACK delay",
            "[CID] 추가 필요 ??  m_pExecuteInfo->FlagCIDReport.bCIDStop = false;..",
            "[CID] Direction changed..",
            "[CID] Job change bit set before CID area...",
            "[CID] CID Direction and Path Direction are different...",
            "[CID] set bChangeResult = false..",
            "[CID] Slope CID_DIRECTION_LEFT",
            "[CID] Slope CID_DIRECTION_RIGHT",
            "[CID] Slope CID_DIRECTION_NOT_LINK",
            "[CID] Select_ON_Left!!!!!!!!!",
            "[CID] Select_ON_Right!!!!!!!!!",
            "[CID] CID Error reported..Moving to Abnormal step..",
            "[CID] RestNdoe = 0, Direction Change",
            "[CID] RestNdoe = 0, Direction Not Change",
            "[CID] CID_STEP_WAITING_FOR_PATH - else case",
            "[CID] OCS Pause Released...Moving to Checking Step..Check CID Map Offset!!",
            "[CID] OCS Pause Released...Moving to Completion Step",
            "[CID] OCS Resume 300cycle",
            "[CID] OCS Resume by CIDBypass",
            "[CID] OCS Resume",
            "[CID] OCS Resume by Link check",
            "[CID] Not Returning occupancy..(Step",
            "[CID] Returning occupancy due to detection status..(Step",
            "[CID] Slope -> Interlock Checked.. OHT Paused",
            "[CID] Completion Step, Interlock on/off else case",
            "[CID] Abnormal Step..Reconnected",
            "[CID] Abnormal Step..Arrived at Reset Tag",
            "[CID] Now Common Reset Tag. O7 = 1 set",
            "[CID] Abnormal Step...Now Common Reset Tag1. O7 = 1 set",
            "[CID] Abnormal Step...Now Common Reset Tag2. O7 = 1 set",
            "[CID] CID_Occup_Resume1",
            "[CID] Now Common Reset Tag. O7 = 1 set",
            "[CID] 작업변경 완료처리 시 common 처리 안함.",
            "[CID] Now Normal Reset Tag. O7 = 0 set Enforced",
            "[CID] Now Common Reset Tag. O7 = 1 set Enforced",
            "[CID] 작업변경시 완료처리는 common 처리 안함.",
            "[CID] Now Normal Reset Tag. O7 = 0 set Enforced",
            "[CID] CID_Find_Stop_Reset exception 3",
            "[CID] CID_Compare_List",
            "[CID] change CID MODE - MANUAL",
            "[CID] Abnormal 세트 (O5는 1로 설정)",
            "[CID] MAN - L Link Off",
            "[CID] R-Link_Manual On",
            "[CID] MAN - R Link Off",
            "[CID] L-Link_Manual On",
            "[CID] MAN CMD - Left ",
            "[CID] MAN CMD - Right ",
            "[CID] L-R-Link_Manual On",
            "[CID] Manual Reserve CID_Occup_OK",
            "[CID] Manual Reserve clear CID_Occup_return",
            "[CID] Manual_On",
            "[CID] Abnormal_off",
            "[CID] LEFT_SELECT",
            "[CID] RIGHT_SELECT",
            "[CID] Reset!!",
            "[CID] Now Manual Mode. executeGoInit() CID I/O Cleared. Abnormal Mode = On",
            "[CID] Now Auto Mode. executeGoInit()",
            "[CID] Manual -> Auto Step executeGoInit()",
            "[CID] executeGoCheckStop() CID_Occup_return",
            "[CID] CID_Mode_ON",
            "[CID] ManualMark OUT_CID_Right_Select_ON",
            "[CID] ManualMark OUT_CID_Left_Select_ON",
            "[CID] ManualMark CIDStop_1",
            "[CID] ManualMark CIDStop_2",
            "[CID] ManualMark No Occupancy",
            "[CID] ManualMark Unknown Node"
        };
        private static string[] TABLE_CID_V_STRING = {
            "NONE",
            "[CID] (NORMAL RETURN) ResetNode Changed CIDResetNode : %d, m_tmpCurrentResetNode %d",
            "[CID] Test1 tmpPreNodeID:%d, tmpCurrentNodeID:%d, m_tmpCurrentResetNode:%d",
            "[CID] Test2 tmpPreNodeID:%d, tmpCurrentNodeID:%d, m_tmpCurrentResetNode:%d",
            "[CID] (NORMAL RETURN) 1 Occup Compt,  CurrenrtNodeID : %d, CurrenrtNextID : %d, CurrentCMDID : %d, CIDStopNode1 : %d, CIDStopNode2 : %d, CIDResetNode : %d, CIDResetNodeType : %d",
            "[CID] CIDReset: %d, CIDResetType: %d, m_tmpCurrentResetNode: %d",
            "[CID] (COMMON RETURN) ResetNode Changed CIDResetNode : %d, m_tmpCurrentResetNode %d",
            "[CID] Common Bug - FrontMoving : %d RearMoving : %d IsLinking() : %d  m_bsecond : %d CIDResetNode_arr : %d  CIDResetNodeType : %d  m_CIDComptFlag : %d",
            "[CID] (COMMON RETURN) 1 Common_GO_CID_Occup_compt_IO, CurrenrtNodeID : %d, CurrenrtNextID : %d, CurrentCMDID : %d, CIDResetNode_arr : %d, ResetNodeType : %d",
            "[CID] CIDReset: %d, CIDResetType: %d, m_tmpCurrentResetNode: %d",
            "[CID]READ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d",
            "[CID] S Command Received : %c, %c, %c",
            "[CID] M Command Received : %d",
            "[CID] Test4 tmpPreNodeID:%d, tmpCurrentNodeID:%d, m_tmpCurrentResetNode:%d",
            "[CID] CID Monitoring Data index_tmp : %d, m_Diag_CID_index : %d",
            "[CID] CID Monitoring Data (OHT -> Agent ID:124) Received : %d Bytes",
            "[CID] CID Monitoring Data stop : %d, Reset : %d, OHT ID : %c%c%c%c%c%c,  CID_UniqueID : %c%c%c%c%c%c",
            "[CID] CID Monitoring m_Diag_CID_R_ID : %c%c%c%c%c%c",
            "[CID] VHL Name is : %s",
            "[CID] VHL Name is : %s",
            "[CID] pStopNode->ID : %d",
            "[CID] pResetNode->ID : %d",
            "[CID] Nearest StopNode : %d, Reset : %d, cycle : %d, TotalCount : %d, callNum : %d",
            "[CID] Nearest StopNode : %d, Reset(nNextNodeIDInfo) : %d, cycle : %d, TotalCount : %d, callNum : %d",
            "[CID] pStopNode->ID : %d",
            "[CID] pResetNode->ID : %d",
            "[CID] Nearest StopNode : %d, Reset : %d, cycle : %d, TotalCount : %d, callNum : %d",
            "[CID] Test Temp tmpPreNodeID:%d, tmpCurrentNodeID:%d",
            "[CID] Node : %d, Offset : %d",
            "[CID] Check Map --> CurrentNode: %d, Stop1: %d, Stop2: %d, Reset1: %d, Reset2: %d, ResetType: %d",
            "[CID] Start 1-1 : %d, 1-2 : %d, 1-3 : %d, 1-4 : %d, 1-5 : %d, Start 2-1 : %d, 2-2 : %d, 2-3 : %d, 2-4 : %d, 2-5 : %d ",
            "[CID] ConStep : %d, tmpNextNID : %d, tmpCurrentCMDID : %d, IsLinking : %d, m_CIDComptTryFlag : %d",
            "[CID] pNodeInfo->NodeType : %d",
            "[CID] Select ON not required...1 (Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Reset Node Info from CID Map and OCS are different..(Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Select ON not required...2 (Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Reset Node Info from CID Map and OCS are different..(Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Select ON not required...3 (Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Reset Node Info from CID Map and OCS are different..(Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Select ON not required...4 (Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Reset Node Info from CID Map and OCS are different..(Current: %d, Stop(CID): %d, Reset(CID): %d, Stop(OCS): %d, Reset(OCS): %d",
            "[CID] Connection Not Available...(Step : m_nCidControlStep), Current Node : %d)",
            "[CID] 작업변경1 Link 대기 %d, %d",
            "[CID] 작업변경1 %d, %d",
            "[CID] curr: %d, next: %d, stop1: %d, stop2:%d, reset1: %d, reset2: %d",
            "[CID] tmpDir: %d, m_CIDDir: %d",
            "[CID] executeInfo_stop: %d, executeInfo_reset: %d",
            "[CID] curr: %d, next: %d, stop1: %d, stop2:%d, reset1: %d, reset2: %d",
            "[CID] tmpDir: %d, m_CIDDir: %d",
            "[CID] executeInfo_stop: %d, executeInfo_reset: %d",
            "[CID] 작업변경 0_1 Reset : %d, Nearest : %d, tmpCurrent : %d, tmpNext : %d",
            "[CID] 작업변경 0_2 Stop : %d, Reset : %d, Stop : %d, Reset : %d )",
            "[CID] 작업변경 0_1 Reset : %d, Nearest : %d, tmpCurrent : %d, tmpNext : %d",
            "[CID] 작업변경 0_2 Stop : %d, Reset : %d, Stop : %d, Reset : %d )",
            "[CID] 작업변경0_3 Link 대기 %d, %d, %d, %d",
            "[CID] 작업변경0_3 Reset : %d, Nearest : %d, tmpCurrent : %d, tmpNext : %d",
            "[CID] CurrentID: %d, NextID: %d",
            "[CID] CID Monitoring Input LH m_Diag_CID_R_ID : %c%c%c%c%c%c, Reset: %d, Stop: %d, m_Diag_CID_index : %d",
            "[CID] CID_STEP_CONNECTING, Info->tmpPreNodeID : %d, Info->tmpCurrentNodeID %d",
            "[CID] m_CurrCIDStartnode: %d",
            "[CID] curr: %d, next: %d, stop1:%d, reset1: %d",
            "[CID] executeInfo_stop: %d, executeInfo_reset: %d, CIDNotUse: %d",
            "[CID] CID Monitoring Input LH m_Diag_CID_R_ID : %c%c%c%c%c%c, Reset: %d, Stop: %d, m_Diag_CID_index : %d",
            "[CID] Select_ON_Left(Step:CID_STEP_CONNECTING, Current Node : %d, Offset: %d )",
            "[CID] curr: %d, next: %d, stop2:%d, reset2: %d",
            "[CID] executeInfo_stop: %d, executeInfo_reset: %d, CIDNotUse: %d",
            "[CID] CID Monitoring Input RH m_Diag_CID_R_ID : %c%c%c%c%c%c, Reset: %d, Stop: %d, m_Diag_CID_index : %d",
            "[CID] Select_ON_Right(Step:CID_STEP_CONNECTING, Current Node : %d, Offset: %d )",
            "[CID] CID Direction not found..(Step:CID_STEP_CONNECTING, Current Node : %d)",
            "[CID] IO Abnormally ON(Step:CID_STEP_CONNECTING, Current Node : %d)",
            "[CID] Info->uLimitNode: %d,m_tmpCurrentResetNode :%d",
            "[CID] Slope CID_STEP_CHECKING_PATH -> CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] StopNode to not CID Node 1, Current Node : %d, NextNode : %d)",
            "[CID] Connection Completed..Moving to OCCUPYING Step_1(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID: %d)",
            "[CID] Connection Completed..Moving to OCCUPYING Step_2(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID: %d, Info->Safety_Level:%d)",
            "[CID] Connection Completed..Moving to OCCUPYING Step_2 else(Step:CID_STEP_WAITING_FOR_PATH, Current Node : %d, CMDID: %d, Info->bIgnolOverRun:%d)",
            "[CID] Connection Completed..Staying at CHECKING step_3(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID: %d)",
            "[CID] Connection Completed..Paused due to Interlock1(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] Connection Completed..Paused due to Interlock2(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] Connection Completed..Paused due to Interlock2 else (Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] Interlock ON..No need for OHT pause(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] m_CID_Pause NOT switched to true..(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] Connection Completed..Paused due to Interlock2(Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID] Try linking(Step:CID_STEP_CHECKING_PATH, Current Node : %d)",
            "[CID] Encoder Reading: %d, Map Offset: %d",
            "[CID] Encoder Reading: %d, Map Offset: %d, tmp_dist: %d",
            "[CID] Paused due to link fail 1 (Step:CID_STEP_CHECKING_PATH, Current Node : %d)",
            "[CID] Encoder Reading: %d, Map Offset: %d, tmp_dist: %d, total Offset: %d",
            "[CID] Paused due to link fail 2 (Step:CID_STEP_CHECKING_PATH, Current Node : %d)",
            "[CID] Select L/R NOT On yet...waiting(Step:CID_STEP_CHECKING_PATH, Current Node : %d)",
            "[CID] Passing Reset Tag..Move to COMPLETION step..(Step:CID_STEP_CHECKING_PATH, Current Node : %d)",
            "[CID] Interlock still on(Step:CID_STEP_WAITING_FOR_PATH, Current Node : %d)",
            "[CID] ERR_CID_CONNECTION_OFF_INTERLOCK_ON : Status: %d, %d, %d, time: %d, %d, %d)",
            "[CID] Interlock off..checking the path again(Step:CID_STEP_WAITING_FOR_PATH, Current Node : %d)",
            "[CID] Slope. not Occup --> move to CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] StopNode to not CID Node 2, Current Node : %d, NextNode : %d)",
            "[CID] StopNode to not CID Node 2, Info->CIDStopNode1 : %d, Info->CIDResetNode1 : %d, Info->CIDStopNode2 : %d, Info->CIDResetNode2 : %d )",
            "[CID] Detected and Not Occupying. (STEP : CID_STEP_OCCUPYING_PATH, Current Node : %d)",
            "[CID] Occupying..(Step:CID_STEP_OCCUPYING_PATH, Current Node : %d, tmpNextNodeID : %d)",
            "[CID] Check Map --> CurrentNode: %d, ResetNode1: %d, ResetNode2: %d",
            "[CID] Check Map --> CurrentNode: %d, ResetNode1: %d, ResetNode2: %d",
            "[CID] Intelock on detected while occupying..(Step:CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] Interlock on detected..(Step:CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] Returning occupancy due to tmpCurrentCMDID is Stopnode..(Step:CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] Check Map --> CurrentNode: %d, StopNode1: %d, StopNode2: %d, ResetNode1: %d, ResetNode2: %d",
            "[CID] Checking Interlock Signal..(Step:CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] CID 종료, %d",
            "[CID] Arrived at Reset Tag after Link Failure..Move to CONNECTING step(Step:CID_STEP_ABNORMAL, Current Node : %d)",
            "[CID] Abnormal Step, else if...m_bCIDNotUse: %d, m_bLinkFail: %d, m_bLinkDisconnected: %d, m_CIDComptTryFlag: %d, CIDResetNodeType: %d",
            "[CID] Abnormal Step, else...m_bCIDNotUse: %d, m_bLinkFail: %d, m_bLinkDisconnected: %d, m_CIDComptTryFlag: %d, CIDResetNodeType: %d",
            "[CID] OHT detected and connection lost while Occupying PASS_OK(Step:CID_STEP_ABNORMAL, Current Node : %d)",
            "[CID] Arrived at Reset Node while m_bCIDNotUse is on..(Step:CID_STEP_ABNORMAL, Current Node : %d)",
            "[CID] Completion Processing..(Step:CID_STEP_COMPLETION, Current Node : %d, tmpPreNodeID : %d, m_CIDComptTryFlag : %d)",
            "[CID] Curr: %d, CIDReset: %d, CIDResetType: %d, mTmpCurrentReset: %d",
            "[CID] completeCIDComm() m_CIDComptTryFlag: %d, nEnforcedSelectOffNodeID: %d",
            "[CID] Completion ACK: %d",
            "[CID] Link Lost...CID output terminated...Current Node : %d)",
            "[CID] Completion Processing..second trial(Step:CID_STEP_COMPLETION, Current Node : %d)",
            "[CID] CID 종료, %d",
            "[CID] Info->tmpCurrentNodeID: %d, nEnforcedSelectOffNodeID: %d, m_CurrCIDStartnode : %d, bCountOfCIDComptACK : %d",
            "[CID] CID timeout 종료, %d",
            "[CID] m_nCidControlStep : %d",
            "[CID] CID 종료 : 작업 변경 전 링크 안된 경우 종료처리 : %d",
            "[CID]SaveNdoe Reset1 : %d",
            "[CID]SaveNdoe Reset2 : %d",
            "[CID] Direction Change ListNode : %d, SaveNode : %d",
            "[CID] Not Change ListNode : %d, SaveNode : %d",
            "[CID] SteerFront : %d, SteerRear",
            "[CID] DRIVING_CTRL_STEP_GO_COMPLETE waiting.. %d, %d",
            "[CID] DRIVING_CTRL_STEP_GO_COMPLETE Fail...ComptTryFlag: %d, COMMON_bit_off_need: %d",
            "[CID] executeGoCheckStop() %d, %d, %d, %d",
            "[CID] StationData.NextNodeID : %d, CIDResetNode : %d",
            "[CID] Link Complete : %d    Interlock On : %d  Pass ACK : %d  Status : %d  NoRegist : %d // Left : %d  Right : %d  Reserve : %d  Pass : %d  Abnormal : %d  OpMode : %d  Common : %d  LinkReq : %d",
            "[CID] Link Complete : %d Interlock On : %d  Pass ACK : %d  Status : %d // Left : %d  Right : %d  Reserve : %d  Pass : %d  Abnormal : %d  OpMode : %d  Common : %d",
            "[CID] I/O ----> Link Complete : %d Interlock On : %d  Pass ACK : %d  Status : %d // Left : %d  Right : %d  Reserve : %d  Pass : %d  Abnormal : %d  OpMode : %d  Common : %d",
        };


        private static string[] TABLE_DRIVING_STRING = {
            "NONE",
            "Pre:  Init",
            "Pre:  Stop",
            "SetBrakeRelease Amp Fault",
            "SetInnerLimitTorque_90 Amp Fault",
            "[PreTrans-m_bShutterCloseMovingFlag] Shutter Close",
            "SetBrakeRelease Amp Fault",
            "SetInnerLimitTorque_90 Amp Fault",
            "[CheckAlways] Fail SetInnerLimitTorque_90",
            "SetInnerLimitTorque_90 Amp Fault",
            "bSlopeCheck Reset",
            "SetBrakeRelease false",
            "SetAMPEnable false",
            "m_pDrivingAxis SetSlaveAMPEnable false",
            "m_pDrivingAxis Reset",
            "m_pDrivingAxis ResetSlave",
            "m_pDrivingAxis SetHome",
            "m_pDrivingAxis SetSlaveHome",
            "SetSlaveAMPEnable true",
            "SetAMPEnable true",
            "SetBrakeRelease & SetSyncControl true",
            "[Reset] Fail SetInnerLimitTorque_90",
            "No Reset",
            "Pre:  Reset",
            "[Obstacle Sensor Fail] Sensor Power Start",
            "[Obstacle Sensor Fail] Sensor Power End",
            "TESTING1",
            "[MapMake] GetMapData false",
            "[AT]SetInnerLimitTorque_90 Amp Fault",
            "Pre:  MD",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_INIT Operation Mode 0 -> 1",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_INIT Abnormal 0 -> 1",
            "CID_Manual_On_1",
            "CID_Manual_On_2",
            "Pre:  MD Move",
            "[CID Manual] Operation Mode 0 -> 1",
            "[CID Manual] Abnormal 0 -> 1",
            "[CID_KYH_Manual] O8 ON",
            "STS_COMPLETE",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_CHECK_STOP Operation Mode 0 -> 1",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_CHECK_STOP Abnormal 0 -> 1",
            "[CID_KYH_Manual] O8 OFF",
            "CID_Manual_off",
            "CmdOn_MarkArea_Driving ->Station_Mark",
            "CmdOn_MarkArea_FollowCtrlOffArea",
            "CmdOn_MarkArea -> Run Step!!",
            "CmdOn_MarkArea",
            "InsertGen Step1",
            "Cmd On Ready",
            "set Station Offset",
            "setStationOffset None",
            "[InsertGen Exception!",
            "[ExecuteJob Exception!",
            "[setStationOffset] EXECUTE_ACK_NOT_AVAILABLE_CMD",
            "<<Exception7777>>",
            "[LIFTER-PRE] CheckPosition - Exception!!!!",
            "<<Exception7777>>",
            "경로 삭제 수행안함.",
            "CID Information Exception!!",
            "[CID_KYH] Passing merger with NO CID link_case 1",
            "[CID_KYH] Select retry",
            "Waiting",
            "[CID_KYH] Passing merger with NO CID link_case 2",
            "[CID_KYH] Link(O8) 시도중 통과허가 반납",
            "[CID_TEST1]",
            "[CID_TEST9]",
            "[CID_TEST8]",
            "SteeringCmd Front Obstacle Detect",
            "SteeringCmd Rear Obstacle Detect",
            "SteeringCmd Front Obstacle No-Detect",
            "SteeringCmd Rear Obstacle No-Detect",
            "SteeringCmd Front Obstacle Detect",
            "SteeringCmd Rear Obstacle Detect",
            "SteeringCmd Front Obstacle No-Detect",
            "SteeringCmd Rear Obstacle No-Detect",
            "경로 삭제 성공_1",
            "경로 삭제 성공_2",
            "경로 삭제 실패1",
            "경로 삭제 실패2",
            "경로 삭제 Exception",
            "======TakeOUT Start3333=====",
            "배출전 정위치",
            "빼냄 요구 ON",
            "빼냄 가능 ON",
            "빼냄 실패",
            "뺴냄요구 OFF",
            "전진 이동 가능 ON",
            "이동시작",
            "이동완료",
            "승강부 정위치 ON",
            "빼냄 완료 신호 Check",
            "배출 완료",
            "뺴냄 NG 완료",
            "TakeIn Start",
            "OCS 투입요청",
            "OCS 투입 가능",
            "OCS 투입 불가능 종료",
            "전진 이동 가능 ON",
            "이동시작",
            "이동완료",
            "MTL Out Node 정위치",
            "투입 완료 신호 Check",
            "투입 완료",
            "OCS 투입불가  Error",
            "======TakeOUT Start3333=====",
            "배출전 정위치",
            "빼냄 요구 ON",
            "빼냄 가능 ON",
            "빼냄 실패",
            "뺴냄요구 OFF",
            "전진 이동 가능 ON",
            "후진 이동 가능 ON",
            "이동시작",
            "이동완료",
            "승강부 정위치 ON",
            "빼냄 완료 신호 Check",
            "배출 완료",
            "뺴냄 NG 완료",
            "TakeIn Start",
            "OCS 투입요청",
            "OCS 투입 가능",
            "OCS 투입 불가능 종료",
            "전진 이동 가능 ON",
            "이동시작",
            "이동완료",
            "MTL Out Node 정위치",
            "투입 완료 신호 Check",
            "투입 완료",
            "OCS 투입불가  Error",
            "SetSlopeTime",
            "Release SlopeTime",
            "AxisRun Exception!!",
            "[MapMake] Init",
            "[MapMake] Marked",
            "[MapMake] MapMake_DrivingTag ON",
            "[MapMake] MapMake_DrivingTag OFF",
            "[MapMake] Init",
            "[STEP] DRIVING_CTRL_STEP_GO_PASSBOX",
            "[STEP] DRIVING_CTRL_STEP_GO_PREPARE",
            "[STEP] DRIVING_CTRL_STEP_GO_MARK",
            "[STEP] DRIVING_CTRL_STEP_GO_FIRST_NODE",
            "[STEP] DRIVING_CTRL_STEP_GO_RUN",
            "[STEP] DRIVING_CTRL_STEP_GO_RUN(Back)",
            "checkReadyToRun[executeGoPrepare] Fail",
            "[STEP] DRIVING_CTRL_STEP_GO_CHECK_STOP",
            "<<Exception8888>>",
            "[STEP] DRIVING_CTRL_STEP_GO_COMPLETE",
            "[checkCompletion_executeGoCheckStop] Fail SetInnerLimitTorque_90",
            "BackMove End",
            "clearDrivingCtrlInfoSet",
            "controlAxis: ClearNodeCountInfo",
            "controlAxis: Axis1 Reset",
            "controlAxis: Axis1 Home",
            "controlAxis: Axis2 Reset",
            "controlAxis: Axis2 Home",
            "Pre:  controlFront",
            "[Error][OverRunTrigger] nSecondCount>nNodeCount",
            "[Error][OverRunTrigger] bFirstPassed==true && bSecondPassed==true && bDetectFirst==false && bDetectSecond==false",
            "[Error][OverRunTrigger] bFirstMetTrigger==true && bSecondMetTrigger==true",
            "[Error][OverRunTrigger] PreMarkState.OnRoutineIn==false && bDetectSecond==true && bSecondMetTrigger==true",
            "markOnNode: OverRun - Recovery = true",
            "[OverRun Retry][ERR_MARK_RSTOPSEN_OFF]",
            "[OverRun][ERR_MARK_RSTOPSEN_OFF]",
            "Test KKS1",
            "Test KKS2",
            "Test KKS3",
            "Test KKS4",
            "Test KKS5",
            "bDetectSecond Step 1",
            "PreMarkState.FirstMet Step 1",
            "bStopWithoutFrontDetect Step 1",
            "bStopWithoutFrontDetect Step 2",
            "bDetectFirst Step 1",
            "bDetectFirst Step 2",
            "bDetectFirst Step 3",
            "[QR Mark] 2nd Met: Stoped and check ID",
            "[QR Mark] 1st Met: Stoped and check ID",
            "[QR Mark]On Routine: On Stopping without ID Readed",
            "[QR Mark] 2nd Met: Stoped and check ID",
            "[QR Mark] 1st Met: Stoped and check ID",
            "[QR Mark]On Routine: On Stopping without ID Readed",
            "[QR Mark] 2nd Met: Stoped and check ID",
            "[QR Mark] 1st Met: Stoped and check ID",
            "checkReadyToRun Slope",
            "checkReadyToRun No_Slope",
            "Detect Init(back)",
            "SetInnerLimitTorque_800",
            "SetBrakeRelease",
            "Complete:SetInnerLimitTorque_90",
            "checkReadyToRun[executeGoPrepare] Fail",
            "[manual_Mark1] SetInnerLimitTorque_90",
            "[checkCompletion_Manual Mark] Fail SetInnerLimitTorque_90",
            "bBreaked = true",
            "[Run]ArearByGuide",
            "[checkStartCmd] StartCmd Added",
            "[checkStartCmd] StartCmd Added",
            "[PreTrans] PIO Open",
            "[PreTrans] PIO Close",
            "[PreTrans] Shutter Close",
            "[PreTrans] Shutter Close Init",
            "[PreTrans] Shutter Open",
            "[PreTrans] Shutter Open Finish",
            "[PreTrans] Shutter Check"
        };

        private static string[] TABLE_DRIVING_V_STRING = {
            "NONE",
            "Ab Move Back Cnt:%f",
            "SteeringIO:%d%d",
            "SteeringIO:%d%d",
            "[FoupCover] %d %d",
            "[FoupCover] %d %d %d",
            "[Err FoupCover] IsDetect : %d, nFoupCoverOpenDetectCount : %d",
            "[ERR_OHT_DETECT] Abnormal %d %d",
            "DeleteCmd(%06d~%06d)",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "F:%d, R:%d",
            "[ERR_DRV_ABNORMAL_STOP_STEER_CROSS] bSteerFrontLeft: %d, bSteerRearLeft: %d, bSteerFrontRight: %d bSteerRearRight: %d",
            "[ERR_DRV_ABNORMAL_STOP] ObsStatus: %d, DetectStatus: %d, bSteerFrontLeft: %d bSteerRearLeft: %d, bSteerFrontRight: %d, bSteerRearRight: %d, Time: %d\n\tPos:%6.1lf(Node:P%6.1lf,N%6.1lf,P%3d,N%3d)(Tag:F%6.1lf,R%6.1lf)",
            "[ERR_NODE_COUNT_RESET_NOT_AVAILABLE] Node count : [%d], Pre-check count : [%d]",
            "[ERR_CURRNODE_INVALID] Node count : [%f], Pre-check count : [%f] Count [%d]",
            "[MapMake] GetMapData true %d ",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_INIT Direction Check (Current Node: %d)",
            "Manual명령 Type:%d New:%d a:d:v:s[%9.8lf:%9.8lf:%9.8lf:%8.1lf]",
            "[CID Manual] Direction Check (Current Node: %d)",
            "[CID Manual] Occupying.. (Current Node: %d)",
            "[CID Manual] DRIVING_CTRL_STEP_MANUAL_CHECK_STOP Direction Check (Current Node: %d)",
            "CmdOverDistLimit, %6.1lf + %6.1lf = %6.1lf > %6.1lf",
            "StationType : %d, TargetInfo.NextNode : %06d, AddInfo.FinalNode : %06d, byStatus[%d]",
            "[Driving]Overrun %d",
            "Driving Start %d",
            "Driving PreviousInfo Node%06d NextNode%06d Station %06d offset %f",
            "MapMake Start:%d %d",
            "[LIFTOUT_NTYPE] - Go.. %d",
            "[checkStartCmd] StartCmd Added TmpExecuteAck %d",
            "InsertGeneralCmd_NEW #%d , %d",
            "InsertGeneralCmd_NEW #%d , %d",
            "MarkFail_S..%d %d",
            "[setStationOffset] In: CmdType:%d, PreMark:%6d(%d)→%6X(%d:%6.1lf)[%6d~%6d]",
            "[setStationOffset] Result:Marked:%d Type:%d S(%d)N(%6d~%6d,%6.1lf)N(%6d),Pos(F:%6.1lf,R:%6.1lf)",
            "makeCmdInfoLists puting #%d , %d",
            "makeCmdInfoLists  Step1 %d",
            "makeCmdInfoLists  dDist %d   %d",
            "makeCmdInfoLists  dDistSumToEndNode %f, dLength %f, DistToStation %f",
            "makeCmdInfoLists  Step2 %d",
            "Before %X~%X : After %X~%X",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists  TmpTargetInfo.Type , %d",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists #%d , %d",
            "Success[%d] = [%d:%d:%d:%d:%d]",
            "makeCmdInfoLists #%d , %d",
            "[IGC]//N.Cnt:%d/Spd:%5.1lf/Str:%d/PreStr:%d/Dist:%5.1lf/End:%d/Target:%d/PassBoxIn:%d/dLength:%5.1lf Offset:%5.1lf/ListCnt %d/Type:%d/Offset(2Station:Start)[%5.1lf,%5.1lf], Saftey:%d",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists puting #%d , %d",
            "makeCmdInfoLists  Step1 %d",
            "makeCmdInfoLists  dDistSumToEndNode %f, dLength %f, DistToStation %f",
            "makeCmdInfoLists  Step2 %d",
            "makeCmdInfoLists  TmpTargetInfo.Type , %d",
            "makeCmdInfoLists Station ID %d dDistSumToEndNode %f, dLength %f, DistToStation %f",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists #%d , %d",
            "makeCmdInfoLists #%d , %d",
            "Success[%d] = [%d:%d:%d:%d:%d]",
            "makeCmdInfoLists #%d , %d",
            "[IGC]//N.Cnt:%d/Spd:%5.1lf/Str:%d/PreStr:%d/Dist:%5.1lf/End:%d/Target:%d/PassBoxIn:%d/ListCnt %d/Type:%d/Offset(2Station:Start)[%5.1lf,%5.1lf]",
            "makeCmdInfoLists #%d , %d",
            "setCompensationInfo: (%d)%5.1lf (%06d~%06d) --> %5.1lf (%06d~%06d)",
            "checkCmdInfoNodeList Success!%d",
            "CancelList NODE1 %d %d",
            "CancelList NODE2 %d %d",
            "CancelList STEER1 %d %d",
            "CancelList STEER2 %d %d",
            "CancelList1 Div %d %d",
            "CancelList Div2 %d %d",
            "CancelList DIRECTION1 %d %d",
            "CancelList DIRECTION2 %d %d",
            "CancelList SPEED1 %d %d",
            "CancelList SPEED2 %d %d",
            "CancelList PATH1 %d %d",
            "CancelList PATH2 %d %d",
            "makeList DIV %d %d %d:%d",
            "Edit ChangeNode : %d/%d/%d/%d/%d ->%d/%d/%d/%d/%d (%f)",
            "MakeList.. pItem->nPBSIgnoreFlag: %d, pItem->Type: 0x%x, DrivingInfo.Direction: 0x%x",
            "Cnt:%d/%d, ListCnt:%d",
            "SetFirstNodeCountAdd:%d/%d MarkType:%d: Offset:%f",
            "[checkPosition] OBS:%d, Detect:%d",
            "<<Compensation>>FollowErr : %6..1lf  (%d)",
            "<<Compensation>>dDistSum:%06f,dPreArriveOffset:%06f dDistErrorSum:%06f, dNodePosition:%06f, dCurrPos%06f,OHTDetcet:%d,OBS:%d<<Debug>>%d/%d(%06f/%06f)/%06f/%06f",
            "<<Compensation>>dDistSum:%06f,dPreArriveOffset:%06f dDistErrorSum:%06f, dNodePosition:%06f, dCurrPos%06f,OHTDetcet:%d,OBS:%d",
            "<PreCheck>(%06f/%06f)/%06f/%06f",
            "<<Compensation>>dCompensatedPosition:%06f,dOffset:%06f",
            "=======NodeID %d NodeDivInfo : %d NextDivInfo:%d",
            "[LIFTER-PRE] ==> execute pre! %d %d %d",
            "[LIFTER-PRE] ==> execute pre! %d",
            "Cnt:%d/%d, ListCnt:%d",
            "<<checkBackPosition>>:%d, %06f",
            "<<Compensation>>dDistSum:%06f,dPreArriveOffset:%06f dDistErrorSum:%06f, dNodePosition:%06f, dCurrPos%06f",
            "<<Compensation>> %d",
            "%x",
            "111  Info->Safety_Level %d, pSData->Safety_Level %d",
            "111 2 Info->Safety_Level %d, pSData->Safety_Level %d",
            "CID m_ClassifiedCmd.TargetInfo.StationSafetyLevel : %d",
            "[CIDTest3]m_tmpCurrentResetNode : %d, m_tmpCurrentResetNode : %d",
            "[CIDTest4]m_tmpCurrentResetNode : %d, m_tmpCurrentResetNode : %d",
            "[CIDTest1]m_tmpCurrentResetNode : %d, m_tmpCurrentResetNode : %d",
            "[CIDTest2]m_tmpCurrentResetNode : %d, m_tmpCurrentResetNode : %d",
            "[CID M Cmd Read] Read Ok. CID R : %c%c%c  CID O : %c%c%c",
            "[CID_TEST] 최종 노드 확인 : %d",
            "current node: %d, next node: %d",
            "stop node1: %d, stop node2: %d,",
            "reset node1: %d, reset node2: %d,",
            "start node1_1: %d, start node1_2: %d,",
            "start node2_1: %d, start node2_2: %d,",
            "m_pExecuteInfo->FlagCIDNotuse.bCIDUse: %d",
            "CIDStartNode1_arr:%d, CIDStartNode2_arr:%d, CIDResetNode_arr:%d, CIDResetNodeType: %d",
            "CIDNotuse.nStopNodeID : %d, CIDNotuse.nResetNodeID : %d",
            "[CID_Test] %d, %d",
            "m_nCidControlStep = %d, m_CIDComptTryFlag : %d",
            "[CID_KYH] Full registration, Current Node : %d)",
            "[CID_KYH] Connection Completed..Paused due to Interlock4 else (Step:CID_STEP_CHECKING_PATH, Current Node : %d, CMDID : %d)",
            "[CID_KYH] Instant I5 OFF, Current Node : %d)",
            "start node 1_1: %d, start node 2_1: %d",
            "start node 1_1: %d, start node 2_1: %d",
            "[CID_KTH] Passing Merger(STEP_WAITING, Current Node : %d)",
            "[CID_KYH] Passing Merger w/ I5 off(STEP_WAITING, Current Node : %d)",
            "[CID_TEST2]%d, %d, %d, %d, %d, %d",
            "[CID_TEST3]Save2 : %d, %d, %d, %d, %d, %d",
            "[CID_TEST4] %d, %d, %d",
            "[CID_TEST5] %d",
            "[CID_TEST10] %d, %d",
            "[CID_TEST6] %d, %d, %d, %d",
            "[CID_TEST7] %d, %d, %d, %d",
            "getSteerCmd: %d,%d,%d",
            "SteeringCmd bCmdExist=%d,pInfoSet->HwInfo.IO.SteerFrontDirCmd %d %d",
            "SteeringCmd bCmdCompleted=%d",
            "SteeringNotMatch[%d]: N(%d) M(%d:%d) F(%d!=%d) R(%d!=%d) A(%d:%d)",
            "SteeringCmd [F:%d:%d:%d:%d:%d/R:%d:%d:%d:%d:%d] F(%d:%d:%d) R(%d:%d:%d) ST:%4X NC(%d:%d:%d)",
            "SteeringCmd bCmdCompleted=%d",
            "SteeringNotMatch[%d]: M(%d:%d) F(%d!=%d) R(%d!=%d) A(%d:%d)",
            "SteeringCmd [F:%d:%d:%d:%d:%d/R:%d:%d:%d:%d:%d] F(%d:%d:%d) R(%d:%d:%d) ST:%4X NC(%d:%d:%d)",
            "경로 삭제 성공 CurrCount:%d, CmdRefInfo[Count:%d,nKeyCount:%d,Speed :%f]  CheckNode :%d, %f",
            "======TakeOUT Start %d, %d %d %d Error %d %d %d",
            "전진 이동 %d %d",
            "======TakeOUT Start %d, %d %d %d Error %d %d %d",
            "전진 이동 %d %d",
            "IsSlope %d",
            "checkServoTorqueFlag :Cnt:%d, ListCnt:%d",
            "checkServoTorqueFlag %d",
            "SetTorqueLimit %d",
            "[OnAbnormalStop.bFlag] Axis1Running:%d, ObsStatus:%d, DetectStatus:%d, dwTime:%d Pos:%6.1lf(P:%6.1lf,%3d,%d)(N:%6.1lf,%3d,%d)(F:%6.1lf,%d:%6.1lf,%d)",
            "DistNotMAtch Stop Speed:%f!",
            "DistNotMAtch %7.1lf (%7.1lf - %7.1lf) > %7.1lf",
            "Pre:%d Node:%06X",
            "%d %d %d %.4f %d %d",
            "%d %d %d %.4f %d %d",
            "Obs setting #2, RD(%7.1lf), TP(%7.1lf), CP(%7.1lf), ToDoAS(%d), NowAS(%d)",
            "Move Cmd %6.1lf,%6.1lf,%6.1lf,%6.1lf",
            "[checkAxisFirstNode] bFirstNIDIsCorrect become true (%6X==%6X) %6.11f %d",
            "TargetChange %d %d %d %d %d %d %d %d %d %d",
            "AxisRun Retry[%06d(%3d:%3d,%6.1lf)]:%6.4lf,%6.4lf, %3.1lf ,%6.1lf(←%6.1lf:E%6.1lf)Event[%4X]SPDN[(%6.1lf)]",
            "[PreTrans] Shutter: %d :PIO %d",
            "AxisMark[%06d(%7.4lf)]: %7.4lf, %7.4lf, %3.1lf, %7.1lf(←%7.1lf), %5.1lf [%5.1lf(%d):%5.1lf(%d)]",
            "checkAxisMark: (%d)%5.1lf (%06d~%06d) --> (%d)%5.1lf (%06d~%06d)",
            "[MapMake] BCR OK %d",
            "[MapMake] Type:%d, NID(%6d~%6d) Dist:%4d(%6.1lf=%6.1lf-%6.1lf) / Skip Dis(%d) / Dis Offset(%d) %d",
            "[MapMake][Abnormal]Station ID:%d !! dTmpCurrPosition -dPreStationCheckPosition",
            "[MapMake][Abnormal]CheckCountBCRRead %d, %f %f",
            "[MapMake][Abnormal]Case1 %d %f %f",
            "[MapMake][Abnormal]Case2 %d %f %f",
            "[MapMake]Type:%d, NID(%6d~%6d) / Station(ID:%6d,Offset:%4d(%6.1lf=%6.1lf-%6.1lf)) [%6.1lf:%6.1lf]Count%d",
            "[nCheckCountBCRRead] %d",
            "[MapMake]Type:%d, NID(%6d~%6d) / Station(ID:%6d,Offset:%4d(%6.1lf=%6.1lf-%6.1lf+%6.1lf))",
            "[MapMake] insertStationInfo OK STID:%d Offset%d",
            "[MapMake] insertStationInfo Delete(Not Match) STID:%d Offset%d",
            "[MapMake] insertStationInfo NG(NextNode) STID:%d Offset%d",
            "ERR_PASSBOX_PIO_GO_SIGNAL_CHECK_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d)",
            "ERR_PASSBOX_PIO_ES_SIGNAL_CHECK_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d)",
            "ERR_PASSBOX_PIO_PB_AVBL_SIGNAL_CHECK_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_GO_SIGNAL_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_GO_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_SIGNAL_OFF, m_nPassBoxPIOStep:(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_ES_SIGNAL_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_ES_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_PB_AVBL_SIGNAL_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_PB_AVBL_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_ABNORMAL_SIGNAL_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_CLOSE_SIGNAL_INIT_ABNORMAL_ON, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "ERR_PASSBOX_PIO_BUSY_SIGNAL_INIT_ABNORMAL_ON, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "ERR_PASSBOX_PIO_OPEN_SIGNAL_INIT_ABNORMAL_ON, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_OHT_STATUS, m_nPassBoxPIOStep(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_CLOSE_ON_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_CLOSE_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_DOOR_SELECT, m_nPassBoxPIOStep(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_OPEN_REQ, m_nPassBoxPIOStep(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_BUSY_ON_SIGNAL_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_BUSY_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_READY_ON, m_nPassBoxPIOStep(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_CLOSE_OFF_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_CLOSE_SIGNAL_ABNORMAL_ON, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_BUSY_OFF_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_BUSY_SIGNAL_ABNORMAL_ON, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[PASSBOX_PIO]PASSBOX_CTRL_STEP_PASSBOX_DOOR_OPEN_CHECK, m_nPassBoxPIOStep(%d)",
            "ERR_PASSBOX_PIO_OPEN_SIGNAL_ABNORMAL_OFF, m_nPassBoxPIOStep(%d), Err(%d), Time(%d)",
            "[executeRideEvevetor]Cnt:%d/%d, ListCnt:%d",
            "MarkType:%d/%d Offset:%6.1lf/%6.1lf SumDis :%f6.11f/%6.11f",
            "[STEP] DRIVING_CTRL_STEP_GO_RUN, Pos:R%5.1lf,C%5.1lf",
            "[STEP] DRIVING_CTRL_STEP_GO_MARK, Pos:R%5.1lf,C%5.1lf",
            "[STEP] DRIVING_CTRL_STEP_GO_MARK_BACK, Pos:R%5.1lf,C%5.1lf",
            "DRIVING_CTRL_STEP_GO_COMPLETE NodeCount: %d, DIV: %d",
            "EncoderDiff확인 : %d",
            "MapMake End:%d",
            "[STEP_Complete] DRIVING_CTRL_STEP_NONE Dis:%f",
            "TmpHwInfo.TagStaion.nBcrId : %X",
            "controlAxis: Axis1 SetAMPEnable: %d",
            "controlAxis: Axis2 SetAMPEnable: %d",
            "controlAxis: Axis MoveDistance: %f",
            "controlAxis: Axis1 Stop: %f",
            "controlSteer:F(%d:%d:%d)/R(%d:%d:%d)",
            "Change Node : NodeType(%d), UBG(%d), OBS(%d), Dist(%d), DistAfter(%d)",
            "ID Cmd:%X:%X",
            "markOnNode: Test %d %d %d %d:%d:%d ",
            "markOnNode:  nTmpID=%d:%d, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: nTmpID=%d:%d, Pos=%6.1lf, bIdReaded=%d, ID=%d, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode_Init: nTmpID = %d, bIdReaded=%d, Pos=%6.1lf, ID=%d, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "bFirstMetMoveDist = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode [bFirstMetTrigger] : 1st:%d,2nd:%d,NC:%d",
            "markOnNode [bSecondMetTrigger] : 1st:%d,2nd:%d,NC:%d",
            "markOnNode [bFirstMetTriggerWithFirstDetect] : %d:%6.1lf, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode [bFirstMetTriggerWithFirstDetect] Dist[%d]: %6.1lf:%6.1lf < %6.1lf  / Speed[%d]: %6.1lf > %6.1lf +2.0, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "[OBS1] Ignor : dRealDistSumToTarget(%6.1lf)-dRealPositionOfNow(%6.1lf)",
            "[OBS2] Ignor : dRealPositionOfFirstDetect(%6.1lf)(RDUT%6.1lf)-dRealPositionOfNow(%6.1lf)<50",
            "[OBS1] Off : dRealDistSumToTarget(%6.1lf)-dRealPositionOfNow(%6.1lf)",
            "[Error][ERR_NOTMATCH_TAGLENGTH_FINALNODE_NODE_MET_LATE] dRealPositionOfNow(%6.1lf)>(dRealDistSumToTarget(%6.1lf)+MarkParam.Dist.dForwardMoveDistLimit(%6.1lf)), Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "[Error][ERR_NOTMATCH_TAGLENGTH_FINALNODE_MOVE_BACK_LIMIT] dRealPositionOfNow(%6.1lf)<(dRealPositionOfMostPositive(%6.1lf)-MarkParam.Dist.dBackMoveDistLimit(%6.1lf), Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "[Error][OverRunTrigger] (dRealPositionOfNow=%f) > (dRealPositionOfSecondDetect=%f) + 20",
            "markOnNode: pMarkState->Arrived=true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: Recovery - OnMoveCmd = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: Recovery - Arrived = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: Recovery - IdCheck = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: Recovery - Recovery = false, Recovery Cnt : %d ,Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "[Error][ERR_NOTMATCH_TAGLENGTH_FINALNODE_IN_POSITION_ERROR] dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), dAddDistAfterSecondMet(%6.1lf), MarkInPosition_OnNode(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), bAxis1InPosition(%d)",
            "markOnNode: Recovery Not InPos: ( %6.1lf - %6.1lf ) < %6.1lf + %6.1lf ,  (%6.1lf , %d), Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: Recovery - OnMoveCmd = false, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: OverRun - OnMoveCmd = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: OverRun - OnMoveCmd = false, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: OverRun - AXIS_CTRL_CMD_MOVE_S, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: SecondMet - Arrived=true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf,T:%6.1lf)",
            "markOnNode: SecondMet - IdCheck=true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: SecondMet - OverRun = true, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: SecondMet Not InPos: fabs( %6.1lf - %6.1lf = %6.1lf )≒%6.1lf±%6.1lf ,  (%6.1lf , %d), Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: FirstMet==true - AXIS_CTRL_CMD_MOVE_DS, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: FirstMet==false - AXIS_CTRL_CMD_MOVE_DS, Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: bFirstMetMoveDist==true - AXIS_CTRL_CMD_MOVE_DS, Target:%6.1lf Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnNode: OnRoutineIn==false - AXIS_CTRL_CMD_MOVE_DS, [%d] Pos:%6.1lf(%6.1lf,%6.1lf,%6.1lf)",
            "markOnTagStation Init: Sensor[%d:%d-->%d] Target:%6d",
            "markOnTagStation bCheckFirstDetectInterlock [%f:%f:%f]",
            "markOnTagStation:  nTmpID=%d:%d",
            "markOnTagStation:  bIdReaded=%d, Pos=%6.1lf(Real:%6.11f)(F:%6.1lf,R:%6.1lf,S:%6.1lf), ID=%d:%d",
            "Test 1 %f %f",
            "dRealCheckFrontBackPoint:%6.11f/Target%6.11f",
            "markOnTagStation [bPauseSecondSmallMove] Dist[%d]: %6.1lf:%6.1lf < %6.1lf  / Speed[%d]: %6.1lf > %6.1lf+2.0",
            "[MARK] SetVHLDetectIgnor : %6.1lf",
            "[MARK] BCR READ Restart : %6.1lf",
            "[MARK] TRANS BCR Restart Time Check : %6.1lf",
            "[MARK] S/W TRANS BCR READ TRIGGER ON2 : %6.1lf",
            "[MARK] S/W TRANS BCR READ TRIGGER ON1 : %6.1lf",
            "[MARK] S/W TRANS BCR READ TRIGGER ON2 : %6.1lf",
            "[MARK] H/W Trans BCR Read Success : %6.1lf",
            "markOnTagStation: pMarkState->Arrived=true, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf T:%6.1lf)%d",
            "markOnTagStation: ERR_BCR_READTIMEOUT, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add: Abnormal SecondMet & Move Back 10, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add: SecondMet %7.1lf, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add: Stop (%6.1lf - %6.1lf = %6.1lf, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf))",
            "Small Add: SecondMet Not In Pos, bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "[Error][ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR] bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "[OverRun Retry][ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR] bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "[OverRun][ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR] bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "Recoverry: FirstMet %7.1lf, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "[Error][ERR_MARK_FSTOPSEN_OFF] bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "Small Add Move 2.5mm 2-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "[MARK] First Met! H/W TRANS BCR READ TRIGGER ON : %6.1lf",
            "Small Add Move 2.5mm 2-2, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move -2.5mm 2-4, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move 2.5mm 2-3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "MoveStop(OverRun) 3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf) (%6.1lf:%6.1lf:%6.1lf:%6.11f)",
            "MoveStop_2(OverRun) 4, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf) (%6.1lf:%6.1lf:%6.1lf)",
            "Eeception:MoveStop_2(OverRun) 3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf) (%6.1lf:%6.1lf:%6.1lf)",
            "Small Add Move -2.5mm 3-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move 125mm 3-2, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move 125mm 3-2-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move 125mm 3-3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "markOnTagStation Back Init: Sensor[%d:%d-->%d] Target:%6d %f",
            "dRealPositionOfMostPositive Back %6.11f",
            "markOnTagStation Back [bPauseSecondSmallMove] Dist[%d]: %6.1lf:%6.1lf < %6.1lf  / Speed[%d]: %6.1lf > %6.1lf+2.0",
            "markOnTagStation Back:  nTmpID=%d:%d",
            "markOnTagStation Back:  bIdReaded=%d, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf), ID=%d",
            "markOnTagStation Back:  bIdReaded=%d, nTmpID=%d",
            "markOnTagStation Back: pMarkState->Arrived=true, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf T:%6.1lf %d [%d %d])",
            "markOnTagStation Back: ERR_BCR_READTIMEOUT, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Move 125mm 3-2-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)",
            "Small Add Back: Abnormal SecondMet & Move Back 10, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf[%d %d])",
            "Small Add Back: SecondMet %7.1lf, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf[%d %d])",
            "Small Add Back: Stop (%6.1lf - %6.1lf = %6.1lf, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)[%d %d])",
            "Small Add Back: SecondMet Not In Pos, bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf) [%d %d]",
            "Back [Error][ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR] bAxis1InPosition(%d), dRealPositionOfNow(%6.1lf), dRealPositionOfSmallAddCheck(%6.1lf), MarkParam.Dist.dAddDistAfterSecondMet(%6.1lf), MarkParam.Dist.dMarkInPosLimit(%6.1lf)",
            "Small Add Move Back 2.5mm 2-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf [%d %d])",
            "Small Add back Move 2.5mm 2-2, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf[%d %d])",
            "Small Add Back Move 2.5mm 2-3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)[%d %d]",
            "MoveStop Back(OverRun) 3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf) (%6.1lf:%6.1lf:%6.1lf)[%d %d]",
            "Small Add Back Move -2.5mm 3-1, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)[%d %d]",
            "Small Add Back Move 125mm 3-2, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)[%d %d]",
            "Small Add Back Move 125mm 3-3, Pos=%6.1lf(F:%6.1lf,R:%6.1lf,S:%6.1lf)[%d %d]",
            "[QR Mark] 2nd Met: Arrived [Pos:%6.1lf ID:%6X, Offset:%6.1lf]",
            "Count ZigZag Position : %d",
            "[QR Mark] 2nd Met: ID Readed And move offset [Pos:%6.1lf ID:%6X, Offset:%6.1lf]",
            "[QR Mark] 2nd Met: ReEnable[%d]",
            "[QR Mark] 1st Met: ID readed and move offset [Pos:%6.1lf ID:%6d, Offset:%6.1lf]",
            "[QR Mark] On Routine: ID Readed And MoveStop [Pos:%6.1lf ID:%6d, Offset:%6.1lf]",
            "[QR Mark] On Routine: Stoped and Renable [Pos:%6.1lf]",
            "[QR Mark] On Routine: Stoped and go more(moveDS) to meet QR(%6.1lf) [Pos:%6.1lf]",
            "[QR Mark] Routine In: Move To Target Dist(%6.1lf) [Pos:%6.1lf]",
            "[QR Mark] 2nd Met: Arrived [Pos:%6.1lf ID:%6X, Offset:%6.1lf, STBG:%6.11f]",
            "Count ZigZag Position : %d",
            "[QR Mark] 2nd Met: ID Readed And move offset [Pos:%6.1lf ID:%6X, Offset:%6.1lf, STBG:%6.11f]",
            "[QR Mark] 2nd Met: ReEnable[%d]",
            "[QR Mark] 1st Met: ID readed and move offset [Pos:%6.1lf ID:%6d, Offset:%6.1lf, STBG:%6.11f]",
            "[QR Mark] On Routine: ID Readed And MoveStop [Pos:%6.1lf ID:%6d, Offset:%6.1lf, STB:%6.11f]",
            "[QR Mark] On Routine: Stoped and Renable [Pos:%6.1lf]",
            "[QR Mark] On Routine: Stoped and go more(moveDS) to meet QR(%6.1lf) [Pos:%6.1lf]",
            "[QR Mark] Routine In: Move To Target Dist(%6.1lf) [Pos:%6.1lf]",
            "[QR Mark] 2nd Met: Arrived [Pos:%6.1lf ID:%6X, Offset:%6.1lf]",
            "[QR Mark] 2nd Met: No MarkInPosLimit  [Pos:%6.1lf ID:%6X, Offset:%6.1lf]",
            "[QR Mark] 2nd Met: ID Readed And move offset [Pos:%6.1lf ID:%6X, Offset:%6.1lf]",
            "[QR Mark] 2nd Met: ReEnable[%d]",
            "[QR Mark] 1st Met: ID readed and move offset [Pos:%6.1lf ID:%6d, Offset:%6.1lf]",
            "[QR Mark] On Routine: ID Readed And MoveStop [Pos:%6.1lf ID:%6d, Offset:%6.1lf]",
            "[QR Mark] On Routine: Stoped and go more(moveDS) to meet QR(%6.1lf) [Pos:%6.1lf]",
            "[QR Mark] Routine In: Move To Target Dist(%6.1lf) [Pos:%6.1lf]",
            "checkReadyToRun: %d=Ax%d & Sr%d & FD%d VDS %d LiTor %d// %d %d %d//[F:%6.1lf(%d),R:%6.1lf(%d)] NC:%d %d %d",
            "[Station Mark] Rear Sensor Check OK(%d)",
            "[Station Mark] Rear Sensor Check NG(%d)",
            "Tag Station Mark Test: %d=%d & %d & %d & %d",
            "Mark Test1:%d",
            "Mark Test2:%d",
            "Mark Test: m_StatusSet.uStep = %d",
            "QR Left Mark Test: = %d ID %d",
            "QR Right Mark Test: = %d %d %d",
            "QR Right Mark Test: = %d ID:%d",
            "Mark Test: m_StatusSet.uStep = %d",
            "Mark Test: m_StatusSet.uStep = %d",
            "Mark Test3:%d",
            "Mark Test: %d : %d",
            "Mark Test:  Error: %d, %d : %d",
            "<<IsMark : %f>>",
            "<<IsMark2 : %06f, %06f, %06f, %06f>>",
            "MarkAreaIn[%06d(%7.1lf) %d:%06d(%7.1lf) %06d(%7.1lf)] Curr:%06d(%7.1lf)",
            "%7.1lf=%7.1lf+%7.1lf-%7.1lf : %7.1lf>%7.1lf(%7.1lf+%7.1lf-%7.1lf)",
            "getSteerCmd[%d] %d(%d), (%d:%d), %6.1lf",
            "pItem->Type = %d",
            "[getSteerCmdTypeFrom] %d %d",
            "pItem->Type=%d / TmpSteerCmd=%d / *pSteerCmd=%d",
            "[getSteerCmdTypeFromNext] %d %d",
            "getSteerCmdOnNBranch FP:%d, Path:%d",
            "[isSteerChangeAreaByGuide] Type:%d",
            "[isSteerChangeAreaByGuide] Offset:%6.1lf, Margin:%6.1lf, NC[%d:%d]",
            "[isSteerChangeAreaByGuide] Type:%d",
            "[isSteerChangeAreaByGuide] Offset:%6.1lf, Margin:%6.1lf, NC[%d:%d]",
            "nNodeCount:%d/%d (%d,%d)",
            "getAxisCmdInfo Step3 %d %d %d",
            "getAxisCmdInfo Step4 %d %f",
            "getAxisCmdInfo Step5 %d %f",
            "getMaxSpeed[%f][%f][%f] %d",
            "nCheckPointOfPosition[%d] %f",
            "AxisRun[%f][%f][%f][%f][%f]",
            "AxisRunBack[%06d(%3d:%3d(%3d),%6.1lf/%8.1lf:E%6.1lf)]:%6.4lf,%6.4lf, %3.1lf(←%3.1lf,Ref%3.1lf), %6.1lf(←%6.1lf:E%6.1lf[%2d])Event[%4X]SPDN[%d(%6.1lf)] NC[%d]->[%d,(%d:%d)] Steer[%d]",
            "nTmpIndex=%d/%d (%d,%5lf,%d,%d) (%d:%d→%d)",
            "Case9 %d, %d",
            "Case10 %d, %d",
            "Case11 %d, %d",
            "%d,%d",
            "Run-TargetChanged %d",
            "getAxisCmdInfo Step3 %d %d %d",
            "STEER_HW_INFO_POSITION_LEFT %d %d %d",
            "STEER_HW_INFO_POSITION_RIGHT %d %d %d",
            "getAxisCmdInfo Step5 %d %f",
            "[Run]Speed Data: dSpeedResult %f, dSpeedLimit :%f, Count:%d",
            "getAxisCmdInfo Step4 CuurnSteer %d, key%d Zero:%d One:%d Speed %d getMaxSpeed[%f][%f][%f] %d",
            "AxisRun[%f][%f][%f]22",
            "nCheckPointOfPosition[%d]",
            "Lifter Region :%6.4lf,%6.4lf",
            "AxisRun[%f][%f][%f][%f][%f]Smooth[%f]",
            "AMCDecelStatus1:%d(TargetPos:%f)",
            "AMCDecelStatus2:%d(TargetPos:%f)",
            "AMCDecelStatus3:%d(TargetPos:%f)",
            "AMCDecelCommit_1:%f,%f,%f) CurPos:%f,SP:%f",
            "AMCDecelCommit_2_NoCase:%f,%f,%f) CurPos:%f,SP:%f",
            "AxisRun[%06d(%3d:%3d(%3d),%6.1lf/%8.1lf:E%6.1lf)]:%6.4lf,%6.4lf, %3.1lf(←%3.1lf,Ref%3.1lf), %6.1lf(←%6.1lf:E%6.1lf[%2d])Event[%4X]SPDN[%d(%6.1lf)] NC[%d]->[%d,(%d:%d)] Steer[%d] %d",
            "[getPostionInfoOnRunning] %d %d retrun",
            "[getPostionInfoOnRunning]false(%d) PreCount:%d",
            "[getPostionInfoOnRunning]true PreCount:%d",
            "[getPostionInfoOnMarked] %f ",
            "[getPostionInfoOnManual]TurnOff[%d %d]",
            "[getPostionInfoOnManual]NodeID%d %d",
            "[getPostionInfoOnManual] Offset %d : %f",
            "OHTID Miss %s",
            "OHTID Miss %s"
        };

        public static int TableChar90(char sz)
		{
			switch (sz)
			{
				case '0': return 0;
				case '1': return 1;
				case '2': return 2;
				case '3': return 3;
				case '4': return 4;
				case '5': return 5;
				case '6': return 6;
				case '7': return 7;
				case '8': return 8;
				case '9': return 9;
				case 'a': return 10;
				case 'b': return 11;
				case 'c': return 12;
				case 'd': return 13;
				case 'e': return 14;
				case 'f': return 15;
				case 'g': return 16;
				case 'h': return 17;
				case 'i': return 18;
				case 'j': return 19;
				case 'k': return 20;
				case 'l': return 21;
				case 'm': return 22;
				case 'n': return 23;
				case 'o': return 24;
				case 'p': return 25;
				case 'q': return 26;
				case 'r': return 27;
				case 's': return 28;
				case 't': return 29;
				case 'u': return 30;
				case 'v': return 31;
				case 'w': return 32;
				case 'x': return 33;
				case 'y': return 34;
				case 'z': return 35;
				case 'A': return 36;
				case 'B': return 37;
				case 'C': return 38;
				case 'D': return 39;
				case 'E': return 40;
				case 'F': return 41;
				case 'G': return 42;
				case 'H': return 43;
				case 'I': return 44;
				case 'J': return 45;
				case 'K': return 46;
				case 'L': return 47;
				case 'M': return 48;
				case 'N': return 49;
				case 'O': return 50;
				case 'P': return 51;
				case 'Q': return 52;
				case 'R': return 53;
				case 'S': return 54;
				case 'T': return 55;
				case 'U': return 56;
				case 'V': return 57;
				case 'W': return 58;
				case 'X': return 59;
				case 'Y': return 60;
				case '`': return 61;
				case '~': return 62;
				case '!': return 63;
				case '@': return 64;
				case '#': return 65;
				case '$': return 66;
				case '^': return 67;
				case '&': return 68;
				case '*': return 69;
				case '(': return 70;
				case ')': return 71;
				case '_': return 72;
				case '=': return 73;
				case '+': return 74;
				case '\\': return 75;
				case '|': return 76;
				case '[': return 77;
				case '{': return 78;
				case ']': return 79;
				case '}': return 80;
				case ';': return 81;
				case ':': return 82;
				case '\'': return 83;
				case '\"': return 84;
				case ',': return 85;
				case '<': return 86;
				case '.': return 87;
				case '>': return 88;
				case '?': return 89;
			}
			return 0;
		}

		// N진수를 10 진수로
		public static int N2Dec(string data)
		{
			int nSystem = TABLE_CHAR90.Length;
			int nSum = 0;
			bool bMinus = false;
			for (int i = 0; i < data.Length; i++)
			{
				char sz = data.ElementAt(i);
				if (sz == '-' && i == 0)
				{
					bMinus = true;
					continue;
				}
				int nPow = data.Length - i - 1;
				int nVal = TableChar90(sz);
				nSum += nVal * (int)Math.Pow(nSystem, nPow);
			}
			return nSum * (bMinus ? (-1) : 1);
		}

		// AMC Recv 용
		public static bool DecodingLogLine_AmcRcv(string strIn, out string strOut)
		{
			strOut = "";
			string[] arr = strIn.Split('/');
			if (arr.Length < 2)
				return false;

			List<string> outVals = new List<string>();

			for(int i=0; i<arr.Length; i++)
			{
				string val = arr[i];
				if (i == 0) // 시간
				{
					outVals.Add(val);	
					continue;
				}
				
				if(val.Length == 0)
				{
					outVals.Add(val);
					continue;
				}
				if(val.ElementAt(0) == 'Z')
				{
					if (val.Length < 2)
						continue;   //exception case

					string Zval = val.Remove(0, 1);
					int nZCnt = N2Dec(Zval);
					string str0 = "0";	// Z 개수만큼 연속되는 0 추가
					for(int z=0 ; z<nZCnt ; z++)
					{
						outVals.Add(str0);
					}
					continue;
				}

				// 나머지 모든 경우는 진법 변환
				int nDec = N2Dec(val);
				outVals.Add(Convert.ToString(nDec));
			}

			//bool bTest = true;
			//if(bTest)
			{
				// amp enable bit
				if (outVals.Count > 1)
				{
					int enableBit = Convert.ToInt32(outVals[1]);
					int a1 = (enableBit >> 3 & 0x1) != 1 ? 0 : 1;
					int a2 = (enableBit >> 2 & 0x1) != 1 ? 0 : 1;
					int a3 = (enableBit >> 1 & 0x1) != 1 ? 0 : 1;
					int a4 = (enableBit >> 0 & 0x1) != 1 ? 0 : 1;

					outVals.RemoveAt(1);
					outVals.Insert(1, String.Format("{0}{1}{2}{3}", a4, a3, a2, a1));
				}

				// running bit
				if (outVals.Count > 10)
				{
					int running = Convert.ToInt32(outVals[10]);
					int a1 = (running >> 3 & 0x1) != 1 ? 0 : 1;
					int a2 = (running >> 2 & 0x1) != 1 ? 0 : 1;
					int a3 = (running >> 1 & 0x1) != 1 ? 0 : 1;
					int a4 = (running >> 0 & 0x1) != 1 ? 0 : 1;

					outVals.RemoveAt(10);
					outVals.Insert(10, String.Format("{0}{1}{2}{3}", a4, a3, a2, a1));
				}
				// 10진수 -> 16진수
				if(outVals.Count > 35)
				{
					int src1 = Convert.ToInt32(outVals[32]);
					int src2 = Convert.ToInt32(outVals[33]);
					int src3 = Convert.ToInt32(outVals[34]);
					int src4 = Convert.ToInt32(outVals[35]);

					outVals[32] = String.Format("{0:x}", src1);
					outVals[33] = String.Format("{0:x}", src2);
					outVals[34] = String.Format("{0:x}", src3);
					outVals[35] = String.Format("{0:x}", src4);
				}
			}
			

			// 슬러시 구분으로 한개 라인을 꾸며서 리턴보냄
			for (int n=0; n<outVals.Count; n++)
			{
				string it = outVals[n];
				strOut += it;
				if (n != outVals.Count - 1)
					strOut += "/";
			}
			return true;
		}

		// AMC SEND Periodic 용
		public static bool DecodingLogLine_AmcSend(string strIn, out string strOut)
		{
			strOut = "";
			string[] arr = strIn.Split('/');
			if (arr.Length < 2)
				return false;

			List<string> outVals = new List<string>();

			for (int i = 0; i < arr.Length; i++)
			{
				string val = arr[i];
				if (i == 0) // 시간
				{
					outVals.Add(val);
					continue;
				}

				if (val.Length == 0)
				{
					outVals.Add(val);
					continue;
				}
				if (val.ElementAt(0) == 'Z')
				{
					if (val.Length < 2)
						continue;   //exception case

					string Zval = val.Remove(0, 1);
					int nZCnt = N2Dec(Zval);
					string str0 = "0";  // Z 개수만큼 연속되는 0 추가
					for (int z = 0; z < nZCnt; z++)
					{
						outVals.Add(str0);
					}
					continue;
				}

				// 나머지 모든 경우는 진법 변환
				int nDec = N2Dec(val);
				outVals.Add(Convert.ToString(nDec));
			}
			
			// 슬러시 구분으로 한개 라인을 꾸며서 리턴보냄
			for (int n = 0; n < outVals.Count; n++)
			{
				string it = outVals[n];
				strOut += it;
				if (n != outVals.Count - 1)
					strOut += "/";
			}
			return true;
		}
        // AMC SEND Periodic 용
        public static bool DecodingLogLine_Driving(string strIn, out string strOut)
        {
            strOut = "";
            string[] time = strIn.Split(']');
            time[0] += "]";

            string[] arr = strIn.Split('-','_');
            string[] log = strIn.Split('/');
            int indexofCID = strIn.IndexOf("C-");
            int indexofDrv = strIn.IndexOf("D-");
            int indexofVCID = strIn.IndexOf("C_");
            int indexofVDrv = strIn.IndexOf("D_");

            int Case = 0;
           

            if (arr.Length < 2)
            {
                strOut = strIn;
                return true;
            }
            else
            {
                if(  indexofCID  !=14  &&
                     indexofDrv  !=14  &&
                     indexofVCID != 14 && 
                     indexofVDrv != 14  )
                {
                  
                    strOut = strIn;
                    return true;
                }
                else
                {                 
                   

                    if (indexofCID == 14)
                    {
                        Case = 1;
                    }
                    else if (indexofDrv == 14)
                    {
                        Case = 2;
                    }
                    else if(indexofVCID == 14)
                    {
                        Case = 3;
                    }
                    else if (indexofVDrv == 14)
                    {
                        Case = 4;
                    }
                }
            }
           

            List<string> outVals = new List<string>();

            string []a = arr[1].Split('/');
            int n = 0;
            if (a.Length>1)
            {
               n = int.Parse(a[0]);
            }
            else
            {
                n = int.Parse(arr[1]);
            }

            string numFormat = "";
            string temp = "";
            if (Case == 1)
            {
                numFormat = string.Format("C-{0} ", n);
                if (cid_contents_log.ContainsKey(n))
                {
                    strOut = time[0] + numFormat + cid_contents_log[n];
                }
                else strOut = strIn;

            }
            else if (Case == 2)
            {
                numFormat = string.Format("D-{0} ", n);
                if (driving_contents_log.ContainsKey(n))
                {
                    strOut = time[0] + numFormat + driving_contents_log[n];
                }
                else strOut = strIn;
            }
            else if (Case == 3)
            {
                numFormat = string.Format("C_{0} ", n);
                if (cid_numeric_log.ContainsKey(n))
                {
                    temp = VariableChange(cid_numeric_log[n], log);
                    strOut = time[0] + numFormat + temp;
                }
                else strOut = strIn;
            }
            else if(Case == 4)
            {
                numFormat = string.Format("D_{0} ", n);
                if (driving_numeric_log.ContainsKey(n))
                {
                    temp = VariableChange(driving_numeric_log[n], log);
                    strOut = time[0] + numFormat + temp;
                }
                else strOut = strIn;
            }

            return true;
        }
        public static string VariableChange(string In, string[]In2)
        {
            string Out ="";
            string[] parse = In.Split('%');
            Out += parse[0];


            for (int i=1;i < parse.Length;i++)
            {
                int len = parse[i].Length;
                int rem = 0;

                char[] s = parse[i].ToCharArray();
                string a = "";
                bool bsuccess = false;
               
                for (int j = 0; j < len; j++)
                {
                    if (bsuccess)
                        break;

                    if (s[j] == 'd' || s[j] == 'u' || s[j] == 'o' || s[j] == 'x' || s[j] == 'X'
                        || s[j] == 'f' || s[j] == 'e' || s[j] == 'E' || s[j] == 'g' || s[j] == 'G'
                        || s[j] == 'c' || s[j] == 's' || s[j] == 'p' || s[j] == 'n')
                    {
                        bsuccess = true;

                    }
                    rem++;
                }
                if (rem == len)
                {
                    parse[i] = parse[i].Remove(0);
                    parse[i] = parse[i].Insert(0, In2[i]);
                }
                else
                {
                    parse[i] = parse[i].Remove(0, rem );
                    parse[i] = parse[i].Insert(0, In2[i]);

                }

                
                
                Out += parse[i];
            }

            return Out;
        }

        // 로그 버전 변환
        public static bool LogVer2Conv(string file)
		{
			bool bAmcRcv = Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("AMC_RECV2") == 0;
			bool bAmcSend = Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("AMC_SEND_PERIODIC2") == 0;
            bool bDriving = Path.GetFileNameWithoutExtension(file).ToUpper().IndexOf("DRIVINGCTRL2") == 0;

            if (!bAmcRcv && !bAmcSend && !bDriving)
				return false;

			string[] lines = File.ReadAllLines(file, Encoding.Default);
			if (lines.Length == 0)
				return false;

			// 로그 버전 확인, Driving Control 빼고~!
            if(!bDriving)
            {
                for (int m = 0; m < lines.Length; m++)
                {
                    if (lines[m].Length == 0)
                        continue;

                    string f = lines[m];
                    f = f.Replace(" ", "");
                    if (f.IndexOf("LOG_VERSION=") != 0)
                        return false;
                    f = f.Replace("LOG_VERSION=", "");
                    if (f.IndexOf("2.0") != 0)
                        return false;

                    break;
                }
            }          
			

			List<string> outLines = new List<string>();

            if(lines[0]=="LogConv by HJH")
            {
                return false;
            }
            else
            {
                outLines.Add("LogConv by HJH");
                bDriving = true;
            }

			foreach(string line in lines)
			{
				string outLine = "";
				if (bAmcRcv)
				{
					if (DecodingLogLine_AmcRcv(line, out outLine))
						outLines.Add(outLine);
				}
				else if(bAmcSend)
				{
					if (DecodingLogLine_AmcSend(line, out outLine))
						outLines.Add(outLine);
				}
                else if(bDriving)
                {
                    if (DecodingLogLine_Driving(line, out outLine))
                        outLines.Add(outLine);
                }
			}
			File.Delete(file);
			File.WriteAllLines(file, outLines);
			return true;
		}

	}
}
