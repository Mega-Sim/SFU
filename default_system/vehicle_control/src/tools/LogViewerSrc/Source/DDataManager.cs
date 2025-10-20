using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MyTools;
using System.Runtime.CompilerServices;
using System.Diagnostics;
using System.Data;
using System.IO;
using System.Threading;
using LogViewer;
using Excel = Microsoft.Office.Interop.Excel;
using System.Reflection;
using System.Drawing;
using OHT_TOOL;
using System.ComponentModel;
using System.Collections;

namespace LogViewer_DataManager
{
    public enum eDataManTask
    {
        None, Ping, Read_OHT_Version, File_Upload, Log_Download, File_Download, Monitor_Download, Check_Monitor_Log, D_Drv_CHK, Parameter_CHK, CountOf
    }

    public class DDataManager
    {      
        public int nProgress_tPer = 0;
        private eDataManTask m_CurTask = eDataManTask.None;

        public const int PING_TIME = 1000;
        public bool m_bKillThread = false;
        public bool m_bSuspendThread = false;
        public bool m_bJobCancel = false;
		//private static DDataManager ThisObj = null;
		static object g_cs = new object();

		public List<sOHTList> m_OhtList = new List<sOHTList>();
        public List<sUploadFile> m_UploadFileList = new List<sUploadFile>();
        public List<sDownloadFile> m_DownloadFileList = new List<sDownloadFile>();
        public List<sDownloadLog> m_DownloadLogList = new List<sDownloadLog>();
		public List<string> m_MonitoringLogList = new List<string>();
		public string m_MonitoringSavePath = "";
		public List<string> m_CurLogFileList = new List<string>();
		public bool m_bJobFinished = false;
        private int m_nCurFileIdx = 0;
        private int m_nCurOhtIdx = 0;
        public UDataManager m_Parent = null;
        private int m_nTotalOhtCnt = 0;
        private int m_nProgressCnt = 0;
        public Thread m_taskThread = null;

        // OHT 네트워크 접근 id, pwd
        private string m_NetworkID = "";    //"design";	//#IMSI
		private string m_NetworkPWD = "";   //"sem$desi01";  //"";                

        
        public MultiKeyDictionary<string, string, string, string> DParaCompare 
            = new MultiKeyDictionary<string, string, string, string>();

        public MultiKeyDictionary<string, string, string, bool> DParaCompareChk
            = new MultiKeyDictionary<string, string, string, bool>();


        public Dictionary <string,bool> DParaFilename = new Dictionary<string,bool>();

        public StreamReader sr;
        public bool GetParameter(string path)
        {            

            DParaCompare.Clear();
            DParaCompareChk.Clear();
            DParaFilename.Clear();

            try
            {
                foreach (string strFile in Directory.GetFiles(path))
                {

                    if (strFile.IndexOf(".ini") > 0 || strFile.IndexOf(".INI") > 0)
                    {
                        if (strFile.IndexOf("DefaultParam.ini") > 0)
                           continue;
                        if (strFile.IndexOf(".bak") > 0)
                            continue;
                      
                        sr = new StreamReader(strFile);
                        string filename = Path.GetFileName(strFile);
                        DParaFilename.Add(filename, true);
                        string block = "";


                        while (!sr.EndOfStream)
                        {
                            string line = sr.ReadLine();

                            if (line.IndexOf("#") == 0 || line.IndexOf("//") == 0)
                                continue;

                            if (strFile.IndexOf("OHTPARAM.ini") >= 0)
                            {
                                if (line.IndexOf('=') > 0)
                                {
                                    line = line.Replace(" ", "");
                                    line = line.Replace("\t", "");
                                    if (line.IndexOf("//") > 0)
                                        line = line.Remove(line.IndexOf("//"), line.Length - line.IndexOf("//"));

                                    string[] parse2 = line.Split('=');

                                    if (block == "[AUTOTEACHING]" || block == "[WIRELESS]" || block == "[HOIST]"
                                      || block == "[AVS]" || parse2[0] == "MachineName")
                                        continue;

                                    if (parse2.Length == 2)
                                    {
                                        DParaCompare.Add(filename, block, parse2[0], parse2[1]);
                                        DParaCompareChk.Add(filename, block, parse2[0], false);
                                    }


                                }
                                else if (line.IndexOf('[') == 0 && line.IndexOf(']') > 0)
                                {
                                    block = line.Substring(line.IndexOf('['), line.IndexOf(']') + 1);
                                }
                            }
                            else
                            {
                                if (line.IndexOf('=') > 0)
                                {
                                    line = line.Replace(" ", "");
                                    line = line.Replace("\t", "");
                                    if (line.IndexOf("//") > 0)
                                        line = line.Remove(line.IndexOf("//"), line.Length - line.IndexOf("//"));
                                    string[] parse2 = line.Split('=');

                                    if (parse2.Length == 2)
                                    {
                                        DParaCompare.Add(filename, block, parse2[0], parse2[1]);
                                        DParaCompareChk.Add(filename, block, parse2[0], false);
                                    }


                                }
                                else if (line.IndexOf('[') == 0 && line.IndexOf(']') > 0)
                                {
                                    block = line.Substring(line.IndexOf('['), line.IndexOf(']') + 1);
                                }
                            }                            
                        }  

                    }

                  }
            }            
            catch
            {
                return false;
            }           

            return true;
        }
        public DDataManager()
        {
            m_taskThread = new Thread(TaskThread);
            m_taskThread.Priority = ThreadPriority.Highest;
            m_taskThread.Start(this);
        }
        public bool IsJobFinished()
        {
            return m_bJobFinished;
        }
        public void InitJob()
        {
            //if (m_CurTask == eDataManTask.None)
            {
                m_CurTask = eDataManTask.None;
                m_bJobFinished = false;
                m_nCurFileIdx = 0;
                m_nCurOhtIdx = 0;
                nProgress_tPer = 0;
                m_nFailCnt_File = 0;
                m_nFailCnt_OHT = 0;
                m_bSuspendThread = false;
                m_bJobCancel = false;
				m_nPrevOhtIdx = 0;
				m_nProgressCnt = m_nTotalOhtCnt = 0;
            }

        }
        public eDataManTask GetCurTask()	{ return m_CurTask;	}


		// 사용자가 OHT 체크 상태를 작업 도중 바꿀수있기 때문에, 체크 상태를 deep copy 한다
		public int DeepCopy_OhtList(List<sOHTList> ohtList)
		{
			m_OhtList.Clear();
			foreach (sOHTList ss in ohtList)
			{
				if (ss.bChecked == false)
					continue;

				sOHTList obj = new sOHTList();
				obj.DeepCopy(ss);
				m_OhtList.Add(obj);
			}
			return m_OhtList.Count;
		}

		// 작업 시작. 비동기
		public void DoWork(ref List<sOHTList> ohtList, eDataManTask task)
		{
			if (m_CurTask != eDataManTask.None)
			{
				m_Parent.m_LogQ.EnQ(String.Format("> 작업을 할 수 없습니다! 이미 {0} 작업이 진행중입니다. ", m_CurTask.ToString()));
				return;
			}
			
			InitJob();
			DeepCopy_OhtList(ohtList);  // DDataMan으로 Deep Copy 한다

			TimeChk.Start(0);	// job timer 시작. 시간재기

			m_Parent.m_LogQ.EnQ(String.Format("> [{0:00}:{1:00}:{2:00}]  <{3}> 작업 시작됨 ------------------ ", DateTime.Now.Hour, DateTime.Now.Minute, DateTime.Now.Second, task.ToString()));
			m_nTotalOhtCnt = m_OhtList.Count;
			m_CurTask = task;	
		}

	
		
        // task thread
        private static void TaskThread(Object arg)
        {
            DDataManager man = (DDataManager)arg;

            while (!man.m_bKillThread)
            {
				if (man.m_bJobCancel)
				{
					man.JobCancel();
					continue;
				}
                
                if (man.m_bSuspendThread || man.m_CurTask == eDataManTask.None)
                {
                    Thread.Sleep(100);	// idle time sleep
                    continue;
                }

                man.Execute();
				Thread.Sleep(1);	// running time sleep
			}
        }

        int m_nFailCnt_File = 0;
        int m_nFailCnt_OHT = 0;
        
        private void JobCancel()
        {
            m_bJobCancel = false;
			m_Parent.m_LogQ.EnQ(String.Format("> [{0:00}:{1:00}:{2:00}] ------------- {3} Cancel ------------ ", DateTime.Now.Hour, DateTime.Now.Minute, DateTime.Now.Second, m_CurTask.ToString()));
			InitJob();
		}

		int m_nPrevOhtIdx = 0;		// progress 계산하기 위함

        private void Execute()
        {
            if (m_CurTask == eDataManTask.None)
                return;

            // 작업완료 되었을 때 한번**** 
            if (m_nCurOhtIdx == m_OhtList.Count)
            {
                if (m_CurTask == eDataManTask.Monitor_Download)
                {
                    if (m_MonitoringLogList.Count() > 0)
                    {
                        File.WriteAllLines(m_MonitoringSavePath, m_MonitoringLogList);  // 모니터링 로그 저장
                        m_Parent.m_LogQ.EnQ(String.Format(">> {0} 개의 로그를 취합하여 [{1}]에 저장했습니다.", m_MonitoringLogList.Count(), m_MonitoringSavePath));
                    }
                    else
                        m_Parent.m_LogQ.EnQ(String.Format(">> 다운로드 받은 모니터링 로그 데이터가 없습니다."));
                }

                TimeChk.End(0);
                m_Parent.m_LogQ.EnQ(String.Format(">> [{0:00}:{1:00}:{2:00}] <{3} 작업 완료> 성공: {4}, 실패: {5}, ElapseTime: {6} sec",
                        DateTime.Now.Hour, DateTime.Now.Minute, DateTime.Now.Second, m_CurTask.ToString(), m_nTotalOhtCnt - m_nFailCnt_OHT, m_nFailCnt_OHT, TimeChk.Get_sec(0)));


                // 다운로드의 경우 작업이 끝나면 작업 내역 화면 로깅할 걸 txt 파일로 남긴다
                if (m_CurTask == eDataManTask.File_Download || m_CurTask == eDataManTask.Log_Download)
                {
                    string logPath = "";
                    if (m_CurTask == eDataManTask.File_Download)
                        logPath = m_DownloadFileList[0].LocalPath + @"\Download.txt";
                    else if (m_CurTask == eDataManTask.Log_Download)
                        logPath = m_DownloadLogList[0].strLocalPos + @"\Download.txt";

                    m_Parent.m_LogQ.Flush_toFile(logPath);
                }



                InitJob();      // init values
                m_bJobFinished = true;
            }

            // 현재 진행중인 OHT 정보 객체
            sOHTList OHT = m_OhtList[m_nCurOhtIdx];

            // T A S K # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            //
            switch (m_CurTask)
            {
                // PING TASK * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
                //
                case eDataManTask.Ping:
                    {
                        // Ping
                        TimeChk.Start(1);
                        int nTime_ms = GMethod.PingHost(OHT.IP, PING_TIME);
                        TimeChk.End(1);

                        string msg = "";
                        if (nTime_ms >= 0)
                            msg = String.Format("OK: {0}ms", nTime_ms);
                        else
                            msg = "NG";

                        m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, msg); // UI 상 ListView에 Result 열에 결과 표시

                        string tmp = String.Format("[{0}/{1}] Ping: {2} ({3}), PingTime:{4}, Elapse Time: {5}sec", m_nCurOhtIdx + 1, m_nTotalOhtCnt, OHT.Name, OHT.IP, nTime_ms, TimeChk.Get_sec(1));
                        m_Parent.m_LogQ.EnQ(nTime_ms >= 0 ? "> OK: " + tmp : "> NG: " + tmp);
                        if (nTime_ms < 0)
                            m_nFailCnt_File++;

                        m_nCurOhtIdx++;
                        break;
                    }

                // Version Reading Task * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
                //
                case eDataManTask.Read_OHT_Version:
                    {
                        bool bRet = false;

                        string remotePath = @"\\" + OHT.IP + @"\OHT\OHT.exe"; // @"\물류자동화그룹\OHT8.0\OHT.exe";

                        // ping 먼저 시도
                        if (GMethod.PingHost(OHT.IP, PING_TIME) < 0)
                        {
                            m_Parent.m_LogQ.EnQ(String.Format("> NG: {0}, IP: {1} Ping Fail", OHT.Name, OHT.IP));
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Version, OHT.Name, "FAIL"); // UI 상 ListView에 Result 열에 결과 표시
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, "NG");
                            m_nFailCnt_OHT++;
                            m_nCurOhtIdx++;
                            break;
                        }
                        m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Version, OHT.Name, "..."); // UI 상 ListView에 Result 열에 결과 표시

                        string strVersion = "";
                        string msg = "";
                        TimeChk.Start(1);   // 버전 읽기 시도*****
                        if (m_NetworkID == "")   //네트워크 계정 ID가 없는 경우 Connection 시간을 단축할수 있는 함수를 사용하자
                            strVersion = GMethod.FileVersionRead(remotePath, true);
                        else
                            strVersion = GMethod.NetworkFileVersionRead(remotePath, m_NetworkID, m_NetworkPWD, true);

                        TimeChk.End(1);

                        bRet = String.IsNullOrEmpty(strVersion) == false;
                        msg = bRet ? strVersion : "Fail";
                        m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Version, OHT.Name, msg); // UI 상 ListView에 Result 열에 결과 표시

                        string tmp = String.Format("[{0}/{1}] Version Reading: {2} ({3}), Version: {4} , Elapse Time: {5}sec", m_nCurOhtIdx + 1, m_nTotalOhtCnt, OHT.Name, OHT.IP, msg, TimeChk.Get_sec(1));
                        m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + tmp : "> NG: " + tmp);

                        if (!bRet)
                            m_nFailCnt_OHT++;
                        m_nCurOhtIdx++;
                        break;
                    }

                //File Download TASK * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
                //
                case eDataManTask.File_Download:
                    {
                        string msg = "";
                        // 한개 OHT에서 복사하고자 한 파일들을 모두 복사 했을 때, 해당 호기에 대한 ok ng 표시 ***
                        if (m_DownloadFileList.Count == m_nCurFileIdx)
                        {
                            msg = m_nFailCnt_File > 0 ? String.Format("NG: {0}/{1} files", m_nFailCnt_File, m_DownloadFileList.Count) : "OK";
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, msg); // UI 상 ListView에 Result 열에 결과 표시

                            if (m_nFailCnt_File > 0)
                                m_nFailCnt_OHT++;

                            m_nCurFileIdx = 0;
                            m_nFailCnt_File = 0;
                            m_nCurOhtIdx++;
                            break;
                        }
                        // 현재 Download하는 파일 정보
                        sDownloadFile DownInfo = m_DownloadFileList[m_nCurFileIdx];

                        string src = String.Format("\\\\{0}\\{1}", OHT.IP, DownInfo.OhtPath);
                        string fileName = Path.GetFileName(DownInfo.OhtPath);
                        string target = DownInfo.LocalPath + String.Format("\\{0}\\{1}", OHT.Name, fileName);

                        // 파일복사 이전에 Ping 테스트 먼저
                        if (GMethod.PingHost(OHT.IP, PING_TIME) < 0)
                        {
                            m_Parent.m_LogQ.EnQ(String.Format("> NG: {0}, IP: {1} Ping Fail", OHT.Name, OHT.IP));
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, "NG"); // UI 상 ListView에 Result 열에 결과 표시
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Fail");

                            m_nFailCnt_File = 0;
                            m_nFailCnt_OHT++;
                            m_nCurOhtIdx++;     // ping이 실패하면 다른 파일들은 시도하지 않고 곧장 다음 oht 로 넘어간다                         
                            break;
                        }
                        bool bRet = false;

                        // 네트워크 파일 복사
                        TimeChk.Start(1);
                        if (m_NetworkID == "")   //네트워크 계정 ID가 없는 경우 Connection 시간을 단축할수 있는 함수를 사용하자
                            bRet = GMethod.FileCopy(src, target, true);
                        else
                            bRet = GMethod.NetworkFileCopy(false, src, target, m_NetworkID, m_NetworkPWD, true);

                        TimeChk.End(1);

                        if (!bRet)
                            m_nFailCnt_File++;

                        msg = String.Format(" {0} [{1}/{2}], Copy {3} -> {4},  Elapse Time: {5} sec", OHT.Name, m_nCurOhtIdx + 1, m_OhtList.Count, src, target, TimeChk.Get_sec(1));
                        m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + msg : "> NG: " + msg);
                        m_nCurFileIdx++;
                        break;
                    }
                // LOG Download TASK * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
                //
                case eDataManTask.Log_Download:
                    {
                        // 이함수가 끝나고 나면 한개 호기에 대한 로그파일은 모두 복사가 끝난것.
                        // 비동기 처리에 위배되지만, 쉽게쉽게 하기 위함

                        LogFileDownLoad(OHT);       // 너무 길어서 함수로 분리
                        m_nCurOhtIdx++;
                        break;
                    }
                case eDataManTask.Monitor_Download:
                    {
                        if (m_nCurOhtIdx == 0)  // 데이터 초기화
                            m_MonitoringLogList.Clear();

                        if (MonitoringLogFileDownLoad(OHT) == false)
                        {
                            m_Parent.m_LogQ.EnQ("> NG: 모니터링 로그 다운로드 중 에러 발생.");
                            InitJob();
                            break;
                        }
                        m_nCurOhtIdx++;

                        break;
                    }
                case eDataManTask.Check_Monitor_Log:
                    {
                        break;
                    }
                case eDataManTask.D_Drv_CHK:
                    {
                        bool bRet = false;
                        TimeChk.Start(1);
                        //ping 먼저 시도
                        if (GMethod.PingHost(OHT.IP, PING_TIME) < 0)
                        {
                            m_Parent.m_LogQ.EnQ(String.Format(" > NG: {0}, IP: {1} Ping Fail", OHT.Name, OHT.IP));
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Ping Fail"); // UI 상 ListView에 Result 열에 결과 표시
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, "NG");
                            m_nFailCnt_OHT++;
                            m_nCurOhtIdx++;
                            break;
                        }
                        else
                        {
                            bRet = true;
                        }

                        string msg = "";
                        // 버전 읽기 시도*****
                        if (bRet)
                        {
                            try
                            {
                                DateTime ttm = DateTime.Now;
                                int failcnt = 0;
                                // 현재시간의 User로그가 있는지 검사. 최근 3시간치만 검색해서 하나도 없으면 Fail, 한개라도 있으면 OK
                                for (int i = 0; i < 3; i++)
                                {
                                    string userLogPath = String.Format("\\\\{0}\\Log\\User{1}{2:00}{3:00}{4:00}.log", OHT.IP, ttm.Year, ttm.Month, ttm.Day, ttm.Hour);
                                    if (File.Exists(userLogPath))
                                    {
                                        bRet = true;
                                        break;
                                    }
                                    else
                                    {
                                        failcnt++;
                                    }
                                    ttm = ttm.AddHours(-1.0);
                                }
                                if (failcnt >= 3)
                                    bRet = false;

                            }
                            catch { }
                        }


                        if (bRet)
                        {
                            string drvPath = String.Format("\\\\{0}\\\\Log\\", OHT.IP);
                            DirectoryInfo directoryD = new DirectoryInfo(drvPath);
                            FileSystemInfo[] FileArr = directoryD.GetFileSystemInfos();

                            long sum = 0;
                            for (int i = 0; i < FileArr.Length; i++)
                            {
                                FileInfo fileinfo = FileArr[i] as FileInfo;

                                if (fileinfo != null)
                                {
                                    sum += (long)fileinfo.Length;
                                }

                            }
                            double sum2 = (double)sum;
                            sum2 /= (double)1024;
                            sum2 /= (double)1024;
                            sum2 = Math.Round(sum2, 0);
                            if (sum2 >= 3800)
                            {
                                msg = sum2.ToString() + "MB(위험)";
                            }
                            else if (sum2 >= 3700)
                            {
                                msg = sum2.ToString() + "MB(주의)";
                            }
                            else
                            {
                                msg = sum2.ToString() + "MB";
                            }

                        }
                        TimeChk.End(1);

                        m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, msg); // UI 상 ListView에 Result 열에 결과 표시

                        string tmp = String.Format("[{0}/{1}] D Drive Check: {2} ({3}), Volume: {4} , Elapse Time: {5}sec", m_nCurOhtIdx + 1, m_nTotalOhtCnt, OHT.Name, OHT.IP, msg, TimeChk.Get_sec(1));
                        m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + tmp : "> NG: " + tmp);

                        if (!bRet)
                            m_nFailCnt_OHT++;
                        m_nCurOhtIdx++;
                        break;
                    }

                // ******************** File Uploading TASK ********************
                case eDataManTask.File_Upload:
                    {
                        FileUpload(OHT);
                        m_nCurOhtIdx++;
                        break;
                    }

                // ******************** Parameter Check TASK *******************
                case eDataManTask.Parameter_CHK:
                    {                                        

                        bool bRet = false;
                        string msg = "Fail";
                        string memo = "";

                        int success_cnt = 0;
                        int line_cnt = 0;                     
                        int diff_cnt = 0;
                        int abnormal_cnt = 0;
                        int notExist_cnt = 0;

                        int notMatchFile_cnt = 0;
                        //ping 먼저 시도
                        if (GMethod.PingHost(OHT.IP, PING_TIME) < 0)
                        {
                            m_Parent.m_LogQ.EnQ(String.Format(" > NG: {0}, IP: {1} Ping Fail", OHT.Name, OHT.IP));
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Ping Fail"); // UI 상 ListView에 Result 열에 결과 표시
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, "NG");
                            m_nFailCnt_OHT++;
                            m_nCurOhtIdx++;
                            break;
                        }
                        else
                        {
                            bRet = true;
                        }

                        TimeChk.Start(1);

                        if (bRet)
                        {
                            string paraPath = String.Format("\\\\{0}\\OHT\\DataFiles\\", OHT.IP);
                            //string paraPath = String.Format("\\\\{0}\\물류자동화그룹\\oht\\DataFiles\\", OHT.IP);
                            int preCount = line_cnt;


                            foreach (string strFile in Directory.GetFiles(paraPath))
                            {                                
       
                                if (strFile.IndexOf(".ini") > 0 || strFile.IndexOf(".INI") > 0)
                                {
                            
                                    if ( strFile.IndexOf("DefaultParam.ini") > 0)
                                        continue;
                                    if (strFile.IndexOf(".bak") > 0)
                                        continue;

                                    sr = new StreamReader(strFile);
                                    string filename = Path.GetFileName(strFile);
                                    if(!DParaFilename.ContainsKey(filename))
                                    {
                                        notMatchFile_cnt++;
                                        memo += filename;
                                        memo += " is not included/";
                                        continue;
                                    }
                                    string block = "";               

                                    while (!sr.EndOfStream)
                                    {

                                        string line = sr.ReadLine();

                                        if (line.IndexOf("#") == 0 || line.IndexOf("//") == 0)
                                            continue;
                                        if (strFile.IndexOf("OHTPARAM.ini") >= 0)
                                        {

                                            if (line.IndexOf('=') > 0)
                                            {

                                                line = line.Replace(" ", "");
                                                line = line.Replace("\t", "");
                                                if (line.IndexOf("//") > 0)
                                                    line = line.Remove(line.IndexOf("//"), line.Length - line.IndexOf("//"));
                                                string[] parse2 = line.Split('=');
                                                if (block == "[AUTOTEACHING]" || block == "[WIRELESS]" || block == "[HOIST]" 
                                                  || block =="[AVS]"|| parse2[0] == "MachineName")                                                    
                                                    continue;
                                                
                                                
                                                if (!DParaCompare.ContainsKey(filename, block, parse2[0]))
                                                {
                                                    abnormal_cnt++;
                                                    line_cnt++;
                                                    bRet = false;             
                                                }
                                                else
                                                {
                                                    DParaCompareChk[filename, block, parse2[0]] = true;
                                                    if (DParaCompare[filename, block, parse2[0]] != parse2[1])
                                                    {
                                                        line_cnt++;
                                                        diff_cnt++;
                                                    }
                                                    else
                                                    {
                                                        success_cnt++;
                                                    }                  
                                                }

                                            }
                                            else if (line.IndexOf('[') == 0 && line.IndexOf(']') > 0)
                                            {
                                                block = line.Substring(line.IndexOf('['), line.IndexOf(']') + 1);
                                            }

                                        }
                                        else
                                        {
                                            if (line.IndexOf('=') > 0)
                                            {
                                                line = line.Replace(" ", "");
                                                line = line.Replace("\t", "");
                                                if (line.IndexOf("//") > 0)
                                                    line = line.Remove(line.IndexOf("//"), line.Length - line.IndexOf("//"));
                                                string[] parse2 = line.Split('=');

                                                if (!DParaCompare.ContainsKey(filename, block, parse2[0]))
                                                {
                                                    abnormal_cnt++;
                                                    line_cnt++;
                                                    bRet = false;               

                                                }
                                                else
                                                {
                                                    DParaCompareChk[filename, block, parse2[0]] = true;
                                                    if (DParaCompare[filename, block, parse2[0]] != parse2[1])
                                                    {
                                                        line_cnt++;
                                                        diff_cnt++;
                                                            
                                                        bRet = false;
                                                    }
                                                    else
                                                    {
                                                        success_cnt++;
                                                    }

                                                }

                                            }
                                            else if (line.IndexOf('[') == 0 && line.IndexOf(']') > 0)
                                            {
                                                block = line.Substring(line.IndexOf('['), line.IndexOf(']') + 1);
                                            }
                                        }
                                    }

                                    if (preCount != line_cnt)
                                    {
                                        memo += filename;
                                        memo += '/';
                                        notMatchFile_cnt++;
                                    }
                                    preCount = line_cnt;
                                }  
                                
                            }
                        }
                        TimeChk.End(1);
                        int preNotExist_cnt = 0;
                        //누락체크
                        for (int index = 0; index < DParaCompareChk.Count; index++)
                            {
                                var item = DParaCompare.ElementAt(index);
                                string itemkey = item.Key;
                                
                                var Value1 = item.Value;
                                for (int index2 = 0; index2 < Value1.Count; index2++)
                                {
                                    var item2 = Value1.ElementAt(index2);
                                    string itemkey2 = item2.Key;

                                    var Value2 = item2.Value;

                                    for (int index3 = 0; index3 < Value2.Count; index3++)
                                    {
                                        var item3 = Value2.ElementAt(index3);
                                        string itemkey3 = item3.Key;

                                        if (DParaCompareChk[itemkey, itemkey2, itemkey3] == false)
                                        {
                                            notExist_cnt++;
                                            line_cnt++;
                                            bRet = false;                      
                                        }
                                        DParaCompareChk[itemkey, itemkey2, itemkey3] = false;

                                    }
                                }

                                if(preNotExist_cnt != notExist_cnt)
                                {
                                    notMatchFile_cnt++;
                                    memo += itemkey;
                                    memo += '/';
                                }
                                preNotExist_cnt = notExist_cnt;
                            }
                    

                        if (bRet)
                        {
                            msg = "Success";
                        }
                        else
                        {
                            msg = "Fail";
                            memo = "notMatchFile: " + notMatchFile_cnt.ToString() + "/ Success Line: " + success_cnt.ToString() + " / Fail  Line: " + line_cnt.ToString() + " / Diff Line: " + diff_cnt.ToString() +
                                " / NotExist Line: " + notExist_cnt.ToString() + " / Abnormal Line: " + abnormal_cnt.ToString() + "/" + memo;
                            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Memo, OHT.Name, memo); // UI 상 ListView에 Result 열에 결과 표시

                        }

                        m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, msg); // UI 상 ListView에 Result 열에 결과 표시

             
                            string tmp = String.Format("[{0}/{1}] OHT Name:{2} ({3}), Success line: {4} , Fail line: {5}, Elapse Time: {6}sec", m_nCurOhtIdx + 1, m_nTotalOhtCnt, OHT.Name, OHT.IP, success_cnt, line_cnt, TimeChk.Get_sec(1));

                            m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + tmp : "> NG: " + tmp);

                            if (!bRet)
                                m_nFailCnt_OHT++;

                            m_nCurOhtIdx++;
                            break;
                        
                        }
                    

                    // OHT가 다음 인덱스로 넘어가면 Progress 증가
                    if (m_nPrevOhtIdx < m_nCurOhtIdx)
                        m_nProgressCnt++;
                    m_nPrevOhtIdx = m_nCurOhtIdx;

                    if (!m_bJobFinished)
                    {   // progress tousand percent
                        double tPer = (double)m_nProgressCnt / (double)Math.Max(1, m_nTotalOhtCnt);
                        tPer *= 1000;
                        nProgress_tPer = Math.Min(1000, Math.Max(0, (int)tPer));
                    }

            }
        }
		private void FileUpload(sOHTList OHT)
		{
			if (GMethod.PingHost(OHT.IP) < 0)
			{
				m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기 Ping Fail", OHT.Name));
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Ping Fail");
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, String.Format("NG"));
				return;
			}

			TimeChk.Start(2);

			m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기 Upload 시작", OHT.Name));
			m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "...");


			int nOKCnt = 0;
			foreach(sUploadFile obj in m_UploadFileList)
			{
				if (m_bJobCancel)
					break;

				if (File.Exists(obj.srcPath) == false)
				{
					m_Parent.m_LogQ.EnQ(String.Format("> 원본 파일을 찾을 수 없습니다. {0}", obj.srcPath));
					continue;
				}

				TimeChk.Start(1);
				string targetPath = String.Format("\\\\{0}\\{1}\\{2}", OHT.IP, obj.targetPath, Path.GetFileName(obj.srcPath));

				bool bRet = false;
				if (m_NetworkID == "")   //네트워크 계정 ID가 없는 경우 Connection 시간을 단축할수 있는 함수를 사용하자
					bRet = GMethod.FileCopy(obj.srcPath, targetPath, true);
				else
					bRet = GMethod.NetworkFileCopy(true, obj.srcPath, targetPath, m_NetworkID, m_NetworkPWD, true);

				if (bRet)
					nOKCnt++;

				TimeChk.End(1);

				string msg = String.Format("{0} [{1}/{2}], Upload {3} -> {4},  Elapse Time: {5} sec", OHT.Name, m_nCurOhtIdx + 1, m_OhtList.Count, obj.srcPath, targetPath, TimeChk.Get_sec(1));
				m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + msg : "> NG: " + msg);
			}

			TimeChk.End(2);
			
			if (nOKCnt == m_UploadFileList.Count())
			{
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, String.Format("OK: {0}files ", m_UploadFileList.Count()));
				m_Parent.m_LogQ.EnQ(String.Format("> OK: {0} 호기에서 {1} 개의 파일 처리 완료, Elapse Time: {2} sec", OHT.Name, m_UploadFileList.Count(), TimeChk.Get_sec(2)));
			}
			else
			{
				int nFailCnt = m_UploadFileList.Count() - nOKCnt;
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, String.Format("NG: {0}/{1} files", nFailCnt, m_UploadFileList.Count()));
				m_Parent.m_LogQ.EnQ(String.Format("> NG: {0} 호기에서 {1}개 파일중 {2} 파일 실패, Elapse Time: {3} sec", OHT.Name, m_UploadFileList.Count(), nFailCnt, TimeChk.Get_sec(2)));
			}
			
		}

		// 이함수가 끝나고 나면 한개 호기에 대한 로그파일은 모두 복사가 끝난것.
		// 비동기 처리에 위배되지만, 쉽게쉽게 하기 위함
		private void LogFileDownLoad(sOHTList OHT)
		{
            if (GMethod.PingHost(OHT.IP) < 0)
            {
                m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기 Ping Fail", OHT.Name));
                m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Ping Fail");
                m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, String.Format("NG"));
                return;
            }

            TimeChk.Start(2);

            m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기 Download 시작", OHT.Name));
            m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "...");

            // 먼저 로그 폴더에 모든 파일 리스트를 얻어온다. 시간이 다소 걸리는 작업
            string folderPath = String.Format("\\\\{0}\\Log", OHT.IP);
            //string folderPath = String.Format(@"\\23.43.50.138\물류자동화그룹\설계파트\아젠텍\##세메스 요청파일##\하진호 프로\log");
            //folderPath = String.Format("\\\\{0}\\물류자동화그룹\\OHT8.0", OHT.IP);	//#IMSI
            string[] files = null;
			if (m_NetworkID == "")
				files = GMethod.GetFileList(folderPath);
			else
				files = GMethod.GetNetworkFileList(m_NetworkID, m_NetworkPWD, folderPath);

			if (files == null)
				return;

			string strLocalPos = "";
			List<string> copyFileList = new List<string>();

			foreach (sDownloadLog objLog in m_DownloadLogList)
			{
				if (m_bJobCancel)
					break;
				strLocalPos = objLog.strLocalPos;

				List<string> tmpFileList = new List<string>();
				foreach (string file in files)
				{
					if (m_bJobCancel)
						break;

					DateTime tm = new DateTime();
					if (GetLogFileTime_byFileName(file, out tm) == false)
						continue;

					if (objLog.logTime.IsIn(tm.Year, tm.Month, tm.Day, tm.Hour))    // 해당 시간대면 추가
						tmpFileList.Add(file);

				}

				
				if (objLog.bAllLog == false)
				{	
					// 2. 해당 시간대의 모든 로그 파일 중, 다운받고자 하는 파일만 멤버변수 리스트에 추가					
					foreach (string file in tmpFileList)
					{
						if (m_bJobCancel)
							break;

						string fileName = Path.GetFileNameWithoutExtension(file);
                        
                        if (Path.GetExtension(fileName).ToUpper() == ".LOG")
                            fileName = Path.GetFileNameWithoutExtension(fileName).ToUpper();

                        bool bIsAmcLog = fileName.IndexOf("AMC_AXIS") == 0; //amc 로그는 nDateLen	AMC_AXIS[3]_M_TRACE_20181020184303.log 이런식이다(14자). 일반 로그 10자
						int nDateLen = bIsAmcLog ? 14 : 10;
						
						string fileName_woDate = fileName.Substring(0, fileName.Length - nDateLen).ToUpper();
						if (fileName_woDate == objLog.strLogName.ToUpper())
							copyFileList.Add(file);
					}
				}
				else
				{
					// 모든 파일 복사면, 해당 시간대의 모든 파일이다
					copyFileList = tmpFileList;
				}
			}

			// 원하는 시간대에 원하는 로그파일 경로는 모두 copyFielsList에 모여있다.
			// 실제로 복사가 이루어지는 부분
			int nOKCnt = 0;
			foreach (string file in copyFileList)
			{
				if (m_bJobCancel)
					break;

				TimeChk.Start(1);
				string localTargetFile = String.Format("{0}\\{1}\\{2}", strLocalPos, OHT.Name, Path.GetFileName(file));
				bool bRet = false;
				if(m_NetworkID == "")      //네트워크 계정 ID가 없는 경우 Connection 시간을 단축할수 있는 함수를 사용하자
					bRet = GMethod.FileCopy(file, localTargetFile, true);
				else
					bRet = GMethod.NetworkFileCopy(false, file, localTargetFile, m_NetworkID, m_NetworkPWD, true);

				TimeChk.End(1);

				if (bRet)
					nOKCnt++;
				string msg = String.Format("{0} [{1}/{2}], Copy {3} -> {4},  Elapse Time: {5} sec", OHT.Name, m_nCurOhtIdx + 1, m_OhtList.Count, file, localTargetFile, TimeChk.Get_sec(1));
				m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + msg : "> NG: " + msg);
			}

			TimeChk.End(2);

			if (nOKCnt == copyFileList.Count())
			{
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, String.Format("OK: {0}files ", copyFileList.Count()));
				m_Parent.m_LogQ.EnQ(String.Format("> OK: {0} 호기에서 {1} 개의 파일 처리 완료, Elapse Time: {2} sec", OHT.Name, copyFileList.Count(), TimeChk.Get_sec(2)));
			}
			else
			{
				int nFailCnt = copyFileList.Count() - nOKCnt;
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, String.Format("NG: {0}/{1} files", nFailCnt, copyFileList.Count()));
				m_Parent.m_LogQ.EnQ(String.Format("> NG: {0} 호기에서 {1}개 파일중 {2} 파일 실패, Elapse Time: {3} sec", OHT.Name, copyFileList.Count(), nFailCnt, TimeChk.Get_sec(2)));
			}
		}

		// 모니터링 로그 다운로드
		// 이함수가 끝나고 나면 한개 호기에 대한 모니터링 로그파일은 모두 복사가 끝난것.
		private bool MonitoringLogFileDownLoad(sOHTList OHT)
		{

			if (GMethod.PingHost(OHT.IP, 1000) < 0)
			{
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Ping, OHT.Name, String.Format("NG"));
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "Ping Fail");
				m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기 Ping Fail", OHT.Name));
				return true;
			}

			foreach (sDownloadLog objLog in m_DownloadLogList)
			{
				if (m_bJobCancel)
					break;

				TimeChk.Start(2);

				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, "...");
				m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기에서 모니터링 로그 다운로드 시작", OHT.Name));

				// 1. 원하는 시간대의 모든 로그 파일 리스트를 찾는다
				string folderPath = String.Format("\\\\{0}\\Log", OHT.IP);
				//folderPath = String.Format("\\\\{0}\\물류자동화그룹\\OHT8.0", OHT.IP); //#IMSI
				string[] files = null;
				if (m_NetworkID == "")
					files = GMethod.GetFileList(folderPath);
				else
					files = GMethod.GetNetworkFileList(m_NetworkID, m_NetworkPWD, folderPath);

				if (files == null)
					return false;

				List<string> tmpFileList = new List<string>();
				foreach (string file in files)
				{
					if (m_bJobCancel)
						break;

					DateTime tm = new DateTime();
					if (GetLogFileTime_byFileName(file, out tm) == false)
						continue;

					if (objLog.logTime.IsIn(tm.Year, tm.Month, tm.Day, tm.Hour))    // 해당 시간대면 추가
						tmpFileList.Add(file);
				}

				List<string> copyFileList = new List<string>();			
				
				// 2. 해당 시간대의 모든 로그 파일 중, 모니터링 로그만  리스트에 추가					
				foreach (string file in tmpFileList)
				{
					if (m_bJobCancel)
						break;
					string fileName = Path.GetFileNameWithoutExtension(file);
					string fileName_woDate = fileName.Substring(0, fileName.Length - 10).ToUpper();
					if (fileName_woDate == "MONITOR")
						copyFileList.Add(file);
				}
				
				foreach (string file in copyFileList)
				{
					if (m_bJobCancel)
						break;

					TimeChk.Start(1);

					string localTargetFile = String.Format("{0}\\monitoring_log_tmp.txt", UMain.TMP_PATH);
					File.Delete(localTargetFile);
					if (File.Exists(localTargetFile))       // 누가 임시파일 열어놔서 작업할수 없음
					{
						MessageBox.Show(localTargetFile + " 파일을 사용할 수 없어 작업을 할 수 없습니다.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
						return false;
					}

					bool bRet = false;

					if (m_NetworkID == "")   //네트워크 계정 ID가 없는 경우 Connection 시간을 단축할수 있는 함수를 사용하자
						bRet = GMethod.FileCopy(file, localTargetFile, true);
					else
						bRet = GMethod.NetworkFileCopy(false, file, localTargetFile, m_NetworkID, m_NetworkPWD, true);

					if (bRet)
					{
						DateTime tm = new DateTime();
						if (GetLogFileTime_byFileName(file, out tm) == false)
							continue;

						string[] arr = File.ReadAllLines(localTargetFile);
						for (int i = 0; i < arr.Length; i++)
						{
							if (arr[i].Length < 20)
								continue;

							string line = arr[i];
							line = line.Replace('[', '/');      //시작 부분 [04:02:20.765]HO/ <- 이거를 /로 구분하기 위함
							line = line.Replace(']', '/');
							line = String.Format("{0}/{1:00}-{2:00}-{3:00}", OHT.Name, tm.Year, tm.Month, tm.Day) + line;
							// 결국  V70001/2019-01-06/04:02:20.765/HO/0/IA/0/  ... <- 이렇게 만들어져야함
							m_MonitoringLogList.Add(line);
						}
					}
					TimeChk.End(1);

					string msg = String.Format("{0} [{1}/{2}], Monitoring Log Read {3},  Elapse Time: {4} sec", OHT.Name, m_nCurOhtIdx + 1, m_OhtList.Count, file, TimeChk.Get_sec(1));
					m_Parent.m_LogQ.EnQ(bRet ? "> OK: " + msg : "> NG: " + msg);
				}

				TimeChk.End(2);
				m_Parent.SetOhtResult_Extern(UDataManager.eResultType.Result, OHT.Name, String.Format("{0} files", copyFileList.Count()));
				m_Parent.m_LogQ.EnQ(String.Format("> {0} 호기에서 {1} 개의 파일 처리 완료, Elapse Time: {2} sec", OHT.Name, copyFileList.Count(), TimeChk.Get_sec(2)));
			}
			return true;
		}

		public static bool GetLogFileTime_byFileName(string filePath, out DateTime theTime)
		{
			theTime = new DateTime();

			try
			{
				if (Path.GetExtension(filePath).ToUpper() != ".LOG" &&
                    Path.GetExtension(filePath).ToUpper() != ".ZIP")
					return false;

				string fileName = Path.GetFileNameWithoutExtension(filePath).ToUpper();

                if(Path.GetExtension(fileName).ToUpper() == ".LOG")
                    fileName = Path.GetFileNameWithoutExtension(fileName).ToUpper();

                if (fileName.Length < 11)
					return false;

				bool bIsAmcLog = fileName.IndexOf("AMC_AXIS") == 0; //amc 로그는 nDateLen	AMC_AXIS[3]_M_TRACE_20181020184303.log 이런식이다(14자). 일반 로그 10자
				int nDateLen = bIsAmcLog ? 14 : 10;

				string date = fileName.Substring(fileName.Length - nDateLen, 10);
				string year = date.Substring(0, 4);
				string month = date.Substring(4, 2);
				string day = date.Substring(6, 2);
				string hour = date.Substring(8, 2);

				int nYear = Convert.ToInt32(year);
				int nMonth = Convert.ToInt32(month);
				int nDay = Convert.ToInt32(day);
				int nHour = Convert.ToInt32(hour);

				theTime = new DateTime(nYear, nMonth, nDay, nHour, 0, 0);
				return true;
			}
			catch (Exception)
			{
			}
			return false;
		}
        bool CopyFile_ToOht(string src, string target)
        {
            Thread.Sleep(100);

            // debugging
            int nRand = new Random().Next();
            if (nRand % 5 == 0)
                return false;

            return true;
        }
        bool ReadOHTVersion(string addrs, out string version)
        {
            version = "";

            if (GMethod.PingHost(addrs, PING_TIME) < 0)
                return false;

            string path = String.Format("\\OHT.exe", addrs);
            //path = @"D:\#SrcTree\bin\OHT.exe";
            if (File.Exists(path) == false)
                return false;

            FileVersionInfo myFileVersionInfo = FileVersionInfo.GetVersionInfo(path);
            version = myFileVersionInfo.FileVersion;
            return true;

        }

        public bool bThreadStop_CheckMonitor_atLocal = false;

        // THREAD  : 로컬경로에서 모니터링 로그 검사 ***************
        // 모니터링 로그 검사 - 로컬파일에 있을 경우는 DDataManager에서 정식적인 잡으로 처리하지 않고, 빠르게 수행하기 위해서 단일 스레드에서 동작 시킨다
        public static void DoCheckMonitorThread_atLocal(Object arg)
        {
            DDataManager obj = (DDataManager)arg;
            UDataManager UI = obj.m_Parent;

            string[] arrLine = File.ReadAllLines(UI.m_strMonitorLocalPath);
            int nTotalLines = arrLine.Count();
            UI.m_MonitorResult.resultFilePath = "";
			
           // int nCurProgress = 0;

			int nHourCnt = 0;
			DMonitorResult.iRow rowObj = null;
			//foreach (string ln in arrLine)
			//{	작업중
			//	UI.m_MonitorResult.AllData.Add(String.Format("{0},{1}/{2}/{3},{4}", curOHT, nYear, nMonth, nDay, line.Replace("]", "],").Replace('/', ',')));   //V30001,[time],HO,1,AW,2 .... 이런식으로.. CSV로 저장하기위함
			//}
			//rowObj = UI.m_MonitorResult.Parsing_Add(curOHT, line);

			if (rowObj != null)
				rowObj.LogHours = Math.Max(1, nHourCnt);    // 몇시간치 로그인지 기록

															// 파일 리딩 및 취합은 여기까지 끝..

			obj.nProgress_tPer = 950;
            // 취합한 데이터로 기준값 초과하는게 있는지 찾는다
            foreach (DMonitorResult.iRow row in UI.m_MonitorResult.Rows)
            {
                int nItemCnt = UI.m_MonitorResult.itemNames.Count;
                
                // 모든 값을을 기준값과 비교하여 초과하는 항목은 ErrItemIdx에 기록해놓는다.
                for (int y = 0; y < nItemCnt; y++)
                {
                    string name = row.GetItemName(y);
                    DMonitorItems standard = UI.m_dlgMonitorOpt.CheckedItem.Find(o => o.name == name);
                    if(standard.ErrCnt < row.GetItemCount_inAnHour(y))   // 기준값은 시간당 발생건과 비교한다
                    {
                        row.ErrItemIdx.Add(y);
                    }
                }
            }
           obj.nProgress_tPer = 1000;

            // reuslt file 네이밍
            string ResultfileName = String.Format("{0}\\{1}", UI.m_strMonitorLocalPath, Path.GetFileName(UI.m_strMonitorLocalPath));

            // 이미 있는 파일이면 뒤에 1~계속 붙여나간다
            int i = 1;
            while(File.Exists(ResultfileName + ".csv"))
            {
                ResultfileName = String.Format("{0}\\{1}_{2}", UI.m_strMonitorLocalPath, Path.GetFileName(UI.m_strMonitorLocalPath), i);
                i++;
            }

            try
            {
                obj.SaveResult_asFile(ResultfileName, UI.m_MonitorResult, UI.m_dlgMonitorOpt.CheckedItem);
                //obj.SaveResult_asExcel(ResultfileName, UI.m_MonitorResult, UI.m_dlgMonitorOpt.CheckedItem);	// 엑셀은 보안프로그램때문에 꼬인다
                ResultfileName = ResultfileName + ".csv";
                if (File.Exists(ResultfileName))
                    UI.m_MonitorResult.resultFilePath = ResultfileName; // 결과 파일 Path 전달. 결과 파일이 없을 경우 뭔가 잘못된 것임 
            }
            catch (Exception)
            {
            }
            obj.bThreadStop_CheckMonitor_atLocal = false;
            obj.nProgress_tPer = 0;
            //UI.UpdateMonitorLogResult_Invoke(UI); // 인보크하지 않고 UI timer에서 끝난거 감지하면 결과 Update함
            //UI._DelegateMonitorResultUpdate_(); //delegate로 결과 갱신
        }

        private void CalcProgressTPer(int total, int cur)
        {
            nProgress_tPer = Math.Max(0, Math.Min(1000, (int)(((double)cur / total) * 1000)));
        }

        public void SaveResult_asFile(string path, DMonitorResult obj, List<DMonitorItems> standardList)
        {
            List<string> allLines = new List<string>();

			Version v = Assembly.GetExecutingAssembly().GetName().Version;
			
            allLines.Add(String.Format("=== OHT Logviewer Ver. {0}. 모니터링 로그 검사 결과 ===", v.ToString()));
            allLines.Add(String.Format("검사일시: {0} {1}", DateTime.Now.ToShortDateString(), DateTime.Now.ToLongTimeString()));
            allLines.Add("");
            allLines.Add("");
            allLines.Add("=== 선정항목 ===");

            foreach (DMonitorItems t in standardList)
            {
                allLines.Add(String.Format("선정항목: {0} *기준값 {1} *조건: {2} *설명: {3}", t.name, t.ErrCnt, t.cond.Replace(',', '_'), t.desc.Replace(',','_')));   //csv이기 때문에 콤마 삭제
            }

            allLines.Add("");
            allLines.Add("");
            allLines.Add("=== [OHT 기준] 시간당 최대 발생건 현황 ===");

            // 기준값 초과하는 항목 먼저 기록
            int nLoopCnt = 0;
            foreach(DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    allLines.Add(strColHeader);
                }

                string str = row.OHTName + ",";
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount_inAnHour(i));
                }

                allLines.Add(str);
                nLoopCnt++;
            }


            allLines.Add("");
            allLines.Add("");
            allLines.Add("=== [검사 항목 기준]  시간당 최대 발생건이 기준값 초과 내역 ===");
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if(row.ErrItemIdx.Count > 0)
                {
                    foreach(var t in row.ErrItemIdx)
                    {
                        int nCnt = row.GetItemCount_inAnHour(t);
                        int nStandardVal = standardList[t].ErrCnt;
                        string aLine = String.Format("{0},항목,{1},최대값,{2},", row.OHTName, standardList[t].name, nCnt);
                        allLines.Add(aLine);
                    }
                }
            }
            
            allLines.Add("");
            allLines.Add("");
            allLines.Add("=== 전체 OHT 에러 누적 현황 ===");
            nLoopCnt = 0;
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,측정시간,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    allLines.Add(strColHeader);
                }

                string str = String.Format("{0},{1},", row.OHTName, row.LogHours);
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount(i));
                }

                allLines.Add(str);
                nLoopCnt++;
            }

            allLines.Add("");
            allLines.Add("");
            allLines.Add("=== 전체 RAW DATA ===");

            foreach (var t in obj.AllData)
            {
                allLines.Add(t);
            }

            File.WriteAllLines(path, allLines, Encoding.Default);
        }

        public void SaveResult_asExcel(string path, DMonitorResult obj, List<DMonitorItems> standardList)
        {
            List<string> sheetCont_Title = new List<string>();
			Version v = Assembly.GetExecutingAssembly().GetName().Version;

			sheetCont_Title.Add(String.Format(",,### OHT Logviewer Ver. {0}. 모니터링 로그 검사 결과 ###", v.ToString()));
            sheetCont_Title.Add(String.Format(",,검사일시: {0} {1}", DateTime.Now.ToShortDateString(), DateTime.Now.ToLongTimeString()));
            sheetCont_Title.Add("");
            sheetCont_Title.Add("");
            sheetCont_Title.Add("선정항목,기준값,조건,설명");

            foreach (DMonitorItems t in standardList)
            {
                sheetCont_Title.Add(String.Format("{0},{1},{2},{3}", t.name, t.ErrCnt, t.cond.Replace(',', ' '), t.desc.Replace(',', ' ')));   //csv이기 때문에 콤마 삭제          
            }


            List<string> sheetCont_OHTMaxEvent = new List<string>();
            //sheetCont_OHTMaxEvent.Add("=== [OHT 기준] 시간당 최대 발생건 현황 ===");

            // 기준값 초과하는 항목 먼저 기록
            int nLoopCnt = 0;
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    sheetCont_OHTMaxEvent.Add(strColHeader);
                }

                string str = row.OHTName + ",";
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount_inAnHour(i));
                }

                sheetCont_OHTMaxEvent.Add(str);
                nLoopCnt++;
            }


            //sheetCont_Title.Add("");
            //sheetCont_Title.Add("");
            //sheetCont_Title.Add("=== [검사 항목 기준]  시간당 최대 발생건이 기준값 초과 내역 ===");
            List<string> sheetCont_OverItem = new List<string>();

            // 기준값 초과하는 항목 먼저 기록
            sheetCont_OverItem.Add("OHT,항목,시간당 최대 발생");
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (row.ErrItemIdx.Count > 0)
                {
                    foreach (var t in row.ErrItemIdx)
                    {
                        int nCnt = row.GetItemCount_inAnHour(t);
                        int nStandardVal = standardList[t].ErrCnt;
                        string aLine = String.Format("{0},{1},{2},", row.OHTName, standardList[t].name, nCnt);
                        sheetCont_OverItem.Add(aLine);
                    }
                }
            }

            List<string> sheetCont_AvrErr = new List<string>();
            nLoopCnt = 0;
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,측정시간,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    sheetCont_AvrErr.Add(strColHeader);
                }

                string str = String.Format("{0},{1},", row.OHTName, row.LogHours);
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount_Avr(i));
                }

                sheetCont_AvrErr.Add(str);
                nLoopCnt++;
            }


            List<string> sheetCont_MaxErr = new List<string>();
            //sheetCont_OverItem.Add("=== 전체 OHT 에러 누적 현황 ===");
            nLoopCnt = 0;
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,측정시간,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    sheetCont_MaxErr.Add(strColHeader);
                }

                string str = String.Format("{0},{1},", row.OHTName, row.LogHours);
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount_inAnHour(i));
                }

                sheetCont_MaxErr.Add(str);
                nLoopCnt++;
            }


            List<string> sheetCont_Brief = new List<string>();
            //sheetCont_OverItem.Add("=== 전체 OHT 에러 누적 현황 ===");
            nLoopCnt = 0;
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,측정시간,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += String.Format("{0}\r\n평균,{1}\r\n최대,{2}\r\n누적,", row.GetItemName(i), row.GetItemName(i), row.GetItemName(i));
                        
                    sheetCont_Brief.Add(strColHeader);
                }

                string str = String.Format("{0},{1},", row.OHTName, row.LogHours);
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},{1},{2},", row.GetItemCount_Avr(i) , row.GetItemCount_inAnHour(i),  row.GetItemCount(i));
                }

                sheetCont_Brief.Add(str);
                nLoopCnt++;
            }
           

            List<string> sheetCont_TotalErr = new List<string>();
            //sheetCont_OverItem.Add("=== 전체 OHT 에러 누적 현황 ===");
            nLoopCnt = 0;
            // 기준값 초과하는 항목 먼저 기록
            foreach (DMonitorResult.iRow row in obj.Rows)
            {
                if (nLoopCnt == 0)    // 첫줄 헤더 입력
                {
                    string strColHeader = "OHT,측정시간,";
                    for (int i = 0; i < row.Items.Count; i++)
                        strColHeader += row.GetItemName(i) + ",";

                    sheetCont_TotalErr.Add(strColHeader);
                }

                string str = String.Format("{0},{1},", row.OHTName, row.LogHours);
                for (int i = 0; i < row.Items.Count; i++)
                {
                    str += String.Format("{0},", row.GetItemCount(i));
                }

                sheetCont_TotalErr.Add(str);
                nLoopCnt++;
            }
            
            //sheetCont_Title.Add("");
            //sheetCont_Title.Add("");
            //sheetCont_Title.Add("=== 전체 RAW DATA ===");

            List<string> sheetCont_RawData = new List<string>();
            foreach (var t in obj.AllData)
            {
                sheetCont_RawData.Add(t);
            }


            try
            {
                Excel.Application ExcelAppObj;
                Excel.Workbook ExcelWorkBook;
                //Excel.Range ExcelRange;

                //Start Excel and get Application object.
                ExcelAppObj = new Excel.Application();
                //ExcelAppObj.Visible = true;

                //Get a new workbook.
                ExcelWorkBook = (Excel.Workbook)(ExcelAppObj.Workbooks.Add(Missing.Value));
                Excel.Worksheet ExSheet_RawData = ExcelWorkBook.Worksheets.Add(Type.Missing);
                Excel.Worksheet ExSheet_Brief = ExcelWorkBook.Worksheets.Add(Type.Missing);
                Excel.Worksheet ExSheet_TotalErr = ExcelWorkBook.Worksheets.Add(Type.Missing);                
                Excel.Worksheet ExSheet_OHTAvrEvent = ExcelWorkBook.Worksheets.Add(Type.Missing);
                Excel.Worksheet ExSheet_OHTMaxEvent = ExcelWorkBook.Worksheets.Add(Type.Missing);
                Excel.Worksheet ExSheet_OverItem = ExcelWorkBook.Worksheets.Add(Type.Missing);
                Excel.Worksheet ExSheet_Title = ExcelWorkBook.Worksheets.Add(Type.Missing);
                
                Point size;
                object[,] Cell;
                Excel.Range startCell;
                Excel.Range endCell;
                Excel.Range writeRange;

                // TITLE Sheet *************
                try
                {
                    Cell = GMethod.StringListToObjectCells(sheetCont_Title, out size);
                    startCell = ExSheet_Title.Cells[1, 1];
                    endCell = ExSheet_Title.Cells[size.Y, size.X];
                    writeRange = ExSheet_Title.Range[startCell, endCell];
                    writeRange.Value = Cell;
                    ExSheet_Title.Name = "Title";
                    ExSheet_Title.Columns.AutoFit();
                    
                    Cell = null;
                }
                catch (Exception)
                {
                }
                // Over Item Sheet *************
                try
                {
                    Cell = GMethod.StringListToObjectCells(sheetCont_OverItem, out size);
                    startCell = ExSheet_OverItem.Cells[1, 1];
                    endCell = ExSheet_OverItem.Cells[size.Y, size.X];
                    writeRange = ExSheet_OverItem.Range[startCell, endCell];
                    writeRange.Value = Cell;
                    ExSheet_OverItem.Name = "기준값 초과 내역";
                    ExSheet_OverItem.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

                try
                {
                    // OHT Max Event Sheet *************
                    Cell = GMethod.StringListToObjectCells(sheetCont_MaxErr, out size);
                    startCell = ExSheet_OHTMaxEvent.Cells[1, 1];
                    endCell = ExSheet_OHTMaxEvent.Cells[size.Y, size.X];
                    writeRange = ExSheet_OHTMaxEvent.Range[startCell, endCell];
                    writeRange.Value = Cell;
                    ExSheet_OHTMaxEvent.Name = "시간당 최대";
                    ExSheet_OHTMaxEvent.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

                try
                {
                    // OHT avr Event Sheet *************
                    Cell = GMethod.StringListToObjectCells(sheetCont_AvrErr, out size);
                    startCell = ExSheet_OHTAvrEvent.Cells[1, 1];
                    endCell = ExSheet_OHTAvrEvent.Cells[size.Y, size.X];
                    writeRange = ExSheet_OHTAvrEvent.Range[startCell, endCell];
                    writeRange.Value = Cell;
                    ExSheet_OHTAvrEvent.Name = "평균";
                    ExSheet_OHTAvrEvent.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

                // Total Error Sheet *************
                try
                {
                    Cell = GMethod.StringListToObjectCells(sheetCont_TotalErr, out size);
                    startCell = ExSheet_TotalErr.Cells[1, 1];
                    endCell = ExSheet_TotalErr.Cells[size.Y, size.X];
                    writeRange = ExSheet_TotalErr.Range[startCell, endCell];
                    writeRange.Value2 = Cell;
                    ExSheet_TotalErr.Name = "누적";
                    ExSheet_TotalErr.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

                try
                {
                    // OHT Brief Event Sheet *************
                    Cell = GMethod.StringListToObjectCells(sheetCont_Brief, out size);
                    startCell = ExSheet_Brief.Cells[1, 1];
                    endCell = ExSheet_Brief.Cells[size.Y, size.X];
                    writeRange = ExSheet_Brief.Range[startCell, endCell];
                    writeRange.Value = Cell;
                    ExSheet_Brief.Name = "에러평균, 시간당 최대, 누적값";
                    ExSheet_Brief.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

               

                //  RawData Sheet *************
                try
                {
                    Cell = GMethod.StringListToObjectCells(sheetCont_RawData, out size);
                    startCell = ExSheet_RawData.Cells[1, 1];
                    endCell = ExSheet_RawData.Cells[size.Y, size.X];
                    writeRange = ExSheet_RawData.Range[startCell, endCell];
                    writeRange.Value2 = Cell;
                    ExSheet_RawData.Name = "Raw Data";
                    ExSheet_RawData.Columns.AutoFit();
                    Cell = null;
                }
                catch (Exception)
                {
                }

                try
                {
                    ExcelWorkBook.SaveAs(path);
                    ExcelWorkBook.Close();
                }
                catch (Exception)
                {
                }
            }
            catch (Exception eee)
            {
                MessageBox.Show(eee.Message);
                //throw;
            }
        }
    }
    public class sUploadFile   // inner class
    {
        public string srcPath = "";
        public string targetPath = "";
        public void DeepCopy(sUploadFile t)
        {
            srcPath = t.srcPath;
            targetPath = t.targetPath;
        }
    }
    public class sDownloadFile   // inner class
    {
        public string OhtPath = "";	// 여기서
        public string LocalPath = "";		//여기로 파일 복사
        public void DeepCopy(sDownloadFile t)
        {
            OhtPath = t.OhtPath;
            LocalPath = t.LocalPath;
        }
    }
    public class sDownloadLog   // inner class
    {
		public bool bAllLog = false;
        public string strLogName = "";
        public string strLocalPos = "";
        public DLogTime logTime = new DLogTime();
    }
    public class sOHTList  // inner class
    {
        public bool bChecked = false;
        public string Name = "";
        public int nOHTNo = 0;
        public string IP = "";
        public string Result = "";
        public string Version = "";
        public string Ping = "";
        public string Memo = "";

        public void DeepCopy(sOHTList t)
        {
            bChecked = t.bChecked;
            Name = t.Name;
            nOHTNo = t.nOHTNo;
            IP = t.IP;
            Result = t.Result;
            Version = t.Version;
            Ping = t.Ping;
            Memo = t.Memo;
        }
    }

    // 모니터링 로그 결과 취함 클래스
    public class DMonitorResult
    {
        public List<string> AllData = new List<string>();
        public List<iRow> Rows = new List<iRow>();
        public List<string> itemNames = new List<string>();
        public string resultFilePath = "";
        public void Init()
        {
            AllData.Clear();
            Rows.Clear();
            itemNames.Clear();
            resultFilePath = "";
        }

        // 파일에서 한줄 파싱하여 알아서 카운트 추가까지 함
        public iRow Parsing_Add(string OhtName, string line)
        {
            iRow row = null;
            try
            {
                 row = Rows.Find(o => o.OHTName == OhtName);
                // 해당 OHT가 없다면 추가
                if (row == null)
                {
                    row = new iRow();
                    row.Init(itemNames);
                    row.OHTName = OhtName;
                    Rows.Add(row);  //new하고 바로 list에 추가
                }

                int nStartPos = line.IndexOf(']') + 1;
                if (nStartPos <= 0)
                    return row;
                string data = line.Substring(nStartPos, line.Length - nStartPos);
                string[] arr = data.Split('/');

                // HO/0/IA/0/IB/0/FC/0/SH/0/OC/0/ .... 있기 때문에 두개의 항목씩 건너뛰면서 이름을 찾아봄
                for (int i = 0; i < arr.Length; i += 2)
                {
                    if (arr[i].Length < 2)
                        continue;

                    // 사용자가 검사하겠다고 하지 않은 항목은 skip
                    if (itemNames.Find(o => o == arr[i]) == null)
                        continue;

                    int nVal = 0;
                    if (Int32.TryParse(arr[i + 1], out nVal) == false)
                        continue;

                    row.AddCount(arr[i], nVal);
                }
            }
            catch (Exception)
            {
            }
            return row;
        }

        public void AddMonitorItemDefine(string name)
        {
            itemNames.Add(name);
        }

        // 모니터링 항목 값 누적 시키기
        public void AddCountValue(string OhtName, string ItemName, int Val)
        {
            iRow obj = Rows.Find(o => o.OHTName == OhtName);

            // 새로운 OHT 항목
            if (obj == null)
            {
                obj = new iRow();
                obj.Init(itemNames);
                obj.OHTName = OhtName;
            }

            // 값 누적
            obj.AddCount(ItemName, Val);
        }

        // *************************** Inner Class
        public class iRow
        {
            public string OHTName = "";
            public List<iItem> Items = new List<iItem>();
            public List<int> ErrItemIdx = new List<int>();     // 기준값을 초과한 아이템의 인덱스(열번호)
            public int LogHours = 0;     // 모니터링 로그가 몇시간치 인지  ex) 5시간, 12시간

            // UC/UD/UF..... 등등 모든 아이템을 리스트로 만들어 입력해서 초기화 시킨다
            public void Init(List<string> allItems)
            {
                Items.Clear();

                foreach (string name in allItems)
                {
                    iItem a = new iItem();
                    a.name = name;
                    a.sum = 0;

                    Items.Add(a);
                }
            }
            public int GetItemCount(int nIdx)   //전체 개수
            {  
                if (nIdx >= Items.Count || nIdx < 0)
                    return 0;

                return Items[nIdx].sum;
            }
            public int GetItemCount_Avr(int nIdx)   //전체 개수
            {
                if (nIdx >= Items.Count || nIdx < 0)
                    return 0;
                int nRet = (int)(((double)Items[nIdx].sum / LogHours) + 0.5);
                return nRet;
            }
            public int GetItemCount_inAnHour(int nIdx)  // 시간당 개수(최대값)
            {
                if (nIdx >= Items.Count || nIdx < 0)
                    return 0;

                return Items[nIdx].max;

                //LogHours = Math.Max(1, LogHours);       // prevent zero div
                //return Items[nIdx].sum / LogHours;
            }
            public string GetItemName(int nIdx)
            {
                if (nIdx >= Items.Count || nIdx < 0)
                    return "";

                return Items[nIdx].name;
            }
            public void AddCount(string itemName, int val)
            {
                iItem obj = Items.Find(a => a.name == itemName);
				if(obj == null)
				{
					iItem a = new iItem();
					a.name = itemName;
					Items.Add(a);
					obj = a;
				}
                obj.sum += val;
                if (obj.max < val)
                    obj.max = val;
            }
        }

        public class iItem
        {
            public string name = "";
            public int sum = 0;
            public int max = 0;
        }
    }

    // 로그를 다운받거나 할때, 시간대를 지정할 수 있는데 이 시간대를 구조화함
    public class DLogTime
    {
        public enum eTimeZone
        {
            Range = 0, AfterAll, AllTime
        }
        public DLogTime() { }
        public eTimeZone opt = eTimeZone.Range;
        public DateTime From = new DateTime();
        public DateTime To = new DateTime();

        public bool IsIn(int nYear, int nMonth, int nDay, int nHour)
        {
            bool bRet = false;

            DateTime tm;
            try
            {
                tm = new DateTime(nYear, nMonth, nDay, nHour, 0, 0);
            }
            catch (Exception) { return false; } // 시간으로 파싱이 안되는 값이 넘어왔을 때
            switch (opt)
            {
                case eTimeZone.Range:

                    if (tm >= From && tm < To)
                        bRet = true;
                    break;

                case eTimeZone.AfterAll:
                    if (tm >= From)
                        bRet = true;
                    break;

                case eTimeZone.AllTime:
                    bRet = true;
                    break;
            }

            return bRet;
        }

    }
   
    
    public class MultiKeyDictionary<K1, K2, K3, V> : Dictionary<K1, MultiKeyDictionary<K2, K3, V>>
    {
        public V this[K1 key1, K2 key2, K3 key3]
        {
            get
            {
                return ContainsKey(key1) ? this[key1][key2, key3] : default(V);
            }
            set
            {
                if (!ContainsKey(key1))
                    this[key1] = new MultiKeyDictionary<K2, K3, V>();
                this[key1][key2, key3] = value;
            }
        }
        public bool ContainsKey(K1 key1, K2 key2, K3 key3)
        {
            return base.ContainsKey(key1) && this[key1].ContainsKey(key2, key3);
        }
        public void Add(K1 key1, K2 key2, K3 key3, V value)
        {
            if (!ContainsKey(key1))
                this[key1] = new MultiKeyDictionary<K2, K3, V>();
            this[key1][key2, key3] = value;
        }
       

    }
    public class MultiKeyDictionary<K1, K2, V> : Dictionary<K1, Dictionary<K2, V>>
    {
        public V this[K1 key1, K2 key2]
        {
            get
            {
                if (!ContainsKey(key1) || !this[key1].ContainsKey(key2))
                    throw new ArgumentOutOfRangeException();
                return base[key1][key2];
            }
            set
            {
                if (!ContainsKey(key1))
                    this[key1] = new Dictionary<K2, V>();
                this[key1][key2] = value;
            }
        }
        public void Add(K1 key1, K2 key2, V value)
        {
            if (!ContainsKey(key1))
                this[key1] = new Dictionary<K2, V>();
            this[key1][key2] = value;
        }
        public bool ContainsKey(K1 key1, K2 key2)
        {
            return base.ContainsKey(key1) && this[key1].ContainsKey(key2);
        }
        public new IEnumerable<V> Values
        {
            get
            {
                return from baseDict in base.Values
                       from baseKey in baseDict.Keys
                       select baseDict[baseKey];
            }
        }
    }

}
