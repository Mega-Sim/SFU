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
using System.IO;

namespace ObjectDrawig
{
    // 어떤건 0, 어떤건 1부터 시작하는데 이건 내맘대로 한게 아니고 Station Editor 따라서 한것
    public enum CID_LayoutType    {        NormalCID = 1, SlopCID    }
    public enum CID_ResetNodeType    {        NormalNode = 1, CommonNode, SlopFront, SlopRear    }
    public enum CID_Direction    {        Left = 1, Right    }
    public enum eNodeType    {        Normal = 0, Slope, LiftPreLeft, LiftPreRight, LiftInLeft, LiftInRight, LiftOut, MTLSelectOnleft, MTLSelectOnRight, MTLStopLeft, MTLStopRight, AutoDoorLeft, AutoDoorRight, InitType    }
    public enum eSteering    {        NoDirection = 0, LeftSteering, RightSteering    }
    public enum eBranch    {        Normal_Diverge = 1, N_Diverge    }
    public enum eStationType { Acquire=0, Deposit, DualAccess, Maintenance, Dummy}
    public enum eStationPortType { EQ, Stocker, LSTB, RSTB, UTB, Loader, Conveyor, Vehicle}
    public enum eMaterialType { FOUP, RETICLE, FOSB, MAC}
    public enum eUBGRegion { N=1, L, R, S, B, V, SL, SR, CL, CR}
    public enum eObstacleRegion { N = 1, L, R, S, B, V, STB_L, STB_R, N_R, N_L, U_L, U_R, BAY}
    
    // DObjBase를 상속받은 객체들을 다 갖을수 있는 container
    public class DObjArray : List<DObjBase>
    {
        private Graphics dc = null;
        private DScale scale = null;

        public DObjArray() { }
        public DRect_mm GetTotalRect_mm(bool VisibleOnly, bool bSelectedOnly)
        {
            if (this.Count() < 1)
                return new DRect_mm();

            DRect_mm totalRect = new DRect_mm();
            foreach (DObjBase obj in this)
            {
                if (!obj.Visible && VisibleOnly)
                    continue;

                if (!obj.Selected && bSelectedOnly)
                    continue;

                obj.AddToRect(ref totalRect);
            }
            return totalRect;
        }   
        // 해당 xy 좌표를 품고있는 객체들 리스트로 리턴
        public DObjArray PtInRectArray(Point pt_mm, bool bVisibleOnly)
        {
            DObjArray list = new DObjArray();
            foreach(DObjBase obj in this)
            {
                if (bVisibleOnly && !obj.Visible)
                    continue;

                if (obj.GetRect_mm().PtInRect(pt_mm))
                    list.Add(obj);
            }
            return list;
        }
        public void SetGraphicsScale(Graphics g, DScale scale)
        {
            if (g != null)
                this.dc = g;
            if (scale != null)
                this.scale = scale;
        }

        // 영역 선택
        public void SelectArea(DRect_mm rtArea, bool bSelectAdd, bool bSelectMinus)
        {
            bool bInverse = false;
            if (bSelectAdd && bSelectMinus)
            {
                bInverse = true;
                bSelectAdd = bSelectMinus = false;
            }
            
            foreach (DObjBase obj in this)
            {
                // 노드와, 스테이션만 선택가능하도록
                bool bSelectAbleType = obj.ObjType == DObjBase.eChildObjType.Node || obj.ObjType == DObjBase.eChildObjType.Station;
                if (!obj.Visible) continue;
                
                if(obj.ObjType == DObjBase.eChildObjType.Node)
                {
                    DObjNode nd = (DObjNode)obj;
                    if (DObjBase.VisibleZRange.IsInRange(nd.Z_mm) == false)
                        continue;                    
                }
                else if (obj.ObjType == DObjBase.eChildObjType.Station)
                {
                    DObjStation nd = (DObjStation)obj;
                    if (DObjBase.VisibleZRange.IsInRange(nd.Z_mm) == false)
                        continue;
                }



                if (!bSelectAdd && !bSelectMinus)
                    obj.Selected = false;
                if (!bSelectAbleType)
                    continue;

                DRect_mm rt = obj.GetRect_mm();
                // 영역에 포함될때
                if (rtArea.PtInRect(rt.TopLeft()) && rtArea.PtInRect(rt.BottomRight()))
                {
                    if (obj.ObjType == DObjBase.eChildObjType.Station && !DObjStation.bDrawStation)
                        continue;
                    if (obj.ObjType == DObjBase.eChildObjType.Node && !DObjNode.bDrawNode)
                        continue;

                    if (bInverse)
                        obj.Selected = !obj.Selected;
                    if (bSelectMinus)
                        obj.Selected = false;
                    else
                        obj.Selected = true;
                }
            }
        }
        public void SelectAll(bool bSelect)
        {
            foreach (DObjBase obj in this)
                obj.Selected = bSelect;
        }
        public void VisibleAll(bool bVisible)
        {
            foreach (DObjBase obj in this)
                obj.Visible = bVisible;
        }
        // 어레이의 모든 도형들을 draw 한다
        public void Draw()
        {
            if (dc == null)
            {
                Debug.Assert(false, "DObjArray 클래스에 Graphics 객체를 할당하지 않았다!");
                return;
            }
            if (scale == null)
            {
                Debug.Assert(false, "DObjArray 클래스에 scale 객체를 할당하지 않았다!");
                return;
            }

            DObjBase obj = null;
            for (int i = 0; i < this.Count; i++)
            {
                try
                {
                    obj = this.ElementAt(i);
                    obj.Draw(this.dc, this.scale);
                }
                catch (Exception exp)
                {
                    Debug.Assert(false, String.Format("DObjArray의 {0} 번째 객체에서 drawing 중 에러 발생\n", i) + exp.Message);
                    continue;
                    //throw;
                }
            }
        }
        public void DrawFlag()
        {
            if (dc == null)
            {
                Debug.Assert(false, "DObjArray 클래스에 Graphics 객체를 할당하지 않았다!");
                return;
            }
            if (scale == null)
            {
                Debug.Assert(false, "DObjArray 클래스에 scale 객체를 할당하지 않았다!");
                return;
            }

            foreach(DObjBase obj in this)
            {
                if (!obj.bDrawFlag)
                    continue;

                obj.DrawFlag(this.dc, this.scale);
            }
           
        }
    }
    





}