using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using MyTools;
using MathGeometry2D;

namespace LogViewer
{
	// 영역 감지 센서 Drawing 관리 클래스
	public class DSensorMan
	{
		public int nCurPatternNo = 1;
		public List<DObjAreaBase> arrObj_Pattern = new List<DObjAreaBase>();
		public List<DObjAreaBase> arrObj_LogData = new List<DObjAreaBase>();

		// 전체 객체 Drawing
		public void DrawALL(Graphics g, DScale scale)
		{
			// ara 패턴 먼저 그리고
			foreach (DObjAreaBase obj in arrObj_Pattern)
			{
				DObjAreaAraPattern pattern = (DObjAreaAraPattern)obj;
				if (pattern.nAreaNo != nCurPatternNo)
					continue;
				obj.Draw(g, scale);
			}

			// 로그 데이터 그린다
			foreach (DObjAreaBase obj in arrObj_LogData)
				obj.Draw(g, scale);
		}

		// ara file reading
		public bool ReadAraFile(string path)
		{
			List<DObjAreaAraPattern> aras = DObjAreaAraPattern.ReadAraFile(path);
			if (aras.Count == 0)
				return false;

			arrObj_Pattern.Clear();
			arrObj_Pattern.AddRange(aras);
			return true;
		}
		public DObjAreaAraPattern GetPattern(int nNo)
		{
			foreach (DObjAreaAraPattern obj in arrObj_Pattern)
			{
				if (obj.nAreaNo == nNo)
					return obj;
			}
			return null;
		}
	}
	
	// 영역 객체의 최상위 부모 추상 클래스
	public abstract class DObjAreaBase
	{
		public static Point curMousePos_mm = new Point();
		public abstract void Draw(Graphics g, DScale scale);
	}

	// 한개의 패턴 데이터 객체.  ara파일 한개에 여러 패턴이 들어있다.
	public class DObjAreaAraPattern : DObjAreaBase
	{
		public List<DObjAreaPoint> pointList = new List<DObjAreaPoint>();
		static SolidBrush areaBrush = new SolidBrush(Color.GhostWhite);
		static Pen pen_out3 = new Pen(Color.Red, 3);
		static Pen pen_out2 = new Pen(Color.Orange, 3);
		static SolidBrush brush_Out3 = new SolidBrush(pen_out3.Color);
		static SolidBrush brush_Out2 = new SolidBrush(pen_out2.Color);


		static Font font = new Font("맑은 고딕", 9F);

		public int nAreaNo = 0;
		public int nOutput2_mm = 0;
		public int nOutput3_mm = 0;

		public eShape nPartitionShape = eShape.Straight;
		public int n2ScanMode = 0;
		public int nMirrorReflectionMode = 0;
		public eHysteresis nHalfHysteresis = eHysteresis._6p25;

		public enum eShape{ Straight=0, FanStraight, Ratio,		COUNTOF};
		public enum eHysteresis { _6p25 = 0, _3p17, COUNTOF };

		public DObjAreaAraPattern()
		{
			pen_out3.DashStyle = DashStyle.DashDotDot;
			pen_out2.DashStyle = DashStyle.DashDotDot;
		}

		public override void Draw(Graphics g, DScale scale)
		{
			List<Point> arr = new List<Point>();

			foreach(var obj in pointList)
			{
				int nx = scale.MM2DC_X(obj.x_mm);
				int ny = scale.MM2DC_Y(obj.y_mm);

				arr.Add(new Point(nx, ny));
			}
			
			//g.FillPolygon(areaBrush, arr.ToArray());
			g.DrawPolygon(Pens.WhiteSmoke, arr.ToArray());
			g.DrawPolygon(Pens.White, arr.ToArray());
			
			foreach (var obj in pointList)
				obj.Draw(g, scale);     // 각각의 area point 들도 그려준다



			int nDelta = nOutput3_mm;
			Point pt1 = new Point(0, scale.MM2DC_Y(nOutput3_mm));
			Point pt2 = new Point(scale.nDrawHalfWidth_dc*2, scale.MM2DC_Y(nOutput3_mm));
			Point ptZero = new Point(0,0);
			ptZero.X = scale.MM2DC_X(0);
			Point ptStrShow = new Point(scale.MM2DC_X(0), scale.MM2DC_Y(nOutput3_mm));

			// output 3번 가이드 그려주기
			g.DrawLine(pen_out3, pt1, pt2);
			g.DrawString(String.Format("Output3: {0}m", (nOutput3_mm) / 1000.0), font, brush_Out3, ptStrShow);


			// output 2번 가이드 그려주기
			pt1 = new Point(0, scale.MM2DC_Y(nOutput2_mm));
			pt2 = new Point(scale.nDrawHalfWidth_dc * 2, scale.MM2DC_Y(nOutput2_mm));
			ptStrShow = new Point(scale.MM2DC_X(0), scale.MM2DC_Y(nOutput2_mm));
			g.DrawLine(pen_out2, pt1, pt2);
			g.DrawString(String.Format("Output2: {0}m", (nOutput2_mm) / 1000.0), font, brush_Out2, ptStrShow);
		}

		// Ara 파일 reading 하여 ara 패턴 리스트를 리턴
		public static List<DObjAreaAraPattern> ReadAraFile(string path)
		{
			List<DObjAreaAraPattern> retList = new List<DObjAreaAraPattern>();
			string[] allLines = File.ReadAllLines(path);
			if (allLines.Length < 3)
				return retList;
			
			DObjAreaAraPattern objCurPattern = null;

			for (int k=0; k<allLines.Length-1; k++)
			{
				string line = allLines[k];
				line = line.Replace("[", "");	// remove [, ]
				line = line.Replace("]", "");
				line = line.Replace(" ", "");

				double dVal = 0;
				bool bArea = GMethod.GetThisValue(line, "Area", ref dVal);
				bool bOutput = GMethod.GetThisValue(line, "OutPut", ref dVal);
				bool bPoint = GMethod.GetThisValue(line, "Point", ref dVal);
				int nVal = (int)dVal;

				bool bOpt_PShape = line == "PartitionShape";
				bool bOpt_2S = line == "2ScanMode";
				bool bOpt_Mir = line == "MirrorReflectionMode";
				bool bOpt_HHyst = line == "HalfHysteresis";
				
				// 새로운 Area 시작
				if(bArea)
				{
					objCurPattern = new DObjAreaAraPattern();
					objCurPattern.nAreaNo = nVal;
					retList.Add(objCurPattern);
					continue;
				}

				if (objCurPattern == null)
					continue;

				// 출력 2, 3번 위치 Y값 파싱
				if(bOutput)
				{
					if(nVal == 2 || nVal == 3)
					{
						int nOut = 0;
						if (Int32.TryParse(allLines[k + 1], out nOut))
						{
							if (nVal == 2)	objCurPattern.nOutput2_mm = nOut;
							else				objCurPattern.nOutput3_mm = nOut;
						}
					}
					continue;
				}

				// 포인트 좌표값 파싱
				if(bPoint)
				{
					string []xy = allLines[k + 1].Split(' ');
					if (xy.Length != 2)
						continue;

					int nx = 0, ny = 0;
					if (Int32.TryParse(xy[0], out nx) && Int32.TryParse(xy[1], out ny))
					{
						DObjAreaPoint pt = new DObjAreaPoint();
						pt.x_mm = nx;
						pt.y_mm = ny;
						objCurPattern.pointList.Add(pt);
					}
				}

				// 옵션들 파싱
				if (bOpt_PShape || bOpt_2S || bOpt_Mir || bOpt_HHyst)
				{
					int nOut = 0;
					if (Int32.TryParse(allLines[k + 1], out nOut))
					{
						if (bOpt_PShape) objCurPattern.nPartitionShape = (eShape)nOut;
						if (bOpt_2S) objCurPattern.n2ScanMode = nOut;
						if (bOpt_Mir) objCurPattern.nMirrorReflectionMode = nOut;
						if (bOpt_HHyst) objCurPattern.nHalfHysteresis = (eHysteresis)nOut;
					}
					continue;
				}
			}
			return retList;
		}
	}

	public class DObjAreaPoint : DObjAreaBase
	{
		public int x_mm = 0;
		public int y_mm = 0;
		static private SolidBrush brush = new SolidBrush(Color.DeepPink);

		public override void Draw(Graphics g, DScale scale)
		{

			int nx = scale.MM2DC_X(x_mm);
			int ny = scale.MM2DC_Y(y_mm);
			g.FillEllipse(brush, nx - 3, ny - 3, 6, 6);
		}
		
	}

	// 로그데이터에서 추출된 감지 영역 상태
	class DObjAreaLogData : DObjAreaBase
	{
		List<DObjAreaPoint> pointList = new List<DObjAreaPoint>();
		static SolidBrush brush = new SolidBrush(Color.LightGreen);

		public override void Draw(Graphics g, DScale scale)
		{
			List<Point> arr = new List<Point>();

			foreach (var obj in pointList)
			{
				int nx = scale.MM2DC_X(obj.x_mm);
				int ny = scale.MM2DC_Y(obj.y_mm);

				arr.Add(new Point(nx, ny));
			}

			g.FillPolygon(brush, arr.ToArray());
			g.DrawPolygon(Pens.White, arr.ToArray());
		}
	}
	
}
