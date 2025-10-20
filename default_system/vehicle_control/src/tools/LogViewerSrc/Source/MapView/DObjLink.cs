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
using MapView;

namespace ObjectDrawig
{
    public class DObjLink : DObjBase
    {
        public DObjNode NodeA, NodeB;      // A ---> B

        // 이게 뭐냐면... 링크 라인과 평행하는 왼쪽, 오른쪽 두 선분을 갖는 변수.
        // Station STB들은 좌우측에 위치하기 때문에 화면에 표현할 때 좌우측을 표현해야 하기때문에, 이 값을 알고잇음 매우 계산하기 유리하다
        public DPoint_mm[] PerallerLine_LeftSide_mm = new DPoint_mm[2];     // 왼쪽 평행선
        public DPoint_mm[] PerallerLine_RightSide_mm = new DPoint_mm[2];    // 우측 평행선
        public static int ParallelLength_mm = 200;
        public int Flag;
        public int Speed = -1;
        public int distance_mm;
        public DObjHID Hid;

        // Station Editor 색 따라함
        public static Pen Pen_SpdUnset = new Pen(Color.FromArgb(0x80, 0x80, 0x80), 2);          // unset speed          // unset speed
        public static Pen Pen_SpdOver0 = new Pen(Color.FromArgb(0x00, 0xFF, 0x00), 2);          // 0<= speed <1300          // 0<= speed <1300
        public static Pen Pen_SpdOver1300 = new Pen(Color.FromArgb(0xFF, 0xC4, 0x00), 2);     // 1300<= speed <3300     // 1300<= speed <3300
        public static Pen Pen_SpdOver3300 = new Pen(Color.FromArgb(0xFF, 0x00, 0x00), 2);      // 3300<= speed <4000     // 3300<= speed <4000
        public static Pen Pen_SpdOver4000 = new Pen(Color.FromArgb(0x9D, 0x0C, 0x14), 2);      // 4000<= speed     // 4000<= speed
                                              
        public static Font Font_Big = new Font("맑은 고딕", 10F, FontStyle.Regular);
		public static Font Font_Small = new Font("맑은 고딕", 7F, FontStyle.Regular);

		private Pen customPen = null;
        
        public static Pen Pen_Normal = new Pen(Color.FromArgb(0xFF, 0x00, 0x80), 3);             //보라색;  // 기본 그리기 펜
        public static Pen Pen_Selected = new Pen(Color.FromArgb(0xFF, 0x00, 0x80), 5);    // 도형 선택 됐을 때 그려질 펜

        public DPoint_mm[] arrowPolygon_mm = new DPoint_mm[5];    // 방향 화살표
        public Point []arrowPolygon_dc= new Point[5];

        public string strCollisionInfo = "";
        public string strMapSpeedUpInfo = "";
        public static int nAverageLinkLength_mm = 0;       // 모든 링크들의 평균 링크 길이. 나중에 보기 좋은 scale 값을 구할 떄 쓰이기 때문에
        public DObjLink()
        {
            ObjType = eChildObjType.Link;
            PerallerLine_LeftSide_mm[0] = new DPoint_mm();
            PerallerLine_LeftSide_mm[1] = new DPoint_mm();
            PerallerLine_RightSide_mm[0] = new DPoint_mm();
            PerallerLine_RightSide_mm[1] = new DPoint_mm();
            
        }
        public void Draw(Graphics g, DScale scale, Pen customPen)
        {
            this.customPen = customPen;
            Draw(g, scale);
            this.customPen = null;
        }
        // 자신이 가지고 있는 NodeA  ---> NodeB 까지 그린다
        public override void Draw(Graphics g, DScale scale)
        {
            if (!IsInVisibleArea(scale))
                return;

            if (!VisibleZRange.IsInRange(NodeA.Z_mm) || !VisibleZRange.IsInRange(NodeB.Z_mm))
                return;

            int x_dc = scale.MM2DC_X(NodeA.X_mm);
            int y_dc = scale.MM2DC_Y(NodeA.Y_mm);
            Point pt1_dc = new Point(x_dc, y_dc);

            x_dc = scale.MM2DC_X(NodeB.X_mm);
            y_dc = scale.MM2DC_Y(NodeB.Y_mm);
            Point pt2_dc = new Point(x_dc, y_dc);

            Pen thePen = null;

            if (customPen != null)
                thePen = customPen;
            else
            {
                if (Selected)
                    thePen = Pen_Selected;
                else
                {
                    if (Speed >= 0 && Speed < 1300)
                        thePen = Pen_SpdOver0;
                    else if (Speed >= 1300 && Speed < 3300)
                        thePen = Pen_SpdOver1300;
                    else if (Speed >= 3300 && Speed < 4000)
                        thePen = Pen_SpdOver3300;
                    else if (Speed >= 4000)
                        thePen = Pen_SpdOver4000;
                    else
                        thePen = Pen_SpdUnset;
                }
            }
            g.DrawLine(thePen, pt1_dc, pt2_dc);
           
            if ((bDrawCollisionInfo|| bDrawMapSpeedUp) && scale.dScale < VISIBLE_SCALE_LINK_UBG_INFO)
            {
                Point center_mm = GetRect_mm().CenterPoint();
                Point center_dc = scale.MM2DC(center_mm);
				// 스케일이 조밀할때는 작은 폰트, 확대됐을때는 큰폰트
				Font applyFont = scale.dScale < 20 ? Font_Big : Font_Small;

                g.DrawString(bDrawCollisionInfo ? strCollisionInfo : strMapSpeedUpInfo, applyFont, Brushes.LightGray, center_dc.X, center_dc.Y);
            }

            // 일정 스케일 이하일 때만 방향 화살표 표시
            if (scale.dScale < 60 )
            {
                arrowPolygon_dc[0].X = scale.MM2DC_X(arrowPolygon_mm[0].x);
                arrowPolygon_dc[0].Y = scale.MM2DC_Y(arrowPolygon_mm[0].y);
                arrowPolygon_dc[1].X = scale.MM2DC_X(arrowPolygon_mm[1].x);
                arrowPolygon_dc[1].Y = scale.MM2DC_Y(arrowPolygon_mm[1].y);
                arrowPolygon_dc[2].X = scale.MM2DC_X(arrowPolygon_mm[2].x);
                arrowPolygon_dc[2].Y = scale.MM2DC_Y(arrowPolygon_mm[2].y);
                arrowPolygon_dc[3].X = scale.MM2DC_X(arrowPolygon_mm[3].x);
                arrowPolygon_dc[3].Y = scale.MM2DC_Y(arrowPolygon_mm[3].y);
                arrowPolygon_dc[4].X = scale.MM2DC_X(arrowPolygon_mm[4].x);
                arrowPolygon_dc[4].Y = scale.MM2DC_Y(arrowPolygon_mm[4].y);


                g.DrawPolygon(thePen, arrowPolygon_dc);
            }
            
        }
        public override string GetID()
        {
            return String.Format("{0}_{1}", NodeA == null ? "null" : Convert.ToString(NodeA.ID), NodeB == null ? "null" : Convert.ToString(NodeB.ID));
        }
        public override int GetID_Int() { return NodeA.ID; }
        /// <summary>
        /// 아규먼트로 받은 rect에 현재 객체의 영역을 더해서 넘겨준다.
        /// </summary>
        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }
        public override DRect_mm GetRect_mm()
        {
            DRect_mm rect = new DRect_mm();
            rect.SumOfSet(NodeA.Get2DPoint());
            rect.SumOfSet(NodeB.Get2DPoint());
            rect.NormalizeRect();

            return rect;

        }

        // 한번만 new 하기 위해서 static 글로벌로
        private static DPoint_mm NodeAPos = new DPoint_mm();
        private static DPoint_mm NodeBPos = new DPoint_mm();
        private static DPoint_mm tmpPt_mm = new DPoint_mm();

        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 5)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }


            DObjLink newLink = new DObjLink();
            try
            {
                int nNodeA = Convert.ToInt32(strArr[0].Split('_')[0]);
                int nNodeB = Convert.ToInt32(strArr[0].Split('_')[1]);
                double dSpeed = Convert.ToDouble(strArr[2]);
                int distance_mm = Convert.ToInt32(strArr[3]);
                string strHID = strArr[4];

                newLink.Speed = (int)dSpeed;
                newLink.distance_mm = distance_mm;

                // 노드 찾기
                newLink.NodeA = newLink.NodeB = null;
                newLink.NodeA = DObjNode.FindNode(nNodeA);
                newLink.NodeB = DObjNode.FindNode(nNodeB);
                
                
                //foreach (DObjNode node in DObjMan.arrNode)
                //{
                //    if (newLink.NodeA != null && newLink.NodeB != null)
                //        break;  // found all

                    
                //    if (newLink.NodeA == null)
                //    {
                //        if (node.ID == nNodeA)
                //        {
                //            newLink.NodeA = node;   // found a
                //            continue;
                //        }
                //    }
                //    if (newLink.NodeB == null)
                //    {
                //        if (node.ID == nNodeB)
                //        {
                //            newLink.NodeB = node;   //found b
                //            continue;
                //        }
                //    }
                //}

                if (newLink.NodeA == null || newLink.NodeB == null)
                {
                    DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("Link의 노드 A or B가 null 입니다.", new StackFrame(true)) );
                    return false;
                }

                // hid 찾기
                newLink.Hid = null;
                foreach (DObjHID hid in DObjMan.arrHID)
                {
                    if (String.Format("{0}", hid.ID) == strHID)
                    {
                        newLink.Hid = hid;
                        break;
                    }
                }
                if(newLink.Hid == null)
                {
                    newLink.Hid = null;
                }
                NodeAPos.x = newLink.NodeA.X_mm;
                NodeAPos.y = newLink.NodeA.Y_mm;
                NodeBPos.x = newLink.NodeB.X_mm;
                NodeBPos.y = newLink.NodeB.Y_mm;

                // 계산 뻑나면 MathException 던지기 위해서 try로 묶음
                try
                {
                    // 평행선 구하는 계산 작업
                    if (!MathSupport.gPointPerpendicularRv(NodeAPos, NodeBPos, ParallelLength_mm, ref newLink.PerallerLine_RightSide_mm[1])) throw new MathException();
                    if (!MathSupport.gPointPerpendicularRv(NodeBPos, NodeAPos, - ParallelLength_mm, ref newLink.PerallerLine_RightSide_mm[0])) throw new MathException();
                    if (!MathSupport.gPointPerpendicularRv(NodeAPos, NodeBPos, -ParallelLength_mm, ref newLink.PerallerLine_LeftSide_mm[1])) throw new MathException();
                    if (!MathSupport.gPointPerpendicularRv(NodeBPos, NodeAPos, ParallelLength_mm, ref newLink.PerallerLine_LeftSide_mm[0])) throw new MathException();
                    
                    // Arrow Create 시간 오래걸리는 계산 작업. 처음에 한번만 하는게 좋을듯
                    double dLength_mm = MathSupport.gDistLine(NodeAPos, NodeBPos);
                    DPoint_mm pt70 = MathSupport.gPointInLineEx(NodeAPos, NodeBPos, dLength_mm - 170);
                    DPoint_mm pt80 = MathSupport.gPointInLineEx(NodeAPos, NodeBPos, dLength_mm - 100);
                    DPoint_mm pt90 = MathSupport.gPointInLineEx(NodeAPos, NodeBPos, dLength_mm - 10);

                    DPoint_mm ptEdge1 = new DPoint_mm();
                    DPoint_mm ptEdge2 = new DPoint_mm();
                    if (!MathSupport.gPointPerpendicularRv(NodeAPos, pt70, 80, ref ptEdge1)) throw new MathException();
                    if (!MathSupport.gPointPerpendicularRv(NodeAPos, pt70, -80, ref ptEdge2)) throw new MathException();

                    // 화살표 벡터들 추가
                    newLink.arrowPolygon_mm[0] = ptEdge1;
                    newLink.arrowPolygon_mm[1] = pt90;
                    newLink.arrowPolygon_mm[2] = ptEdge2;
                    newLink.arrowPolygon_mm[3] = pt80;
                    newLink.arrowPolygon_mm[4] = ptEdge1;

                }
                catch(MathException )
                {
                    DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                    return false;
                }

            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(newLink);
            return true;
        }
        public static DObjLink FindLink(DObjNode nodeA, DObjNode nodeB)
        {
            return FindLink(nodeA.ID, nodeB.ID);
        }

        public static DObjLink FindLink(int nodeA, int nodeB)
        {
            DObjArray arr = DObjMan.arrLink;
            foreach (DObjLink link in arr)
            {
                if (link.NodeA.ID == nodeA && link.NodeB.ID == nodeB)
                    return link;
            }
            return null;
        }

        // 화면에 3000/L/L/BAY  이런 Collision 정보 표시하기 위해서 text 꾸미기
        public bool MakeCollisionInfoText(DObjArray collisionArray)
        {
            foreach (DObjCollision coll in collisionArray)
            {
                // Left Collision
                if (coll.node.ID == this.NodeA.ID && coll.leftNode.ID == this.NodeB.ID)
                {
                    strCollisionInfo = String.Format("{0}/{1}/{2}/{3}", this.NodeA.leftNodeSpeed, Convert.ToString(this.NodeA.leftSteering).Substring(0, 1), coll.leftUBG.ToString(), coll.leftObstacle.ToString());
                    return true;
                }
                // Right Collision
                else if(coll.node.ID == this.NodeA.ID && coll.rightNode.ID == this.NodeB.ID)
                {
                    strCollisionInfo = String.Format("{0}/{1}/{2}/{3}", this.NodeA.rightNodeSpeed, Convert.ToString(this.NodeA.rightSteering).Substring(0, 1), coll.rightUBG.ToString(), coll.rightObstacle.ToString());
                    return true;
                }
            }

            if(String.IsNullOrEmpty(strCollisionInfo))
            {
                DObjMan.errMsg.Add(String.Format("Collision 정보를 찾을 수 없는 링크입니다. LeftNode: {0}, RightNode: {1}", this.NodeA.ID, this.NodeB.ID));
            }
            
            return false;
        }
        public bool MakeMapSpeedUpInfoText(DObjArray collisionArray)
        {
            foreach (DObjCollision coll in collisionArray)
            {
                // Left Collision
                if (coll.node.ID == this.NodeA.ID && coll.leftNode.ID == this.NodeB.ID)
                {
                    strMapSpeedUpInfo = Convert.ToString(this.NodeA.leftSteering).Substring(0, 1) + "/";
                    strMapSpeedUpInfo += Convert.ToString(this.NodeA.leftNodeSpeed/1000.0);
                    if (this.NodeA.NewSpeed > 0)
                        strMapSpeedUpInfo += "->" + Convert.ToString(this.NodeA.NewSpeed/1000.0);
                    if (this.NodeA.NewSpeed_raw > 0)
                    {
                        strMapSpeedUpInfo += "(" + Convert.ToString(this.NodeA.NewSpeed_raw / 1000.0) + ")";
                        //strMapSpeedUpInfo += "/D" + Convert.ToString(this.NodeA.NewCalDist / 1000.0);
                    }
                    return true;
                }
                // Right Collision
                else if (coll.node.ID == this.NodeA.ID && coll.rightNode.ID == this.NodeB.ID)
                {
                    strMapSpeedUpInfo = Convert.ToString(this.NodeA.rightSteering).Substring(0, 1) + "/";
                    strMapSpeedUpInfo += String.Format("{0}", this.NodeA.rightNodeSpeed/1000.0);
                    return true;
                }
            }

            if (String.IsNullOrEmpty(strCollisionInfo))
            {
                DObjMan.errMsg.Add(String.Format("Collision 정보를 찾을 수 없는 링크입니다. LeftNode: {0}, RightNode: {1}", this.NodeA.ID, this.NodeB.ID));
            }

            return false;
        }
        // 링크 길이 평균 구하기. 나중에 보기 좋은 scale 값을 구할 떄 쓰이기 때문에
        public static void CalcAverageLinkLength()
        {
            int nSum = 0;
            foreach(DObjLink link in DObjMan.arrLink)
            {
                nSum += link.distance_mm;
            }
            nAverageLinkLength_mm = (int)((float)nSum / (float)DObjMan.arrLink.Count);
        }
    }
}