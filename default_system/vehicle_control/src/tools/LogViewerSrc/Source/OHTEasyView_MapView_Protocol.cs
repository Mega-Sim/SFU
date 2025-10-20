using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Msg
{
    /// <summary>
    /// LogViewer 와 MapView 간 프로세스 통신 프로토콜 정의 
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
        /// E2M_REQ_STATUS / 0,1...[열린 맵뷰 넘버링]
        /// 각 맵뷰들 상태를 알려달라고 요청.
        /// </summary>
        public static string LOG_REQ_STATUS = "LOG_REQ_STATUS";


        /// <summary>
        ///   MAP_REP_STATUS / 상태정보 / 0,1... [위와 같은값] / P-PRJ / P1L_7F / 0.1577 / 2018-04-27 오전 10:15 / D:\....맵경로
        ///  위의 요청에 대한 응답. 맵뷰어 상태 전달
        ///  상태정보
        ///  public enum MapViewStat
        ///   None = 0                // 아직 신호 받지 못함
        ///  Standby               // 맴 잘열려있고 대기상태
        ///  EmptyMap          // 맵 아직 안연상태
        ///  Openning          // 여는중
        ///  Working           // 뭔가 다른 작업중
        /// </summary>
		public static string MAP_REP_STATUS = "MAP_REP_STATUS";

        /// <summary>
        /// E2M_REQ_FIND / 0, 1 [0=node, 1=Station]/  200302 [해당 아이디]
        /// 특정 Node나 Station 을 맵상에서 찾아사 보여달라
        /// </summary>
        public static string LOG_REQ_FIND = "LOG_REQ_FIND";
    }
}
