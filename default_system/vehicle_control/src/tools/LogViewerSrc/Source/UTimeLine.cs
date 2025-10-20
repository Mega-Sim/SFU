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
namespace LogViewer
{
    public partial class UTimeLine : Form
    {
        private DTimeLine timeLine = new DTimeLine();
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
            timeLine.Dock = DockStyle.Fill;
            this.panel1.Controls.Add(timeLine);
            
            timeLine.MouseMove += new MouseEventHandler(timeLine.Event_MouseMove);
            timeLine.MouseUp += new MouseEventHandler(timeLine.Event_MouseUp);
            timeLine.MouseDown += new MouseEventHandler(timeLine.Event_MouseDown);
            timeLine.MouseClick += new MouseEventHandler(timeLine.Event_MouseClick);
        }


        // 그래프 만들기
        public void MakeTimeLine(string filePath, DataGridView table, int nIdxFrom, int nIdxTo, int nTimeCol, int[] arrBoolCols, int []arrDoubleCols)
        {
            if(nIdxTo - nIdxFrom < 2)
            {
                MessageBox.Show("2개 이상의 데이터가 있어야 그래프를 만들 수 있습니다.");
                return;
            }
            if(arrBoolCols.Length == 0 && arrDoubleCols.Length == 0)
            {
                MessageBox.Show("데이터를 선택해야 합니다.");
                return;
            }

            timeLine.Title = "";
            if (String.IsNullOrEmpty(filePath) == false)
                timeLine.Title = GMethod.GetLastDirAndFileName(filePath);

            // Time 넣기
            try
            {
                int nCnt = 0;
                for (int i = nIdxFrom; i <= nIdxTo; i++)
                {
                    DataGridViewRow row = table.Rows[i];
                    DateTime tm =  DateTime.Parse((string)row.Cells[nTimeCol].Value);
                    timeLine.AddHeader(nCnt++, tm);
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return;
            }
            string errMsg = "";

            // boolean 데이터 넣기
            for (int k=0; k< arrBoolCols.Length; k++)
            {
                try
                {
                    int nIdx = arrBoolCols[k];

                    List<bool> dataList = new List<bool>();
                    bool bFailCase = false;
                    for (int i = nIdxFrom; i < nIdxTo; i++)
                    {
                        DataGridViewRow row = table.Rows[i];
                        
                        bool b = false;
                        string strVal = Convert.ToString(row.Cells[nIdx].Value);
                        if (strVal == "1")
                            b = true;
                        else if (strVal == "0")
                            b = false;
                        else
                        {
                            bFailCase = true;
                            errMsg += String.Format("Bool 값 변환 실패하여 {0} 항목 그래프 생성실패. 값={1}\n", table.Columns[nIdx].Name, strVal);
                            break;
                        }

                        dataList.Add(b);
                    }
                    if (bFailCase)
                        continue;

                    TimeLineItem item = new TimeLineItem();
                    item.bBoolTypeItem = true;
                    item.BVals = dataList;
                    item.Name = table.Columns[nIdx].Name;
                    item.Color = colorList.ElementAt(k % colorList.Length);
                    timeLine.AddTimeLineItem(item);
                }
                catch (Exception e)
                {
                    Debug.Assert(false, e.Message);
                    continue;
                }
            }

            
            // double 형 데이터 넣기
            for (int k = 0; k < arrDoubleCols.Length; k++)
            {
                try
                {
                    int nIdx = arrDoubleCols[k];

                    List<double> dataList = new List<double>();
                    bool bFailCase = false;
                    double dMin = Double.MaxValue;
                    double dMax = Double.MinValue;

                    for (int i = nIdxFrom; i < nIdxTo; i++)
                    {
                        DataGridViewRow row = table.Rows[i];

                        double dVal = 0;
                        string strVal = Convert.ToString(row.Cells[nIdx].Value);
                        if (Double.TryParse(strVal, out dVal) == false)
                        {
                            bFailCase = true;
                            errMsg += String.Format("실수값 변환 실패하여 {0} 항목 그래프 생성실패. 값={1}\n", table.Columns[nIdx].Name, strVal);
                            break;
                        }
                        if (dMin > dVal)
                            dMin = dVal;

                        if (dMax < dVal)
                            dMax = dVal;

                        dataList.Add(dVal);
                    }

                    if (bFailCase)
                        continue;

                    TimeLineItem item = new TimeLineItem();
                    item.bBoolTypeItem = false;
                    item.DVals = dataList;
                    item.dValMin = dMin;
                    item.dValMax = dMax;
                    item.Name = table.Columns[nIdx].Name;
                    item.Color = colorList.ElementAt(k % colorList.Length);
                    timeLine.AddTimeLineItem(item);
                }
                catch (Exception e)
                {
                    Debug.Assert(false, e.Message);
                    continue;
                }
            }

            // 에러 메시지
            if(String.IsNullOrEmpty(errMsg) == false)
                MessageBox.Show(errMsg);

            if(timeLine.itemList.Count == 0)
            {
                MessageBox.Show("생성 할 수 있는 TimeLine 이 없습니다.");
                return;
            }
            // 그래프 만들기
            timeLine.MakeTimeLine();
        }

        private void btn_saveImg_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            string dir = Application.StartupPath + "\\" + "TimeLine";
            Directory.CreateDirectory(dir);

            dlg.InitialDirectory = dir;
            DateTime tm = DateTime.Now;
            dlg.FileName = String.Format("{0:00}{1:00}{2:00}_{3:00}{4:00}{5:00}_", tm.Year-2000, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second) + ".PNG";
            dlg.FileName = dlg.FileName.Replace('/', '_');
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            timeLine.SaveImage(dlg.FileName);
            if(File.Exists(dlg.FileName))
                MessageBox.Show("저장성공");
            else
                MessageBox.Show("저장실패");
        }
    }
}
