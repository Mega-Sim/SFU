using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MathGeometry2D;
using System.Diagnostics;
using System.IO;
namespace LogViewer
{
	public partial class USensorArea : Form
	{
		DScale m_Scale = new DScale();
		SolidBrush brush_background = new SolidBrush(Color.FromArgb(0x2D, 0x2D, 0x2D));
		Rectangle client_rect = new Rectangle();
		Pen m_penGrid = new Pen(Color.DarkGray, 1);
		Font m_fontGrid = new Font("맑은 고딕", 9F);

		Pen m_penCurPos = new Pen(Color.Yellow, 1);

		DSensorMan m_manager = new DSensorMan();

		Graphics m_g = null;
		// 마우스 휠 확대 축소 할때 필요한 변수들
		Point ptWheelDownPos_dc = new Point();
		Point ptBackupCenter_mm = new Point();
		public Point curPos_mm = new Point();       // mouse 움직일 때 항상 마우스의 현재 좌표를 mm 단위로 갖음
		bool bLBtnDown = false;
		bool bOnSelecting = false;
		public bool m_bInBox = false;                  // 마우스가 움직이고 있는데, 현재 draw 박스 안에 있는지

		public USensorArea()
		{
			InitializeComponent();
			m_Scale.bYRevese = false;
			m_Scale.nDrawHalfWidth_dc = pictureBox_Draw.Width / 2;
			m_Scale.nDrawHalfHeight_dc = pictureBox_Draw.Height / 2;
			m_penGrid.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
			m_penCurPos.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
			this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseWheel);
		}

		bool bFirst = true;
		public void Redraw(bool byMessage = true, bool bFitWindow = false)
		{
			if (m_g == null)
				return;

			if (byMessage)
			{
				pictureBox_Draw.Invalidate();
				return;
			}

			pictureBox_Draw.SuspendLayout();

			if (bFitWindow && bFirst)
			{
				bFirst = false;
				FitWindow();
			}

			//Point ptZero = m_scale.DC2MM(new Point(pictureBox_Draw.Width / 2, pictureBox_Draw.Height));
			//m_scale.CenterPos_mm = ptZero;

			DrawGrid(m_g);

			m_manager.DrawALL(m_g, m_Scale);

			// 마우스 현재 위치 값 표시
			int nDelta = curPos_mm.X;
			double dDist = MathSupport.gDistance(0, 0, curPos_mm.X, curPos_mm.Y);
			Point curPos_dc = m_Scale.MM2DC(curPos_mm);
			curPos_dc.Offset(0, -12);
			m_g.DrawString(String.Format("{0:0.000}m, [{1:0.000}, {2:0.000}]", dDist / 1000, curPos_mm.X/1000.0, curPos_mm.Y/1000.0), m_fontGrid, Brushes.LightPink, curPos_dc);
			
			pictureBox_Draw.ResumeLayout();
		}
		private void FitWindow()
		{
			DRect_mm rtData_mm = new DRect_mm(-2500, 2500, 2500, -2500);
			Point ptCenter = rtData_mm.CenterPoint();
			m_Scale.SetCenterX(ptCenter.X);
			m_Scale.SetCenterY(-2500);

			double newScale = MathSupport.GetRectScale(rtData_mm, pictureBox_Draw.ClientRectangle);
			newScale = Math.Max(newScale, 1);
			newScale = Math.Min(newScale, 1000);
			m_Scale.SetScale(newScale);
		}

		private void DrawGrid(Graphics g)
		{
			client_rect = pictureBox_Draw.DisplayRectangle;

			g.FillRectangle(brush_background, client_rect);  //back ground

			int nX_mm = m_Scale.DC2MM_X(client_rect.Width);
			int nY_mm = m_Scale.DC2MM_Y(client_rect.Height);
			int nMax_mm = Math.Max(nX_mm, nY_mm);
			Point ptCenter_dc = new Point();
			for (int i = 1; i < 10; i++)
			{
				int nDelta = 1000 * i;

				Rectangle rt = m_Scale.MM2DC(new DRect_mm(-nDelta, nDelta, nDelta, -nDelta));
				if (i == 1)
					ptCenter_dc = new Point((rt.Left + rt.Right) / 2, (rt.Top + rt.Bottom) / 2);
				g.DrawEllipse(m_penGrid, rt);
				g.DrawString(String.Format("{0} m", (nDelta) / 1000.0), m_fontGrid, Brushes.White, new PointF(rt.Left, (rt.Bottom + rt.Top) / 2));
			}

			g.DrawLine(Pens.White, new Point(ptCenter_dc.X - 5, ptCenter_dc.Y), new Point(ptCenter_dc.X + 5, ptCenter_dc.Y));
			g.DrawLine(Pens.White, new Point(ptCenter_dc.X, ptCenter_dc.Y - 5), new Point(ptCenter_dc.X, ptCenter_dc.Y + 5));
		}

		static bool bOnce = true;
		private void pictureBox_Draw_Paint(object sender, PaintEventArgs e)
		{
			m_g = e.Graphics;
			Redraw(false, false);
			bOnce = false;
		}

		private void pictureBox_Draw_Resize(object sender, EventArgs e)
		{
			m_Scale.nDrawHalfWidth_dc = pictureBox_Draw.Width / 2;
			m_Scale.nDrawHalfHeight_dc = pictureBox_Draw.Height / 2;
			Redraw(true, false);
		}

		private void pictureBox_Draw_MouseMove(object sender, MouseEventArgs e)
		{
			// 휠버튼 이동으로 도면 이동 (마우스 없이 쓸수도 있어서 ALT 키 눌른것도 같은 효과)
			if (MouseButtons == MouseButtons.Middle || (MouseButtons == MouseButtons.Left && Control.ModifierKeys == Keys.Alt))
			{
				Point ptOffset_dc = new Point(ptWheelDownPos_dc.X - e.X, ptWheelDownPos_dc.Y - e.Y);
				Point ptOffset_mm = m_Scale.DC2MM(ptOffset_dc);
				Point ptZero = m_Scale.DC2MM(new Point(0, 0));
				ptOffset_mm = new Point(ptOffset_mm.X - ptZero.X, -ptOffset_mm.Y + ptZero.Y);
				m_Scale.CenterPos_mm.X = ptBackupCenter_mm.X - ptOffset_mm.X;
				m_Scale.CenterPos_mm.Y = ptBackupCenter_mm.Y - ptOffset_mm.Y;
				//Redraw(false);
				//return;
			}
			
			curPos_mm.X = m_Scale.DC2MM_X(e.X);
			curPos_mm.Y = m_Scale.DC2MM_Y(e.Y);
			DObjAreaBase.curMousePos_mm = curPos_mm;
			
			Redraw();
		}
		private void pictureBox_Draw_MouseUp(object sender, MouseEventArgs e)
		{
			if (Control.ModifierKeys == Keys.Alt)
			{
				bLBtnDown = false;
				bOnSelecting = false;
			}
		}
		private void pictureBox_Draw_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left && Control.ModifierKeys == Keys.Alt)   // alt + L버튼 == 휠버튼과 같은 효과
			{
				ptWheelDownPos_dc.X = e.X;
				ptWheelDownPos_dc.Y = e.Y;

				ptBackupCenter_mm = m_Scale.CenterPos_mm;
			}
			// 휠버튼 down시 위치 기억
			if (e.Button == MouseButtons.Middle)    // 휠 이동
			{
				ptWheelDownPos_dc.X = e.X;
				ptWheelDownPos_dc.Y = e.Y;

				ptBackupCenter_mm = m_Scale.CenterPos_mm;
			}
		}
		private void panel1_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//pictureBox_Draw 컨트롤의 글로번한 위치를 알아내기 위함
			Point locationOnForm = pictureBox_Draw.FindForm().PointToClient(pictureBox_Draw.Parent.PointToScreen(pictureBox_Draw.Location));

			int nX_dc = e.X - locationOnForm.X;
			int nY_dc = e.Y - locationOnForm.Y;

			ZoomIn(e.Delta > 0, nX_dc, nY_dc);
		}
		// 스크롤바로 줌 조정할때
		public void ZoomIn(bool bIn, int x_dc, int y_dc/*int x_dc = INT_MIN, int y_dc = INT_MIN*/) // INT_MIN값이면 화면 중앙을 중심으로 확대한다.
		{
			if (!m_bInBox) // 마우스가 도면에 있을때만 처리위함
				return;
			int x1_mm = m_Scale.DC2MM_X(x_dc);
			int y1_mm = m_Scale.DC2MM_Y(y_dc);
			if (bIn)
			{
				if (m_Scale.dScale < 0.5)
					return;

				m_Scale.MultiplyScale(0.9);
			}
			else
			{
				if (m_Scale.dScale > 1000)
					return;

				m_Scale.MultiplyScale(1.1);
			}
			// 좌표를 입력하면 해당 좌표를 중심으로 확대한다.
			//if (x_dc != DBL_MIN && y_dc != DBL_MIN)
			{
				int x2_mm = m_Scale.DC2MM_X(x_dc);
				int y2_mm = m_Scale.DC2MM_Y(y_dc);
				m_Scale.SetCenterX(m_Scale.CenterPos_mm.X + (x2_mm - x1_mm));
				m_Scale.SetCenterY(m_Scale.CenterPos_mm.Y - (y2_mm - y1_mm));
			}
			Redraw(true, false);
		}
		public void ZoomPercent(double dPercent, int x_dc, int y_dc/* int x_dc = INT_MIN, int y_dc = INT_MIN*/) // 전체 오브젝트 기준의 스케일 값에서 퍼센트 곱해서 확대/축소한다.
		{
			m_Scale.dScale = 100.0 / dPercent;
			Redraw(false);
		}
		double GetZoomPercent()
		{
			return 100.0 / m_Scale.dScale;
		}

		private void timer1_Tick(object sender, EventArgs e)
		{
			Rectangle rt = splitContainer2.Panel2.RectangleToScreen(pictureBox_Draw.DisplayRectangle);
			Point ptGlobal = this.PointToScreen(MousePosition);
			m_bInBox = rt.Contains(MousePosition.X, MousePosition.Y);
		}

		private void btn_FitWnd_Click(object sender, EventArgs e)
		{
			Redraw(true, true);
		}

		private void USensorArea_FormClosing(object sender, FormClosingEventArgs e)
		{
			e.Cancel = true;
		}

		string lastOpenPath = "";
		private void btn_OpenAraFile_Click(object sender, EventArgs e)
		{
			OpenFileDialog dlg = new OpenFileDialog();
			if (String.IsNullOrEmpty(lastOpenPath) == false)
				dlg.InitialDirectory = lastOpenPath;

			dlg.Filter = "Hokuyo Area file (*.ara)|*.ara|All files (*.*)|*.*";
			if (dlg.ShowDialog() != DialogResult.OK)
				return;

			lastOpenPath = Path.GetDirectoryName(dlg.FileName);

			comboBox_Pattern.Items.Clear();
			if (m_manager.ReadAraFile(dlg.FileName) == false)
			{
				MessageBox.Show("파일 열기 실패");
				return;
			}
			foreach(DObjAreaAraPattern ara in m_manager.arrObj_Pattern)
			{
				comboBox_Pattern.Items.Add(Convert.ToString(ara.nAreaNo));
			}
		}

		private void comboBox_Pattern_SelectedIndexChanged(object sender, EventArgs e)
		{
			int nPatternNo = 0;
			if(Int32.TryParse(comboBox_Pattern.Text, out nPatternNo))
				PatternChange(nPatternNo);
		}

		enum eSunSeo { AreaNo=0, Shape, Out1_Point1, Out1_Point2, Out1_Point3, Out1_Point4, Out1_Point5, Out1_Point6, Out1_Point7, Out2, Out3, Hystersis }
		private void PatternChange(int nNo)
		{
			m_manager.nCurPatternNo = nNo;
			DObjAreaAraPattern obj = m_manager.GetPattern(nNo);
			if (obj == null)
				return;

			// 리스트 뷰의 Area 파일 정보 표시
			listView_PatternInfo.Items.Clear();

			string[] arr = new string[3];
			arr[0] = eSunSeo.AreaNo.ToString();
			arr[1] = String.Format("{0}", obj.nAreaNo);
			arr[2] = "";
			ListViewItem lvi = new ListViewItem(arr);
			listView_PatternInfo.Items.Add(lvi);

			arr = new string[3];
			arr[0] = eSunSeo.Shape.ToString();
			arr[1] = obj.nPartitionShape.ToString();
			arr[2] = "";
			lvi = new ListViewItem(arr);
			listView_PatternInfo.Items.Add(lvi);
			
			for (int k = 0; k < 7; k++)
			{
				int nx = 0, ny = 0;

				if(k < obj.pointList.Count)
				{
					nx = obj.pointList[k].x_mm;
					ny = obj.pointList[k].y_mm;
				}

				arr = new string[3];
				eSunSeo ii = (eSunSeo)(int)eSunSeo.Out1_Point1 + k;
				arr[0] = ii.ToString();
				arr[1] = String.Format("{0}", nx);
				arr[2] = String.Format("{0}", ny);
				lvi = new ListViewItem(arr);
				listView_PatternInfo.Items.Add(lvi);
			}

			arr = new string[3];
			arr[0] = eSunSeo.Out2.ToString();
			arr[1] = String.Format("{0}", obj.nOutput2_mm);
			arr[2] = "";
			lvi = new ListViewItem(arr);
			listView_PatternInfo.Items.Add(lvi);

			arr = new string[3];
			arr[0] = eSunSeo.Out3.ToString();
			arr[1] = String.Format("{0}", obj.nOutput3_mm);
			arr[2] = "";
			lvi = new ListViewItem(arr);
			listView_PatternInfo.Items.Add(lvi);

			string hy = obj.nHalfHysteresis.ToString().Replace("_", "");
			hy = hy.Replace("p", "");

			arr = new string[3];
			arr[0] = eSunSeo.Hystersis.ToString();
			arr[1] = hy;
			arr[2] = "";
			lvi = new ListViewItem(arr);
			listView_PatternInfo.Items.Add(lvi);
			
			Redraw();
		}
	}
}
