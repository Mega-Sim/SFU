using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;   //INI 추가하기 위함 class 추가
using System.Diagnostics;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Specialized;
using System.Drawing;
using System.Runtime.CompilerServices;
/// <summary>
/// SHCHO Golbal using tool, method 집합
/// 2048.04
/// </summary>
namespace MyTool
{
    class GMethod
    {
        public static bool IsDigit_Int(string val)
        {
            int nTmp = 0;
            if (int.TryParse(val, out nTmp))
                return true;
            return false;
        }
        public static bool IsDigit_Dbl(string val)
        {
            double dVal = 0.0;
            if (Double.TryParse(val, out dVal))
                return true;
            return false;
        }
        public static bool IsBoolean(string val)
        {
            if (val != "1" && val != "0")
                return false;

            return true;
        }
        public static string GetTimeString(DateTime dt)
        {
            return String.Format("{0:00}:{1:00}:{2:00}.{3:00}", dt.Hour, dt.Minute, dt.Second, dt.Millisecond);
        }
        public static string GetLastDirAndFileName(string path)
        {
            string fileName = "";
            string lastDir = "";
            try
            {
                fileName = Path.GetFileName(path);
                lastDir = Path.GetDirectoryName(path).Split('\\').Last();        //마지막 폴더 경로만 표시. 다 표시하긴 너무 번잡함
            }
            catch { return path; }

            return lastDir + "/" + fileName;
        }
        public static string GetLastDir(string path)
        {
            //string fileName = "";
            string lastDir = "";
            try
            {
                lastDir = Path.GetDirectoryName(path).Split('\\').Last();        //마지막 폴더 경로만 표시. 다 표시하긴 너무 번잡함
            }
            catch { return path; }

            return lastDir;
        }
        static public bool GetTimeFromLine(string filepath, string line, ref SmallDateTime _out_theTime)
        {
            int nYear = 0;
            int nMonth = 0;
            int nDay = 0;
            int nHour = 0;
            int nMin = 0;
            int nSec = 0;
            int nMSec = 0;

            // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
            int nStart = line.IndexOf("[") + 1;
            int nEnd = line.IndexOf("]");
            if (nStart < 0 || nEnd < 0 || nStart >= nEnd)
            {
                return false;
            }

            string strTime = line.Substring(nStart, nEnd - nStart);

            //22:59:57.984
            string[] arr = strTime.Split(':');
            nHour = Convert.ToInt32(arr[0]);
            nMin = Convert.ToInt32(arr[1]);

            string[] arr_sec = arr[2].Split('.');
            nSec = Convert.ToInt32(arr_sec[0]);
            nMSec = Convert.ToInt32(arr_sec[1]);

            // 파일 이름에서 날짜 얻기
            string strFileName = Path.GetFileName(filepath);

            // 파일 이름은 일반적으로 xxxx20180406.log 이런식으로붙으니까 .log 이전에 8자리만 빼온다
            int nLen = strFileName.Length;
            if (nLen <= 12)
            {
                return false;
            }

            int nTmp = -1;
            string strDate = "";
            for (int i = 2018; i < 2099; i++)
            {
                nTmp = strFileName.IndexOf(Convert.ToString(i));
                if (nTmp >= 0)
                    break;
            }
            if (nTmp >= 0)
            {
                strDate = strFileName.Substring(nTmp, 8);
            }

            // 파싱한 내용이 숫자가 아니면 문자기 때문에 날짜 정보가 아니므로
            // 파일 이름으로 날짜가 적혀있는게 아니다. 이럴경우 실패로 빼기 보다는 그냥 오늘 날짜로 만들어버리자.
            if (!GMethod.IsDigit_Int(strDate) || nTmp < 0)
            {
                nYear = DateTime.Today.Year;
                nMonth = DateTime.Today.Month;
                nDay = DateTime.Today.Day;
            }
            else
            {
                nYear = Convert.ToInt32(strDate.Substring(0, 4));
                nMonth = Convert.ToInt32(strDate.Substring(4, 2));
                nDay = Convert.ToInt32(strDate.Substring(6, 2));
            }
            
            _out_theTime.SetTime(nYear, nMonth, nDay, nHour, nMin, nSec, nMSec);
            return true;

        }
        public static string FindExcelExePath()
        {
            List<string> suspect = new List<string>();
            suspect.Add(@"C:\Program Files\Microsoft Office\Office10\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office10\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office10\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office\Office11\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office11\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office11\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office\Office12\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office12\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office12\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office 14\ClientX64\Root\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office 14\ClientX86\Root\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office 14\ClientX86\Root\Office14\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office 15\ClientX64\Root\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office 15\ClientX86\Root\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office 15\ClientX86\Root\Office15\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office\Office16\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office\Office16\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office\Office16\EXCEL.EXE");
            suspect.Add(@"C:\Program Files\Microsoft Office 16\ClientX64\Root\Office16\EXCEL.EXE");
            suspect.Add(@"C:\Program Files (x86)\Microsoft Office 16\ClientX86\Root\Office16\EXCEL.EXE");
            suspect.Add(@"C:\Program Files(x86)\Microsoft Office 16\ClientX86\Root\Office16\EXCEL.EXE");

            foreach (string path in suspect)
            {
                if (File.Exists(path))
                    return path;
            }
            return "";
        }

        // using System.Runtime.CompilerServices; 필요
        public static string MakeStackErrMsg(string str, [CallerLineNumber] int lineNumber = 0, [CallerMemberName] string caller = null)
        {
            return String.Format("{0}(..) [Ln:{1}]: {2}", caller, lineNumber, str);
        }
        // 파일 이름도 표시 . 반드시 new StackFrame(true)
        public static string MakeStackErrMsg2(string str, StackFrame st, [CallerLineNumber] int lineNumber = 0, [CallerMemberName] string caller = null)
        {
            return String.Format("{0}::{1}() [Ln:{2}]: {3}", Path.GetFileName(st.GetFileName()), caller, lineNumber, str);
        }

        public static Point GetMousePos_onControl(Control ctrl)
        {
            Rectangle rt = ctrl.RectangleToScreen(ctrl.ClientRectangle);
            return new Point(Control.MousePosition.X - rt.X, Control.MousePosition.Y - rt.Y);
        }

    }


    public class GridHelper
    {
        // Column Width 를 INI에 자동 저장
        public void SaveColWidth(Form form, DataGridView grid, string iniPath)
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = iniPath;
                string strSection = String.Format("Form_{0}__Grid_{1}", form.Name, grid.Name);

                for (int i = 0; i < grid.Columns.Count; i++)
                {
                    string strKey = "Col_" + Convert.ToString(i);
                    ini.Write(strSection, strKey, Convert.ToString(grid.Columns[i].Width));
                }
            }
            catch (Exception e)            {                Debug.Assert(false, e.Message);            }
        }

        // Column Width를 INI에서 가져와서 세팅함
        public void LoadColWidth(Form form, DataGridView grid, string iniPath)
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = iniPath;
                string strSection = String.Format("Form_{0}__Grid_{1}", form.Name, grid.Name);

                for (int i = 0; i < grid.Columns.Count; i++)
                {
                    string strKey = "Col_" + Convert.ToString(i);
                    string strWidth = ini.Read(strSection, strKey, Convert.ToString(grid.Columns[i].Width));
                    grid.Columns[i].Width = Convert.ToInt32(strWidth);
                }
            }
            catch (Exception e)            {                Debug.Assert(false, e.Message);            }
        }

        public void ZoomCellSize_Height(DataGridView grid, bool bZoomIn)
        {
            try
            {
                if (grid.RowTemplate.Height > 1 && bZoomIn)  // 축소
                    grid.RowTemplate.Height -= 1;

                if (grid.RowTemplate.Height < 100 && !bZoomIn) //확대
                    grid.RowTemplate.Height += 1;
            }
            catch (Exception e)            {                Debug.Assert(false, e.Message);            }
        }

        public void ZoomCellSize_Width(DataGridView grid, bool bZoomIn, bool bSelectedCellOnly)
        {
            try
            {
                if (bSelectedCellOnly)
                {
                    if (grid.SelectedCells.Count == 0)
                        return;

                    int nCol = grid.SelectedCells[0].ColumnIndex;
                    if (bZoomIn)
                        grid.Columns[nCol].Width -= 1;
                    else
                        grid.Columns[nCol].Width += 1;

                    return;
                }

                for (int i = 0; i < grid.Columns.Count; i++)
                {
                    if (bZoomIn)
                        grid.Columns[i].Width -= 1;
                    else
                        grid.Columns[i].Width += 1;
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        public void FontZoom(DataGridView grid, bool bZoomIn, float dWeight = (float)0.5)
        {
            Font font = grid.Font;
            float nNewSize = font.Size+(bZoomIn ? +dWeight : -dWeight);
            Font newFont = new Font(font.Name, nNewSize, font.Style);
            grid.Font = newFont;
        }
        public void ExportExcel(DataGridView grid, bool bSelectedOnly)
        {

        }
    }

    public class IniMan
    {

        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);




        public string Path = "";

        public void Write(string section, string key, string val)
        {
            Debug.Assert(String.IsNullOrEmpty(Path) == false);
            WritePrivateProfileString(section, key, val, Path);
        }

        public string Read(string section, string key, string default_)
        {
            Debug.Assert(String.IsNullOrEmpty(Path) == false);

            StringBuilder val = new StringBuilder();

            GetPrivateProfileString(section, key, default_, val, 32, Path);
            string str = val.ToString();
            if (String.IsNullOrEmpty(str))
                return "";
            return str;
        }
    }

    // 범위 체크를 편하게 위한 클래스
    public class Range
    {
        double dFrom = 0, dTo = 0;

        public Range() { }

        public Range(double dFrom, double dTo)
        {
            this.dFrom = dFrom;
            this.dTo = dTo;
        }

        public bool ValidationCheck()
        {
            return dFrom < dTo;
        }

        public bool IsInRange(double dVal)
        {
            System.Diagnostics.Debug.Assert(ValidationCheck());
            return dVal >= dFrom && dVal < dTo;
        }
        // static
        public static bool IsInRange(double dFrom, double dTo, double dVal)
        {
            Range a = new Range();

            System.Diagnostics.Debug.Assert(a.ValidationCheck());
            return a.IsInRange(dVal);
        }
    }

    public class SmallDateTime      //size = 8 Bytes
    {
        // byte = 0~255
        // short = -32768 ~ +32768
        protected byte Year = 18;  // 2018 다 표현 안하고 18만 표현
        protected byte Month = 01;
        protected byte Day = 01;
        protected byte Hour = 00;
        protected byte Min = 00;
        protected byte Sec = 00;
        protected short MSec = 00;

        public SmallDateTime()
        {
            SetAsToday();
        }
        public SmallDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMsec)
        {
            SetTime(nYear, nMonth, nDay, nHour, nMin, nSec, nMsec);
        }
        public SmallDateTime(int nHour, int nMin, int nSec, int nMsec)
        {
            SetAsToday();
            Hour = (byte)nHour;
            Min = (byte)nMin;
            Sec = (byte)nSec;
            MSec = (short)nMsec;
        }
        public SmallDateTime(DateTime tm)
        {
            SetTime(tm);
        }
        public void SetTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMsec)
        {
            int nTmpYear = nYear - 2000;
            System.Diagnostics.Debug.Assert(nTmpYear >= 0 && nTmpYear < 255, "SmallDateTime에서는 2000년 부터 2255년까지만 지원합니다;;;;");
            Year = (byte)(nTmpYear);
            Month = (byte)nMonth;
            Day = (byte)nDay;
            Hour = (byte)nHour;
            Min = (byte)nMin;
            Sec = (byte)nSec;
            MSec = (short)nMsec;
        }
        public void SetTime(DateTime tm)
        {
            int nTmpYear = tm.Year - 2000;
            System.Diagnostics.Debug.Assert(nTmpYear >= 0 && nTmpYear < 255, "SmallDateTime에서는 2000년 부터 2255년까지만 지원합니다;;;;");
            Year = (byte)(nTmpYear);
            Month = (byte)tm.Month;
            Day = (byte)tm.Day;
            Hour = (byte)tm.Hour;
            Min = (byte)tm.Minute;
            Sec = (byte)tm.Second;
            MSec = (short)tm.Millisecond;
        }
        public void SetAsToday()
        {
            int nTmpYear = DateTime.Today.Year - 2000;
            Year = (byte)nTmpYear;
            Month = (byte)DateTime.Today.Month;
            Day = (byte)DateTime.Today.Day;
            Hour = (byte)DateTime.Today.Hour;
            Min = (byte)DateTime.Today.Minute;
            Sec = (byte)DateTime.Today.Second;
            MSec = (short)DateTime.Today.Millisecond;
        }
        public void SetYear(int nYear)
        {
            int nTmpYear = nYear - 2000;
            System.Diagnostics.Debug.Assert(nTmpYear >= 0 && nTmpYear < 255, "SmallDateTime에서는 2000년 부터 2255년까지만 지원합니다;;;;");
            Year = (byte)nTmpYear;
        }

        public void SetMonth(int nMonth) { Month = (byte)nMonth; }
        public void SetDay(int nDay) { Day = (byte)nDay; }
        public void SetHour(int nHour) { Hour = (byte)nHour; }
        public void SetMinute(int nMin) { Min = (byte)nMin; }
        public void SetSecond(int nSec) { Sec = (byte)nSec; }
        public void SetMSecond(int nMS) { MSec = (byte)nMS; }


        public int GetYear() { return Year + 2000; }
        public int GetMonth() { return Month; }
        public int GetDay() { return Day; }
        public int GetHour() { return Hour; }
        public int GetMinute() { return Min; }
        public int GetSecond() { return Sec; }
        public int GetMiliSecond() { return MSec; }

        public DateTime GetDateTime()
        {
            return new DateTime(Year+2000, Month, Day, Hour, Min, Sec, MSec);
        }
        public string GetTimeString()
        {
            return String.Format("{0:00}:{1:00}:{2:00}.{3:000}", Hour, Min, Sec, MSec);
        }
        public string GetDateString()
        {
            return String.Format("{0:0000}-{1:00}-{2:00}", Year, Month, Day);
        }
        public string GetDateTimeString()
        {
            return GetDateString() + " " + GetTimeString();
        }
        static public bool GetTimeFromLine(string filepath, string line, out DateTime _out_theTime)
        {
            int nYear = 0;
            int nMonth = 0;
            int nDay = 0;
            int nHour = 0;
            int nMin = 0;
            int nSec = 0;
            int nMSec = 0;

            // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
            int nStart = line.IndexOf("[") + 1;
            int nEnd = line.IndexOf("]");
            if (nStart < 0 || nEnd < 0 || nStart >= nEnd)
            {
                _out_theTime = new DateTime();
                _out_theTime = DateTime.Today;
                return false;
            }

            string strTime = line.Substring(nStart, nEnd - nStart);

            //22:59:57.984
            string[] arr = strTime.Split(':');
            nHour = Convert.ToInt32(arr[0]);
            nMin = Convert.ToInt32(arr[1]);

            string[] arr_sec = arr[2].Split('.');
            nSec = Convert.ToInt32(arr_sec[0]);
            nMSec = Convert.ToInt32(arr_sec[1]);

            // 파일 이름에서 날짜 얻기
            string strFileName = Path.GetFileName(filepath);

            // 파일 이름은 일반적으로 xxxx20180406.log 이런식으로붙으니까 .log 이전에 8자리만 빼온다
            int nLen = strFileName.Length;
            if (nLen <= 12)
            {
                _out_theTime = new DateTime();
                _out_theTime = DateTime.Today;
                return false;
            }

            int nTmp = -1;
            string strDate = "";
            for (int i = 2018; i < 2099; i++)
            {
                nTmp = strFileName.IndexOf(Convert.ToString(i));
                if (nTmp >= 0)
                    break;
            }
            if (nTmp >= 0)
            {
                strDate = strFileName.Substring(nTmp, 8);
            }

            // 파싱한 내용이 숫자가 아니면 문자기 때문에 날짜 정보가 아니므로
            // 파일 이름으로 날짜가 적혀있는게 아니다. 이럴경우 실패로 빼기 보다는 그냥 오늘 날짜로 만들어버리자.
            if (!GMethod.IsDigit_Int(strDate) || nTmp < 0)
            {
                nYear = DateTime.Today.Year;
                nMonth = DateTime.Today.Month;
                nDay = DateTime.Today.Day;
            }
            else
            {
                nYear = Convert.ToInt32(strDate.Substring(0, 4));
                nMonth = Convert.ToInt32(strDate.Substring(4, 2));
                nDay = Convert.ToInt32(strDate.Substring(6, 2));
            }



            _out_theTime = new DateTime(nYear, nMonth, nDay, nHour, nMin, nSec, nMSec);
            return true;

        }

    }


    /// <summary>
    /// shcho created 2018.04  MyListView
    /// 대용량 리스트 뷰를 화면에 보여줄 때 행이 몇만개가 넘어가면 너무 오래 걸림
    /// 미리 표를 만들어 놓고 나중에 들어오는 데이터는 값만 복사하는 형식으로 하면 빠르게 해결할 수 있다
    /// </summary>
    public class MyListView : ListView
    {
        public MyListView() { }
        private int m_nItemCnt = 0;

        public void CreatePool(string[] colHeader, int nRowCnt, ref ProgressBar prg)
        {
            BeginUpdate();
            this.Items.Clear();
            this.Columns.Clear();

            foreach (string col in colHeader)
                this.Columns.Add(col);

            if (prg != null)
            {
                prg.Minimum = 0;
                prg.Maximum = nRowCnt;
                prg.Value = 0;
            }

            for(int i=0; i<nRowCnt; i++)
            {
                ListViewItem lvi = new ListViewItem(new string[colHeader.Length]);
                this.Items.Add(lvi);
                if (prg != null)
                    prg.Increment(1);
            }
            m_nItemCnt = 0;
            
            //RedrawItems(0, nRowCnt-1, false);
        }
        public void ClearData()
        {
            foreach (ListViewItem lvi in this.Items)
            {
                foreach (ListViewItem.ListViewSubItem subItem in lvi.SubItems)
                    subItem.Text = "";
            }
        }

        public void AddItem(string[] row)
        {
            // pool 개수를 초과하면 새로 생성
            if (this.Items.Count >= m_nItemCnt)
            {
                ListViewItem lvi = new ListViewItem(row);
                this.Items.Add(lvi);
                m_nItemCnt++;
                return;
            }

            //새로 생성하지 않고 text만 바꾼다
            for (int i = 0; i < row.Length; i++)
                this.Items[m_nItemCnt].SubItems[i].Text = row[i];

            m_nItemCnt++;
        }
        public void AddItem(int nCol, string val)
        {
            // pool 개수를 초과하면 새로 생성
            if (this.Items.Count >= m_nItemCnt)
            {
                ListViewItem lvi = new ListViewItem();
                this.Items.Add(lvi);
                this.Items[m_nItemCnt].SubItems[nCol].Text = val;
                m_nItemCnt++;
                return;
            }

            //새로 생성하지 않고 text만 바꾼다
            this.Items[m_nItemCnt].SubItems[nCol].Text = val;
            m_nItemCnt++;
        }

    }
}
namespace OHT_TOOL
{
    public class MyOHT
    {
        static public string GetOhtNumer(string strPath)
        {
            string strLastDir = MyTool.GMethod.GetLastDir(strPath);

            string[] arr = strLastDir.Split('_');
            string strOhtNum = "";

            foreach (string elm in arr)
            {
                int nPos = elm.IndexOf('V');
                if (nPos < 0)
                    continue;

                int nNameLen = elm.Length;
                int nStart = nPos;
                int nSubCnt = nNameLen - nStart;

                if (nNameLen < nStart)
                    continue;

                strOhtNum = elm.Substring(nStart, nSubCnt);
            }
            return strOhtNum;
        }
        static public string GetDate(string strPath)
        {
            strPath = Path.GetFileName(strPath);
            // 파일 이름은 일반적으로 xxxx20180406.log 이런식으로붙으니까
            int nLen = strPath.Length;
            if (nLen <= 12)
            {
                return "";
            }

            int nTmp = -1;
            string strDate = "";
            for (int i = 2018; i < 2099; i++)
            {
                nTmp = strPath.IndexOf(Convert.ToString(i));
                if (nTmp >= 0)
                    break;
            }
            if (nTmp >= 0)
            {
                strDate = strPath.Substring(nTmp, 8);
            }

            // 파싱한 내용이 숫자가 아니면 문자기 때문에 날짜 정보가 아니므로
            // 파일 이름으로 날짜가 적혀있는게 아니다. 이럴경우 실패로 빼기 보다는 그냥 오늘 날짜로 만들어버리자.
            if (!MyTool.GMethod.IsDigit_Int(strDate) || nTmp < 0)
            {
                return "";
            }

            return strDate;
        }
    }
}