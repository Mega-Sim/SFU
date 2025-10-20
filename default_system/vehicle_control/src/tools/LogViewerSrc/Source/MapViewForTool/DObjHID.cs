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

    public class DObjHID : DObjBase
    {
        public string ID = "";
        public bool bLoaded = false;
        DRect_mm myRect = new DRect_mm();
        static Pen Pen_area = new Pen(Color.Black, 1);
        public static Pen Pen_Area = new Pen(Color.LightGreen, 2);

        // 이 HID 를 쓰는 링크 어레이
        public DObjArray myLinkArray = new DObjArray();
        public int nTotalLength_mm = 0;     // 이 HID를 쓰는 Link의 총 거리 mm
      

        public static Pen Pen_Normal = new Pen(Color.LightYellow, 2);  // 기본 그리기 펜
        public static Pen Pen_Selected = new Pen(Color.YellowGreen, 4);   // 도형 선택 됐을 때 그려질 펜
        public static Font font = new Font("맑은 고딕", 10F, FontStyle.Regular);

        public DObjHID()
        {
            ObjType = eChildObjType.HID;
            
            
        }
        // node array에서 자신을 쓰는 링크들을 찾아서 myLinkArray 에 넣음
        public void FindMyLink(DObjArray objArray)
        {
            nTotalLength_mm = 0;
            foreach (DObjLink aLink in objArray)
            {
                if (aLink.Hid == null)
                    continue;

                if (this.ID == aLink.Hid.ID)
                {
                    myLinkArray.Add(aLink);
                    nTotalLength_mm += aLink.distance_mm;
                }
            }
            bLoaded = true;
        }

        // 자신이 소속된 링크들 그리기
        public override void Draw(Graphics g, DScale scale)
        {
            if (myLinkArray.Count == 0)
                return;

            if (!Visible)
                return;

            DRect_mm rt_mm = GetRect_mm();
            Rectangle rectArea_dc = scale.MM2DC(rt_mm);
            Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());

            rectArea_dc.Width = Math.Max(2, rectArea_dc.Width);
            rectArea_dc.Height = Math.Max(2, rectArea_dc.Height);

            rectArea_dc.Inflate(40, 40);

            // 도면 배경을 투명하고 어둡게 해서 hid만 부각되게 한다
            g.FillRectangle(new SolidBrush(Color.FromArgb(150, Color.Black)), new Rectangle(0, 0, scale.nDrawHalfWidth_dc * 2, scale.nDrawHalfHeight_dc * 2));
            g.DrawRectangle(Pen_Area, rectArea_dc); //CID 전체 영역 점선 표시
            g.DrawString(GetID(), font, Brushes.White, rectArea_dc.X, rectArea_dc.Y);   //CID 이름 표시

            foreach (DObjLink link in myLinkArray)
            {
                link.Draw(g, scale,Pen_Selected);
            }

            // 선택되어있을 경우 영영 박스 그리기
            //if (Selected)
            //{
            //    Rectangle dcRect = scale.MM2DC(GetRect_mm());
            //    g.DrawRectangle(Pen_area, dcRect);
            //}
        }
        public override string GetID()
        {
            return String.Format("{0}", ID);
        }
        public override int GetID_Int() { return 0; }
        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }

        // HID 하나에 link들 한 30개는 묶여있는데, Show 할때마다 여길 타면 오래걸리니까, 한번 로드 되면 그 이후는 타지 않도록
        // bLoaded 여부로 갱신함
        public override DRect_mm GetRect_mm()
        {
            //if (bLoaded)
            //    return myRect;

            foreach (DObjLink aLink in myLinkArray)
                myRect.SumOfSet(aLink.GetRect_mm());

            bLoaded = true;
            return myRect;
        }
        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 3)
            {
                DObjMan.errMsg.Add(String.Format("OcsLayout.dat에서 HID 파싱 실패 line: {0}\n", line));
                return false;
            }

            DObjHID newHID = new DObjHID();
            try
            {
                newHID.ID = strArr[0];
            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true))); 
                return false;
            }

            objArray.Add(newHID);
            return true;

        }
        public static DObjHID FindHID(string id)
        {
            DObjArray arr = DObjMan.arrHID;
            foreach (DObjHID hid in arr)
            {
                if (hid.ID == id)
                    return hid;
            }
            return null;
        }
    }
}
