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
using Excel = Microsoft.Office.Interop.Excel;   // 이걸 쓰려면 참조에 Microsoft.Office.Interop.Excel 을 추가해줘야 한다
using System.Reflection;
using System.Net.NetworkInformation;
using OHT_TOOL;
using LogViewer;


/// <summary>
/// MyTools # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
/// 
/// 
/// Author: 조수환 #SHCHO  in Korea
/// 
/// ALL CODE ARE WRITTEN BY 조수환(SHCHO)
/// 
/// 2018.04 ~  2019.01
/// 
/// All classes and function method are opened for everyone
/// 
/// This is to help #DataProcess #Winform #Parsing #Network #Geomatry2D #Grid and everything as global static function.
/// 
/// Enjoy and Add anything you need for long term aspects.
/// 
/// 
///  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
/// </summary>
namespace MyTools
{
    class GMethod
    {
        // 문자열에서 특정 값 추출
        // ex)  txt = "Tolerance_mm=130.01,Max=1234" ,  name= "Tolerance_mm=" 일 경우
        // val 값 리턴 130.01
        // 만약 찾는 값이 없을 경우 return false
        public static bool GetThisValue(string txt, string name, ref double val)
        {
            int pos = txt.IndexOf(name);
            if (pos == -1)
            {
                return false;
            }
            string strVal = "";

            int nCnt = 0;
            bool bDot = false;

            for (int i = pos + name.Length; i < txt.Length; i++, nCnt++)
            {
                char ch = txt[i];
                if (ch < '0' || ch > '9')
                {
                    bool bItsHuho = ch == '-' || ch == '+';
                    if (ch == '.' && bDot == false)
                    {
                        bDot = true;    // dot가 한번까지는 올수 있다
                    }
                    else
                        break;      // 나머지는 허용되지 않음. 숫자와 관계없는 값
                }
                strVal += txt.ElementAt(i);
            }

            // 마지막 값이 .일 경우 제거
            // ex 120.  ==> 120
            if (strVal.Length >= 1)
            {
                if (strVal[strVal.Length - 1] == '.')
                {
                    strVal.Remove(strVal.Length - 1, 1);
                }
            }
            val = 0;
            // 개수가 없을 경우 return false
            if (strVal.Length == 0)
                return false;

            return Double.TryParse(strVal, out val);
        }

        // 문자열에서 특정 값 추출
        // ex)  txt = "ParamName=OHT_Detect_Area" ,  name= "ParamName=" 일 경우
        // val 값 리턴 OHT_Detect_Area
        // 만약 찾는 값이 없을 경우 return false
        public static bool GetThisValue(string txt, string name, out string val)
        {
            int pos = txt.IndexOf(name);
            val = "";
            string strVal = "";

            for (int i = pos + name.Length; i < txt.Length; i++)
            {
                bool bEndChar = txt[i] == ' ' || txt[i] == '#' || txt[i] == '/';
                if (bEndChar)
                    break;
                strVal += txt.ElementAt(i);
            }

            // 마지막 값이 .일 경우 제거
            // ex 120.  ==> 120
            if (strVal.Length >= 1)
            {
                if (strVal[strVal.Length - 1] == '.')
                {
                    strVal.Remove(strVal.Length - 1, 1);
                }
            }

            // 개수가 없을 경우 return false
            if (strVal.Length == 0)
                return false;

            val = strVal;
            return true;
        }

        public static void MoveForm_ParentCenter(Form form, Form parent)
        {
            Point ptParent = parent.Location;
            ptParent.X += parent.DisplayRectangle.Width / 2 - form.ClientRectangle.Width / 2;
            ptParent.Y += parent.DisplayRectangle.Height / 2 - form.ClientRectangle.Height / 2;

            //Rectangle screentRect = parent.RectangleToScreen(parent.DisplayRectangle);
            //Rectangle formRect = form.ClientRectangle;
            //Point screentPos = new Point(screentRect.Left, screentRect.Top);
            //screentPos.X += parent.DisplayRectangle.Width / 2;// - formRect.Width/2;
            //screentPos.Y += parent.DisplayRectangle.Height / 2;// - formRect.Height/2;
            //Point clientPos = form.PointToClient(screentPos);
            form.Location = ptParent;
        }



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

        // 문자열에 알파벳 혹은 숫자만 있는지 확인
        public static bool IsOnlyAZ09(string val)
        {
            for (int i = 0; i < val.Length; i++)
            {
                char sz = val[i];
                bool bLower = (sz >= 'a' && sz <= 'z');
                bool bUpper = (sz >= 'A' && sz <= 'Z');

                if (!bLower && !bUpper)
                    return false;
                else
                    continue;

            }
            return true;
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

        // 재귀호출로 하위폴더까지 모두 포함하는 파일 리스트를 얻음
        public static void GetAllFiles_inDir(string dir, string strContainText, ref List<string> list)
        {
            bool bNoFilter = String.IsNullOrEmpty(strContainText);

            if (bNoFilter)
                list.AddRange(Directory.GetFiles(dir));
            else
            {
                foreach (string strFile in Directory.GetFiles(dir))
                {
                    if (strFile.ToUpper().IndexOf(strContainText.ToUpper()) >= 0)
                        list.Add(strFile);
                }
            }

            string[] dirs = Directory.GetDirectories(dir);
            foreach (string aDir in dirs)
            {
                GetAllFiles_inDir(aDir, strContainText, ref list);  // 재귀호출
            }
        }

        // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        // 파일이름에 user2018101016 <- 이것까지 파싱해서 날짜만듬
        static public bool GetTimeFromLine(string filepath, string line, out DateTime _out_theTime)
        {
            SmallDateTime stm = new SmallDateTime();
            DateTime tm = new DateTime();
            bool bRet = false;

            bRet = GetTimeFromLine(filepath, line, ref stm);

            if (bRet == false)
            {
                _out_theTime = tm;
                return false;
            }

            _out_theTime = stm.GetDateTime();
            return true;

        }

        // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        // 파일이름에 user2018101016 <- 이것까지 파싱해서 날짜만듬
        // bDateTm10char 옵션  true : [2019010106] 형식으로 년월일시간 까지 있는경우 10자리가 됨.
        // 옵션 값이 false 일 경우 : [20190101] 형식으로 년월일 까지만(8자리) 가 있는 경우
        static public bool GetTimeFromLine(string filepath, string line, ref SmallDateTime _out_theTime, bool bDateTm10char = true)
        {
            try
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
                string strFileName = Path.GetFileNameWithoutExtension(filepath);
                string strDate = "";
                bool bDayCarry = false;     // 00시 로그의 경우 시작에 23시로 나오면 이전날짜로 -carry 시킨다
                                            // [2019010101] 년월일시간 의 경우
                if (bDateTm10char)
                {
                    if (strFileName.Length < 11)
                        return false;

                    int nPos_ = strFileName.Length - 10;
                    strDate = strFileName.Substring(nPos_, 8);
                    string hour_ = strFileName.Substring(nPos_ + 8, 2);
                    if (hour_ == "00")
                    {
                        if (nHour == 23)
                            bDayCarry = true;       // 특정날짜 00시 로그 시작줄에는 이전날 23시 로그부터 시작하는 경우가 있다 이때는 day carry 시켜서 이전날짜로 만들어줘야함
                    }
                }
                // [20190101] 년월일 의 경우
                else
                {
                    if (strFileName.Length < 9)
                        return false;

                    int nPos_ = strFileName.Length - 8;
                    strDate = strFileName.Substring(nPos_, 8);
                }

                nYear = Convert.ToInt32(strDate.Substring(0, 4));
                nMonth = Convert.ToInt32(strDate.Substring(4, 2));
                nDay = Convert.ToInt32(strDate.Substring(6, 2));


                _out_theTime.SetTime(nYear, nMonth, nDay, nHour, nMin, nSec, nMSec);
                if (bDayCarry)  // day carry 발행한 경우 하루 빼준다. nDay -1 하면 1일의 경우 0일이 되어버리기 때문에 달까지 이동되로고 DateTime 기본함수를 쓰자
                {
                    DateTime carryDay = _out_theTime.GetDateTime();
                    carryDay = carryDay.AddDays(-1);
                    _out_theTime.SetTime(carryDay);
                }
                return true;
            }
            catch (Exception)
            {

                return false;
            }

        }

        //// [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        //// 파일이름으로 날짜 파싱까지는 하지 않고 그냥 today 날짜로 만듬. 시간만 의미가 있다
        //static public bool GetTimeFromLine_asToday(string line, ref DateTime _out_theTime)
        //{
        //    try
        //    {


        //        int nHour = 0;
        //        int nMin = 0;
        //        int nSec = 0;
        //        int nMSec = 0;
        //        int nTick = Environment.TickCount;


        //        // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        //        int nStart = line.IndexOf("[") + 1;
        //        int nEnd = line.IndexOf("]");
        //        if (nStart < 0 || nEnd < 0 || nStart >= nEnd)
        //        {
        //            return false;
        //        }

        //        Trace.WriteLine(String.Format("Tick11 : {0}", Environment.TickCount - nTick));
        //        nTick = Environment.TickCount;

        //        string strTime = line.Substring(nStart, nEnd - nStart);

        //        Trace.WriteLine(String.Format("Tick12 : {0}", Environment.TickCount - nTick));
        //        nTick = Environment.TickCount;

        //        //22:59:57.984
        //        string[] arr = strTime.Split(':');
        //        nHour = Convert.ToInt32(arr[0]);
        //        nMin = Convert.ToInt32(arr[1]);

        //        string[] arr_sec = arr[2].Split('.');
        //        nSec = Convert.ToInt32(arr_sec[0]);
        //        nMSec = Convert.ToInt32(arr_sec[1]);

        //        Trace.WriteLine(String.Format("Tick13 : {0}", Environment.TickCount - nTick));
        //        nTick = Environment.TickCount;

        //        _out_theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, nHour, nMin, nSec, nMSec);

        //        Trace.WriteLine(String.Format("Tick14 : {0}", Environment.TickCount - nTick));


        //        return true;
        //    }
        //    catch (Exception)
        //    {

        //        return false;
        //    }

        //}
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

            for (int i = suspect.Count - 1; i >= 0; i--)  // 최신버전 먼저 찾기 위해서 거꾸로 검색
            {
                string path = suspect[i];
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
        public static int PingHost(string nameOrAddress, int nTimeout_ms = 3000)    // ping 시도. 실패시 -1, 성공했을 때 걸린 시간ms
        {
            Ping pinger = null;

            try
            {
                pinger = new Ping();
                PingReply reply = pinger.Send(nameOrAddress, nTimeout_ms);

                if (reply.Status == IPStatus.Success)
                    return (int)reply.RoundtripTime;
                else
                    return -1;

            }
            catch (PingException)
            {
                // Discard PingExceptions and return false;
            }
            finally
            {
                if (pinger != null)
                {
                    pinger.Dispose();
                }
            }

            return -1;
        }

        // 엑셀로 저장하기 위함
        // CSV처럼 콤마로 구분된 string list를 object 2차원 배열로 리턴한다.
        // 이렇게 하는 이유는 엑셀객체로 데이터를 쓸 때 한개씩 입력하는것은 너무 느리기 때문에
        // 한번에 범위를 지정하고 wrting 해야 resonable한 속도로 데이터를 쓸 수 있다.
        // 리턴형 2차원 배열은 [row, col] 순서이다
        /*ex
         * 학생부 전화번호     ==> 1개 cell 필요
         * 이름,전화번호,나이       ==>3개 cell 필요
         * 수환,01011112222,33    ==>3개 cell 필요
         * 이유,01033334444,25    ==>3개 cell 필요
         * 끝                            ==> 1개cell 필요
         * 
         * 이경우 col은 3개, row는 5개짜리 이차원 object cell에 데이터를 입력하여 리턴한다
         */
        public static object[,] StringListToObjectCells(List<string> stringList, out Point size, char sepa = ',')
        {
            // row는 stringList 개수
            // col은 콤마로 구분된 최대 개수
            // 모든 라인의 콤마 개수를 파악해야함

            int row = stringList.Count();
            int col = 1;
            foreach (var line in stringList)
            {

                int nTmp = line.Where(c => c == sepa).Count() + 1;
                if (col < nTmp)
                    col = nTmp;
            }

            size = new Point(0, 0);
            if (row == 0 || col == 0)
                return null;

            // 전체 이차원 배열 생성(cells)
            var cells = new object[row, col];
            size = new Point(col, row);     //x = col, y = row

            for (int nCurRow = 0; nCurRow < row; nCurRow++)
            {
                string[] arr = stringList[nCurRow].Split(sepa);

                for (int nCurCol = 0; nCurCol < arr.Count(); nCurCol++)
                {
                    cells[nCurRow, nCurCol] = arr[nCurCol];
                }
            }
            return cells;
        }

        /// <summary>
        /// 원격지의 dll, exe 파일 버전정보 가져오기
        /// </summary>

        public static string NetworkFileVersionRead(string path, string id, string pwd, bool bAddFileDate)
        {

            string[] arr = path.Split('\\');
            string ip = "";
            bool bTryAgain = false;
            foreach (string v in arr)
            {
                if (String.IsNullOrEmpty(v))
                    continue;

                ip = v;
                break;
            }

            if (ip.Split('.').Length != 4)  // ip addr validation
                return "";

            string strVersion = "";

            // OHT에 파일이 있는 File Exist를 먼저 해봄 ---> 네트웍 세션이 없다면 당연히 실패고, 이미 가지고 있다면 세션 연결시간 단축 가능
            // 이미 연결을 가지고 있는데 또 연결하면 exception 결리면서 3~4초 이상 소요되버림
            bool bConnected = false;
            try
            {
                bConnected = File.Exists(path);
            }
            catch { }


            try
            {
                if (bConnected == false)
                {// 네트워크 세션 얻기
                    using (new NetworkConnection(@"\\" + ip, new System.Net.NetworkCredential(id, pwd)))
                    {
                        FileVersionInfo myFileVersionInfo = FileVersionInfo.GetVersionInfo(path);
                        strVersion = myFileVersionInfo.FileVersion;
                        DateTime fileDate = File.GetLastWriteTime(path);
                        if (bAddFileDate)
                            strVersion += String.Format(" [{0:00}.{1:00}.{2:00}]", fileDate.Year, fileDate.Month, fileDate.Day);
                    }
                }
                else
                {
                    FileVersionInfo myFileVersionInfo = FileVersionInfo.GetVersionInfo(path);
                    strVersion = myFileVersionInfo.FileVersion;
                    DateTime fileDate = File.GetLastWriteTime(path);
                    if (bAddFileDate)
                        strVersion += String.Format(" [{0:00}.{1:00}.{2:00}]", fileDate.Year, fileDate.Month, fileDate.Day);
                }
            }
            catch (Win32Exception eeee)
            {
                // Win32Exception 1219 예외는 이미 remote 네트워크와 연결이 되어있어서 연결할 수 없다는 예외이다.
                // 그렇기 때문에 원래 의도한 동작을 해도 된다
                if (eeee.NativeErrorCode == 1219)
                {
                    bTryAgain = true;
                }
            }
            catch { }

            if (bTryAgain)
            {
                try
                {
                    FileVersionInfo myFileVersionInfo = FileVersionInfo.GetVersionInfo(path);
                    strVersion = myFileVersionInfo.FileVersion;
                    DateTime fileDate = File.GetLastWriteTime(path);
                    strVersion += String.Format(" , {0:00}.{1:00}.{2:00}", fileDate.Year, fileDate.Month, fileDate.Day);
                }
                catch { }
            }

            return strVersion;
        }

        /// <summary>
        /// 원격지의 특정 폴더에 파일 목록을 얻어내기 위해서 사용. *, ?와같은 와일드카드 사용가능
        /// </summary>
        /// <param name="folderPath"> 원격지 폴더 경로. ex: [\\192.168.66.1\Log] </param>
        /// <param name="cond">검색하고자 하는 파일. ex: [AMC_Recv20190105*.log]</param>
        /// <param name="opt">하위 디렉토리까지 검사 할지 옵션</param>
        /// <returns></returns>
        public static string[] GetNetworkFileList(string id, string pwd, string folderPath, string cond = "*.*", SearchOption opt = SearchOption.TopDirectoryOnly)
        {
            string[] files = new string[0];

            string[] arr = folderPath.Split('\\');
            string ip = "";
            foreach (string v in arr)
            {
                if (String.IsNullOrEmpty(v))
                    continue;

                ip = v;
                break;
            }

            if (ip.Split('.').Length != 4)  // ip addr validation
                return files;


            //if (GMethod.PingHost(ip, 1000) < 0)
            //	return files;

            // 네트워크 경로 Exist를 먼저 해봄 ---> 네트웍 세션이 없다면 당연히 실패고, 이미 가지고 있다면 세션 연결시간 단축 가능
            // 이미 연결을 가지고 있는데 또 연결하면 exception 결리면서 3~4초 이상 소요되버림. 엄청난 낭비를 해소할수 있다
            bool bConnected = false;
            try
            {
                bConnected = Directory.Exists(folderPath);
            }
            catch { }

            //bool bRet = false;

            bool bTryAgain = false;
            try
            {
                if (bConnected == false)    // 세션 없는 경우만 시도한다. 시간이 많이 걸리는 작업이기 때문에
                {   // 네트워크 세션 얻기
                    using (new NetworkConnection(@"\\" + ip, new System.Net.NetworkCredential(id, pwd)))
                    {
                        if (Directory.Exists(folderPath))
                            files = Directory.GetFiles(folderPath, cond, opt);
                    }
                }
                else
                {
                    if (Directory.Exists(folderPath))
                        files = Directory.GetFiles(folderPath, cond, opt);
                }
            }
            catch (Win32Exception eeee)
            {
                // Win32Exception 1219 예외는 이미 remote 네트워크와 연결이 되어있어서 연결할 수 없다는 예외이다.
                // 그렇기 때문에 원래 의도한 동작을 해도 된다
                if (eeee.NativeErrorCode == 1219)
                {
                    bTryAgain = true;
                }
            }
            catch { }

            if (bTryAgain)
            {
                try
                {
                    if (Directory.Exists(folderPath))
                        files = Directory.GetFiles(folderPath, cond, opt);
                }
                catch
                {

                }
            }
            return files;
        }

        public static bool FileCopy(string src, string target, bool bMakeTargetDir)
        {
            bool bRet = false;
            try
            {
                string targetFolder = Path.GetDirectoryName(target);    // OHT 호기 이름의 폴더 생성

                if (bMakeTargetDir)
                {
                    if (Directory.Exists(targetFolder) == false)
                        Directory.CreateDirectory(targetFolder);
                }
                if (Directory.Exists(targetFolder) == false)
                    return false;

                File.Copy(src, target, true);
                bRet = new FileInfo(target).Length == new FileInfo(src).Length; // 복사하고 난후 원본과 사이즈 대조해서 확인함
                if (bRet == false)
                    File.Delete(target);    // 파일 크기가 다르다는건 네트워크 복사할때 문제가 생긴것
            }
            catch// ()
            {
            }
            return bRet;
        }
        public static string FileVersionRead(string path, bool bAddFileDate)
        {
            string strVersion = "";
            try
            {

                if (File.Exists(path) == false)
                    return strVersion;

                FileVersionInfo myFileVersionInfo = FileVersionInfo.GetVersionInfo(path);
                strVersion = myFileVersionInfo.FileVersion;
                DateTime fileDate = File.GetLastWriteTime(path);
                if (bAddFileDate)
                    strVersion += String.Format(" [{0:00}.{1:00}.{2:00}]", fileDate.Year, fileDate.Month, fileDate.Day);

            }
            catch// (Exception)
            {
            }

            return strVersion;
        }

        public static string[] GetFileList(string folderPath, string cond = "*.*", SearchOption opt = SearchOption.TopDirectoryOnly)
        {
            string[] files = null;

            try
            {
                if (Directory.Exists(folderPath))
                    files = Directory.GetFiles(folderPath, cond, opt);
            }
            catch// (Exception)
            {
            }

            return files;
        }
        /// <summary>
        /// 원격지로 파일 복사할 때 사용. 여러가지 네트워크 예외에 대한 처리 포함되어 있음
        /// </summary>
        public static bool NetworkFileCopy(bool bFromLocal, string src, string target, string id, string pwd, bool bMakeTargetDir)
        {
            string[] arr = bFromLocal ? target.Split('\\') : src.Split('\\');
            string ip = "";
            foreach (string v in arr)
            {
                if (String.IsNullOrEmpty(v))
                    continue;

                ip = v;
                break;
            }

            if (ip.Split('.').Length != 4)  // ip addr validation
                return false;


            // 네트워크 파일 File Exist를 먼저 해봄 ---> 네트웍 세션이 없다면 당연히 실패고, 이미 가지고 있다면 세션 연결시간 단축 가능
            // 이미 연결을 가지고 있는데 또 연결하면 exception 결리면서 3~4초 이상 소요되버림
            bool bConnected = false;
            try
            {
                bConnected = bFromLocal ? File.Exists(target) : File.Exists(src);
            }
            catch { }

            bool bRet = false;

            bool bTryAgain = false;
            string targetFolder = Path.GetDirectoryName(target);    // OHT 호기 이름의 폴더 생성
            try
            {
                if (bConnected == false)    // 세션 없는 경우만 시도한다. 시간이 많이 걸리는 작업이기 때문에
                {   // 네트워크 세션 얻기
                    using (new NetworkConnection(@"\\" + ip, new System.Net.NetworkCredential(id, pwd)))
                    {
                        if (bMakeTargetDir)
                        {
                            if (Directory.Exists(targetFolder) == false)
                                Directory.CreateDirectory(targetFolder);
                        }
                        if (Directory.Exists(targetFolder) == false)
                            return false;

                        File.Copy(src, target, true);
                        bRet = new FileInfo(target).Length == new FileInfo(src).Length; // 복사하고 난후 원본과 사이즈 대조해서 확인함
                        if (bRet == false)
                        {
                            File.Delete(target);    // 파일 크기가 다르다는건 네트워크 복사할때 문제가 생긴것
                        }
                    }
                }
                else
                {
                    if (bMakeTargetDir)
                    {
                        if (Directory.Exists(targetFolder) == false)
                            Directory.CreateDirectory(targetFolder);
                    }
                    if (Directory.Exists(targetFolder) == false)
                        return false;

                    File.Copy(src, target, true);
                    bRet = new FileInfo(target).Length == new FileInfo(src).Length; // 복사하고 난후 원본과 사이즈 대조해서 확인함
                    if (bRet == false)
                    {
                        File.Delete(target);    // 파일 크기가 다르다는건 네트워크 복사할때 문제가 생긴것
                    }
                }
            }
            catch (Win32Exception eeee)
            {
                // Win32Exception 1219 예외는 이미 remote 네트워크와 연결이 되어있어서 연결할 수 없다는 예외이다.
                // 그렇기 때문에 원래 의도한 동작을 해도 된다
                if (eeee.NativeErrorCode == 1219)
                {
                    bTryAgain = true;
                }
            }
            catch { }

            if (bTryAgain)
            {
                try
                {
                    if (bMakeTargetDir)
                    {
                        if (Directory.Exists(targetFolder) == false)
                            Directory.CreateDirectory(targetFolder);
                    }
                    if (Directory.Exists(targetFolder) == false)
                        return false;

                    File.Copy(src, target, true);
                    bRet = new FileInfo(target).Length == new FileInfo(src).Length; // 복사하고 난후 원본과 사이즈 대조해서 확인함
                    if (bRet == false)
                    {
                        File.Delete(target);    // 파일 크기가 다르다는건 네트워크 복사할때 문제가 생긴것
                    }
                }
                catch
                {

                }
            }
            return bRet;
        }

        // FindLogTimeLineNo
        // 특정 로그파일 전체 라인에서 특정 시간대의 위치를 찾고 싶을때
        // 검색을 line by line으로 하지 않고 분할정복 알고리즘으로 검색한다
        // 10만 line 이상의 로그도 15안에 원하는 시간대 라인 번호를 찾을 수 있다.
        public static int FindLogTimeLineNo(string[] lines, DateTime theTime, out double dGapSec)
        {
            int nLineCnt = lines.Length;
            if (nLineCnt < 3)
            {
                dGapSec = 0;
                return -1;
            }

            // 날짜만 today로 바꿈
            theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, theTime.Hour, theTime.Minute, theTime.Second, theTime.Millisecond);

            DateTime tmLine = new DateTime();

            int nFoundLine = -1;
            do
            {
                DateTime tmFirstLine = new DateTime(); ;
                // 시작줄 확인                
                if (GetTimeFromLine_asToday(lines[0], ref tmFirstLine) == false)
                {
                    if (GetTimeFromLine_asToday(lines[1], ref tmFirstLine) == false)
                        break;
                }

                // 00 시대의 로그 로딩시 전날 밤12:59~당일날 0시59분까지 로그가 쌓이기 때문에 시간만 놓고 보면 <미래~과거> 역전 현상이 생긴다
                // 그래서 이부분은 예외로 빼서 간주해야 한다
                if (tmFirstLine.Hour == 23 && theTime.Hour == 0)
                    tmFirstLine = tmFirstLine.AddDays(-1);  // 시작일을 하루 빼주면 역전현상을 막을 수 있다

                if (theTime < tmFirstLine)   // 첫줄은 예외로 검사해봄
                {
                    nFoundLine = 0;
                    break;
                }

                // 마지막줄 확인
                DateTime tmLastLine = new DateTime(); ;
                int nTmp = nLineCnt - 1;

                // 첫줄라인, 마지막줄 라인은 시작하자마자 먼저 체크해본다
                if (GetTimeFromLine_asToday(lines[nTmp], ref tmLastLine) == false)
                {
                    nTmp--;

                    if (GetTimeFromLine_asToday(lines[nTmp], ref tmLastLine) == false)
                        break;
                }

                if (theTime > tmLastLine)   // 마지막줄 예외로 검사해봄
                {
                    nFoundLine = nTmp;
                    break;
                }

            } while (false);

            // ##############################################
            // 분할정복 알고리즘 사용 Divide And Conqueror.
            // 분할정복을 사용하면 10만줄의 라인이 있어도, 원하는 시간을 최악의 경우에도 15번 이내의 검색으로 찾을 수 있다
            //
            int nPrevDivIdx = nLineCnt;
            int nDivIdx = nLineCnt / 2;
            int nCnt = 0;

            while (nFoundLine < 0)
            {
                if (GetTimeFromLine_asToday(lines[nDivIdx], ref tmLine) == false)
                    break;

                // 00 시대의 로그 로딩시 전날 밤12:59~당일날 0시59분까지 로그가 쌓이기 때문에 시간만 놓고 보면 <미래~과거> 역전 현상이 생긴다
                // 그래서 이부분은 예외로 빼서 간주해야 한다
                if (tmLine.Hour == 23 && theTime.Hour == 0)
                    tmLine = tmLine.AddDays(-1);  // 시작일을 하루 빼주면 역전현상을 막을 수 있다


                // Divied 된 인덱스가 5일경우 6번과 비교해서 찾았는지 확인한다
                if (nDivIdx < nLineCnt - 1)
                {
                    DateTime tm_p1 = new DateTime();

                    if (GetTimeFromLine_asToday(lines[nDivIdx + 1], ref tm_p1) == false)
                        break;

                    bool bHubo = tm_p1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_p1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx + 1;
                        break;
                    }
                }

                // Divied 된 인덱스가 5일경우 4번과 비교해서 찾았는지 확인한다
                if (nDivIdx > 0)
                {
                    DateTime tm_s1 = new DateTime();
                    if (GetTimeFromLine_asToday(lines[nDivIdx - 1], ref tm_s1) == false)
                        break;

                    bool bHubo = tm_s1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_s1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx - 1;
                        break;
                    }
                }

                // Divided 된 인덱스와 인접하는 값들이 아니라면 다시 Divide한다
                int nGap = Math.Abs(nDivIdx - nPrevDivIdx);
                if (nGap <= 1)
                    break;

                // 분할정복 알고리즘. 반으로 나눈다
                int nDiv = nGap / 2;
                nPrevDivIdx = nDivIdx;
                nDivIdx = theTime > tmLine ? (nDivIdx + nDiv) : (nDivIdx - nDiv);
                nCnt++;
            }

            // 찾기 실패
            if (nFoundLine < 0)
            {
                dGapSec = 0;
                return -1;
            }

            DateTime tmFound = new DateTime();
            if (GetTimeFromLine_asToday(lines[nFoundLine], ref tmFound) == false)
            {
                dGapSec = 0;
                return -1;
            }


            TimeSpan span = theTime - tmFound;
            dGapSec = span.TotalSeconds;
            return nFoundLine;
        }


        // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        // 파일이름으로 날짜 파싱까지는 하지 않고 그냥 today 날짜로 만듬. 시간만 의미가 있다
        public static bool GetTimeFromLine_asToday(string __line, ref DateTime _out_theTime)
        {
            if (String.IsNullOrEmpty(__line)) return false;
            if (__line.Length < 8) return false;

            // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
            int __nHour = 0, __nMin = 0, _nSec = 0, __nMSec = 0;
            int __nStart = __line.IndexOf("[") + 1;
            int __nEnd = __line.IndexOf("]");
            if (__nStart < 0 || __nEnd < 0 || __nEnd - __nStart < 8 || __nEnd > 15) return false;
            string __strTime = __line.Substring(__nStart, __nEnd - __nStart);

            string[] __arr = __strTime.Split(':');
            if (__arr.Length == 3)
            {
                if (__arr[2].Length < 3) return false;
                string[] __arr_sec = __arr[2].Split('.');
                if (__arr_sec.Length != 2) return false;

                if (Int32.TryParse(__arr[0], out __nHour) == false || Int32.TryParse(__arr[1], out __nMin) == false) return false;
                if (MyRange.IsInRange(0, 25, __nHour) == false || MyRange.IsInRange(0, 60, __nMin) == false) return false;
                if (Int32.TryParse(__arr_sec[0], out _nSec) == false) return false;
                if (Int32.TryParse(__arr_sec[1], out __nMSec) == false) return false;
                if (MyRange.IsInRange(0, 60, _nSec) == false || MyRange.IsInRange(0, 1000, __nMSec) == false) return false;
            }
            else if (__arr.Length == 4)
            {
                if (Int32.TryParse(__arr[0], out __nHour) == false || Int32.TryParse(__arr[1], out __nMin) == false) return false;
                if (Int32.TryParse(__arr[2], out _nSec) == false || Int32.TryParse(__arr[3], out __nMSec) == false) return false;
                if (MyRange.IsInRange(0, 25, __nHour) == false || MyRange.IsInRange(0, 60, __nMin) == false) return false;
                if (MyRange.IsInRange(0, 60, _nSec) == false || MyRange.IsInRange(0, 1000, __nMSec) == false) return false;
            }
            else return false;

            _out_theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, __nHour, __nMin, _nSec, __nMSec);
            return true;


        }
    }


    public class GridHelper
    {
        const string CMNT_LINE_START = "<%NEWLINE%>  ";

        public static void SaveTableContents(string path, DataGridView grid)
        {

            //string strText = "";
            //int nCnt = 0;
            //for(int nRow = 0; nRow< grid.RowCount; nRow++)
            //{
            //    DataGridViewRow row = grid.Rows[nRow];

            //    for(int nCol = 0; nCol < grid.ColumnCount; nCol++)
            //    {
            //        string str = (string)row.Cells[nCol].Value;
            //        strText += CMNT_LINE_START + CMNT_KEY + str + "\n";
            //    }
            //    nCnt++;
            //}

            //if (File.Exists(path))
            //    File.Delete(path);

            //// 내용 저장
            //File.WriteAllText(path, strText, Encoding.Default);
        }
        public static void SaveGridInfo(DataGridView refGrid, string iniPath, string gridName)
        {
            try
            {
                string section = gridName;
                if (section == "")
                    section = refGrid.Name;

                IniMan ini = new IniMan();
                ini.Path = iniPath;

                Font font = refGrid.DefaultCellStyle.Font;
                float fontSize = font.Size;
                int fontStype = (int)font.Style;

                ini.Write(section, "FontSize", Convert.ToString(fontSize));
                ini.Write(section, "FontStyle", Convert.ToString(fontStype));
                ini.Write(section, "RowTemplate.Height", Convert.ToString(refGrid.RowTemplate.Height));
                for (int i = 0; i < refGrid.Columns.Count; i++)
                {
                    ini.Write(section, String.Format("Col_Width_{0}", i), Convert.ToString(refGrid.Columns[i].Width));
                }
            }
            catch (Exception)
            {
                //exp.Message
                //throw;
            }
        }

        // 그리드의 폰트정보, 셀사이즈 등을 INI에 로딩
        // static 으로 모든 그리드가 쉽게 접근하기 위함
        public static void LoadGridInfo(DataGridView refGrid, string iniPath, string gridName)
        {
            IniMan ini = new IniMan();

            string section = gridName;
            if (section == "")
                section = refGrid.Name;

            ini.Path = iniPath;
            try
            {

                Font font = refGrid.DefaultCellStyle.Font;
                float OrgfontSize = font.Size;
                int OrgfontStype = (int)font.Style;

                double dFontSize = Convert.ToDouble(ini.Read(section, "FontSize", Convert.ToString(OrgfontSize)));
                int nFontStyle = Convert.ToInt32(ini.Read(section, "FontStyle", Convert.ToString(OrgfontStype)));
                Font newFont = new Font("맑은 고딕", (float)dFontSize, (FontStyle)nFontStyle);
                refGrid.DefaultCellStyle.Font = newFont;

                int Height = Convert.ToInt32(ini.Read(section, "RowTemplate.Height", Convert.ToString(refGrid.RowTemplate.Height)));
                for (int i = 0; i < refGrid.Columns.Count; i++)
                {
                    int nWidth = Convert.ToInt32(ini.Read(section, String.Format("Col_Width_{0}", i), Convert.ToString(refGrid.Columns[i].Width)));
                    refGrid.Columns[i].Width = nWidth;
                }
            }
            catch (Exception)
            {
                //Debug.Assert(false, exp.Message);
            }
        }

        public static int LoadInputNumber(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int InputCount = Convert.ToInt32(ini.Read(section, "InputCount", "96"));

            return InputCount;
        }
        public static int LoadOutputNumber(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int OutputCount = Convert.ToInt32(ini.Read(section, "OutputCount", "96"));

            return OutputCount;
        }
        public static int[] LoadUBGSensorInput(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int[] SensorArray = new int[3];
            for (int i = 1; i <= 3; i++)
            {
                SensorArray[i - 1] = Convert.ToInt32(ini.Read(section, String.Format("UBGInput{0}", i), "0"));
            }
            return SensorArray;
        }
        public static int[] LoadOBSSensorInput(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int[] SensorArray = new int[3];
            for (int i = 1; i <= 3; i++)
            {
                SensorArray[i - 1] = Convert.ToInt32(ini.Read(section, String.Format("OBSInput{0}", i), "0"));
            }
            return SensorArray;
        }

        public static int[] LoadUBSSensorOutput(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int[] SensorArray = new int[4];
            for (int i = 1; i <= 4; i++)
            {
                SensorArray[i - 1] = Convert.ToInt32(ini.Read(section, String.Format("UBGOutput{0}", i), "0"));
            }
            return SensorArray;
        }
        public static int[] LoadOBSSensorOutput(string iniPath, string gridName)
        {
            IniMan ini = new IniMan();
            string section = gridName;

            ini.Path = iniPath;
            int[] SensorArray = new int[4];
            for (int i = 1; i <= 4; i++)
            {
                SensorArray[i - 1] = Convert.ToInt32(ini.Read(section, String.Format("OBSOutput{0}", i), "0"));
            }
            return SensorArray;
        }
        public static string LoadIOInfo(eAmcRcvIdx idx, string iniPath, string gridName)
        {
            IniMan ini = new IniMan();

            string strEnumName = idx.ToString();
            string section = gridName;
            string ret = "";

            ini.Path = iniPath;

            try
            {
                ret = ini.Read(section, strEnumName, "0");

            }
            catch (Exception)
            {
                //Debug.Assert(false, exp.Message);
            }

            return ret;
        }

        // Column Width 를 INI에 자동 저장
        public static void SaveColWidth(Form form, DataGridView grid, string iniPath)
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
            catch (Exception e) { Debug.Assert(false, e.Message); }
        }

        // Column Width를 INI에서 가져와서 세팅함
        public static void LoadColWidth(Form form, DataGridView grid, string iniPath)
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
            catch (Exception e) { Debug.Assert(false, e.Message); }
        }

        public static void ZoomCellSize_Height(DataGridView grid, bool bZoomIn)
        {
            try
            {
                if (grid.RowTemplate.Height > 1 && !bZoomIn)  // 축소
                    grid.RowTemplate.Height -= 1;

                if (grid.RowTemplate.Height < 100 && bZoomIn) //확대
                    grid.RowTemplate.Height += 1;

                foreach (DataGridViewRow row in grid.Rows)
                {
                    row.Height = grid.RowTemplate.Height;
                }

            }
            catch (Exception e) { Debug.Assert(false, e.Message); }
        }

        public static void ZoomCellSize_Width(DataGridView grid, bool bZoomIn, bool bSelectedCellOnly)
        {
            try
            {
                if (bSelectedCellOnly)
                {
                    if (grid.SelectedCells.Count == 0)
                        return;

                    int nCol = grid.SelectedCells[0].ColumnIndex;
                    if (bZoomIn)
                        grid.Columns[nCol].Width += 1;
                    else
                        grid.Columns[nCol].Width -= 1;

                    return;
                }

                for (int i = 0; i < grid.Columns.Count; i++)
                {
                    if (bZoomIn)
                        grid.Columns[i].Width += 1;
                    else
                        grid.Columns[i].Width -= 1;
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }
        // 글자 크기 조정
        public static void FontZoom(DataGridView grid, bool bZoomIn, float dWeight = (float)0.5)
        {
            try
            {
                Font font = grid.DefaultCellStyle.Font;
                float nNewSize = font.Size + (bZoomIn ? +dWeight : -dWeight);
                Font newFont = new Font(font.Name, nNewSize, font.Style);
                grid.DefaultCellStyle.Font = newFont;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
        }

        // 그리드 틀고정
        public static void FixCol(DataGridView grid, int nCol, int nRow, bool bClearFix = false)
        {
            try
            {

                foreach (DataGridViewColumn col in grid.Columns)
                    col.Frozen = false;
                foreach (DataGridViewRow row in grid.Rows)
                    row.Frozen = false;

                if (bClearFix)
                    return;

                if (nCol >= grid.ColumnCount || nRow >= grid.ColumnCount)
                    return;

                if (nCol >= 0)
                    grid.Columns[nCol].Frozen = true;
                if (nRow >= 0)
                    grid.Rows[nRow].Frozen = true;

            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
        }

        // 현재 틀고정
        public static void FixCurrentCol(DataGridView grid, bool bClearFix = false)
        {
            try
            {
                if (bClearFix)
                {
                    foreach (DataGridViewColumn col in grid.Columns)
                        col.Frozen = false;
                    return;
                }
                if (grid.SelectedCells.Count == 0)
                    return;

                foreach (DataGridViewColumn col in grid.Columns)
                    col.Frozen = false;

                int nCol = grid.SelectedCells[0].ColumnIndex;
                grid.Columns[nCol].Frozen = true;

            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
        }
        private static void GridKeyDown(DataGridView grid, KeyEventArgs e)
        {
            try
            {
                do
                {
                    // Ctrl + Space : 열선택 처리
                    if (e.KeyCode == Keys.Space && e.Control)
                    {
                        if (grid.SelectedCells.Count <= 0)
                            break;

                        int nCurSelectedCol = grid.SelectedCells[0].ColumnIndex;

                        int nPrevCol = nCurSelectedCol;
                        bool bDiffCol = false;
                        bool bSelectAdd = true; // 선택된 셀이 여러개일 때 추가로 선택할지 여부

                        List<int> cols = new List<int>();

                        // 다른셀하고 같이 선택되었는지 확인
                        foreach (DataGridViewCell cell in grid.SelectedCells)
                        {
                            if (cell.ColumnIndex != nPrevCol)
                                bDiffCol = true;

                            // 중복되지 않게 선택된 col 번호만 list에 추가
                            if (Array.IndexOf(cols.ToArray(), cell.ColumnIndex) < 0)
                                cols.Add(cell.ColumnIndex);
                        }
                        if (bDiffCol)
                        {
                            if (DialogResult.Yes != MessageBox.Show("다른 셀과 함께 선택 하시겠습니까?", "열 선택", MessageBoxButtons.YesNo, MessageBoxIcon.Question))
                            {
                                bSelectAdd = false;
                            }
                        }

                        SelectCulmns(grid, nCurSelectedCol, bSelectAdd);
                        break;
                    }
                }
                while (false);
            }
            catch (Exception exp)
            {
                Debug.Assert(false, exp.Message);
                //throw;
            }
        }

        // 특정 col 선택
        public static void SelectCulmns(DataGridView grid, int col, bool bSelectAdd)
        {
            try
            {
                if (grid.SelectedCells.Count <= 0)
                    return;

                if (!bSelectAdd)
                    grid.ClearSelection();

                foreach (DataGridViewRow row in grid.Rows)
                {
                    row.Cells[col].Selected = true;
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
        }
        // 선택된 여러개 셀 중에서 각가의 col 인덱스만 얻어옴. 중복없이
        public static List<int> GetSelectedColumns(DataGridView grid)
        {
            List<int> cols = new List<int>();

            try
            {
                if (grid.SelectedCells.Count <= 0)
                    return cols;

                int nCurSelectedCol = grid.SelectedCells[0].ColumnIndex;
                int nPrevCol = nCurSelectedCol;

                // 다른셀하고 같이 선택되었는지 확인
                foreach (DataGridViewCell cell in grid.SelectedCells)
                {
                    // 중복되지 않게 선택된 col 번호만 list에 추가
                    if (Array.IndexOf(cols.ToArray(), cell.ColumnIndex) < 0)
                        cols.Add(cell.ColumnIndex);
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
            return cols;
        }

        // 선택된 여러개 셀 중에서 각각의 row 인덱스만 얻어옴. 중복없이
        public static List<int> GetSelectedRows(DataGridView grid)
        {
            List<int> rows = new List<int>();

            try
            {
                if (grid.SelectedCells.Count <= 0)
                    return rows;

                int nCurSelectedRow = grid.SelectedCells[0].RowIndex;
                int nPrevRow = nCurSelectedRow;

                // 다른셀하고 같이 선택되었는지 확인
                foreach (DataGridViewCell cell in grid.SelectedCells)
                {
                    // 중복되지 않게 선택된 col 번호만 list에 추가
                    if (Array.IndexOf(rows.ToArray(), cell.RowIndex) < 0)
                        rows.Add(cell.RowIndex);
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
            return rows;
        }


        // CSV 로 엑셀 저장
        public static void ExportExcel_CSV(DataGridView grid, string path, bool bSelectedOnly, int nTimeCol = -1, int nSkipCol = -1, int nSkipCol2 = -1, int nSkipRow = -1)
        {
            string contents = "";

            List<int> selectedColArr = new List<int>();
            List<int> selectedRowArr = new List<int>();

            if (bSelectedOnly)
            {
                List<Point> selectedCol = new List<Point>();
                Point NullPt = new Point();
                foreach (DataGridViewCell cell in grid.SelectedCells)
                {
                    if (selectedCol.Find(o => o == new Point(cell.ColumnIndex, -1)) == NullPt)
                    {
                        selectedCol.Add(new Point(cell.ColumnIndex, -1));
                    }
                }

                foreach (Point pt in selectedCol)
                    selectedColArr.Add(pt.X);

                selectedColArr.Sort();

                List<Point> selectedRow = new List<Point>();
                foreach (DataGridViewCell cell in grid.SelectedCells)
                {
                    if (selectedRow.Find(o => o == new Point(cell.RowIndex, -1)) == NullPt)
                    {
                        selectedRow.Add(new Point(cell.RowIndex, -1));
                    }
                }

                foreach (Point pt in selectedRow)
                    selectedRowArr.Add(pt.X);

                selectedRowArr.Sort();


                //// Time Col
                //string str = grid.Columns[nTimeCol].Name;
                //str = str.Replace('\n', '.');
                //contents += str + ",";

                //// Selected Col
                //for (int n = 0; n < selectedColArr.Count; n++)
                //{
                //    int nIdx = selectedColArr[n];
                //    str = grid.Columns[nIdx].Name;
                //    str = str.Replace('\n', '.');
                //    contents += str;

                //    if (n != selectedColArr.Count - 1)
                //        contents += ",";
                //}
            }

            // 헤더추가
            for (int nHead = 0; nHead < grid.ColumnCount; nHead++)
            {
                if (nHead == nSkipCol || nHead == nSkipCol2 || grid.Columns[nHead].Visible == false)
                    continue;

                string str = String.Format("{0}", grid.Columns[nHead].Name);    // 굳이 이렇게 한 이유는... 이렇게 안하면 실제 Grid Header의 text 내용이 바뀐다. 포인터로 연결되나봄
                str = str.Replace('\n', '.');
                contents += str;

                if (nHead != grid.ColumnCount - 1)
                    contents += ",";
            }

            contents += "\r\n";
            System.IO.File.WriteAllText(path, contents);

            List<string> contlist = new List<string>();
            // 내용 추가
            for (int row = 0; row < grid.RowCount; row++)
            {
                if (row == nSkipRow)
                    continue;
                string line = "";
                for (int col = 0; col < grid.ColumnCount; col++)
                {
                    DataGridViewCell cell = grid[col, row];
                    string strCellVal = Convert.ToString(cell.Value);
                    if (bSelectedOnly)
                    {
                        bool bTimeCol = col == nTimeCol;

                        if (col != 0 && bTimeCol == false && cell.Selected == false)
                        {
                            line += ",";
                            continue;
                        }

                        // 선택되지 않은 Row
                        if (selectedRowArr.FindAll(o => o == row).Count == 0)
                            continue;
                    }

                    if (col == nSkipCol || col == nSkipCol2 || grid.Columns[col].Visible == false)
                        continue;

                    if (col == nTimeCol)
                    {
                        string strTmp = strCellVal;
                        strTmp = strTmp.Replace(':', '_');
                        //strTmp = strTmp.Replace(".", " . ");
                        line += strTmp;
                    }
                    else
                        line += cell.Value;
                    if (col != grid.ColumnCount - 1)
                        line += ",";
                }

                contlist.Add(line);
            }

            string strSum = contents;
            foreach (string line in contlist)
            {
                string tmpLine = line.Replace(",", "");
                if (tmpLine.Length < 1)
                    continue;

                strSum += line + "\r\n";
            }
            System.IO.File.WriteAllText(path, strSum);

            // 엑셀 실행파일 찾아서 실행
            string excelPath = GMethod.FindExcelExePath();

            // 엑셀 없는 PC면 그냥 탐색기에서 선택한 채로 열어줌
            if (String.IsNullOrEmpty(excelPath))
            {
                string arg = "/select, \"" + path + "\"";
                System.Diagnostics.Process.Start("explorer.exe", arg);
            }
            else
            {
                System.Diagnostics.Process.Start(excelPath, path);
            }

        }

        // 엑셀의 열 번호는 AA, AB, BD 이런식이다. 이걸 쉽게 얻어오기 위함
        // 그리고 행번호까지 합쳐지면 A1, AB10 이런식이됨
        public static string GetExcelRangeName(int nCol, int nRow)
        {
            int x = nCol / 26;
            int y = nCol % 26;

            if (x == 0)
            {
                string str = String.Format("{0}{1}", 'A' + nCol, nRow + 1);
                return str;
            }
            else
            {
                string strA = String.Format("{0}", (char)('A' + x));
                string strB = String.Format("{0}{1}", (char)('A' + y), nRow + 1);
                return strA + strB;
            }
        }
        public static void ExportExcel(DataGridView grid, bool bSelectedOnly, bool bHeaderAdd, string sheetName)
        {
            Excel.Application excelApp;
            #region 엑셀 객체 생성
            try
            {
                excelApp = new Excel.Application();
            }
            catch (Exception)
            {
                MessageBox.Show("엑셀 객체를 생성할 수 없습니다!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            #endregion
            Excel._Workbook excelWorkbook;
            Excel._Worksheet excelSheet;
            Excel.Range excelRange;

            try
            {
                excelApp.ScreenUpdating = false;    // 이거 안하면 느려짐
                //excelApp.Calculation = Excel.XlCalculation.xlCalculationManual;
                //excelApp.EnableEvents = false;      //Prevent Excel events
                //excelApp.Interactive = false;       //Prevent user clicks and keystrokes

                excelWorkbook = (Excel._Workbook)(excelApp.Workbooks.Add(Missing.Value));
                excelSheet = (Excel._Worksheet)excelWorkbook.ActiveSheet;
                excelSheet.Name = sheetName;

                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                // 전체 그리드 모두 Export 
                if (!bSelectedOnly)
                {


                    // 헤더 추가한다
                    if (bHeaderAdd)
                    {
                        // 선택된 셀들의 헤더만 헤더 name 기록
                        for (int i = 1; i <= grid.Columns.Count; i++)
                        {
                            DataGridViewColumn col = grid.Columns[i - 1];
                            excelSheet.Cells[1, i] = col.Name;  //기록!
                            Excel.Range r = excelSheet.Cells[1, i];
                            //Color clr = col.DefaultCellStyle.BackColor;     //System.Drawing.Color.Red
                            //r.Interior.Color = System.Drawing.ColorTranslator.ToOle(clr);
                            //szCol++;
                        }

                        string strColRange = GetExcelRangeName(grid.Columns.Count - 1, 0);
                        excelSheet.get_Range("A1", strColRange).Font.Bold = true;
                        excelSheet.get_Range("A1", strColRange).Font.Size = 13;
                        excelSheet.get_Range("A1", strColRange).Font.Name = grid.Font.Name;
                        excelSheet.get_Range("A1", strColRange).VerticalAlignment = Excel.XlVAlign.xlVAlignCenter;

                        for (int c = 0; c < grid.ColumnCount; c++)
                        {
                            for (int r = 0; r < grid.RowCount; r++)
                            {
                                excelSheet.Cells[r + 2, c + 1] = grid[c, r].Value;
                                //Excel.Range rng = excelSheet.Cells[r + 1, c + 1];
                                //Color clr = grid[c, r].Style.BackColor;
                                //rng.Interior.Color = System.Drawing.ColorTranslator.ToOle(clr);
                            }
                        }
                        strColRange = GetExcelRangeName(grid.ColumnCount - 1, grid.RowCount - 1);
                        excelRange = excelSheet.get_Range("A1", strColRange);
                        excelRange.EntireColumn.AutoFit();
                    }

                    // 마지막에 무조건 SHOW
                    try
                    {
                        excelApp.ScreenUpdating = true;    // 이거 안하면 느려짐
                        //excelApp.EnableEvents = true;      //Prevent Excel events
                        //excelApp.Interactive = true;       //Prevent user clicks and keystrokes

                        excelApp.Visible = true;
                        excelApp.UserControl = true;
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("Failed to show Excel!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    return;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                // 선택된 셀만 export
                List<int> selectedCols = GetSelectedColumns(grid);
                selectedCols.Sort();    // 정렬
                int nColIdx = 1;// selectedCols[0] + 1;  // excel은 1번부터 시작이라
                char szColRange = 'A';
                string colRange = "";
                // 헤더 추가한다
                if (bHeaderAdd)
                {
                    // 선택된 셀들의 헤더만 헤더 name 기록
                    for (int i = 0; i < grid.Columns.Count; i++)
                    {
                        if (Array.IndexOf(selectedCols.ToArray(), i) < 0)
                            continue;   // 선택되지 않은 열

                        DataGridViewColumn col = grid.Columns[i];
                        excelSheet.Cells[1, nColIdx++] = col.Name;  //기록!
                        szColRange++;
                    }
                    colRange = String.Format("{0}1", szColRange);
                    excelSheet.get_Range("A1", colRange).Font.Bold = true;
                    excelSheet.get_Range("A1", colRange).Font.Size = 13;
                    excelSheet.get_Range("A1", colRange).Font.Name = grid.Font.Name;
                    excelSheet.get_Range("A1", colRange).VerticalAlignment = Excel.XlVAlign.xlVAlignCenter;
                }

                nColIdx = 1;// selectedCols[0] + 1;  // excel은 1번부터 시작이라
                int nRow = 1;
                foreach (int col in selectedCols)
                {
                    foreach (DataGridViewCell cell in grid.SelectedCells)
                    {
                        if (cell.ColumnIndex == col)
                        {
                            excelSheet.Cells[nRow, nColIdx++] = cell.Value;
                        }
                    }
                }

                ////Add table headers going cell by cell.
                //excelSheet.Cells[1, 1] = "First Name";
                //excelSheet.Cells[1, 2] = "Last Name";
                //excelSheet.Cells[1, 3] = "Full Name";
                //excelSheet.Cells[1, 4] = "Salary";



                //// Create an array to multiple values at once.
                //string[,] saNames = new string[5, 2];

                //saNames[0, 0] = "John";
                //saNames[0, 1] = "Smith";
                //saNames[1, 0] = "Tom";
                //saNames[1, 1] = "Brown";
                //saNames[2, 0] = "Sue";
                //saNames[2, 1] = "Thomas";
                //saNames[3, 0] = "Jane";
                //saNames[3, 1] = "Jones";
                //saNames[4, 0] = "Adam";
                //saNames[4, 1] = "Johnson";

                ////Fill A2:B6 with an array of values (First and Last Names).
                //excelSheet.get_Range("A2", "B6").Value2 = saNames;

                ////Fill C2:C6 with a relative formula (=A2 & " " & B2).
                //excelRange = excelSheet.get_Range("C2", "C6");
                //excelRange.Formula = "=A2 & \" \" & B2";

                ////Fill D2:D6 with a formula(=RAND()*100000) and apply format.
                //excelRange = excelSheet.get_Range("D2", "D6");
                //excelRange.Formula = "=RAND()*100000";
                //excelRange.NumberFormat = "$0.00";

                ////AutoFit columns A:D.
                excelRange = excelSheet.get_Range("A1", colRange);
                excelRange.EntireColumn.AutoFit();

            }

            #region 예외처리
            catch (Exception theException)
            {
                String errorMessage;
                errorMessage = "엑셀 문서 생성중 에러발생:";
                errorMessage = String.Concat(errorMessage, theException.Message);
                errorMessage = String.Concat(errorMessage, " Line: ");
                errorMessage = String.Concat(errorMessage, theException.Source);

                MessageBox.Show(errorMessage, "Error");
            }

            // 마지막에 무조건 SHOW
            try
            {
                excelApp.Visible = true;
                excelApp.UserControl = true;
            }
            catch (Exception)
            {
                MessageBox.Show("Failed to show Excel!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            #endregion

        }
    }

    public class IniMan
    {

        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);



        private readonly static object iniLock = new object();
        public string Path = "";

        public void Write(string section, string key, string val)
        {
            Debug.Assert(String.IsNullOrEmpty(Path) == false);
            WritePrivateProfileString(section, key, val, Path);
        }
        public void WriteEx(string section, string key, dynamic val)
        {
            Write(section, key, Convert.ToString(val));
        }

        public string Read(string section, string key, string default_)
        {
            Debug.Assert(String.IsNullOrEmpty(Path) == false);

            StringBuilder val = new StringBuilder(4096);
            val.Length = GetPrivateProfileString(section, key, default_, val, 10000, Path);
            string str = val.ToString();
            if (String.IsNullOrEmpty(str))
                return "";

            str = str.Replace("\0", "");
            return str;
        }

        public int ReadInt(string section, string key, int default_)
        {

            string str = Read(section, key, Convert.ToString(default_));
            int nRet = default_;
            Int32.TryParse(str, out nRet);
            return nRet;
        }
        public double ReadDouble(string section, string key, double default_)
        {
            string str = Read(section, key, Convert.ToString(default_));
            double dRet = default_;
            Double.TryParse(str, out dRet);
            return dRet;
        }
    }

    // 범위 체크를 편하게 위한 클래스
    public class MyRange
    {
        double dFrom = 0, dTo = 0;

        public MyRange() { }

        public MyRange(double dFrom, double dTo)
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
            MyRange a = new MyRange();

            System.Diagnostics.Debug.Assert(a.ValidationCheck());
            return a.IsInRange(dVal);
        }
        public static bool IsInRange(int nFrom, int nTo, int nVal)
        {
            return nVal >= nFrom && nVal < nTo;
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
            return new DateTime(Year + 2000, Month, Day, Hour, Min, Sec, MSec);
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

            for (int i = 0; i < nRowCnt; i++)
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

    public class IniElem
    {
        public string name = "";
        public string val = "";
    }

    public class IniSection
    {
        public string name = "";
        public List<IniElem> elemList = new List<IniElem>();
    }

    public class DIni
    {
        public string filePath = "";
        public List<IniSection> sectionList = new List<IniSection>();
        public bool bLoaded = false;

        public bool LoadINI(string filePath)
        {

            string[] lines = null;
            if (bLoaded)
                return true;
            try
            {
                if (File.Exists(filePath) == false)
                    return false;

                lines = File.ReadAllLines(filePath);
            }
            catch (Exception)
            {
                return false;
            }
            this.filePath = filePath;
            IniSection curSection = null;

            foreach (string str in lines)
            {
                string line = str;
                line.Trim();
                line = line.Replace(" ", "");
                line = line.Replace("	", "");
                line = line.Replace("\t", "");

                if (line.Length < 3)
                    continue;

                bool bIsAlphabet = (line[0] >= 'a' && line[0] <= 'z') || (line[0] >= 'A' && line[0] <= 'Z');
                bool bIsDigit = (line[0] >= '1' && line[0] <= '9') || line[0] == '0';
                bool bIsBrace = line[0] == '[';
                bool bIsSpecialChar = !bIsAlphabet && !bIsDigit && !bIsBrace;

                // 특수문자로 시작하면 해석 안함
                if (bIsSpecialChar)
                    continue;

                bool bBraceStarted = line[0] == '[';
                int nBraceEndPos = line.IndexOf(']');
                bool bBraceEnded = nBraceEndPos >= 2;

                // 새 섹션 시작됨 -- 괄호 시작과 끝이 있는 라인 [ ]
                if (bBraceStarted && bBraceEnded)
                {
                    if (curSection != null)
                    {
                        if (curSection.elemList.Count > 0)
                        {
                            sectionList.Add(curSection);
                            curSection = null;
                        }
                    }

                    string sectionName = line.Substring(1, nBraceEndPos - 1);
                    curSection = new IniSection();
                    curSection.name = sectionName;
                    continue;
                }

                int nValPos = line.IndexOf('=');
                if (nValPos >= 1)
                {
                    try
                    {
                        // element 값을 찾았으면 이름-값 을 섹션안에 저장한다
                        string strVal = "";
                        if (GMethod.GetThisValue(line, line.Substring(0, nValPos), out strVal) == false)
                            continue;

                        IniElem elem = new IniElem();
                        elem.name = line.Substring(0, nValPos);
                        elem.val = strVal;
                        curSection.elemList.Add(elem);
                    }
                    catch (Exception exp)
                    {
                        MessageBox.Show(line + "\n파싱 도중 에러 발생\n\n" + exp);
                        continue;
                    }
                }
            }

            // 끝값 처리
            if (sectionList.Count > 0)
            {
                if (sectionList.Last() != curSection)
                    sectionList.Add(curSection);
            }
            bLoaded = true;
            return true;
        }

    }
    public class TimeChk
    {
        public const int MaxEventCnt = 30;  // 최대 이벤트

        // 인터페이스
        public static void Start(int nEventID)
        {
            if (nEventID >= MaxEventCnt && nEventID < 0)
                return;

            nArrStart[nEventID] = Environment.TickCount;
        }
        public static void End(int nEventID)
        {
            if (nEventID >= MaxEventCnt && nEventID < 0)
                return;
            if (nArrStart[nEventID] == 0)
                return;

            nArrEnd[nEventID] = Environment.TickCount;
        }
        public static double Get_sec(int nEventID) // msec
        {
            int nmsec = Get(nEventID);
            return nmsec / 1000.0;

        }
        public static int Get(int nEventID) // msec
        {
            if (nEventID >= MaxEventCnt && nEventID < 0)
                return -1;
            if (nArrStart[nEventID] == 0)
                return -1;
            if (nArrEnd[nEventID] == 0)
                return -1;
            if (nArrEnd[nEventID] < nArrStart[nEventID])
                return -1;

            return nArrEnd[nEventID] - nArrStart[nEventID];
        }
        public static void Init(int nEventID)     // nEventID == -1  전체 타이머 초기화, else 해당 타이머 초기화
        {
            //ASSERT(nEventID < MaxEventCnt);

            if (nEventID < 0)
            {
                for (int i = 0; i < nEventID; i++)  //init all
                    nArrStart[i] = nArrEnd[i] = 0;

                return;
            }

            nArrStart[nEventID] = nArrEnd[nEventID] = 0;
        }
        private static int[] nArrStart = new int[MaxEventCnt];
        private static int[] nArrEnd = new int[MaxEventCnt];

    };

}
namespace OHT_TOOL
{
    public class MyOHT
    {
        static public string GetOhtNumer(string strPath)
        {
            string strLastDir = MyTools.GMethod.GetLastDir(strPath);

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
        static public string GetOHTName_inDir(string dirPath)
        {
            string dir = Path.GetFileName(dirPath).ToUpper();
            int nVPos = dir.IndexOf('V');
            if (nVPos >= 0 && (nVPos + 5 < dir.Length))
            {
                string name = dir.Substring(nVPos, 6);
                return name;
            }

            return dir;
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
            if (!MyTools.GMethod.IsDigit_Int(strDate) || nTmp < 0)
            {
                return "";
            }

            return strDate;
        }
    }


    // NetworkConnection 클래스 : 네트워크 공유폴더에 접근할 수 있도록, 접속 해제를 해줌
    // 사용법
    /*
     * 
     using(new NetworkConnection(_directoryPath, new NetworkCredential(_userName, _password)))
     {
        File.Copy(localPath, _directoryPath);
     }
    */

    /// <summary>
    /// Represents a network connection along with authentication to a network share.
    /// </summary>
    public class NetworkConnection : IDisposable
    {
        #region Variables

        /// <summary>
        /// The full path of the directory.
        /// </summary>
        private readonly string _networkName;

        #endregion

        #region Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="NetworkConnection"/> class.
        /// </summary>
        /// <param name="networkName">
        /// The full path of the network share.
        /// </param>
        /// <param name="credentials">
        /// The credentials to use when connecting to the network share.
        /// </param>
        public NetworkConnection(string networkName, System.Net.NetworkCredential credentials)
        {
            _networkName = networkName;

            var netResource = new NetResource
            {
                Scope = ResourceScope.GlobalNetwork,
                ResourceType = ResourceType.Disk,
                DisplayType = ResourceDisplaytype.Share,
                RemoteName = networkName.TrimEnd('\\')
            };

            var result = WNetAddConnection2(
                netResource, credentials.Password, credentials.UserName, 0);

            if (result != 0)
            {
                throw new Win32Exception(result);
            }
        }

        #endregion

        #region Events

        /// <summary>
        /// Occurs when this instance has been disposed.
        /// </summary>
        public event EventHandler<EventArgs> Disposed;

        #endregion

        #region Public methods

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        #endregion

        #region Protected methods

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        /// <param name="disposing"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                var handler = Disposed;
                if (handler != null)
                    handler(this, EventArgs.Empty);
            }

            WNetCancelConnection2(_networkName, 0, true);
        }

        #endregion

        #region Private static methods

        /// <summary>
        ///The WNetAddConnection2 function makes a connection to a network resource. The function can redirect a local device to the network resource.
        /// </summary>
        /// <param name="netResource">A <see cref="NetResource"/> structure that specifies details of the proposed connection, such as information about the network resource, the local device, and the network resource provider.</param>
        /// <param name="password">The password to use when connecting to the network resource.</param>
        /// <param name="username">The username to use when connecting to the network resource.</param>
        /// <param name="flags">The flags. See http://msdn.microsoft.com/en-us/library/aa385413%28VS.85%29.aspx for more information.</param>
        /// <returns></returns>
        [DllImport("mpr.dll")]
        private static extern int WNetAddConnection2(NetResource netResource,
                                                     string password,
                                                     string username,
                                                     int flags);

        /// <summary>
        /// The WNetCancelConnection2 function cancels an existing network connection. You can also call the function to remove remembered network connections that are not currently connected.
        /// </summary>
        /// <param name="name">Specifies the name of either the redirected local device or the remote network resource to disconnect from.</param>
        /// <param name="flags">Connection type. The following values are defined:
        /// 0: The system does not update information about the connection. If the connection was marked as persistent in the registry, the system continues to restore the connection at the next logon. If the connection was not marked as persistent, the function ignores the setting of the CONNECT_UPDATE_PROFILE flag.
        /// CONNECT_UPDATE_PROFILE: The system updates the user profile with the information that the connection is no longer a persistent one. The system will not restore this connection during subsequent logon operations. (Disconnecting resources using remote names has no effect on persistent connections.)
        /// </param>
        /// <param name="force">Specifies whether the disconnection should occur if there are open files or jobs on the connection. If this parameter is FALSE, the function fails if there are open files or jobs.</param>
        /// <returns></returns>
        [DllImport("mpr.dll")]
        private static extern int WNetCancelConnection2(string name, int flags, bool force);

        #endregion

        /// <summary>
        /// Finalizes an instance of the <see cref="NetworkConnection"/> class.
        /// Allows an <see cref="System.Object"></see> to attempt to free resources and perform other cleanup operations before the <see cref="System.Object"></see> is reclaimed by garbage collection.
        /// </summary>
        ~NetworkConnection()
        {
            Dispose(false);
        }
    }

    #region Objects needed for the Win32 functions
#pragma warning disable 1591

    /// <summary>
    /// The net resource.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public class NetResource
    {
        public ResourceScope Scope;
        public ResourceType ResourceType;
        public ResourceDisplaytype DisplayType;
        public int Usage;
        public string LocalName;
        public string RemoteName;
        public string Comment;
        public string Provider;
    }

    /// <summary>
    /// The resource scope.
    /// </summary>
    public enum ResourceScope
    {
        Connected = 1,
        GlobalNetwork,
        Remembered,
        Recent,
        Context
    };

    /// <summary>
    /// The resource type.
    /// </summary>
    public enum ResourceType
    {
        Any = 0,
        Disk = 1,
        Print = 2,
        Reserved = 8,
    }

    /// <summary>
    /// The resource displaytype.
    /// </summary>
    public enum ResourceDisplaytype
    {
        Generic = 0x0,
        Domain = 0x01,
        Server = 0x02,
        Share = 0x03,
        File = 0x04,
        Group = 0x05,
        Network = 0x06,
        Root = 0x07,
        Shareadmin = 0x08,
        Directory = 0x09,
        Tree = 0x0a,
        Ndscontainer = 0x0b
    }
#pragma warning restore 1591
    #endregion


}