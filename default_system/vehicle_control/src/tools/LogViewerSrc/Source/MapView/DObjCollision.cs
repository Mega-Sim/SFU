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
    public class DObjCollision : DObjBase
    {
      
        public DObjNode node;

        public DObjNode leftNode;
        public eUBGRegion leftUBG;
        public eObstacleRegion leftObstacle;

        public DObjNode rightNode;
        public eUBGRegion rightUBG;
        public eObstacleRegion rightObstacle;

        public static Pen Pen_Normal = new Pen(Color.LightYellow, 2);
        public static Pen Pen_Selected = new Pen(Color.YellowGreen, 3);
        
        
        public DObjCollision()
        {
            ObjType = eChildObjType.Collision;
            

        }
        //public void Draw(Graphics g, DScale scale, Pen customPen)
        //{
        //    this.customPen = customPen;
        //    Draw(g, scale);
        //    customPen = null;
        //}
        public override void Draw(Graphics g, DScale scale)
        {
            //if (!IsInVisibleArea(scale))
            //    return;

            int x_dc = scale.MM2DC_X(node.X_mm);
            int y_dc = scale.MM2DC_Y(node.Y_mm);
            Point pt1_dc = new Point(x_dc, y_dc);

            x_dc = scale.MM2DC_X(leftNode.X_mm);
            y_dc = scale.MM2DC_Y(leftNode.Y_mm);
            Point pt2_dc = new Point(x_dc, y_dc);

            x_dc = scale.MM2DC_X(node.X_mm);
            y_dc = scale.MM2DC_Y(node.Y_mm);
            Point pt3_dc = new Point(x_dc, y_dc);

            x_dc = scale.MM2DC_X(rightNode.X_mm);
            y_dc = scale.MM2DC_Y(rightNode.Y_mm);
            Point pt4_dc = new Point(x_dc, y_dc);
    
            g.DrawLine(Selected ? Pen_Selected : Pen_Normal, pt1_dc, pt2_dc);
            g.DrawLine(Selected ? Pen_Selected : Pen_Normal, pt3_dc, pt4_dc);

        }
        public override string GetID()
        {
            string theID = "";
            try
            {

                theID = String.Format("{0}_{1}, {2}_{3}", node.ID, (leftNode != null) ? Convert.ToString(leftNode.ID) : "null", node.ID, (rightNode != null) ? Convert.ToString(rightNode.ID) : "null");

            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return "null";
            }
            return theID;
        }
        public override int GetID_Int() { return node.ID; }
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
            rect.SumOfSet(node.Get2DPoint());
            if(leftNode != null)
                rect.SumOfSet(leftNode.Get2DPoint());
            if(rightNode != null)
                rect.SumOfSet(rightNode.Get2DPoint());
            rect.NormalizeRect();

            return rect;

        }
        public static bool ParseLayoutFileLine(string line, DObjArray objArray)
        {
            string[] strArr = line.Split('/');

            if (strArr.Length < 9)
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("파싱 실패: " + line, new StackFrame(true)));
                return false;
            }

            DObjCollision collision = new DObjCollision();
            try
            {
                int nNode = Convert.ToInt32(strArr[0]);
                int nLeftNode = Convert.ToInt32(strArr[1]);
                int nRightNode = Convert.ToInt32(strArr[5]);

                if (nNode <= 0)
                {
                    DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("노드 없음", new StackFrame(true)));
                    return false;
                }

                collision.node = DObjNode.FindNode(nNode);

                if (collision.node == null)
                {
                    DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("Collision에는 있는 노드가 실제로 없는 노드임. Node " + Convert.ToString(nNode), new StackFrame(true)));
                    return false;
                }
                
                if (nLeftNode > 0)
                {
                    collision.leftNode = DObjNode.FindNode(nLeftNode);
                    if (collision.leftNode == null)
                    {
                        DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("노드 없음", new StackFrame(true)));
                        return false;
                    }

                }
                if (nRightNode > 0)
                {
                    collision.rightNode = DObjNode.FindNode(nRightNode);
                    if (collision.rightNode == null)
                    {
                        DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2("노드 없음", new StackFrame(true)) );
                        return false;
                    }

                }
                collision.leftUBG = (eUBGRegion)Convert.ToInt32(strArr[2]);
                collision.leftObstacle = (eObstacleRegion)Convert.ToInt32(strArr[3]);
                collision.rightUBG = (eUBGRegion)Convert.ToInt32(strArr[6]);
                collision.rightObstacle = (eObstacleRegion)Convert.ToInt32(strArr[7]);
                
            }
            catch
            {
                DObjMan.errMsg.Add(MyTool.GMethod.MakeStackErrMsg2(" Exception - " + line, new StackFrame(true)) );
                return false;
            }

            objArray.Add(collision);
            return true;
        }
           

        
    }
}
