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
    class DObjFlowControl : DObjBase
    {
        public DObjNode objStopNode = null;
        public DObjNode objResetNodeMain = null;
        public DObjLink flowLink = null;
        
        public int StopNodeID;
        public int ResetNodeMain;
        public int ResetNodeMainDefaultValue;
        public int ResetNodeMainDefaultRatio;
        public int ResetNodeSub;
        public int ResetNodeSubDefaultValue;
        public int ResetNodeSubDefaultRatio;
        public int ResetNodeRedundance1;
        public int BZStopRedundance1;
        public int ResetNodeRedundance1DefaultValue;
        public int ResetNodeRedundance1DefaultRatio;

        public static Pen Pen_Selected = new Pen(Color.YellowGreen, 4);   // 도형 선택 됐을 때 그려질 펜

        public DObjFlowControl()
        {
            Visible = false;
        }
        public override void AddToRect(ref DRect_mm rect)
        {
            if (flowLink == null)
                return;
            flowLink.AddToRect(ref rect);
        }

        public override void Draw(Graphics g, DScale scale)
        {
            if (flowLink == null || Visible == false)
                return;
            flowLink.Draw(g, scale, Pen_Selected);
        }

        public override string GetID()
        {
            return Convert.ToString(StopNodeID);
        }

        public override int GetID_Int()
        {
            return StopNodeID;
        }

        public override DRect_mm GetRect_mm()
        {
            if (flowLink == null)
                return new DRect_mm();
            return flowLink.GetRect_mm();
        }
        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 11)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }
            
            // [TEACHING] 
            // 263175 / 263176 / 0 / 100 / 0 / 0 / 0 / 0 / 0 / 0 / 0
            // StopNodeID / ResetNodeMain / ResetNodeMainDefaultValue / ResetNodeMainDefaultRatio / ResetNodeSub / ResetNodeSubDefaultValue /
            // ResetNodeSubDefaultRatio / ResetNodeRedundance1 / BZStopRedundance1 / ResetNodeRedundance1DefaultValue / ResetNodeRedundance1DefaultRatio

            DObjFlowControl flow = new DObjFlowControl();
            try
            {
                flow.StopNodeID = Convert.ToInt32(strArr[0]);
                flow.ResetNodeMain = Convert.ToInt32(strArr[1]);
                flow.ResetNodeMainDefaultValue = Convert.ToInt32(strArr[2]);
                flow.ResetNodeMainDefaultRatio = Convert.ToInt32(strArr[3]);
                flow.ResetNodeSub = Convert.ToInt32(strArr[4]);
                flow.ResetNodeSubDefaultValue = Convert.ToInt32(strArr[5]);
                flow.ResetNodeSubDefaultRatio = Convert.ToInt32(strArr[6]);
                flow.ResetNodeRedundance1 = Convert.ToInt32(strArr[7]);
                flow.BZStopRedundance1 = Convert.ToInt32(strArr[8]);
                flow.ResetNodeRedundance1DefaultValue = Convert.ToInt32(strArr[9]);
                flow.ResetNodeRedundance1DefaultRatio = Convert.ToInt32(strArr[10]);

                flow.objStopNode = DObjNode.FindNode(flow.StopNodeID);
                if (flow.objStopNode != null)
                    flow.objStopNode.bIsFlowControl = true;
                
                flow.objResetNodeMain = DObjNode.FindNode(flow.ResetNodeMain);
                if (flow.objResetNodeMain != null)
                    flow.objResetNodeMain.bIsFlowControl = true;

                if (flow.objStopNode == null)
                {
                    DObjMan.errMsg.Add("FlowControl의 StopNode " + Convert.ToString(flow.StopNodeID) + "는 존재하지 않는 노드입니다.");
                    return false;
                }
                if (flow.objResetNodeMain == null)
                {
                    DObjMan.errMsg.Add("FlowControl의 ResetNodeMain " + Convert.ToString(flow.StopNodeID) + "는 존재하지 않는 노드입니다.");
                    return false;
                }

                flow.flowLink = DObjLink.FindLink(flow.StopNodeID, flow.ResetNodeMain);
                if(flow.flowLink == null)
                {
                    DObjMan.errMsg.Add(String.Format("FlowControl의 {0} --> {1} Link가 존재하지 않는 link 입니다. ", flow.StopNodeID, flow.ResetNodeMain));
                    return false;
                }
            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)));
                return false;
            }

            objArray.Add(flow);
            return true;

        }
    }
}
