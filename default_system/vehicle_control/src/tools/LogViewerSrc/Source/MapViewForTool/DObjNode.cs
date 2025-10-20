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
    public enum eCIDRefType  {  None, ResetNode,  StopNode1, StopNode2}
    public enum eCommonStartType { None, CS11, CS12, CS13, CS14, CS15, CS21, CS22, CS23, CS24, CS25};

    public class DObjNode : DObjBase
    {
        public int ID;
        public int X_mm = int.MinValue;
        public int Y_mm = int.MinValue;
        public int Z_mm = int.MinValue;        //position [mm]
        public static int MinZ_mm = 0;
        public static int MaxZ_mm = 0;
        
        public eNodeType nodeType = eNodeType.Normal;
        public eCommonStartType commonStartType = eCommonStartType.None;
        public int leftNodeID;
        public int leftNodeDist;
        public int leftNodeSpeed;
        public eSteering leftSteering;
        public eBranch leftBranch;

        public int rightNodeID;
        public int rightNodeDist;
        public int rightNodeSpeed;
        public eSteering rightSteering;
        public eBranch rightBranch;

        public static Pen Pen_Normal = new Pen(Color.FromArgb(0xEF, 0xEF, 0xEF), 2);          // 노리끼리한 색
        public static Pen Pen_Hover = new Pen(Color.FromArgb(0xEF, 0xEF, 0xEF), 2);        
        public static SolidBrush Brush_Hover = new SolidBrush(Color.FromArgb(0xEF, 0xEF, 0xEF));      
        public static Font Font_Big = new Font("맑은 고딕", 10F, FontStyle.Regular);
        public static Font Font_Middle = new Font("맑은 고딕", 9F, FontStyle.Regular);
		public static Font Font_Small = new Font("맑은 고딕", 7F, FontStyle.Regular);

		// 자신의 노드가 CID와 관련이 있으면 그 CID를 참조한다
		// 이 변수를 세팅하는 부분은 DObjCID 클래스의 FindNodeInfo() 에서 한번에 한다
		//public DObjCID relatedCID = null;
		public eCIDRefType cidRefType = eCIDRefType.None;  // relatedCID와 연결되었을 때 어떤 걸로 연결되어 있는지,  reset노드인지 stop노드인지
        public static int nNodeSize_mm = 80;
        static bool bOnce = true;

        public int reserved_data = 0;

        // 맵증속용
        public int NewSpeed = 0;
        public int NewSpeed_raw = 0;
        public int NewCalDist = 0;

        public bool bIsCSnode = false;
        public bool bIsStopNode = false;
        public bool bIsStopNode2 = false;
        public bool bIsResetNode = false;
        public bool bIsFlowControl = false;


        public DObjNode()
        {
            ObjType = eChildObjType.Node;
            reserved_data = 0;
            if (!bOnce) // static 변수들 한번만 할당하기 위해서
                return;
            bOnce = false;
        }
        public void CopyFrom(DObjNode src)
        {
            this.ID = src.ID;
            this.X_mm = src.X_mm;
            this.Y_mm = src.Y_mm;
            this.Z_mm = src.Z_mm;
            this.nodeType = src.nodeType;
            this.leftNodeID = src.leftNodeID;
            this.leftNodeDist = src.leftNodeDist;
            this.leftNodeSpeed = src.leftNodeSpeed;
            this.leftSteering = src.leftSteering;
            this.leftBranch = src.leftBranch;
            this.rightNodeID = src.rightNodeID;  
            this.rightNodeDist = src.rightNodeDist;
            this.rightNodeSpeed = src.rightNodeSpeed;
            this.rightSteering = src.rightSteering;
            this.rightBranch = src.rightBranch;
            //this.relatedCID = src.relatedCID;
            this.cidRefType = src.cidRefType;
            this.reserved_data = src.reserved_data;
        }
        public override string GetHoverInfo()
        {
            return String.Format("Node ID: {0}\nType: {1}\nXyz: {2}, {3}, {4}\nLeftNode: {5}\nLeftNodeDist: {6}\nLeftSpeed: {7}\nLeftSteering: {8}\nLeftBranch: {9}\nRightNode: {10}\nRightNodeDist: {11}\nRightSpeed: {12}\nRightSteering: {13}\nRightBranch: {14}"
            , ID
            , nodeType.ToString()
            , X_mm
            , Y_mm
            , Z_mm
            , leftNodeID
            , leftNodeDist
            , leftNodeSpeed
            , leftSteering.ToString()
            , leftBranch.ToString()
            , rightNodeID
            , rightNodeDist
            , rightNodeSpeed
            , rightSteering.ToString()
            , rightBranch.ToString()
                );
        }

        // OCSLayout에서 파싱을 하는데 [MAP_DATA] 부분에도 Node 정보가 있고 [NODE] 부분는 위치에 대한 정보가 있다.
        // 그래서 두번 따로따로 읽어야 하는데 처음에 읽고 objArray에 node 객체들이 들어있고
        // 두번째 읽을땐 objArray에서 자신의 node를 찾아서 파싱된 정보를 넣어준다
        // 여긴 [MAP_DATA] 파싱에 대한 부분
        public static bool ParseMapDataFile(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 12)
            {
                DObjMan.errMsg.Add(String.Format("MapData.dat에서 정보를 얻을 수 없습니다."));
                return false;
            }

            int nodeID = Convert.ToInt32(strArr[0]);

            // 파싱하는 모든 라인마다 검색을 하면 느려질 수 있다...
            int idx = 0;
            bool bExists = false;
            foreach (DObjNode aNode in objArray)
            {
                if (aNode.ID == nodeID)
                {
                    bExists = true;
                    break;
                }
                idx++;
            }

            if (!bExists)
            {
                DObjMan.errMsg.Add(String.Format("{0} Node는 OcsLayout 파일는 있고 MapData.Dat 파일에는 없습니다.", nodeID));
                return false;
            }

            DObjNode theNode = (DObjNode)objArray.ElementAt(idx);
         

            theNode.nodeType = (eNodeType)Convert.ToInt32(strArr[1]);
            theNode.leftNodeID = Convert.ToInt32(strArr[2]);
            theNode.leftNodeDist = Convert.ToInt32(strArr[3]);
            theNode.leftNodeSpeed = Convert.ToInt32(strArr[4]);
            theNode.leftSteering = (eSteering)Convert.ToInt32(strArr[5]);
            theNode.leftBranch = (eBranch)Convert.ToInt32(strArr[6]);
            theNode.rightNodeID = Convert.ToInt32(strArr[7]);
            theNode.rightNodeDist = Convert.ToInt32(strArr[8]);
            theNode.rightNodeSpeed = Convert.ToInt32(strArr[9]);
            theNode.rightSteering = (eSteering)Convert.ToInt32(strArr[10]);
            theNode.rightBranch = (eBranch)Convert.ToInt32(strArr[11]);
            return true;
        }

        // 여긴 [NODE] 파싱에 대한 부분
        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 3)
            {
                DObjMan.errMsg.Add(String.Format("OcsLayout.dat에서 node정보 파싱 실패 line: {0}", line));
                return false;
            }

            DObjNode newNode = new DObjNode();
            try
            {
                newNode.ID = Convert.ToInt32(strArr[0]);
                newNode.X_mm = Convert.ToInt32(strArr[1]);
                newNode.Y_mm = Convert.ToInt32(strArr[2]);
                newNode.Z_mm = Convert.ToInt32(strArr[3]);

                if(DObjNode.MaxZ_mm < newNode.Z_mm)
                    DObjNode.MaxZ_mm = newNode.Z_mm;
                if (DObjNode.MinZ_mm > newNode.Z_mm)
                    DObjNode.MinZ_mm = newNode.Z_mm;

                DObjBase obj = DObjMan.arrNode.Find(o => o.GetID_Int() == newNode.ID);
                if (obj != null)
                {
                    DObjMan.errWarningMsg.Add(Convert.ToString(newNode.ID) + " - OcsLayout.dat에 중복되는 노드");
                }

            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(newNode);
            return true;

        }
        

        public override void Draw(Graphics g, DScale scale)
        {
            if (X_mm == Int32.MinValue)
                return;

            if (!bDrawNode)                return;
            if(!VisibleZRange.IsInRange(Z_mm))      return;
            if (!IsInVisibleArea(scale))                return;
            
            DRect_mm rt_mm = GetRect_mm();
            Rectangle rect_dc = scale.MM2DC(rt_mm);
            Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());
            
            rect_dc.Width = Math.Max(2, rect_dc.Width);
            rect_dc.Height = Math.Max(2, rect_dc.Height);
            bool isMouseHover = rt_mm.PtInRect(DObjBase.curMousePos_mm);
            if (isMouseHover)
            {
                rect_dc.Inflate(2, 2);
                g.FillRectangle(Brush_Hover, rect_dc);
                g.DrawRectangle(Pen_Hover, rect_dc);
            }
            else if (Selected)
            {
                rect_dc.Inflate(2, 2);
                g.FillRectangle(Brushes.Turquoise, rect_dc);
                g.DrawRectangle(Pen_Hover, rect_dc);
            }
            else
            {
                g.DrawRectangle(Pen_Normal, rect_dc);
            }

            if (scale.dScale < VISIBLE_SCALE_NODE_TEXT)
            {
                int xdc = rect_dc.X + rect_dc.Width + 1;
                int ydc = rect_dc.Y - 3;

				// 스케일이 조밀할때는 작은 폰트, 확대됐을때는 큰폰트
				Font applyFont = scale.dScale < 20 ? Font_Big : Font_Small;
				
                if(bDrawNodeDetail) //detail view
                    g.DrawString(GetNodeDetailText(), applyFont, Brushes.Black, xdc, ydc);
                else // normal view
                    g.DrawString(GetID(), applyFont, Brushes.Black, xdc, ydc);
            }

            //if (isMouseHover)
            //{
            //    DHoverInfo h = new DHoverInfo();
            //    h.strInfo = GetID();
            //    h.pos_dc.X = rect_dc.X + rect_dc.Width + 10;
            //    h.pos_dc.Y = rect_dc.Y;

            //    DObjMan.HoverInfoArr.Add(h);        // hover된거 여기서 직접 그리진 않고 어레이에 추가하고 나중에 그려줘야 한다.
            //    // 왜냐면 겹쳐진 경우 호버링 정보가 겹쳐져서 이상하게 보임
            //}
        }
        
        // 화면에 세부정보까지 표시
        public string GetNodeDetailText()
        {
            string str = Convert.ToString(ID);

            if (nodeType != eNodeType.Normal)
                str += "/" + nodeType.ToString();
            if(commonStartType != eCommonStartType.None)
                str += "/" + commonStartType.ToString();
            if (bIsResetNode)
                str += "/Reset";
            if (bIsStopNode)
                str += "/Stop";
            if (bIsStopNode2)
                str += "/Stop2";
            if(bIsFlowControl)
                str += "/FC";

            return str;
        }

        public int GetSpeed(int NextNodeID)
        {
            return NextNodeID == rightNodeID ? rightNodeSpeed : leftNodeSpeed;
        }

        public eSteering GetSteer(int NextNodeID)
        {
            return NextNodeID == rightNodeID ?  rightSteering : leftSteering;
        }

        public int GetDistance(int NextNodeID)
        {
            return NextNodeID == leftNodeID ?  rightNodeDist : leftNodeDist;
        }

        public void Draw_Custom(Graphics g, DScale scale, int nYOffset_dc, string str)
        {
            if (X_mm == Int32.MinValue)
                return;

            if (!IsInVisibleArea(scale)) return;

            DRect_mm rt_mm = GetRect_mm();
            Rectangle rect_dc = scale.MM2DC(rt_mm);
            Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());

            rect_dc.Width = Math.Max(2, rect_dc.Width);
            rect_dc.Height = Math.Max(2, rect_dc.Height);
            bool isMouseHover = rt_mm.PtInRect(DObjBase.curMousePos_mm);
            if (isMouseHover)
            {
                rect_dc.Inflate(2, 2);
                g.FillRectangle(Brushes.LightYellow, rect_dc);
            }
            else if (Selected)
            {
                rect_dc.Inflate(2, 2);
                g.FillRectangle(Brushes.Turquoise, rect_dc);
            }
            else
            {
                g.DrawRectangle(Pen_Normal, rect_dc);
            }

            //if (scale.dScale < VISIBLE_SCALE_NODE_TEXT)
            {
                int xdc = rect_dc.X + rect_dc.Width + 1;
                int ydc = rect_dc.Y - 3;
                g.DrawString(GetID()+ str, Font_Big, Brushes.Orchid, xdc, ydc + nYOffset_dc);

            }
        }
        public override string GetID()
        {
            return String.Format("{0}", ID); ;
        }
        public override int GetID_Int() { return ID; }
        public Point Get2DPoint()
        {
            return new Point(X_mm, Y_mm);
        }
        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }

        DRect_mm tmpRect = new DRect_mm();
        public override DRect_mm GetRect_mm()
        {
            //tmpRect.SumOfSet(Get2DPoint());
            //tmpRect.NormalizeRect();
            tmpRect.xMin = X_mm - nNodeSize_mm / 2;
            tmpRect.xMax = X_mm  + nNodeSize_mm / 2;
            tmpRect.yMax = Y_mm + nNodeSize_mm / 2;
            tmpRect.yMin = Y_mm - nNodeSize_mm / 2;
            return tmpRect;
        }



        // 노드들은 매우 많기 때문에 로딩 시점에 오름차순으로 소팅 해 놓는다. 그럼 이후에 매우 빠르게 접근해서 쓸수있다.
        // 퀵소팅과 분할정복 검색 알고리즘을 쓰면 케미가 잘 맞는다
        public static void SortNodes()
        {
            DObjBase []arr = DObjMan.arrNode.ToArray();
        }

        public static DObjNode FindNode(int id)
        {

            DObjArray arr = DObjMan.arrNode;

            int nLowIdx = 0;
            int nHighIdx = arr.Count;
            int nCurIdx = arr.Count / 2;
            int nPrevIdx = -1;
            int Cnt = 0;
            bool bFound = false;
            int nVal = 0;

            // 분할정복 알고리즘. Searching
            while (nLowIdx < nHighIdx)
            {
                if (nPrevIdx == nCurIdx)    // 못찾은 경우
                    break;

                nPrevIdx = nCurIdx;

                if (nCurIdx < 0 || nCurIdx >= arr.Count)
                    break;
                nVal = arr[nCurIdx].GetID_Int();

                if (nVal == id) // 찾음
                {
                    bFound = true;
                    break;
                }
                else if (nVal < id) // 더 커
                {
                    nLowIdx = nCurIdx + 1;
                    nCurIdx = (nHighIdx + nCurIdx) / 2;
                }
                else // 더 작어
                {
                    nHighIdx = nCurIdx;
                    nCurIdx = (nLowIdx + nCurIdx) / 2;
                }

                
                Cnt++;
            }

            if (!bFound)
                return null;

            //Trace.WriteLine(String.Format("FindNode: {0}", Cnt));
            return (DObjNode)arr[nCurIdx];


            //DObjArray arr = DObjMan.arrNode;
            //foreach (DObjNode node in arr)
            //{
            //    if (node.ID == id)
            //        return node;
            //}
            //return null;
        }

    }
}
