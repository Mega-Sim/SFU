using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using Microsoft.VisualBasic;
using System.Collections;
using System.Diagnostics;
using System.Drawing.Imaging;
using System.Xml;
using LogViewer;

namespace TimeLine
{
    // ****************************************************************************************************************************
    // Time Line Control *********************************************************************************************************
    // SHCHO Created
    // 2018. 06. 23
    // for viewing In/Out signal with horizontal time zone
    // 타임란인
    // 주말마다 취미생활로 만듬 ...
    // 간트챠트 형식으로 로그 데이터를 그래프로 표현하기 위함
    // 너무 급하고 빡시게 만들어서 주석이 많이 없음.
    // 천천히 코드 보면 다 이해될만 함

    public class DTimeLine
    {
		//public List<HorizonHeader> headerList = new List<HorizonHeader>();  // 상단 헤더 리스트
		//public List<TimeLineItem> itemList = new List<TimeLineItem>();  //좌측 타임라인 아이템들 리스트
		//public List<Bar> barList = new List<Bar>();     //실제로 표시될 값을 나타내는 막대기들. 한개의 TimeLineItem은 여러개의 Bar를 가질 수 있다
		//public List<DblChart> chartList = new List<DblChart>(); // 실수형 차트 리스트
		//public List<TimeZoneArea> timeZoneList = new List<TimeZoneArea>();  // 세로 시간 존 영역

		Pen pen_forNet = new Pen(Color.Black, 1);
		public DDrawAttribute m_DrawAtt = new DDrawAttribute();

		public List<DTimeItemBase> m_ObjectList = new List<DTimeItemBase>();

		public DAmcRcv m_AmcRecv = null;

        //public string Title = "";
        //private string strStatus_CurentTimeZone = "";
        //private string strStatus_SelectedTimeSpan = "";

        //int barStartLeft_x = 140;  // bar 를 그리기 시작하는 좌측 시작위치 x
        //int headerTimeStartTop_y = 60;        // 상단 헤더가 시작되는 위치 y
        //int ItemStart_x = 30;            // 좌측 아이템 이름 시작하는 위치 x
        //int barStartTop_y = 30;           //  막대기를 그리기 시작하는 위치 y
        //int barHeight = 20;     // bar의 height
        //int itemHeightSpace = 10;      // item들 간의 세로 간격
        //int widthPerItem;       // 각 time zone의 width. redraw를 할때마다 갱신된다 전체 크기 대비로 갱신됨
        //int nIdxHoveredTimeZone = -1;    // 현재 마우스 호버링 된 타임존
        //int legendXPos = 10;    // 좌측 래전드(범례 색) 색깔 표시 박스 x 위치
        //Point titlePos = new Point(5, 10);
        //Point statusPos = new Point(140, 30);
        //Graphics m_gfx = null;
        
        public DTimeLine()
        {
			pen_forNet.DashStyle = DashStyle.Dot;
			//SetStyle(ControlStyles.DoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
		}
        

        //public void AddHeader(int nIdx, DateTime tm)
        //{
        //    HorizonHeader h = new HorizonHeader();
        //    h.HeaderText = String.Format("{0}",nIdx+1);
        //    h.HeaderIdx = nIdx;
        //    h.theTime = tm;
        //    headerList.Add(h);
        //}

        //private Rectangle GetBarRect(int nItemIdx, int nStartIdx, int nEndIdx)
        //{
        //    int availableWidth = Width - 10 - barStartLeft_x;
        //    widthPerItem = availableWidth / headerList.Count;
        //    int nCnt = nEndIdx - nStartIdx +1;
        //    if (nCnt < 1)
        //    {
        //        Debug.Assert(false);
        //        return new Rectangle();
        //    }

        //    int nLeft = barStartLeft_x + (nStartIdx * widthPerItem);
        //    int nTop = GetItemYPos(nItemIdx);
        //    int nWidth = nCnt * widthPerItem;
        //    int nHeight = barHeight;
            
        //    Rectangle rect = new Rectangle(nLeft, nTop, nWidth, nHeight);
        //    return rect;
        //}

        //private bool bMade = false;
        //public bool MakeTimeLine()
        //{
        //    if (bMade)
        //        return false;
        //    bMade = true;

        //    if (itemList.Count() == 0 || headerList.Count <= 1)
        //        return false;

        //    int nItemIdx = 0;

        //    // 막대기들 만들기.  Bool 값 막대기
        //    foreach (TimeLineItem item in itemList)
        //    {
        //        if (item.bBoolTypeItem == false)
        //            continue;

        //        // 해당 item 내에 여러개의 bar가 나올수 있는데
        //        // 각 bar들 rect까지 미리 계산해서 barList 에 넣어둔다

        //        int nStartIdx = -1;
        //        int nEndIdx = -1;

        //        for (int i = 0; i < item.BVals.Count(); i++)
        //        {
        //            if (nStartIdx == -1 && item.BVals[i] == true)
        //            {
        //                nStartIdx = i;
        //                nEndIdx = i;
        //                continue;
        //            }
        //            if (item.BVals[i] == true)
        //                nEndIdx = i;

        //            if (nStartIdx != -1 && item.BVals[i] == false)
        //            {
        //                Bar bar = new Bar();
        //                bar.parentItem = item;
        //                bar.nStartIdx = nStartIdx;
        //                bar.nEndIdx = nEndIdx;
        //                bar.startTime = headerList.ElementAt(nStartIdx).theTime;
        //                bar.endTime = headerList.ElementAt(nEndIdx).theTime;
        //                bar.rect = GetBarRect(nItemIdx, nStartIdx, nEndIdx); // rect calc
        //                barList.Add(bar);   // Add

        //                nStartIdx = -1;
        //                nEndIdx = -1;
        //                continue;
        //            }
        //        }

        //        // 끝 값 처리
        //        if (nEndIdx != -1)
        //        {
        //            Bar bar = new Bar();
        //            bar.parentItem = item;
        //            bar.nStartIdx = nStartIdx;
        //            bar.nEndIdx = nEndIdx;
        //            bar.startTime = headerList.ElementAt(nStartIdx).theTime;
        //            bar.endTime = headerList.ElementAt(nEndIdx).theTime;
        //            bar.rect = GetBarRect(nItemIdx, nStartIdx, nEndIdx); // rect calc

        //            barList.Add(bar);   // Add
        //        }
        //        nItemIdx++;
        //    }

        //    // 실수형 그래프 만들기
        //    foreach (TimeLineItem item in itemList)
        //    {
        //        if (item.bBoolTypeItem)
        //            continue;

        //        DblChart chart = new DblChart();
        //        chart.parentItem = item;
        //        chart.dataList = item.DVals;
        //        chart.pen = new Pen(item.Color, 2);
        //        chart.dDataMin = item.dValMin;
        //        chart.dDataMax = item.dValMax;
        //        chart.dVisibleMin = item.dValMin;
        //        chart.dVisibleMax = item.dValMax;

        //        chartList.Add(chart);   //add
        //        nItemIdx++;
        //    }

        //    // 타임존 만들기

        //    int nEndPos_y = itemList.Last().pos_y + barHeight + 10;
        //    int nLastPos_x = 0;

        //    for (int i = 0; i < headerList.Count(); i++)
        //    {
        //        HorizonHeader header = headerList.ElementAt(i);

        //        // 다임존 영역을 Rect로 미리 만들어놓면 나중에 마우스 좌표 처리하기도 매우 편리함
        //        if (i > 0)
        //        {
        //            TimeZoneArea area = new TimeZoneArea();
        //            area.rect = new Rectangle(nLastPos_x, headerTimeStartTop_y, widthPerItem, nEndPos_y - headerTimeStartTop_y);
        //            area.parent = headerList.ElementAt(i - 1);
        //            timeZoneList.Add(area);
        //        }

        //        nLastPos_x = header.pos_x;
        //    }
            
        //    nLastPos_x += widthPerItem;

        //    // 마지막 타임존 만들기
        //    TimeZoneArea lastArea = new TimeZoneArea();
        //    lastArea.rect = new Rectangle(nLastPos_x - widthPerItem, headerTimeStartTop_y, widthPerItem, nEndPos_y - headerTimeStartTop_y);
        //    lastArea.parent = headerList.Last();
        //    timeZoneList.Add(lastArea);
            
        //    return true;
        //}

        //// itemList 를 기준으로 bar의 영역과 정보들을 갱신한다
        //private void RefreshBarRect()
        //{
        //    foreach(Bar bar in barList)
        //    {
        //        bar.rect = GetBarRect(bar.parentItem.nIdx, bar.nStartIdx, bar.nEndIdx);
        //    }
        //}

        //// item 을 list에 추가한다
        //public void AddTimeLineItem(TimeLineItem item)
        //{
        //    item.nIdx = itemList.Count();
        //    itemList.Add(item);
        //}
        
        //// 챠트 영역의 rect 반환
        //public Rectangle GetControlRect()
        //{
        //    if(timeZoneList.Count < 1)
        //        return new Rectangle();

        //    TimeZoneArea lastZone = timeZoneList.Last();
                

        //    int nWidth = lastZone.rect.X +widthPerItem + 5;
        //    Rectangle rect = new Rectangle(0, 0, nWidth + ItemStart_x, lastZone.rect.Height+ headerTimeStartTop_y+30);
        //    return rect;
        //}

        //protected override void OnPaint(PaintEventArgs e)
        //{
        //    //base.OnPaint(e);
        //    m_gfx = e.Graphics;
        //    Redraw(m_gfx);
        //}

        //protected override void OnResize(EventArgs e)
        //{
        //    base.OnResize(e);
        //    Invalidate();
        //}

        public void Redraw(Graphics gfx)
        {
            if (gfx == null)
                return;

            try
            {
				//if (!bMade) return;

				//int availableWidth = Width - 10 - barStartLeft_x;
				//widthPerItem = availableWidth / headerList.Count;
				//RefreshBarRect();  // itemList 를 기준으로 bar의 영역과 정보들을 갱신한다

				// 영역 확인 용도
				//gfx.FillRectangle(Brushes.DarkOrange, GetControlRect());
				//foreach(var timezone in timeZoneList)
				//{
				//    gfx.FillRectangle(Brushes.Yellow, timezone.rect);
				//}

				// draw background
				//gfx.FillRectangle(Brushes.White, GetControlRect());

				//// draw title
				//gfx.DrawString("  <Time Line>  " + Title, new Font("맑은 고딕", 12F, FontStyle.Bold), Brushes.Black, titlePos.X, titlePos.Y);

				//DrawSelectedTimeZone(gfx);
				
				DrawBackground(gfx, ref m_DrawAtt);
				m_DrawAtt.lastDrawPosY = -1;
				foreach (DTimeItemBase obj in m_ObjectList)
				{
					obj.Draw(gfx, ref m_DrawAtt);
				}

                //DrawBars(gfx);
                //DrawChart(gfx);
               // DrawStatus(gfx);

                // 마우스 끌어서 지정중인 영역 drawing
              //  if(bOnMouseDown)
              //      gfx.DrawRectangle(new Pen(Color.Red), rtOnSelectingBox);
                
            }
            catch (Exception)
            {
                //throw;
            }
        }
		
		void DrawBackground(Graphics g, ref DDrawAttribute att)
		{

			g.Clear(Color.White);
			
			//g.FillRectangle(Brushes.Black, new Rectangle(0, 0, 2000, 2000));  //back ground


			g.FillRectangle(Brushes.White, att.designRect);
			g.DrawString(att.title, att.Font1, Brushes.Black, new Point(att.MainTitleTopMargin, att.MainTitleTopMargin));

			Point DataStartPos = att.GetDataStartPos();
			int nDataAreaWidth = att.designRect.Width - DataStartPos.X;
			att.cellWidth = nDataAreaWidth / att.dataCount;


			//if (att.cellWidth > 5)
			// Draw Net
			try
			{
				for (int i = 0; i < att.dataCount; i++)
				{

					Point pt1 = new Point(DataStartPos.X + (i * att.cellWidth), DataStartPos.Y);
					Point pt2 = new Point(DataStartPos.X + (i * att.cellWidth), att.designRect.Height - DataStartPos.Y);
					g.DrawLine(pen_forNet, pt1, pt2);
				}
			}
			catch { }


		}
		//private double GetTimeSpan_sec(int nStartIdx, int nEndIdx)
		//{

		//    TimeSpan span = headerList.ElementAt(nEndIdx).theTime - headerList.ElementAt(nStartIdx).theTime;
		//    double dSec = span.TotalSeconds;

		//    double dLastAddSec = 0; // 그냥 빼기 하면 마지막 값은 지속이 된것으로 안쳐지는 셈이기 때문에 마지막 한칸에 대한 걸 처리해 줘야함

		//    if (nEndIdx == headerList.Count - 1)
		//    {
		//        span = headerList.ElementAt(nEndIdx).theTime - headerList.ElementAt(nEndIdx - 1).theTime;
		//        dLastAddSec = span.TotalSeconds;

		//    }
		//    else
		//    {
		//        span = headerList.ElementAt(nEndIdx + 1).theTime - headerList.ElementAt(nEndIdx).theTime;
		//        dLastAddSec = span.TotalSeconds;
		//    }


		//    return dSec + dLastAddSec;
		//}

		//// bar는 height가 좁고 chart는 height 가 크기 떄문에 약간 복잡해짐
		//private int GetItemYPos(int idx)
		//{
		//    int nChartStart = 0;

		//    for(int i=0; i<itemList.Count; i++)
		//    {
		//        if(itemList.ElementAt(i).bBoolTypeItem == false)
		//        {
		//            nChartStart = i;
		//            break;
		//        }
		//    }

		//    int nRet = 0;

		//    // bar 일경우
		//    if (idx < nChartStart)
		//        nRet = headerTimeStartTop_y + barStartTop_y + (barHeight * idx) + (itemHeightSpace * idx) + 2;
		//    // 차트일 경우
		//    else
		//    {
		//        int nChartCnt = idx - nChartStart;
		//        int nBarCnt = nChartStart;
		//        nRet = headerTimeStartTop_y + barStartTop_y + (barHeight * nBarCnt) + (nChartCnt * DblChart.chartHeight) + (itemHeightSpace * idx) + 2;
		//    }

		//    return nRet;
		//}

		//// 마우스로 선택된 타임존만 보기좋게 선택한다
		//private void DrawSelectedTimeZone(Graphics gfx)
		//{
		//    foreach(TimeZoneArea zone in timeZoneList)
		//    {
		//        try
		//        {
		//            if (zone.selected)
		//            {
		//                gfx.FillRectangle(Brushes.Green, zone.rect);
		//            }
		//        }
		//        catch { continue; }
		//    }

		//    // 호버링된 타임존 그리기
		//    if(nIdxHoveredTimeZone >= 0)
		//    {
		//        Rectangle r = timeZoneList.ElementAt(nIdxHoveredTimeZone).rect;
		//        gfx.FillRectangle(Brushes.LightGreen, r);
		//    }
		//}
		//// 좌측해더와 상단헤더 두개 모두 그리기
		//private void DrawHeader(Graphics gfx)
		//{

		//    if (headerList.Count == 0)
		//        return;
		//    try
		//    {

		//        int nCnt = 0;
		//        int nMargin = widthPerItem / 4;
		//        if (nMargin < 5)
		//            nMargin = 1;

		//        DateTime start = headerList.First().theTime;
		//        gfx.DrawString(String.Format("{0}:{1}:{2}.{3}", start.Hour, start.Minute, start.Second, start.Millisecond), new Font("맑은 고딕", 10F, FontStyle.Bold), Brushes.Black, ItemStart_x, headerTimeStartTop_y - 15);

		//        // 상단헤더 그리기 ********
		//        foreach (HorizonHeader header in headerList)
		//        {
		//            int pos_x = barStartLeft_x + (nCnt * widthPerItem);

		//            header.pos_x = pos_x;
		//            if(widthPerItem > 22)
		//            {
		//                TimeSpan span = header.theTime - start;
		//                gfx.DrawString(String.Format("{0:0.0}", span.TotalSeconds), new Font("맑은 고딕", 8F), Brushes.Black, pos_x, headerTimeStartTop_y - 15);
		//            }
		//            if(widthPerItem > 15)
		//                gfx.DrawString(header.HeaderText, new Font("맑은 고딕", 9F), Brushes.Black, pos_x + nMargin, headerTimeStartTop_y);
		//            nCnt++;
		//        }

		//        // 좌측 헤더 그리기 ***********

		//        for (int i = 0; i < itemList.Count(); i++)
		//        {
		//            TimeLineItem item = itemList.ElementAt(i);
		//            Font font = new Font("맑은 고딕", 9F, FontStyle.Bold);
		//            int nYPos = GetItemYPos(i);  //barStartTop_y + (barHeight * nCnt++) + barHeight / 2;
		//            item.pos_y = nYPos;
		//            item.pos_yEnd = GetItemYPos(i+1);

		//            if (item.bBoolTypeItem == false)
		//                nYPos += DblChart.chartHeight / 2;

		//            Rectangle legendRect = new Rectangle(legendXPos, nYPos, 10, 10);
		//            gfx.FillRectangle(new SolidBrush(item.Color), legendRect);
		//            gfx.DrawRectangle(new Pen(Color.Black), legendRect);
		//            gfx.DrawString(item.Name, font, Brushes.Black, ItemStart_x, nYPos - barHeight / 4);
		//        }
		//    }
		//    catch{}
		//}

		//private void DrawChart(Graphics gfx)
		//{
		//    int availableWidth = Width - 10 - barStartLeft_x;
		//    widthPerItem = availableWidth / headerList.Count;

		//    Pen penArea = new Pen(Color.Black, 1);
		//    penArea.DashStyle = DashStyle.Dot;

		//    foreach (DblChart chart in chartList)
		//    {
		//        try
		//        {
		//            int nYStart = GetItemYPos(chart.parentItem.nIdx);
		//            Point ptStartPos = new Point(barStartLeft_x, nYStart);
		//            List<Point> ptList = chart.Get_asPixelValList(widthPerItem, ptStartPos);
		//            gfx.DrawLines(chart.pen, ptList.ToArray());

		//            int xRight = headerList.Last().pos_x;
		//            chart.rectArea = new Rectangle(legendXPos, nYStart - itemHeightSpace/2, xRight, DblChart.chartHeight + itemHeightSpace); // 그래프 가로 점선 영역표시
		//            Point ptLeftTop = new Point(chart.rectArea.Left, chart.rectArea.Top);
		//            Point ptRightTop = new Point(chart.rectArea.Right, chart.rectArea.Top);
		//            Point ptLeftBtm = new Point(chart.rectArea.Left, chart.rectArea.Bottom);
		//            Point ptRightBtm = new Point(chart.rectArea.Right, chart.rectArea.Bottom);

		//            // 차트 영역 점선 그리기
		//            gfx.DrawLine(penArea, ptLeftTop, ptRightTop);
		//            gfx.DrawLine(penArea, ptLeftBtm, ptRightBtm);

		//            string str = String.Format("Min: {0}\r\nMax: {1}", chart.dDataMin, chart.dDataMax);
		//            try
		//            {
		//                if (nIdxHoveredTimeZone >= 0)
		//                {
		//                    double dVal = chart.dataList.ElementAt(nIdxHoveredTimeZone);
		//                    str += String.Format("\r\nCur: {0}", dVal);
		//                    gfx.DrawString(str, new Font("맑은 고딕", 9), Brushes.Black, ItemStart_x, ptStartPos.Y);
		//                }
		//            }
		//            catch { }

		//            gfx.DrawString(str, new Font("맑은 고딕", 9), Brushes.Black, ItemStart_x, ptStartPos.Y);



		//        }
		//        catch { continue; }
		//    }
		//}
		// Bar를 모두 그린다
		//private void DrawBars(Graphics gfx)
		//{
		//    RefreshBarRect();

		//    foreach (Bar bar in barList)
		//    {
		//        try
		//        {
		//            // Draws the bar
		//            Brush brush = null;
		//            Pen pen = null;
		//            if (bar.selected)
		//            {
		//                brush = new SolidBrush(Color.YellowGreen);
		//                pen = Pens.SlateGray;
		//            }
		//            else
		//            {
		//                brush = new LinearGradientBrush(bar.rect, bar.parentItem.Color, Color.White, LinearGradientMode.Vertical);
		//                pen = Pens.Black;
		//            }

		//            gfx.FillRectangle(brush, bar.rect);
		//            gfx.DrawRectangle(pen, bar.rect);

		//            if (widthPerItem > 10)
		//            {
		//                double dSpan = GetTimeSpan_sec(bar.nStartIdx, bar.nEndIdx);
		//                string str = String.Format("{0:0.000}", dSpan);
		//                gfx.DrawString(str, new Font("맑은 고딕", 8), Brushes.Black, bar.rect.X + 2, bar.rect.Y + 1);
		//            }
		//        }
		//        catch{ continue; }
		//    }

		//}

		//// Status 문자열을 그린다
		//private void DrawStatus(Graphics grfx)
		//{
		//    try
		//    {
		//        if (barList.Count == 0) return;
		//        if (nIdxHoveredTimeZone >= 0)
		//        {
		//            DateTime tm = timeZoneList.ElementAt(nIdxHoveredTimeZone).parent.theTime;
		//            strStatus_CurentTimeZone = String.Format("<Current: {0:00}h {1:00}m {2:00}.{3:000}s>", tm.Hour, tm.Minute, tm.Second, tm.Millisecond);
		//        }
		//        else
		//            strStatus_CurentTimeZone = "<Current:>";

		//        int nStart = -1;
		//        int nEnd = -1;
		//        for(int i=0; i<timeZoneList.Count; i++)
		//        {
		//            if (nStart == -1 && timeZoneList.ElementAt(i).selected)
		//                nStart = i;
		//            else if (nStart != -1 && timeZoneList.ElementAt(i).selected == false)
		//            {
		//                nEnd = i - 1;
		//                break;
		//            }

		//        }

		//        strStatus_SelectedTimeSpan = "";
		//        if (nStart != -1 && nEnd != -1)
		//        {
		//            double dSelectedTime_sec = GetTimeSpan_sec(nStart, nEnd);
		//            strStatus_SelectedTimeSpan = String.Format("<Selected Time: {0:0.000} sec>", dSelectedTime_sec);
		//        }

		//        TimeZoneArea z = timeZoneList.First();
		//        int pos_y = z.rect.Y + z.rect.Height + 30;
		//        //grfx.DrawString(strStatus_CurentTimeZone, new Font("맑은 고딕", 10, FontStyle.Bold), Brushes.Black, z.rect.X, pos_y+10);
		//        //grfx.DrawString(strStatus_SelectedTimeSpan, new Font("맑은 고딕", 10, FontStyle.Bold), Brushes.Black, z.rect.X+200, pos_y+10);
		//        grfx.DrawString(strStatus_CurentTimeZone, new Font("맑은 고딕", 10, FontStyle.Bold), Brushes.Black, statusPos.X, statusPos.Y);
		//        grfx.DrawString(strStatus_SelectedTimeSpan, new Font("맑은 고딕", 10, FontStyle.Bold), Brushes.Black, statusPos.X + 200, statusPos.Y);
		//    }
		//    catch {}

		//}
		//// 세로 그리드 같은 칸을 그린다
		//public void DrawNet(Graphics grfx)
		//{
		//    TimeZoneArea tz = null;
		//    Pen pen = new Pen(Color.Black, 1);
		//    pen.DashStyle = DashStyle.Dot;

		//    int nEndPos_y = itemList.Last().pos_yEnd + barHeight+ 10;
		//    int nLastPos_x = 0;
		//    int nTzIdx = 0;
		//    for (int i=0; i<headerList.Count(); i++)
		//    {
		//        HorizonHeader header = headerList.ElementAt(i);
		//        grfx.DrawLine(pen, header.pos_x, headerTimeStartTop_y, header.pos_x, nEndPos_y);

		//        // 타임존 영역을 Rect Refresh
		//        if (i > 0)
		//        {
		//            tz  = timeZoneList.ElementAt(nTzIdx++);
		//            tz.rect = new Rectangle(nLastPos_x, headerTimeStartTop_y, widthPerItem, nEndPos_y - headerTimeStartTop_y);
		//        }

		//        nLastPos_x = header.pos_x;
		//    }

		//    // 마지막선 그리기
		//    nLastPos_x += widthPerItem;
		//    grfx.DrawLine(pen, nLastPos_x, headerTimeStartTop_y, nLastPos_x, nEndPos_y);

		//    // 마지막 타임존 영역 Rect Refresh
		//    tz = timeZoneList.ElementAt(nTzIdx++);
		//    tz.rect = new Rectangle(nLastPos_x- widthPerItem, headerTimeStartTop_y, widthPerItem, nEndPos_y - headerTimeStartTop_y);           
		//}

		//bool bOnMouseDown = false;
		//Point ptMouseDownPos = new Point();
		//Rectangle rtOnSelectingBox = new Rectangle();


		//public void Event_MouseUp(object sender, MouseEventArgs e)
		//{
		//    if (itemList.Count() < 1 || headerList.Count() <= 1 || timeZoneList.Count() < 1 || bOnMouseDown == false)
		//        return;

		//    bOnMouseDown = false;
		//    Point ptMouseUpPos = new Point(e.X, e.Y);
		//    bool bSelectStart = false;
		//    for(int i=0; i<timeZoneList.Count(); i++)
		//    {
		//        TimeZoneArea tz = timeZoneList.ElementAt(i);
		//        tz.selected = false;

		//        if (tz.rect.Contains(ptMouseDownPos))
		//            bSelectStart = true;

		//        tz.selected = bSelectStart;

		//        if (tz.rect.Contains(ptMouseUpPos))
		//            bSelectStart = false;
		//    }

		//    Invalidate();
		//}

		//    public void Event_MouseDown(object sender, MouseEventArgs e)
		//    {
		//        if (itemList.Count() < 1 || headerList.Count() <= 1 || timeZoneList.Count() < 1)
		//            return;

		//        ptMouseDownPos.X = e.X;
		//        ptMouseDownPos.Y = e.Y;
		//        bOnMouseDown = true;
		//    }

		//    public void Event_MouseClick(object sender, MouseEventArgs e)
		//    {
		//        if (itemList.Count() < 1 || headerList.Count() <= 1 || timeZoneList.Count() < 1)
		//            return;

		//        //ptMouseDownPos.X = e.X;
		//        //ptMouseDownPos.Y = e.Y;
		//        //bOnMouseDown = false;

		//        //foreach (var obj in timeZoneList)
		//        //    obj.selected = false;
		//    }

		//    public void Event_MouseMove(object sender, MouseEventArgs e)
		//    {
		//        if (itemList.Count() < 1 || headerList.Count() <= 1 || timeZoneList.Count() < 1)
		//            return;

		//        if (e.X < ptMouseDownPos.X || e.Y < ptMouseDownPos.Y)
		//            bOnMouseDown = false;

		//        if (bOnMouseDown)
		//        {
		//            rtOnSelectingBox = new Rectangle(ptMouseDownPos.X, ptMouseDownPos.Y, e.X - ptMouseDownPos.X, e.Y - ptMouseDownPos.Y);
		//        }

		//        // 특정 타임존에 마우스가 올라왔으면 선택처리
		//        int i = 0;
		//        nIdxHoveredTimeZone = -1;
		//        foreach (TimeZoneArea obj in timeZoneList)
		//        {
		//            //obj.selected = false;

		//            if (obj.rect.Contains(e.X, e.Y))
		//                nIdxHoveredTimeZone = i; //obj.selected = true;
		//            i++;
		//        }

		//        // 특정 막대기 에 마우스가 올라왔으면 선택처리
		//        foreach (Bar obj in barList)
		//        {
		//            obj.selected = false;
		//            if (obj.rect.Contains(e.X, e.Y))
		//                obj.selected = true;
		//        }

		//       // Invalidate();
		//    }


		//    public bool SaveImage(string filePath)
		//    {
		//        if (headerList.Count <= 1 || itemList.Count < 1)
		//            return false;
		//        try
		//        {
		//            Rectangle rect = GetControlRect();
		//            Bitmap objBmp = new Bitmap(rect.Width, rect.Height);
		//            Graphics g = Graphics.FromImage(objBmp);

		//            Redraw(g);

		//            objBmp.Save(filePath, ImageFormat.Png);
		//            return true;
		//        }
		//        catch (Exception)
		//        {

		//            return false;
		//        }
		//    }
	}


	// Drawing 할때 사용되는 글로벌 파라미터 ********************
	public class DDrawAttribute
	{
		public string title = "";
		public Rectangle designRect = new Rectangle();
		public int dataCount = 0;
		public int barHeight = 30;
		public int graphDefaultHeight = 100;
		public int graphMinHeight = 60;
		public int graphMaxHeight = 300;

		// 좌측 Item Title 쪽에는 아래 1~4까지 순서대로 좌 -> 우 방향. 모두 더한 값이 총 Item Title width가 된다
		public int clrBoxLeftMargin = 3;       // 1 : Color Box 왼쪽 마진
		public int clrBoxSize = 10;            // 2 : 컬러박스 크기
		public int clrBoxRightMargin = 3;  // 3 : 컬러박스 오른쪽 마진
		public int titleWidth = 100;              // 4 : 아이템 이름 폭

		//Top 부분 그리는 순서 간격(세로방향) 상 -> 하
		public int MainTitleTopMargin = 10;
		public int MainTitleHeight = 60;
		public int SubTitleTopMargin = 10;
		public int SubTitleHeight = 40;
		public int DataTopMargin = 10;


		public Point GetDataStartPos()
		{
			int nX = clrBoxLeftMargin + clrBoxSize + clrBoxRightMargin + titleWidth;
			int nY = MainTitleTopMargin + MainTitleHeight + SubTitleTopMargin + SubTitleHeight + DataTopMargin;
			return new Point(nX, nY);
		}

		public int cellWidth;       // 셀폭
		public int lastDrawPosY; // 마지막까지 그린 item의 Y pixel pos. 이어서 다음 아이템 그리기 위함

		public int itemMargin;  //height 방향 margin	

		public Font Font1 = new Font("맑은 고딕", 11F, FontStyle.Bold);
		public Font Font2 = new Font("맑은 고딕", 9F);

		public XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public void Load(ref XmlNode xmlNode)
		{

		}

	}
	// 화면에 보여지는 항목, 그려지는 항목 등 모든 항목의 부모 클래스 ******************
	//
	public abstract class DTimeItemBase
	{
		public enum eType { Bool, Graph, Arrow, Rect, Text}
		public eType m_type = eType.Bool;		// Item 타입
		public string m_name = "";		// Item 이름
		public int m_nOrgIdx = -1;		// 전체 데이터에서 몇번제 Item 인지
		public int m_nNumber = -1;	// 보여지는 항목중 몇번째 인지
		public Color m_ColorFront = Color.Black;	// front color
		public Color m_ColorBack = Color.White;  // back color

		public List<dynamic> m_DataList = new List<dynamic>();	// data list
		

		public abstract void Draw(Graphics g, ref DDrawAttribute att);
		public abstract XmlNode Save(ref XmlDocument xmlDoc);
		public abstract void Load(ref XmlNode xmlNode);

		public void SetData(List<dynamic> dataList)
		{
			this.m_DataList.Clear();
			foreach(dynamic d in dataList)
				this.m_DataList.Add(d);
			
		}


	}

	
	// bool 타입 객체 클래스. 막대기 형태로 보여진다 ***************************
	//
	public class DTimeLineItem_Bool : DTimeItemBase
	{
		public class iBarInfo { public Rectangle rect; public Point XToY; }
        List<iBarInfo> m_BarList = new List<iBarInfo>();
        bool m_bReverse = false;

		public DTimeLineItem_Bool()
		{
			m_type = eType.Bool;
		}

        private void MakeBars(ref DDrawAttribute att)
        {
			// point로 한 이유는 x= 시작 인덱스,  y=끝 인덱스
			List<Point> bars = new List<Point>();   // x to y index
			Point curBar = new Point(-1, -1);
            for (int i = 0; i < m_DataList.Count; i++)
			{
				bool bVal = m_DataList[i];
				if (m_bReverse)
					bVal = !bVal;

				if(bVal)
				{
					if (curBar.X == -1)
						curBar.X = i;
					else
						curBar.Y = i;
				}
				else
				{
					if (curBar.Y != -1)
					{
						bars.Add(curBar);
						curBar = new Point(-1, -1);
					}
				}				
			}
			// 마지막 객체 추가 안되어 있으면 추가
			if (curBar.Y != -1)
			{
				if (bars.Count > 0)
				{
					if (bars.Last() != curBar)
						bars.Add(curBar);
				}
				else
					bars.Add(curBar);
			}


			// 이제 진짜 bar rect 생성
			m_BarList.Clear();      // clear
			foreach (Point oneBar in bars)
			{
				int left = oneBar.X * att.cellWidth;
				int width = (oneBar.Y - oneBar.X + 1) * att.cellWidth;
				Rectangle rt = new Rectangle(left, 0, width, att.barHeight);        // local 좌표계에서 bar 생성. 실제로 그릴때는 global 좌표계로 그리면 됨
				iBarInfo obj = new iBarInfo();
				obj.rect = rt;
				obj.XToY = oneBar;
				m_BarList.Add(obj);
			}
		
        }

		public override void Draw(Graphics g, ref DDrawAttribute att)
		{
            MakeBars(ref att);

			// 좌측 헤더 그리기 ***********
			
			Font font = new Font("맑은 고딕", 9F, FontStyle.Bold);

			if (att.lastDrawPosY <= 0)
				att.lastDrawPosY = att.GetDataStartPos().Y;

			Rectangle legendRect = new Rectangle(att.clrBoxLeftMargin, att.lastDrawPosY + 3, att.clrBoxSize, att.clrBoxSize);

			g.FillRectangle(new SolidBrush(m_ColorFront), legendRect);
			g.DrawRectangle(new Pen(Color.Black), legendRect);
			g.DrawString(m_name, font, Brushes.Black, att.clrBoxLeftMargin + att.clrBoxSize + att.clrBoxRightMargin, att.lastDrawPosY);
			

			Point ptStartPos = att.GetDataStartPos();
			ptStartPos.Y = att.lastDrawPosY;

			foreach (iBarInfo bar in m_BarList)
			{
				bar.rect.Offset(ptStartPos);	// global 위치로 이동
				Brush brush = new LinearGradientBrush(bar.rect, m_ColorFront, Color.White, LinearGradientMode.Vertical);
				
				g.FillRectangle(brush, bar.rect);
				g.DrawRectangle(Pens.Black, bar.rect);
				if (bar.rect.Width > 30)
				{
					//double dSpan = GetTimeSpan_sec(bar.XToY.X, bar.XToY.Y);
					//string str = String.Format("{0:0.000}", dSpan);
					//gfx.DrawString(str, new Font("맑은 고딕", 8), Brushes.Black, bar.rect.X + 2, bar.rect.Y + 1);
				}
			}
			att.lastDrawPosY += att.barHeight + att.DataTopMargin;
		}

		public override XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public override void Load(ref XmlNode xmlNode)
		{

		}
	}

	// int, double 타입 클래스. 그래프 형태로 보임 ***************************
	//
	public class DTimeLineItem_Graph : DTimeItemBase
	{
		public override void Draw(Graphics g, ref DDrawAttribute att)
		{

		}
		public override XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public override void Load(ref XmlNode xmlNode)
		{

		}
	}

	// 그리기 객체 - 화살표 ***************************
	//
	public class DTimeLineItem_Arrow : DTimeItemBase
	{
		public override void Draw(Graphics g, ref DDrawAttribute att)
		{

		}
		public override XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public override void Load(ref XmlNode xmlNode)
		{

		}
	}

	// 그리기 객체 - 사각형 ***************************
	//
	public class DTimeLineItem_Rect : DTimeItemBase
	{
		public override void Draw(Graphics g, ref DDrawAttribute att)
		{

		}
		public override XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public override void Load(ref XmlNode xmlNode)
		{

		}
	}

	// 그리기 객체 - 텍스트 ***************************
	//
	public class DTimeLineItem_Text : DTimeItemBase
	{
		public override void Draw(Graphics g, ref DDrawAttribute att)
		{

		}
		public override XmlNode Save(ref XmlDocument xmlDoc)
		{
			return xmlDoc.CreateNode("", "", "");
		}

		public override void Load(ref XmlNode xmlNode)
		{

		}
	}











	// 세로 시간 존 영역
	public class TimeZoneArea
    {
        public Rectangle rect;
        public HorizonHeader parent;
        public bool selected = false;
    }

    public class HorizonHeader
    {

        public int HeaderIdx;
        public string HeaderText;
        public int pos_x;

        public DateTime theTime = new DateTime();
    }

    // IO의 막대기 한개. On, Off의 값을 가진다. 한개의 TimeLineItem 은 여러개의 Bar를 가질 수 있다
    public class Bar
    {
        public TimeLineItem parentItem;

        public int nStartIdx;
        public int nEndIdx;
        public DateTime startTime;
        public DateTime endTime;

        public Rectangle rect;

        public bool selected = false;

    }

    //  실수형 차트
    public class DblChart
    {
        public TimeLineItem parentItem;

        public static int chartHeight = 140; // 한개의 차트 높이

        public List<double> dataList = new List<double>();
        public Pen pen = new Pen(Color.Black);
        public double dDataMin, dDataMax;   // 데이터의 최대최소값
        public double dVisibleMin, dVisibleMax; // 그래프의 최대최소값.
        public Rectangle rectArea = new Rectangle();   // 점선으로 표시할 그래프 영역

        // dVal을 Pix 값으로 변환
        public int Val2Pixel(double dVal)
        {
            double dDataHeight = dVisibleMax - dVisibleMin;
            double dPer = dVal / dDataHeight;  // 100 퍼를 넘지 않게 하기 위함
            int ret = (int)(dPer* chartHeight);
            return ret;
        }

        // double 데이터 값들을 그래프의 픽셀값 리스트로 얻어옴
        public List<Point> Get_asPixelValList(int nWidthPerItem, Point ptStartPos)
        {
            List<Point> ptList = new List<Point>();
            int i = 0;
            foreach(double dVal in dataList)
            {
                int nY = ptStartPos.Y + chartHeight;    // Y축 시작 위치
                nY -= Val2Pixel(dVal);      // 픽셀은 Y축 아래방향이 부호 반대
                nY += Val2Pixel(this.dDataMin);     //최소값을 0으로 만들기 위해서
                int nX = nWidthPerItem * i + ptStartPos.X;
                ptList.Add(new Point(nX, nY));
                i++;
            }

            return ptList;
        }
    }

    public class TimeLineItem
    {
        public int nIdx = 0;
        public string Name;
        public Color Color;
        public Color HoverColor;
        public List<bool> BVals = new List<bool>();
        public List<double> DVals = new List<double>();
        public double dValMin = 0;
        public double dValMax = 0;
        public Object Tag;
        public int pos_y;
        public int pos_yEnd;
        public bool bBoolTypeItem = true;       // true = bool Type,  false = double Type
        public TimeLineItem()
        {
        }

        public TimeLineItem(string name, Color color, Color hoverColor, List<bool> Values)
        {
            this.Name = name;
            this.Color = color;
            this.HoverColor = hoverColor;
            this.BVals = Values;
        }
    }

}

