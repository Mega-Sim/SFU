using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using TimeLine;
using MyTools;
using System.Diagnostics;
using System.IO;
using LogViewer;

namespace LogViewer
{
    public partial class UTimeLine : Form
    {
		public class DTimeLineItem
		{

		}

		private int m_nStartIdx = 0;
		private int m_nEndIdx = 0;

        private DTimeLine m_TimeLine = new DTimeLine();
		private DAmcRcv m_AmcDataTable = null;
		private List<int> m_ShowItemList = new List<int>();
		private int m_nCurSelectedItemIdx = -1;
		private DAmcRecvTimeLineAtt m_CurSelectedITemAtt = new DAmcRecvTimeLineAtt();


		private Color[] colorList =         // 기본 색상
            {
                Color.Black
                , Color.DarkGray
                , Color.Blue
                , Color.Chartreuse
                , Color.Crimson
                ,  Color.DarkCyan
                , Color.DarkRed
                , Color.Indigo
                , Color.DeepPink
                , Color.RoyalBlue
                , Color.Navy
                , Color.Orange
                , Color.Orchid
                };

        public UTimeLine()
        {
            InitializeComponent();
            this.Load += new System.EventHandler(this.UTimeLine_Load);
			
        }

        private void UTimeLine_Load(object sender, EventArgs e)
        {
            //m_TimeLine.Dock = DockStyle.Fill;
            //this.panel1.Controls.Add(m_TimeLine);
            
            //m_TimeLine.MouseMove += new MouseEventHandler(m_TimeLine.Event_MouseMove);
            //m_TimeLine.MouseUp += new MouseEventHandler(m_TimeLine.Event_MouseUp);
            //m_TimeLine.MouseDown += new MouseEventHandler(m_TimeLine.Event_MouseDown);
            //m_TimeLine.MouseClick += new MouseEventHandler(m_TimeLine.Event_MouseClick);
        }
		
		// 외부에서 AMC Time Line을 생성할때 ***************
		//
		public void CreateAmcTimeLine(DAmcRcv data, int nIdxStart, int nIdxEnd, List<int> SelectedCol)
		{
			m_AmcDataTable = data;
			m_TimeLine.m_AmcRecv = data;
			m_TimeLine.m_DrawAtt.title = MyTools.GMethod.GetLastDir(data.filePath);
			m_ShowItemList = SelectedCol;
			m_nStartIdx = nIdxStart;
			m_nEndIdx = nIdxEnd;
			m_TimeLine.m_DrawAtt.dataCount = m_nEndIdx - m_nStartIdx + 1;

			RefreshAllItemList();
			CreateObject();
			RefreshShowItemListView();
			//m_TimeLine
		}

		Font BoldFont = new Font("맑은 고딕", 10, FontStyle.Bold);

		private void RefreshAllItemList()
		{
			listView_AllItem.Items.Clear();

			for (int i = 0; i < (int)eAmcRcvIdx.COUNTOF; i++)
			{

				string[] arr = new string[2];
				arr[0] = String.Format("{0}", i);
				arr[1] = DAmcRcvRowItem.GetHeaderName((eAmcRcvIdx)i, false);
				ListViewItem lvi = new ListViewItem(arr);
				lvi.Tag = i;

				if(m_ShowItemList.FindIndex(o => o == i) >=0)
					lvi.Font = BoldFont;	// 이미 선택된 항목은 진하게 표시

				listView_AllItem.Items.Add(lvi);
			}
		}

		// Time Line 아이템 객체들 생성
		private void CreateObject()
		{
			m_TimeLine.m_ObjectList.Clear();
			int i = 0;
			foreach(int nIdx in m_ShowItemList)
			{
				if(nIdx >= (int)eAmcRcvIdx.In1_Hand_Inpos)
				{
					DTimeLineItem_Bool obj = new DTimeLineItem_Bool();
					obj.m_name = DAmcRcvRowItem.GetHeaderName((eAmcRcvIdx)nIdx, false);
					obj.m_nNumber = i;
					obj.m_ColorFront = colorList[i%colorList.Count()];
					obj.m_nOrgIdx = nIdx;
					for (int k= m_nStartIdx;  k<m_nEndIdx; k++)
					{
						DAmcRcvRowItem row = m_AmcDataTable.GetRowObj(k);
						int n = row.subItems[nIdx];
						bool bVal = n > 0;
						obj.m_DataList.Add(bVal);
					}
					m_TimeLine.m_ObjectList.Add(obj);
				}

				i++;
			}
			
		}

		// 보여지는 항목에 추가할 때
		private void AddToShowItem(int nIdx)
		{
			if (m_ShowItemList.FindIndex(o => o == nIdx) < 0)
				m_ShowItemList.Add(nIdx);

			RefreshShowItemListView();
		}

		// ListView Refresh - 보여지는 항목 
		public void RefreshShowItemListView()
		{
			listView_ShowItem.Items.Clear();

			for (int i = 0; i < m_ShowItemList.Count; i++)
			{
				string[] arr = new string[2];
				arr[0] = String.Format("{0}", i);
				arr[1] = DAmcRcvRowItem.GetHeaderName((eAmcRcvIdx)m_ShowItemList[i], false);
				ListViewItem lvi = new ListViewItem(arr);
				lvi.Tag = m_ShowItemList[i];
				listView_ShowItem.Items.Add(lvi);
			}

			if (m_nCurSelectedItemIdx >= 0)
			{
				foreach (ListViewItem lv in listView_ShowItem.Items)
				{
					if (lv.Tag != null)
					{
						if ((int)lv.Tag == m_nCurSelectedItemIdx)
						{
							lv.Selected = true;
							break;
						}
					}
				}
			}
			
		}



        //// 그래프 만들기
        //public void MakeTimeLine(string filePath, DataGridView table, int nIdxFrom, int nIdxTo, int nTimeCol, int[] arrBoolCols, int []arrDoubleCols)
        //{
        //    if(nIdxTo - nIdxFrom < 2)
        //    {
        //        MessageBox.Show("2개 이상의 데이터가 있어야 그래프를 만들 수 있습니다.");
        //        return;
        //    }
        //    if(arrBoolCols.Length == 0 && arrDoubleCols.Length == 0)
        //    {
        //        MessageBox.Show("데이터를 선택해야 합니다.");
        //        return;
        //    }

        //    m_TimeLine.Title = "";
        //    if (String.IsNullOrEmpty(filePath) == false)
        //        m_TimeLine.Title = GMethod.GetLastDirAndFileName(filePath);

        //    // Time 넣기
        //    try
        //    {
        //        int nCnt = 0;
        //        for (int i = nIdxFrom; i <= nIdxTo; i++)
        //        {
        //            DataGridViewRow row = table.Rows[i];
        //            DateTime tm =  DateTime.Parse((string)row.Cells[nTimeCol].Value);
        //            m_TimeLine.AddHeader(nCnt++, tm);
        //        }
        //    }
        //    catch (Exception e)
        //    {
        //        Debug.Assert(false, e.Message);
        //        return;
        //    }
        //    string errMsg = "";

        //    // boolean 데이터 넣기
        //    for (int k=0; k< arrBoolCols.Length; k++)
        //    {
        //        try
        //        {
        //            int nIdx = arrBoolCols[k];

        //            List<bool> dataList = new List<bool>();
        //            bool bFailCase = false;
        //            for (int i = nIdxFrom; i < nIdxTo; i++)
        //            {
        //                DataGridViewRow row = table.Rows[i];
                        
        //                bool b = false;
        //                string strVal = Convert.ToString(row.Cells[nIdx].Value);
        //                if (strVal == "1")
        //                    b = true;
        //                else if (strVal == "0")
        //                    b = false;
        //                else
        //                {
        //                    bFailCase = true;
        //                    errMsg += String.Format("Bool 값 변환 실패하여 {0} 항목 그래프 생성실패. 값={1}\n", table.Columns[nIdx].Name, strVal);
        //                    break;
        //                }

        //                dataList.Add(b);
        //            }
        //            if (bFailCase)
        //                continue;

        //            TimeLineItem item = new TimeLineItem();
        //            item.bBoolTypeItem = true;
        //            item.BVals = dataList;
        //            item.Name = table.Columns[nIdx].Name;
        //            item.Color = colorList.ElementAt(k % colorList.Length);
        //            m_TimeLine.AddTimeLineItem(item);
        //        }
        //        catch (Exception e)
        //        {
        //            Debug.Assert(false, e.Message);
        //            continue;
        //        }
        //    }

            
        //    // double 형 데이터 넣기
        //    for (int k = 0; k < arrDoubleCols.Length; k++)
        //    {
        //        try
        //        {
        //            int nIdx = arrDoubleCols[k];

        //            List<double> dataList = new List<double>();
        //            bool bFailCase = false;
        //            double dMin = Double.MaxValue;
        //            double dMax = Double.MinValue;

        //            for (int i = nIdxFrom; i < nIdxTo; i++)
        //            {
        //                DataGridViewRow row = table.Rows[i];

        //                double dVal = 0;
        //                string strVal = Convert.ToString(row.Cells[nIdx].Value);
        //                if (Double.TryParse(strVal, out dVal) == false)
        //                {
        //                    bFailCase = true;
        //                    errMsg += String.Format("실수값 변환 실패하여 {0} 항목 그래프 생성실패. 값={1}\n", table.Columns[nIdx].Name, strVal);
        //                    break;
        //                }
        //                if (dMin > dVal)
        //                    dMin = dVal;

        //                if (dMax < dVal)
        //                    dMax = dVal;

        //                dataList.Add(dVal);
        //            }

        //            if (bFailCase)
        //                continue;

        //            TimeLineItem item = new TimeLineItem();
        //            item.bBoolTypeItem = false;
        //            item.DVals = dataList;
        //            item.dValMin = dMin;
        //            item.dValMax = dMax;
        //            item.Name = table.Columns[nIdx].Name;
        //            item.Color = colorList.ElementAt(k % colorList.Length);
        //            m_TimeLine.AddTimeLineItem(item);
        //        }
        //        catch (Exception e)
        //        {
        //            Debug.Assert(false, e.Message);
        //            continue;
        //        }
        //    }

        //    // 에러 메시지
        //    if(String.IsNullOrEmpty(errMsg) == false)
        //        MessageBox.Show(errMsg);

        //    if(m_TimeLine.itemList.Count == 0)
        //    {
        //        MessageBox.Show("생성 할 수 있는 TimeLine 이 없습니다.");
        //        return;
        //    }
        //    // 그래프 만들기
        //    m_TimeLine.MakeTimeLine();
        //}

        private void btn_saveImg_Click(object sender, EventArgs e)
        {
            //SaveFileDialog dlg = new SaveFileDialog();
            //string dir = Application.StartupPath + "\\" + "TimeLine";
            //Directory.CreateDirectory(dir);

            //dlg.InitialDirectory = dir;
            //DateTime tm = DateTime.Now;
            //dlg.FileName = String.Format("{0:00}{1:00}{2:00}_{3:00}{4:00}{5:00}_", tm.Year-2000, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second) + ".PNG";
            //dlg.FileName = dlg.FileName.Replace('/', '_');
            //if (dlg.ShowDialog() != DialogResult.OK)
            //    return;

            //m_TimeLine.SaveImage(dlg.FileName);
            //if(File.Exists(dlg.FileName))
            //    MessageBox.Show("저장성공");
            //else
            //    MessageBox.Show("저장실패");
        }

		private void timer1_Tick(object sender, EventArgs e)
		{

			//chk_Guide1
			//	btn_Guide1Color
			//	numeric_GuideLine1
			//	textBox_GuideLine1
		}

		private void listView_ShowItem_SelectedIndexChanged(object sender, EventArgs e)
		{
			if(listView_ShowItem.SelectedItems.Count > 0)
			{
				m_nCurSelectedItemIdx = (int)listView_ShowItem.SelectedItems[0].Tag;
			}

			label_ItemName.Text = DAmcRcvRowItem.GetHeaderName((eAmcRcvIdx)m_nCurSelectedItemIdx, false);
			m_CurSelectedITemAtt = m_AmcDataTable.ArrTimeLineAtt[m_nCurSelectedItemIdx];

			chk_Reverse.Checked = m_CurSelectedITemAtt.bReverse;

			numeric_ChartMin.Value = (decimal)m_CurSelectedITemAtt.dMin;
			numeric_ChartMax.Value = (decimal)m_CurSelectedITemAtt.dMax;
			numeric_GraphHeight.Value = (decimal)m_CurSelectedITemAtt.dHeight;
			chk_Guide1.Checked = m_CurSelectedITemAtt.guideLines[0].Use;
			btn_Guide1Color.ForeColor = m_CurSelectedITemAtt.guideLines[0].color;
			numeric_GuideLine1.Value = (decimal)m_CurSelectedITemAtt.guideLines[0].Val;
			textBox_GuideLine1.Text = m_CurSelectedITemAtt.guideLines[0].text;

			chk_Guide2.Checked = m_CurSelectedITemAtt.guideLines[1].Use;
			btn_Guide2Color.ForeColor = m_CurSelectedITemAtt.guideLines[1].color;
			numeric_GuideLine2.Value = (decimal)m_CurSelectedITemAtt.guideLines[1].Val;
			textBox_GuideLine2.Text = m_CurSelectedITemAtt.guideLines[1].text;

			chk_Guide3.Checked = m_CurSelectedITemAtt.guideLines[2].Use;
			btn_Guide3Color.ForeColor = m_CurSelectedITemAtt.guideLines[2].color;
			numeric_GuideLine3.Value = (decimal)m_CurSelectedITemAtt.guideLines[2].Val;
			textBox_GuideLine3.Text = m_CurSelectedITemAtt.guideLines[2].text;

		}

		private void btn_Apply_Click(object sender, EventArgs e)
		{
			m_CurSelectedITemAtt.bReverse = chk_Reverse.Checked;

			m_CurSelectedITemAtt.dMin = (double)numeric_ChartMin.Value;
			m_CurSelectedITemAtt.dMax = (double)numeric_ChartMax.Value;
			m_CurSelectedITemAtt.dHeight = (double)numeric_GraphHeight.Value;
			m_CurSelectedITemAtt.guideLines[0].Use = chk_Guide1.Checked;
			m_CurSelectedITemAtt.guideLines[0].color = btn_Guide1Color.ForeColor;
			m_CurSelectedITemAtt.guideLines[0].Val = (double)numeric_GuideLine1.Value;
			m_CurSelectedITemAtt.guideLines[0].text = textBox_GuideLine1.Text;

			m_CurSelectedITemAtt.guideLines[1].Use = chk_Guide2.Checked;
			m_CurSelectedITemAtt.guideLines[1].color = btn_Guide2Color.ForeColor;
			m_CurSelectedITemAtt.guideLines[1].Val = (double)numeric_GuideLine2.Value;
			m_CurSelectedITemAtt.guideLines[1].text = textBox_GuideLine2.Text;

			m_CurSelectedITemAtt.guideLines[2].Use = chk_Guide3.Checked;
			m_CurSelectedITemAtt.guideLines[2].color = btn_Guide3Color.ForeColor;
			m_CurSelectedITemAtt.guideLines[2].Val = (double)numeric_GuideLine3.Value;
			m_CurSelectedITemAtt.guideLines[2].text = textBox_GuideLine3.Text;

			Redraw();
		}

		private void Redraw()
		{
			//m_TimeLine.Invalidate();
			Invalidate();
		}

		// Time Line Draw ***************************


		private void pictureBox1_Resize(object sender, EventArgs e)
		{
			Invalidate();
		}

		private void pictureBox1_Move(object sender, EventArgs e)
		{
			Invalidate();
		}

		private void pictureBox1_Paint_1(object sender, PaintEventArgs e)
		{
			pictureBox1.SuspendLayout();

			m_TimeLine.m_DrawAtt.designRect = pictureBox1.ClientRectangle;
			m_TimeLine.Redraw(e.Graphics);

			pictureBox1.ResumeLayout();
		}
	}
	
}
