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

    public class DObjCID : DObjBase
    {
        public CID_LayoutType layoutType;
        public string layoutID;
        public int resetNodeID;
        public int stopNode1ID;
        public int stopNode2ID;

        public DObjNode resetNode;
        public DObjNode stopNode1;
        public DObjNode stopNode2;

        public CID_ResetNodeType resetNodeType;

        public CID_Direction dir1;
        public int commonStartNode1_1;
        public int commonStartNode1_2;
        public int commonStartNode1_3;
        public int commonStartNode1_4;
        public int commonStartNode1_5;
        public int commonStartNode1Offset;

        public CID_Direction dir2;
        public int commonStartNode2_1;
        public int commonStartNode2_2;
        public int commonStartNode2_3;
        public int commonStartNode2_4;
        public int commonStartNode2_5;
        public int commonStartNode2Offset;

        public DObjNode cmNode11;
        public DObjNode cmNode12;
        public DObjNode cmNode13;
        public DObjNode cmNode14;
        public DObjNode cmNode15;

        public DObjNode cmNode21;
        public DObjNode cmNode22;
        public DObjNode cmNode23;
        public DObjNode cmNode24;
        public DObjNode cmNode25;

        public static Pen Pen_Area = new Pen(Color.LightGreen, 2);
        public static Font font = new Font("맑은 고딕", 10F, FontStyle.Regular);
        public static Font font2 = new Font("맑은 고딕", 9F, FontStyle.Regular);
        static bool bOnce = true;
        public DObjCID()
        {
            ObjType = eChildObjType.CID;
            Visible = false;        // 기본적으로 안보이는게 Default

            if (!bOnce)
                return;
            bOnce = false;

            Pen_Area.DashStyle = DashStyle.Dash;
        }
        public override void Draw(Graphics g, DScale scale)
        {
            if (!Visible)                return;
            //if (!IsInVisibleArea(scale))                return;

            DRect_mm rt_mm = GetRect_mm();
            Rectangle rectArea_dc = scale.MM2DC(rt_mm);
            Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());

            rectArea_dc.Width = Math.Max(2, rectArea_dc.Width);
            rectArea_dc.Height = Math.Max(2, rectArea_dc.Height);

            rectArea_dc.Inflate(40, 40);

            // 도면 배경을 투명하고 어둡게 해서 CID만 부각되게 한다
            g.FillRectangle(new SolidBrush(Color.FromArgb(150, Color.Black)), new Rectangle(0, 0, scale.nDrawHalfWidth_dc * 2, scale.nDrawHalfHeight_dc * 2));
            g.DrawRectangle(Pen_Area, rectArea_dc); //CID 전체 영역 점선 표시
            g.DrawString(GetID(), font, Brushes.White, rectArea_dc.X, rectArea_dc.Y);   //CID 이름 표시

            // stop 노드와 commonstart 가 같은 노드인 경우가 많기 때문에
            // 노드번호를 그릴 때 겹쳐져서 이상하게 보임
            // 때문에 이미 그린 노드라면 Y 옵셋을 줘서 다음줄에 보이는것 같이 해야함.
            // 꾀 테크닉이 필요함
            int nLineFeedPix = 13;
            DObjArray drawNodeArr = new DObjArray();
            if (resetNode != null)
            {
                drawNodeArr.Add(resetNode);
                resetNode.Draw_Custom(g, scale, 0, " Reset");
            }
            if (stopNode1 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == stopNode1);
                stopNode1.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " STOP1");
                drawNodeArr.Add(stopNode1);
            }
            if (stopNode2 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == stopNode2);
                stopNode2.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " STOP2");
                drawNodeArr.Add(stopNode2);
            }
            if (cmNode11 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode11);
                cmNode11.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM1_1");
                drawNodeArr.Add(cmNode11);
            }
            if (cmNode12 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode12);
                cmNode12.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM1_2");
                drawNodeArr.Add(cmNode12);
            }
            if (cmNode13 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode13);
                cmNode13.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM1_3");
                drawNodeArr.Add(cmNode13);
            }
            if (cmNode14 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode14);
                cmNode14.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM1_4");
                drawNodeArr.Add(cmNode14);
            }
            if (cmNode15 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode15);
                cmNode15.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM1_5");
                drawNodeArr.Add(cmNode15);
            }
            if (cmNode21 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode21);
                cmNode21.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM2_1");
                drawNodeArr.Add(cmNode21);
            }
            if (cmNode22 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode22);
                cmNode22.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM2_2");
                drawNodeArr.Add(cmNode22);
            }
            if (cmNode23 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode23);
                cmNode23.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM2_3");
                drawNodeArr.Add(cmNode23);
            }
            if (cmNode24 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode24);
                cmNode24.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM2_4");
                drawNodeArr.Add(cmNode24);
            }
            if (cmNode25 != null)
            {
                List<DObjBase> alreadyDraw = drawNodeArr.FindAll(o => o == cmNode25);
                cmNode25.Draw_Custom(g, scale, alreadyDraw.Count * nLineFeedPix, " CM2_5");
                drawNodeArr.Add(cmNode25);
            }
        }
        public override string GetID() { return layoutID; }
        public override int GetID_Int() { return 0; }
        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }
        public override DRect_mm GetRect_mm()
        {
            DRect_mm rect = new DRect_mm();
            if(resetNode != null)
                rect.SumOfSet(resetNode.GetRect_mm());
            if(stopNode1 != null)
                rect.SumOfSet(stopNode1.GetRect_mm());
            if(stopNode2 != null)
                rect.SumOfSet(stopNode2.GetRect_mm());
            if (cmNode11 != null) rect.SumOfSet(cmNode11.GetRect_mm());
            if (cmNode12 != null) rect.SumOfSet(cmNode12.GetRect_mm());
            if (cmNode13 != null) rect.SumOfSet(cmNode13.GetRect_mm());
            if (cmNode14 != null) rect.SumOfSet(cmNode14.GetRect_mm());
            if (cmNode15 != null) rect.SumOfSet(cmNode15.GetRect_mm());
            if (cmNode21 != null) rect.SumOfSet(cmNode21.GetRect_mm());
            if (cmNode22 != null) rect.SumOfSet(cmNode22.GetRect_mm());
            if (cmNode23 != null) rect.SumOfSet(cmNode23.GetRect_mm());
            if (cmNode24 != null) rect.SumOfSet(cmNode24.GetRect_mm());
            if (cmNode25 != null) rect.SumOfSet(cmNode25.GetRect_mm());
            rect.NormalizeRect();

            return rect;
        }
        
        public static Pen Pen_Normal = null;  // 기본 그리기 펜
        public static Pen Pen_Selected = null;    // 도형 선택 됐을 때 그려질 펜

        // 자신이랑 관련된 node들,   reset, stop 노드들을 번호만 가지고 있기 때문에
        // 모든 노드 돌면서 해당 번호를 갖은 node 포인터를 갖고 있는다
        public void FindNodeInfo()
        {
            DObjNode objNode = null;

            // reset node
            if (this.resetNodeID > 0)
            {
                objNode = DObjNode.FindNode(this.resetNodeID);
                if (objNode != null)
                {
                    this.resetNode = objNode;
                    objNode.cidRefType = eCIDRefType.ResetNode;
                    objNode.bIsResetNode = true;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 리셋노드 [{1}] 가 없습니다.", this.layoutID, this.resetNode));
            }

            // stop node1
            if (this.stopNode1ID > 0)
            {
                objNode = DObjNode.FindNode(this.stopNode1ID);

                if (objNode != null)
                {
                    this.stopNode1 = objNode;
                    objNode.cidRefType = eCIDRefType.StopNode1;
                    objNode.bIsStopNode = true;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 StopNod1 [{1}] 가 없습니다.", this.layoutID, this.stopNode1));
            }

            // stop node2
            if (this.stopNode2ID > 0)
            {
                objNode = DObjNode.FindNode(this.stopNode2ID);
                if (objNode != null)
                {
                    this.stopNode2 = objNode;
                    objNode.cidRefType = eCIDRefType.StopNode2;
                    objNode.bIsStopNode2 = true;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 StopNod2 [{1}] 가 없습니다.", this.layoutID, this.stopNode2));
            }

            if (this.commonStartNode1_1 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode1_1);
                if (objNode != null)
                {
                    this.cmNode11 = objNode;
                    objNode.commonStartType = eCommonStartType.CS11;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart1_1 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode1_1));
            }
            if (this.commonStartNode1_2 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode1_2);
                if (objNode != null)
                {
                    this.cmNode12 = objNode;
                    objNode.commonStartType = eCommonStartType.CS12;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart1_2 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode1_2));
            }
            if (this.commonStartNode1_3 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode1_3);
                if (objNode != null)
                {
                    this.cmNode13 = objNode;
                    objNode.commonStartType = eCommonStartType.CS13;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart1_3 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode1_3));
            }
            if (this.commonStartNode1_4 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode1_4);
                if (objNode != null)
                {
                    this.cmNode14 = objNode;
                    objNode.commonStartType = eCommonStartType.CS14;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart1_4 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode1_4));
            }
            if (this.commonStartNode1_5 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode1_5);
                if (objNode != null)
                {
                    this.cmNode15 = objNode;
                    objNode.commonStartType = eCommonStartType.CS15;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart1_5 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode1_5));
            }
            if (this.commonStartNode2_1 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode2_1);
                if (objNode != null)
                {
                    this.cmNode21 = objNode;
                    objNode.commonStartType = eCommonStartType.CS21;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart2_1 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode2_1));
            }
            if (this.commonStartNode2_2 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode2_2);
                if (objNode != null)
                {
                    this.cmNode22 = objNode;
                    objNode.commonStartType = eCommonStartType.CS22;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart2_2 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode2_2));
            }
            if (this.commonStartNode2_3 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode2_3);
                if (objNode != null)
                {
                    this.cmNode23 = objNode;
                    objNode.commonStartType = eCommonStartType.CS23;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart2_3 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode2_3));
            }
            if (this.commonStartNode2_4 > 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode2_4);
                if (objNode != null)
                {
                    this.cmNode24 = objNode;
                    objNode.commonStartType = eCommonStartType.CS24;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart2_4 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode2_4));
            }
            if (this.commonStartNode2_5 != 0)
            {
                objNode = DObjNode.FindNode(this.commonStartNode2_5);
                if (objNode != null)
                {
                    this.cmNode25 = objNode;
                    objNode.commonStartType = eCommonStartType.CS25;
                }
                else
                    DObjMan.errMsg.Add(String.Format("CID ID {0}의 CommonsStart2_5 [{1}] 가 없습니다.", this.layoutID, this.commonStartNode2_5));
            }


            return;
            /////////////////////////////////

            foreach (DObjNode node in DObjMan.arrNode)
            {
                if (this.resetNodeID == node.ID)
                {
                    this.resetNode = node;
                    node.cidRefType = eCIDRefType.ResetNode;
                }
                else if (this.stopNode1ID == node.ID)
                {
                    this.stopNode1 = node;
                    node.cidRefType = eCIDRefType.StopNode1;
                }
                else if (this.stopNode2ID == node.ID)
                {
                    this.stopNode2 = node;
                    node.cidRefType = eCIDRefType.StopNode2;
                }

                if (this.commonStartNode1_1 != 0 && this.commonStartNode1_1 == node.ID)
                    cmNode11 = node;
                if (this.commonStartNode1_2 != 0 && this.commonStartNode1_2 == node.ID)
                    cmNode12 = node;
                if (this.commonStartNode1_3 != 0 && this.commonStartNode1_3 == node.ID)
                    cmNode13 = node;
                if (this.commonStartNode1_4 != 0 && this.commonStartNode1_4 == node.ID)
                    cmNode14 = node;
                if (this.commonStartNode1_5 != 0 && this.commonStartNode1_5 == node.ID)
                    cmNode15 = node;
                if (this.commonStartNode2_1 != 0 && this.commonStartNode2_1 == node.ID)
                    cmNode21 = node;
                if (this.commonStartNode2_1 != 0 && this.commonStartNode2_2 == node.ID)
                    cmNode22 = node;
                if (this.commonStartNode2_1 != 0 && this.commonStartNode2_3 == node.ID)
                    cmNode23 = node;
                if (this.commonStartNode2_1 != 0 && this.commonStartNode2_4 == node.ID)
                    cmNode24 = node;
                if (this.commonStartNode2_1 != 0 && this.commonStartNode2_5 == node.ID)
                    cmNode25 = node;
            }

            if (this.resetNode == null && this.resetNodeID == 0)
                DObjMan.errMsg.Add(String.Format("CID ID {0}의 리셋노드가 없습니다.", this.layoutID));
            if (this.resetNode == null && this.resetNodeID != 0)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 리셋노드 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.resetNodeID));
            if (this.stopNode1ID != 0 && this.stopNode1 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 StopNode1 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.stopNode1ID));
            if (this.stopNode2ID != 0 && this.stopNode2 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 StopNode2 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.stopNode2ID));
            if(this.stopNode1 == null && this.stopNode2 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 stop node 1, 2 둘 다 없습니다. ", this.layoutID));
            if (this.commonStartNode1_1 != 0 && this.cmNode11 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode1_1 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode1_1));
            if (this.commonStartNode1_2 != 0 && this.cmNode12 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode1_2 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode1_2));
            if (this.commonStartNode1_3 != 0 && this.cmNode13 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode1_3 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode1_3));
            if (this.commonStartNode1_4 != 0 && this.cmNode14 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode1_4 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode1_4));
            if (this.commonStartNode1_5 != 0 && this.cmNode15 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode1_5 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode1_5));

            if (this.commonStartNode2_1 != 0 && this.cmNode21 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode2_1 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode2_1));
            if (this.commonStartNode2_2 != 0 && this.cmNode22 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode2_2 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode2_2));
            if (this.commonStartNode2_3 != 0 && this.cmNode23 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode2_3 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode2_3));
            if (this.commonStartNode2_4 != 0 && this.cmNode24 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode2_4 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode2_4));
            if (this.commonStartNode2_5 != 0 && this.cmNode25 == null)
                DObjMan.errMsg.Add(String.Format("CID ID {0} 에서 CommonNode2_5 ID {1}을 노드정보에서 찾을 수 없습니다.", this.layoutID, this.commonStartNode2_5));
        }
        // OcsLayout파일에서 [CID_DATA] 부분을 파싱한다
        public static bool ParseMapDataCIDLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 20)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)) );
                return false;
            }

            DObjCID newCID = new DObjCID();

            newCID.layoutType = (CID_LayoutType)Convert.ToInt32(strArr[0]);
            newCID.layoutID = strArr[1];
            newCID.resetNodeID = Convert.ToInt32(strArr[2]);
            newCID.stopNode1ID = Convert.ToInt32(strArr[3]);
            newCID.stopNode2ID = Convert.ToInt32(strArr[4]);
            newCID.resetNodeType = (CID_ResetNodeType)Convert.ToInt32(strArr[5]);
            newCID.dir1 = (CID_Direction)Convert.ToInt32(strArr[6]);
            newCID.commonStartNode1_1 = Convert.ToInt32(strArr[7]);
            newCID.commonStartNode1_2 = Convert.ToInt32(strArr[8]);
            newCID.commonStartNode1_3 = Convert.ToInt32(strArr[9]);
            newCID.commonStartNode1_4 = Convert.ToInt32(strArr[10]);
            newCID.commonStartNode1_5 = Convert.ToInt32(strArr[11]);
            newCID.commonStartNode1Offset = Convert.ToInt32(strArr[12]);
            newCID.dir2 = (CID_Direction)Convert.ToInt32(strArr[13]);
            newCID.commonStartNode2_1 = Convert.ToInt32(strArr[14]);
            newCID.commonStartNode2_2 = Convert.ToInt32(strArr[15]);
            newCID.commonStartNode2_3 = Convert.ToInt32(strArr[16]);
            newCID.commonStartNode2_4 = Convert.ToInt32(strArr[17]);
            newCID.commonStartNode2_5 = Convert.ToInt32(strArr[18]);
            newCID.commonStartNode2Offset = Convert.ToInt32(strArr[19]);

            objArray.Add(newCID);
            return true;
        }
        public static DObjCID FindCID(string layoutID)
        {
            DObjArray arr = DObjMan.arrCID;
            foreach (DObjCID cid in arr)
            {
                if (cid.layoutID == layoutID)
                    return cid;
            }
            return null;
        }
    }

}
