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
    class DObjPathData : DObjBase
    {
        public enum LayoutType { MTL = 1, Lifter}
        public enum DriveDirection { Forward = 0, Backward}

        public LayoutType nLayoutType = LayoutType.MTL;
        public int nLayoutID = 0;
        
        public int nLeftNodeIDIn = 0;
        public int nLeftNodeIDMiddle = 0;              //< MTL(왼쪽), Lifter(아래) 중간 Node Id
        public int nLeftNodeIDOut = 0;                 //< MTL(왼쪽) 진출 Node Id
        public DriveDirection LeftDriveDirection = DriveDirection.Forward;            //< 차량 진행 방향을 표시

        public int nRightNodeIDIn = 0;                 //< MTL(오른쪽), Lifter(위) 진입 Node Id
        public int nRightNodeIDMiddle = 0;             //< MTL(오른쪽), Lifter(위) 중간 Node Id
        public int nRightNodeIDOut = 0;                //< MTL(오른쪽) 진출 Node Id
        public DriveDirection RightDriveDirection = DriveDirection.Forward;           //< 차량 진행 방향을 표시

        public int nReserved1 = 0;
        public int nReserved2 = 0;

        public DObjStation LeftNodeIDIn = null;             // 스테이션이다. 노드 아님
        public DObjStation LeftNodeIDMiddle = null;
        public DObjStation LeftNodeIDOut = null;
        public DObjStation RightNodeIDIn = null;
        public DObjStation RightNodeIDMiddle = null;
        public DObjStation RightNodeIDOut = null;

        public static Pen Pen_Area = new Pen(Color.LightGreen, 2);
        public static Font font = new Font("맑은 고딕", 10F, FontStyle.Regular);

        public override void Draw(Graphics g, DScale scale)
        {
            if (!Visible) return;
            //if (!IsInVisibleArea(scale))                return;

            DRect_mm rt_mm = GetRect_mm();
            Rectangle rectArea_dc = scale.MM2DC(rt_mm);
            Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());

            rectArea_dc.Width = Math.Max(2, rectArea_dc.Width);
            rectArea_dc.Height = Math.Max(2, rectArea_dc.Height);

            rectArea_dc.Inflate(40, 40);
            
            // 영역만 그려준다
            g.DrawRectangle(Pen_Area, rectArea_dc);
            g.DrawString(String.Format("{0}_{1}", nLayoutID, nLayoutType.ToString()), font, Brushes.White, rectArea_dc.X, rectArea_dc.Y); 
        }

        public override string GetID()
        {
            return Convert.ToString(nLayoutID);
        }

        public override int GetID_Int()
        {
            return nLayoutID;
        }

        public override DRect_mm GetRect_mm()
        {
            DRect_mm rect = new DRect_mm();
            AddToRect(ref rect);
            return rect;
        }

        public override void AddToRect(ref DRect_mm rect)
        {
            if(LeftNodeIDIn != null)
                LeftNodeIDIn.AddToRect(ref rect);

            if (LeftNodeIDMiddle != null)
                LeftNodeIDMiddle.AddToRect(ref rect);

            if(LeftNodeIDOut != null)
                LeftNodeIDOut.AddToRect(ref rect);

            if(RightNodeIDIn != null)
                RightNodeIDIn.AddToRect(ref rect);

            if(RightNodeIDMiddle != null)
                RightNodeIDMiddle.AddToRect(ref rect);

            if(RightNodeIDOut != null)
                RightNodeIDOut.AddToRect(ref rect);
        }

        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 10)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }

            // [PathData] 
            // LayoutType(1 or 2) / LayoutID / LeftNodeIDIn/ LeftNodeIDMiddle/ LeftNodeIDOut/ LeftDriveDir /
            // RightNodeIDIn/ RightNodeIDMiddle/ RightNodeIDOut/ RightDriveDir / Resered1 / Reserved2

            DObjPathData obj = new DObjPathData();
            try
            {
                obj.nLayoutType = (LayoutType)Convert.ToInt32(strArr[0]);
                obj.nLayoutID = Convert.ToInt32(strArr[1]);
                obj.nLeftNodeIDIn = Convert.ToInt32(strArr[2]);
                obj.nLeftNodeIDMiddle = Convert.ToInt32(strArr[3]);
                obj.nLeftNodeIDOut = Convert.ToInt32(strArr[4]);
                obj.LeftDriveDirection = (DriveDirection)Convert.ToInt32(strArr[5]);
                obj.nRightNodeIDIn = Convert.ToInt32(strArr[6]);
                obj.nRightNodeIDMiddle = Convert.ToInt32(strArr[7]);
                obj.nRightNodeIDOut = Convert.ToInt32(strArr[8]);
                obj.RightDriveDirection = (DriveDirection)Convert.ToInt32(strArr[9]);

                obj.nReserved1 = Convert.ToInt32(strArr[10]);
                obj.nReserved2 = Convert.ToInt32(strArr[11]);

                if (obj.nLeftNodeIDIn > 0)
                {
                    obj.LeftNodeIDIn = DObjStation.FindStation(obj.nLeftNodeIDIn);
                    if (obj.LeftNodeIDIn == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.nLeftNodeIDIn) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
                if (obj.nLeftNodeIDMiddle > 0)
                {
                    obj.LeftNodeIDMiddle = DObjStation.FindStation(obj.nLeftNodeIDMiddle);
                    if (obj.LeftNodeIDMiddle == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.nLeftNodeIDMiddle) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
                if (obj.nLeftNodeIDOut > 0)
                {
                    obj.LeftNodeIDOut = DObjStation.FindStation(obj.nLeftNodeIDOut);
                    if (obj.LeftNodeIDOut == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.nLeftNodeIDOut) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
                if (obj.nRightNodeIDIn > 0)
                {
                    obj.RightNodeIDIn = DObjStation.FindStation(obj.nRightNodeIDIn);
                    if (obj.RightNodeIDIn == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.RightNodeIDIn) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
                if (obj.nRightNodeIDMiddle > 0)
                {
                    obj.RightNodeIDMiddle = DObjStation.FindStation(obj.nRightNodeIDMiddle);
                    if (obj.RightNodeIDMiddle == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.RightNodeIDMiddle) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
                if (obj.nRightNodeIDOut > 0)
                {
                    obj.RightNodeIDOut = DObjStation.FindStation(obj.nRightNodeIDOut);
                    if (obj.RightNodeIDOut == null)
                    {
                        DObjMan.errMsg.Add("PathData의 스테이션 노드: " + Convert.ToString(obj.RightNodeIDOut) + "는 존재하지 않습니다.");
                        return false;
                    }
                }
            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(obj);
            return true;

        }
    }
}
