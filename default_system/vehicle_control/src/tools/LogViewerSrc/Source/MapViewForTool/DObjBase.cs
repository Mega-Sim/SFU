using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Diagnostics;
using MathGeometry2D;

namespace ObjectDrawig
{

    // 추상 클래스    // 가장 부모가 되는 클래스
    public abstract class DObjBase
    {
        public enum eChildObjType        {            ObjBase = 0, Node, Station, CID, Collision, Link, HID, Distance, COUNTOF        }
        public eChildObjType ObjType;
        public bool Visible = true;
        public bool Selected = false;
        public bool SelectedRoute = false;
        public int SelectedColorType = 0;
        public bool bVibration = false;
        public double VibrationValue = 0;
        public List<string> TagList = new List<string>();
        public static int MINIMUM_SIZE_MM = 10;     // 점이나, 선같이 넓이가 0인 도형들도 계산을 위해서 최소한의 너비를 주기위한 값
        public static DRange VisibleZRange = new DRange();
        public static Point curMousePos_mm = new Point();

        public static bool bDrawCollisionInfo = false;
        public static bool bDrawMapSpeedUp = false;

        public static bool bDrawNodeDetail = false;
        public static bool bDrawNode = true;
        public static bool bDrawStation = true;
        public static bool bDrawStationID = false;
        public bool bDrawFlag = false;
        public string strFlagMemo = "";
        public static List<Point> flagPloygon_RelPos;      // flag 상대위치 point. dc 기준임
        public static List<Point> flagPloygon_AbsPos;      // flag 그릴때 필요함
        public SolidBrush flagBrush = new SolidBrush(Color.Aquamarine);
        public Font flagFont = new Font("맑은 고딕", 9, FontStyle.Bold);
        public static Pen flagPen = new Pen(Color.FromArgb(0x00, 0x00, 0x00), 2);
        public static int VISIBLE_SCALE_NODE_STATION = 75;  // 이 값 이하의 스케일에서 node와 station을 표시한다
        public static int VISIBLE_SCALE_NODE_TEXT = 40;  // 이 값 이하의 스케일에서 NODE의 번호 TEXT를 표시한다
        public static int VISIBLE_SCALE_STATION_TEXT = 25;  // 이 값 이하의 스케일에서 station의 번호 TEXT를 표시한다
        public static int VISIBLE_SCALE_LINK_UBG_INFO = 60;
		public static int VISIBLE_SCALE_NODE_STATION_MUST_HIDE = 60;      // 이값 이상에서는 NODE와 STATION은 무조건 HIDE 된다
        public DObjBase()
        {
            // 처음에는 모든 영역 다 보여주기 때문에
            VisibleZRange.min = 0;
            VisibleZRange.max = Int32.MaxValue;
            ObjType = eChildObjType.ObjBase;
            // 깃발 좌표. 상대좌표임
            if (flagPloygon_RelPos == null)
            {
                flagPloygon_RelPos = new List<Point>();
                flagPloygon_AbsPos = new List<Point>(); 

                flagPloygon_RelPos.Add(new Point(0, 0));
                flagPloygon_RelPos.Add(new Point(-2, -3));
                flagPloygon_RelPos.Add(new Point(-2, -27));
                flagPloygon_RelPos.Add(new Point(0, -29));
                flagPloygon_RelPos.Add(new Point(2, -27));
                flagPloygon_RelPos.Add(new Point(17, -20));
                flagPloygon_RelPos.Add(new Point(2, -11));
                flagPloygon_RelPos.Add(new Point(2, -3));
                flagPloygon_RelPos.Add(new Point(0, 0));
            }
        }
        public abstract void Draw(Graphics g, DScale scale);
        public void DrawFlag(Graphics g, DScale scale)
        {
            if (bDrawFlag)
            {
                Point pos_mm = GetRect_mm().CenterPoint();
                Point pos_dc = scale.MM2DC(pos_mm); // flag 상대위치를 현재 위치기준으로 절대좌표로 만든다
                foreach (Point pos in flagPloygon_RelPos)
                    flagPloygon_AbsPos.Add(new Point(pos.X + pos_dc.X, pos.Y + pos_dc.Y));

                g.FillPolygon(flagBrush, flagPloygon_AbsPos.ToArray());
                g.DrawPolygon(flagPen, flagPloygon_AbsPos.ToArray());
                //g.DrawString(ObjType.ToString() + ": "+ GetID(), flagFont, Brushes.White, pos_dc.X + 3, pos_dc.Y + 3);
                g.DrawString(strFlagMemo, flagFont, Brushes.Black, pos_dc.X + 12, pos_dc.Y - 24);
                flagPloygon_AbsPos.Clear();
            }
        }

        public abstract string GetID();
        public abstract int GetID_Int();
        public virtual string GetHoverInfo() { return ""; }
        public abstract void AddToRect(ref DRect_mm rect);
        public abstract DRect_mm GetRect_mm();
        public bool IsInVisibleArea(DScale Scale)
        {
            //안보이는 영역이면 건너뜀!
            
            DRect_mm Rect_mm = GetRect_mm();
            Rectangle rt_dc = Scale.MM2DC(Rect_mm);
            Rectangle rtWnd_dc = new Rectangle(0, 0, Scale.nDrawHalfWidth_dc * 2, Scale.nDrawHalfHeight_dc * 2);      // 이게 맞는지 0대신 half 만큼 -시켜야하는지..

            rtWnd_dc.Intersect(rt_dc);      //교차하는 Rect를 구해주는 고마운 CRect 함수

            if (rtWnd_dc.IsEmpty)
                return false;

            return true;
        }
        // Default comparer for Part type.
        public int CompareTo(DObjBase comparePart)
        {
            // A null value means that this object is greater.
            if (comparePart == null)
                return 1;

            else
                return this.GetID_Int().CompareTo(comparePart.GetID_Int());
        }

    }

}
