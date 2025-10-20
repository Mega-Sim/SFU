using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathGeometry2D;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Diagnostics;

namespace ObjectDrawig
{
    public enum eTeachingStationPortType {  EQ=1, EQ_X, Stocker, Loader, LSTB, RSTB, UTB } //eStationPortType 이것과 같은건데 두개의 정의 가 있다. 이건 Oht Main 따라 가는거라서 어쩔수없음
    public enum ePIOType { IR=0, RF, Hybrid}
    public enum ePIODir { Left=1, Right}
    public enum AutoRecovery { NoUse=0, Use}

    class DObjTeaching : DObjBase
    {
        public DObjStation station = null;
        public int nStationID = 0;
        public eTeachingStationPortType    portType;   // 이값이랑 DObjStation의 portType 과 동일하야 한다. 번호는 다른데 text는 같아야함
        public int Hoist;                         // 이적재 Hoist 이동거리(1/10mm)
        public int Shift;                         // 이적재 Shift 이동거리(1/10mm)
        public int Rotate;                        // 이적재 Rotate 이동거리(radian)
        public ePIOType PIOType;            // 0:IR, 1:RF, 2:Hybrid 5.0GHz
        public ePIODir PIODir;                  // PIO 방향(좌, 우)
        public int PIOID;                         // RF-PIO Setting ID 정보
        public int PIOChannel;                    // RF-PIO Setting Channel 정보
        public int PIOTimeLevel;                  // PIO Time out 지연 시간 레벨
        public AutoRecovery AutoRecovery;                  ///< AutoRecovery
        public int HandDetectEQ;                  // HandDetectEQ
        public int STBOffSet;                     // STB Off Set 이동거리(1/10mm, 기본값 0)
        public int LookDown;                      // Station 이적재 시 Lookdown Area 정보(0~16, 기본값 1)
        public int STBL_Shift = 0;                // STBL_Shift 티칭 값에 3mm추가 soming



        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }

        public override DRect_mm GetRect_mm()
        {
            if(station != null)
                return station.GetRect_mm();

            Debug.Assert(false, "Station " + Convert.ToString(nStationID) + "없는 스테이션");
            return new DRect_mm();
        }
        public override void Draw(Graphics g, DScale scale)
        {
            return;
        }

        public override string GetID()
        {
            return Convert.ToString(nStationID);
        }

        public override int GetID_Int()
        {
            return nStationID;
        }
        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 13)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }

            // [TEACHING] 
            // 140432 / 1 / 30200 / -250 / 0 / 1 / 1 / 140432 / 214 / 1 / 1 / 0 / 0 / 1 / 0
            // nStationID / eTeachingStationPortType / Hoist / Shift / Rotate / ePIOType / ePIODir / PIOID / PIOChannel / PIOTimeLevel / AutoRecovery / HandDetectEQ / STBOffSet / LookDown / STB_Left_Shift
            DObjTeaching teach = new DObjTeaching();
            try
            {
                teach.nStationID = Convert.ToInt32(strArr[0]);
                teach.portType = (eTeachingStationPortType)Convert.ToInt32(strArr[1]);
                
                teach.Hoist = Convert.ToInt32(strArr[2]);
                teach.Shift = Convert.ToInt32(strArr[3]);
                teach.Rotate = Convert.ToInt32(strArr[4]);

                teach.PIOType = (ePIOType)Convert.ToInt32(strArr[5]);
                teach.PIODir = (ePIODir)Convert.ToInt32(strArr[6]);

                teach.PIOID = Convert.ToInt32(strArr[7]);
                teach.PIOChannel = Convert.ToInt32(strArr[8]);
                teach.PIOTimeLevel = Convert.ToInt32(strArr[9]);

                teach.AutoRecovery = (AutoRecovery)Convert.ToInt32(strArr[10]);
                teach.HandDetectEQ = Convert.ToInt32(strArr[11]);
                teach.STBOffSet = Convert.ToInt32(strArr[12]);
                teach.LookDown = Convert.ToInt32(strArr[13]);
                if (strArr.Length > 14)
                {
                    if(String.IsNullOrEmpty(strArr[14]) == false)
                        teach.STBL_Shift = Convert.ToInt32(strArr[14]);
                }


                teach.station = DObjStation.FindStation(teach.nStationID);
                if(teach.station == null)
                {
                    DObjMan.errMsg.Add("Station " + Convert.ToString(teach.nStationID) + ": Station에 없는 객체가 Teaching에는 있습니다.");
                    return false;
                }

                // 서로 인덱스 번호는 다르지만, 글자는 똑같아야 됨
                if (teach.portType.ToString().ToUpper() != teach.station.portType.ToString().ToUpper())
                {
                    string str = String.Format("Teaching 데이터의 Station {0}의 Port Type이  실제 Station PortType과 다릅니다. {1} != {2}", teach.nStationID, teach.portType.ToString(), teach.station.portType.ToString());
                    DObjMan.errMsg.Add(str);
                    return false;
                }


            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(teach);
            return true;

        }
    }
}
