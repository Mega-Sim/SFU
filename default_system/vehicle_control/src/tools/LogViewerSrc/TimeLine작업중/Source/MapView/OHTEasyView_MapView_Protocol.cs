using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Msg
{
    /// <summary>
    /// OHTEasyView 와 MapView 간 프로세스 통신 프로토콜 정의 
    /// SHCHO  2018.07.04
    /// </summary>

    public class Cmd
    {
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
    }
}
