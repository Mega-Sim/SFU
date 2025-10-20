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

    public class DObjStation : DObjBase
    {
        public int ID;
        public eStationType type;
        public eStationPortType portType;
        public eMaterialType materialType;

        // Layout file에 스테이션의 직접적인 좌표 위치는 없고 parent node와 next node사이의 몇mm 거리에 있다는 정보만 있다
        // x, y, z 좌표 위치는 파일 파싱 후 계산된 위치임 position [mm]
        public int X_mm = Int32.MinValue;
        public int Y_mm = Int32.MinValue;
        public int Z_mm = Int32.MinValue;

        public int X_STBmm = Int32.MinValue;
        public int Y_STBmm = Int32.MinValue;

        public Point ptSTBLinkPos_mm = new Point();
        public DObjNode parentNode = null;
        public DObjNode nextNode = null;
        public DObjLink parentLink = null;      // 스테이션은 반드시 링크 위에 위치하게 된다
        public int distance_mm = -1;
        
        public string portID;

        public static Pen Pen_Normal = new Pen(Color.DarkGray, 2);  // 기본 그리기 펜
        public static Pen Pen_Selected = new Pen(Color.DarkGray, 3);    // 도형 선택 됐을 때 그려질 펜
        public static Pen Pen_dotSyle = new Pen(Color.White, 1);

        public static Pen Pen_Hover = new Pen(Color.FromArgb(0x01, 0xFF, 0x01), 2);
        public static SolidBrush Brush_Hover = new SolidBrush(Color.FromArgb(0x00, 0x99, 0x46));

        public static int nDrawSize_mm = 120;
        
        //public string strSimpleInfo = "";
        //public Point[] arrSTBPolygon = null;
        public static Font Font_Big = new Font("맑은 고딕", 10F, FontStyle.Regular);
		public static Font Font_Small = new Font("맑은 고딕", 7F, FontStyle.Regular);
        private static Point _ptLinkPos_dc = new Point();
        private bool isMouseHover = false;
        public static bool bOnce = true;
        public DObjStation()
        {
            ObjType = eChildObjType.Station;

            // 한번만 호출됨 *****************************
            if (!bOnce)
                return;
            bOnce = false;
            
            Pen_Normal.Brush = new SolidBrush(Color.LightGray);
            Pen_dotSyle.DashStyle = DashStyle.Dot;
            Pen_Selected.Brush = new SolidBrush(Color.LightGray);
            
        }
        

        public override string GetHoverInfo()
        {
            return String.Format("Station ID: {0}\nPortType: {1}\nXyz: {2}, {3}, {4}\nType: {5}\nParentNode: {6}\nNextNode: {7}\nOffset: {8}\nMaterial: {9}"
            , ID
            , portType.ToString()
            , X_mm
            , Y_mm
            , Z_mm
            , type.ToString()
            , parentNode.ID
            , nextNode.ID
            , distance_mm
            , materialType.ToString()
            );
            
        }
        public override void Draw(Graphics g, DScale scale)
        {
            try
            {
                if (X_mm == Int32.MinValue)
                    return;

                if (!bDrawStation)
                    return;

                if (!VisibleZRange.IsInRange(Z_mm))
                    return;

                if (!IsInVisibleArea(scale))
                    return;
                
                DRect_mm rt_mm = GetRect_mm();
                Rectangle rect_dc = scale.MM2DC(rt_mm);
                Point ptCenter_dc = scale.MM2DC(rt_mm.CenterPoint());
                
                _ptLinkPos_dc.X = scale.MM2DC_X(X_mm);
                _ptLinkPos_dc.Y = scale.MM2DC_Y(Y_mm);

				if (rect_dc.Width <= 1 || rect_dc.Height <= 1)
					return;

                rect_dc.Width = Math.Max(2, rect_dc.Width);
                rect_dc.Height = Math.Max(2, rect_dc.Height);
                isMouseHover = rt_mm.PtInRect(DObjBase.curMousePos_mm);

                //if (isMouseHover)       
                //    rect_dc.Inflate(nHoverAddSize, nHoverAddSize);

                if (portType == eStationPortType.LSTB || portType == eStationPortType.RSTB)
                {
                    g.DrawLine(Pen_dotSyle, ptCenter_dc, _ptLinkPos_dc);
                    if(isMouseHover)
                    {
                        rect_dc.Inflate(2, 2);
                        g.FillRectangle(Brush_Hover, rect_dc);
                        g.DrawRectangle(Pen_Hover, rect_dc);
                    }
                    else if(Selected)
                    {
                        rect_dc.Inflate(2, 2);
                        g.FillRectangle(Brushes.White, rect_dc);
                        g.DrawRectangle(Pen_Hover, rect_dc);
                    }
                    else
                    {
                        g.DrawRectangle(Pen_Normal, rect_dc);
                    }
                }
                else
                {
                    if (isMouseHover)
                    {
                        rect_dc.Inflate(2, 2);
                        g.FillEllipse(Brush_Hover, rect_dc);
                        g.DrawEllipse(Pen_Hover, rect_dc);
                    }
                    else if (Selected)
                    {
                        rect_dc.Inflate(2, 2);
                        g.FillEllipse(Brushes.White, rect_dc);
                        g.DrawEllipse(Pen_Hover, rect_dc);
                    }
                    else
                    {
                        g.DrawEllipse(Pen_Normal, rect_dc);
                    }
                }

                if(bDrawStationID && scale.dScale < VISIBLE_SCALE_STATION_TEXT)
                {
                    int xdc = rect_dc.X - 15;
                    int ydc = rect_dc.Y + rect_dc.Height-2;

					// 스케일이 조밀할때는 작은 폰트, 확대됐을때는 큰폰트
					Font applyFont = scale.dScale < 20 ? Font_Big : Font_Small;

					g.DrawString( GetID(), applyFont, Brushes.Beige, xdc, ydc);
                    
                }
             
                //if(isMouseHover)
                //{
                //    DHoverInfo h = new DHoverInfo();
                //    h.strInfo = strSimpleInfo;
                //    h.pos_dc.X = rect_dc.X + rect_dc.Width + 10;
                //    h.pos_dc.Y = rect_dc.Y;

                //    DObjMan.HoverInfoArr.Add(h);        // hover된거 여기서 직접 그리진 않고 어레이에 추가하고 나중에 그려줘야 한다.
                //    // 왜냐면 겹쳐진 경우 호버링 정보가 겹쳐져서 이상하게 보임
                //}

            }
            catch (Exception)
            {

                throw;
            }
        }
        public Point Get2DPoint()
        {
            if(portType == eStationPortType.LSTB || portType == eStationPortType.RSTB)
                return new Point(X_STBmm, Y_STBmm);

            return new Point(X_mm, Y_mm);
        }
        public override string GetID()
        {
            return String.Format("{0}", ID);
        }
        public override int GetID_Int() { return ID; }
        public override void AddToRect(ref DRect_mm rect)
        {
            rect.SumOfSet(GetRect_mm());
        }
        public override DRect_mm GetRect_mm()
        {
            DRect_mm rect = new DRect_mm();
            rect.SumOfSet(Get2DPoint());
            rect.xMin -= nDrawSize_mm / 2;
            rect.xMax += nDrawSize_mm / 2;
            rect.yMin -= nDrawSize_mm / 2;
            rect.yMax += nDrawSize_mm / 2;
            
            rect.NormalizeRect();

            return rect;
        }

        // 속도 향상을 위해서, 매번 new 되는것 방지하기 위해 글로벌 point만들고 계속쓴다
        static DPoint_mm ptParentPos = new DPoint_mm();
        static DPoint_mm tmpPoint2_mm = new DPoint_mm();
        static DPoint_mm tmpPoint3_mm = new DPoint_mm();

        static DPoint_mm tmpRetPos_mm = new DPoint_mm();
        static DPoint_mm tmpRetPos2_mm = new DPoint_mm();

        static DPoint_mm _ptA = new DPoint_mm();
        static DPoint_mm _ptB = new DPoint_mm();
		static bool CheckValidation = false;
		public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 3)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }

            //[STATION] 
            //562730/DUAL_ACCESS/562009/562010/689/ST-562730/LSTB/FOUP =>
            //번호 /type/이전노드(parent node)/이후노드(nextnode)/이전노드부터의 거리mm/포트아이디/왼쪽L-STB/자재종류 foup            
            DObjStation st = new DObjStation();
            try
            {
                st.ID = Convert.ToInt32(strArr[0]);
                int nParentNode = Convert.ToInt32(strArr[2]);
                int nNextNode = Convert.ToInt32(strArr[3]);
				
				st.parentNode = DObjNode.FindNode(nParentNode);

				if (st.parentNode == null)
				{
					DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 을 생성할수 없습니다. Parent Node {1}를 찾을  수 없습니다.", st.ID, nParentNode));
					return false;
				}

				st.nextNode = DObjNode.FindNode(nNextNode);
				if (st.nextNode == null)
				{
					DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 을 생성할수 없습니다. Next Node {1}를 찾을  수 없습니다.", st.ID, nNextNode));
					return false;
				}

				st.parentLink = DObjLink.FindLink(nParentNode, nNextNode);      // 소속된 링크를 찾는다
				if (st.parentLink == null)
				{
					DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 을 생성할수 없습니다. Link {1}_{2}를 찾을  수 없습니다.", st.ID, nParentNode, nParentNode));
					return false;
				}

				// Validation check 하지 않을 경우 맵 로딩 속도 향상 됨
				if (CheckValidation)
				{
					DObjBase obj = DObjMan.arrStation.Find(o => o.GetID_Int() == st.ID);
					if (obj != null)
					{
					    DObjMan.errWarningMsg.Add(Convert.ToString(st.ID) + " - OcsLayout.dat에 중복되는 존재하는 스테이션");
					}
				}

                st.distance_mm = Convert.ToInt32(strArr[4]);
                st.portID = strArr[5];

                // distance를 기반으로 두개 노드 사이 어디 X, Y 위치에 있는지 알아내기
                if(st.parentLink.distance_mm <= st.distance_mm)
                    DObjMan.errWarningMsg.Add(String.Format("스테이션 Distance가 링크보다 길거나 같습니다. [ID: {0}, link length: {1}, station offset: {2}]", st.ID, st.parentLink.distance_mm, st.distance_mm));

                // 이게 웃긴게... layout data의 노드들 xyz 좌표상에 거리와, 링크 길이와 정확히 맞지 않고 몇십 몇백 mm 차이가 있기 때문에
                // 있는 그대로 station을 표현하면 link 밖에 스테이션이 그려진다. 그래서 적절하게 링크 안으로 끌고 올수 있게 거리가 넘을 경우 예외처리 해줘야함
                double dTmpShowDist = MathSupport.gDistance(st.parentNode.X_mm, st.parentNode.Y_mm, st.nextNode.X_mm, st.nextNode.Y_mm);
                int nShowingDistance = st.distance_mm;
                if (st.distance_mm >= dTmpShowDist)
                    nShowingDistance = (int)dTmpShowDist - 50; // 좌표상에 위치보다 크면 -50해서 안쪽으로 끌고온다

                double tmpX = 0, tmpY = 0;
                bool retTmp = MathSupport.gPointInLineEx(st.parentNode.X_mm, st.parentNode.Y_mm, st.nextNode.X_mm, st.nextNode.Y_mm, nShowingDistance, ref tmpX, ref tmpY);

                if (retTmp == false)
                {
                    DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 을 생성할수 없습니다. 거리 계산에 실패했습니다. [px={1}, py={2}, nx={3}, ny={4}, dist={5}]", st.ID, st.parentNode.X_mm, st.parentNode.Y_mm, st.nextNode.X_mm, st.nextNode.Y_mm, nShowingDistance));
                    return false;
                }
                st.X_mm = (int)tmpX;
                st.Y_mm = (int)tmpY;
                
                st.Z_mm = st.parentNode.Z_mm;   //z축 값은 부모 노드 기준으로...

                // Station Type Set
                //strArr[1] = strArr[1].Replace(" ", "");
                //strArr[1] = strArr[1].ToUpper();
                if (strArr[1] == "DUAL_ACCESS") st.type = eStationType.DualAccess;
                else if (strArr[1] == "ACQUIRE") st.type = eStationType.Acquire;
                else if (strArr[1] == "DEPOSIT") st.type = eStationType.Deposit;
                else if (strArr[1] == "MAINTENANCE") st.type = eStationType.Maintenance;
                else if (strArr[1] == "DUMMY") st.type = eStationType.Dummy;
                else
                {
                    DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 를 생성할 수 없습니다. 알수없는 Type {1}", st.ID, strArr[1]));
                    return false;
                }

                if (strArr[6] == "EQ") st.portType = eStationPortType.EQ;
                else if (strArr[6] == "STOCKER") st.portType = eStationPortType.Stocker;
                else if (strArr[6] == "LSTB") st.portType = eStationPortType.LSTB;
                else if (strArr[6] == "RSTB") st.portType = eStationPortType.RSTB;
                else if (strArr[6] == "UTB") st.portType = eStationPortType.UTB;
                else if (strArr[6] == "LOADER") st.portType = eStationPortType.Loader;
                else if (strArr[6] == "CONVEYOR") st.portType = eStationPortType.Conveyor;
                else if (strArr[6] == "VEHICLE") st.portType = eStationPortType.Vehicle;
                else
                {
                    DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 를 생성할 수 없습니다. 알수없는 Port Type {1}", st.ID, strArr[6]));
                    return false;
                }

                // L/RSTB의 경우 좌우측에 있기 때문에 xy 위치를 손봐줘야 한다
                if(st.portType == eStationPortType.LSTB)
                {
                    double xRetTmp = 0, yRetTmp = 0;
                    bool btmpRet = MathSupport.gPointInLineEx(st.parentLink.PerallerLine_LeftSide_mm[0].x, st.parentLink.PerallerLine_LeftSide_mm[0].y,
                                                                                st.parentLink.PerallerLine_LeftSide_mm[1].x, st.parentLink.PerallerLine_LeftSide_mm[1].y,
                                                                                nShowingDistance, ref xRetTmp, ref yRetTmp);
                    if (btmpRet == false)
                    {
                        DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("failed", new StackFrame(true)));
                        return false;
                    }
                    st.X_STBmm = (int)xRetTmp;
                    st.Y_STBmm = (int)yRetTmp;
                }
                else if (st.portType == eStationPortType.RSTB)
                {
                    double xRetTmp = 0, yRetTmp = 0;
                    bool btmpRet = MathSupport.gPointInLineEx(st.parentLink.PerallerLine_RightSide_mm[0].x, st.parentLink.PerallerLine_RightSide_mm[0].y,
                                                                                st.parentLink.PerallerLine_RightSide_mm[1].x, st.parentLink.PerallerLine_RightSide_mm[1].y,
                                                                                nShowingDistance, ref xRetTmp, ref yRetTmp);
                    if (btmpRet == false)
                    {
                        DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("failed", new StackFrame(true)));
                        return false;
                    }
                    st.X_STBmm = (int)xRetTmp;
                    st.Y_STBmm = (int)yRetTmp;
                }

                if (strArr[7] == "FOUP")                 st.materialType = eMaterialType.FOUP;
                else if (strArr[7] == "RETICLE")        st.materialType = eMaterialType.RETICLE;
                else if (strArr[7] == "FOSB")            st.materialType = eMaterialType.FOSB;
                else if (strArr[7] == "MAC")            st.materialType = eMaterialType.MAC;
                else 
                {
                    DObjMan.errWarningMsg.Add(String.Format("Station ID {0} 를 생성할 수 없습니다. 알수없는 Material {1}", st.ID, strArr[7]));
                    return false;
                }

                //st.strSimpleInfo =	// 시간이 너무 오래걸리는 작업이라 삭제 -> 맵 로딩 속도 저하
                //    String.Format("ID: {0} - {1}\nType: {2}\nPortID: {3}\nParentNode: {4}\nNextNode: {5}\nOffset: {6}",
                //    st.ID, st.portType.ToString(), st.type.ToString(), st.portID, st.parentNode.ID, st.nextNode.ID, st.distance_mm
                //    );
            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(st);
            return true;

        }
       
        public static DObjStation FindStation(int id)
        {
            DObjArray arr = DObjMan.arrStation;

            int nLowIdx = 0;
            int nHighIdx = arr.Count;
            int nCurIdx = arr.Count / 2;
            int nPrevIdx = -1;
            int Cnt = 0;
            bool bFound = false;
            int nVal = 0;

            // 분할정복 알고리즘. Searching
            while (nLowIdx < nHighIdx )
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
            //Trace.WriteLine(String.Format("FindStation: {0}", Cnt));

            return (DObjStation)arr[nCurIdx];
            
            //foreach(DObjStation station in arr)
            //{
            //    if (station.ID == id)
            //        return station;
            //}
        }
    }
}
