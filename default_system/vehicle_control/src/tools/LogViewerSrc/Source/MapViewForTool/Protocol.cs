using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Msg
{
    public struct PROTOCOL_HEADER
    {
        // Header
        public byte MachineType;   // 2Byte, 기종
        public char[] MachineID;   // 6Byte , 머신ID  
    }

    public struct PROTOCOL_TAG
    {
        public ushort Value;           // 2Byte
    }
    public struct PROTOCOL_STATUS //사이즈 30
    {
        public byte status;
        public byte[] dummy;
    }

    public struct PKTtoTP
    {
        public PROTOCOL_HEADER HEADER;
        public PROTOCOL_TAG TAG;
        public PROTOCOL_STATUS STATUS;
        public byte[] Value;
    }

    public class tagID
    {
        public const int CMD_TP_TO_MAPVIEW_STATUS_REQ = 30001;               //TP -> MAP VIEW 상태요청

        public const int CMD_MAPVIEW_TO_TP_STATUS_RESPONSE = 30002;          //MAP VIEW -> TP 상태요청응답

        public const int CMD_TP_TO_MAPVIEW_MAP_VERSION_REQ = 30003;          //TP -> MAP VIEW UI 요청

        public const int CMD_MAPVIEW_TO_TP_MAP_VERSION_RESPONSE = 30004;     //MAP VIEW -> TP UI 요청응답

        public const int CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ = 30005;            //TP -> MAP VIEW UI 요청

        public const int CMD_MAPVIEW_TO_TP_UI_UPDATE_RESPONSE = 30006;       //MAP VIEW -> TP UI 요청응답
    }



    /// <summary>
    /// OHTEasyView 와 MapView 간 프로세스 통신 프로토콜 정의 
    /// SHCHO  2018.07.04
    /// </summary>
    // cmd의 크기는 5보다 크고 
    public class Cmd
    {

        //MapView - LogViewer
        /// <summary>
        /// E2M_REQ_SHOW / 0 or 1 [0=Hide, 1=Show]
        /// 맵뷰 창을 열어달라고 요청. Visible 여부.
        /// </summary>
        public static string LOG_REQ_SHOW = "LOG_REQ_SHOW";

        /// <summary>
        /// E2M_REQ_STATUS / portnum
        /// 각 맵뷰들 상태를 알려달라고 요청.
        /// </summary>
        public static string LOG_REQ_STATUS = "LOG_REQ_STATUS";


        /// <summary>
        ///   MAP_REP_STATUS / 현재 Status /  P-PRJ / P1L_7F / 0.1577 / 2018-04-27 오전 10:15 / D:\....맵경로/ 나의 포트번호
        ///  위의 요청에 대한 응답. 맵뷰어 상태 전달
        /// </summary>
        public static string MAP_REP_STATUS = "MAP_REP_STATUS";

        /// <summary>
        /// E2M_REQ_FIND / 0, 1 [0=node, 1=Station]/  200302 [해당 아이디]
        /// 특정 Node나 Station 을 맵상에서 찾아사 보여달라
        /// </summary>
        public static string LOG_REQ_FIND = "LOG_REQ_FIND";


        //MapView - VibManager
        public static string VIBRATION_READY = "VIBRATION_READY";

        public static string VIBRATION_ROUTE = "VIBRATION_ROUTE";

        public static string VIBRATION_FLAG = "VIBRATION_FLAG";

        public static string VIBRATION_END = "VIBRATION_END";

        public static string VIBRATION_SUCCESS = "VIBRATION_SUCCESS";

        public static string REDRAW = "REDRAW";
        
    }
}
