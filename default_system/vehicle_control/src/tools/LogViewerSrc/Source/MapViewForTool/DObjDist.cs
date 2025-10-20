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
	// 거리 측정 클래스
	public class DObjDist : DObjBase
	{
		string ID = "";
		List<Point> pointList_mm = new List<Point>();      // 측정할 거리 Point List
		public static Pen Pen_Draw = new Pen(Color.LightCyan, 5);    // 그려진 후
		public static Pen Pen_OnDraw = new Pen(Color.LightGreen, 4);    // 그려지고 있을 때
		public static Pen Pen_Dot = new Pen(Color.LightGray, 3);    // 그려지고 있을 때
		bool bAddFinished = false;  // 현재 Add 하는 중인지. Add 중이면 마우스 커서 위치까지 계속 그려줘야한다
		public static SolidBrush Brush_Box = new SolidBrush(Color.Azure);
		public static Pen Pen_Box = new Pen(Color.SlateGray, 2);
		string strDistInfo = "";
		public static Font font = new Font("맑은 고딕", 10F, FontStyle.Regular);

		public DObjDist()
		{
			ObjType = eChildObjType.Distance;
			ID = String.Format("{0}", Environment.TickCount);       // DObjDist에서 ID는 중요하지 않다
		}

		public override void AddToRect(ref DRect_mm rect)
		{
			rect.SumOfSet(GetRect_mm());
		}
		// 위치 추가 시작
		public void AddStart()
		{
			pointList_mm.Clear();
			bAddFinished = false;
			strDistInfo = "";
		}
		// 위치 추가 끝
		public void AddFinish()
		{
			bAddFinished = true;
		}
		public int PointCount()
		{
			return pointList_mm.Count();
		}
		// 위치 추가
		public void AddPoint(Point pt_mm)
		{
			Debug.Assert(!bAddFinished);
			if (bAddFinished)
				return;

			pointList_mm.Add(pt_mm);

			double dDist_sum = 0;
			double dXOffset_mm = 0, dYOffset_mm = 0;

			for(int i=1; i< pointList_mm.Count; i++)
			{
				DPoint_mm pt1 = new DPoint_mm(pointList_mm[i - 1].X, pointList_mm[i - 1].Y);
				DPoint_mm pt2 = new DPoint_mm(pointList_mm[i].X, pointList_mm[i].Y);

				dXOffset_mm += Math.Abs(pt1.x - pt2.x);
				dYOffset_mm += Math.Abs(pt1.y - pt2.y);
				dDist_sum += MathGeometry2D.MathSupport.gDistLine(pt1, pt2);
			}

			// 10 미터 보다 크면 단위를 미터로, 그보다 작으면 mm 로 표현하자
			if(dDist_sum > 10000)
				strDistInfo = String.Format("Dist: {0:0.##0}m\nx: {1:0.##0}m\ny: {2:0.##0}m", dDist_sum / 1000, dXOffset_mm/1000, dYOffset_mm/1000);
			else												 
				strDistInfo = String.Format("Dist: {0}mm\nx: {1}mm\ny: {2}mm", (int)dDist_sum, (int)dXOffset_mm, (int)dYOffset_mm);
			
		}
		public void RemoveLastPoint()
		{
			Debug.Assert(!bAddFinished);
			if (bAddFinished)
				return;

			if (pointList_mm.Count == 0)
				return;

			pointList_mm.RemoveAt(pointList_mm.Count - 1);

			double dDist_sum = 0;
			double dXOffset_mm = 0, dYOffset_mm = 0;

			for (int i = 1; i < pointList_mm.Count; i++)
			{
				DPoint_mm pt1 = new DPoint_mm(pointList_mm[i - 1].X, pointList_mm[i - 1].Y);
				DPoint_mm pt2 = new DPoint_mm(pointList_mm[i].X, pointList_mm[i].Y);
				dXOffset_mm += Math.Abs(pt1.x - pt2.x);
				dYOffset_mm += Math.Abs(pt1.y - pt2.y);
				dDist_sum += MathGeometry2D.MathSupport.gDistLine(pt1, pt2);
			}

			// 10 미터 보다 크면 단위를 미터로, 그보다 작으면 mm 로 표현하자
			if (dDist_sum > 10000)
				strDistInfo = String.Format("Dist: {0:0.##0}m\nx: {1:0.##0}m\ny: {2:0.##0}m", dDist_sum / 1000, dXOffset_mm / 1000, dYOffset_mm / 1000);
			else
				strDistInfo = String.Format("Dist: {0}mm\nx: {1}mm\ny: {2}mm", (int)dDist_sum, (int)dXOffset_mm, (int)dYOffset_mm);
		}

		public override void Draw(Graphics g, DScale scale)
		{
			
			//if (!IsInVisibleArea(scale))
			//	return;

			if (pointList_mm.Count == 0)
			{
				//return;
			}

			// 현재 추가하는 중이면 마우스 커서 위치까지  포함시켜준다
			if (bAddFinished == false)
				AddPoint(DObjBase.curMousePos_mm);
			
			// mm -> dc
			List<Point> lines_dc = new List<Point>();
			foreach(Point pt_mm in pointList_mm)
			{
				Point pt_dc = new Point();
				pt_dc.X = scale.MM2DC_X(pt_mm.X);
				pt_dc.Y = scale.MM2DC_Y(pt_mm.Y);
				lines_dc.Add(pt_dc);
			}
			
			if (lines_dc.Count > 1)
				g.DrawLines(bAddFinished ? Pen_Draw : Pen_OnDraw, lines_dc.ToArray());  // 한번에 Point Path 빡. 그리기

			foreach(Point pt in lines_dc)
				g.DrawEllipse(Pen_Dot, pt.X-2, pt.Y-2, 4, 4);  // 이어진 점들 원으로 그리기

			//g.DrawString("S", font, Brushes.White, lines_dc[0]);

			if (lines_dc.Count > 1)
			{
				//if(bAddFinished)
				//	g.DrawString("E", font, Brushes.White, lines_dc[lines_dc.Count-1]);

				// 마지막 point 위치에 거리정보 표시
				Point lastPt_dc = lines_dc[lines_dc.Count - 1];
				Rectangle rect_dc = new Rectangle(lastPt_dc.X-97, lastPt_dc.Y-55, 90, 50);

				rect_dc.Inflate(2, 2);
				g.FillRectangle(Brush_Box, rect_dc);
				g.DrawRectangle(Pen_Box, rect_dc);
				g.DrawString(strDistInfo, font, Brushes.DarkSlateGray, rect_dc.Left, rect_dc.Top);  // 거리정보 표시
			}

			// 위에 현재위치 임시로 추가해준거 지우자
			if (bAddFinished == false)
				RemoveLastPoint();
		}

		public override string GetID()
		{
			return String.Format("{0}", ID); ;
		}

		public override int GetID_Int() { return Convert.ToInt32(ID); }

		DRect_mm tmpRect = new DRect_mm();
		public override DRect_mm GetRect_mm()
		{
			DRect_mm myRect = new DRect_mm();

			foreach (Point pt in pointList_mm)
				myRect.SumOfSet(pt);
			
			return myRect;
		}
	}
}
