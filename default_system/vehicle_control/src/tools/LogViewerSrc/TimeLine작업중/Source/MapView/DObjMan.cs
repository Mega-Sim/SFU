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
using System.IO;
using System.Threading;
namespace ObjectDrawig
{
    public class DObjMan
    {
        public static DObjArray ALL = new DObjArray();  // 아래 모든 어레이들을 모두 담고있음. 어차피 포인터들만 가리키는 것임

        // 각 속성별 객체들 집합
        public static DObjArray arrNode = new DObjArray();
        public static DObjArray arrLink = new DObjArray();
        public static DObjArray arrStation = new DObjArray();
        public static DObjArray arrCID = new DObjArray();
        public static DObjArray arrHID = new DObjArray();
        public static DObjArray arrCollision = new DObjArray();
        public static DObjArray arrFlowControl = new DObjArray();
        public static DObjArray arrTeaching = new DObjArray();
        public static DObjArray arrPathData = new DObjArray();
		public static DObjArray arrDistance = new DObjArray();

        public static string OrgMapPath = "";
        public static string PrjName = "";
        public static string LineName = "";
        public static string MapVersion = "";
        public static DateTime BuildDate = new DateTime();
        public static List<string> errMsg = new List<string>();
        public static List<string> errWarningMsg = new List<string>();
        public static bool bOnLoading = false;

        public static bool bLoadResult = false;
        public static int nCurLoadPercent = 0;
        static int nTotalLine = 0;
        static int nCurReadLine = 0;   // Loading percent 구하기 위함

        public static List<DHoverInfo> HoverInfoArr = new List<DHoverInfo>();
        public static List<string> strTotalErr = new List<string>();
        public const string DATA_MAIN_PATH = @"C:\.oht_easy_view_temporary";
        public static string DATA_DIR = ""; 

        public static void SetGraphicsScale(Graphics g, DScale scale)
        {
            arrNode.SetGraphicsScale(g, scale);
            arrLink.SetGraphicsScale(g, scale);
            arrStation.SetGraphicsScale(g, scale);
            arrCID.SetGraphicsScale(g, scale);
            arrHID.SetGraphicsScale(g, scale);
            arrCollision.SetGraphicsScale(g, scale); ;
            arrTeaching.SetGraphicsScale(g, scale);
            arrFlowControl.SetGraphicsScale(g, scale);
            arrPathData.SetGraphicsScale(g, scale);
			arrDistance.SetGraphicsScale(g, scale);
		}
        public static void Clear()
        {
            OrgMapPath = "";
            ALL.Clear();
            arrNode.Clear();
            arrLink.Clear();
            arrStation.Clear();
            arrCID.Clear();
            arrHID.Clear();
            arrCollision.Clear();
            arrFlowControl.Clear();
            arrTeaching.Clear();
            arrPathData.Clear();
			arrDistance.Clear();
			errMsg.Clear();
            errWarningMsg.Clear();

        }
        
        public static void LoadLayoutFile_Thread()
        {
            bOnLoading = true;
            var th = new Thread(LoadLayoutFile);
            th.Priority = ThreadPriority.Highest;
            th.Start();
            
        }

        // 한줄한줄 읽을 때마다 호출해서 Loading Percent 올리기
        public static void IncLoadPercent()
        {
            if (nTotalLine <= 0)
            {
                nCurLoadPercent = 0;    // 외부에서는 이 값을 참조함
                return;
            }
            nCurReadLine++;
            
            // 외부에서는 이 값을 참조함
            nCurLoadPercent = Math.Max(0, Math.Min(1000, (int)(((double)(nCurReadLine) / (double)nTotalLine) * 1000)));  // 0~ 100 범위 설정
        }
        // Layout File 및 data 파일 로딩~ 파싱~
        public static void LoadLayoutFile()
        {
            try
            {
                bOnLoading = true;
				nCurReadLine = 0;
				nTotalLine = 0;
                // Read File ALL **************************
                // 파일 로딩 부터 먼저 해야 맵 로딩 Percentage를 구하기 편리함
                string[] ocsLayoutLines = File.ReadAllLines(DATA_DIR + @"\OcsLayout.dat");
                string[] mapDataLines = File.ReadAllLines(DATA_DIR + @"\MAPDATA.TXT");
                string[] cidLines = File.ReadAllLines(DATA_DIR + @"\CID.TXT");
                string[] collisionLines = File.ReadAllLines(DATA_DIR + @"\COLLISION.TXT");
                string[] flowControlLines = File.ReadAllLines(DATA_DIR + @"\FLOWCONTROL.TXT");
                string[] teachingLines = File.ReadAllLines(DATA_DIR + @"\TEACHING.TXT");
                string[] pathDataLines = File.ReadAllLines(DATA_DIR + @"\PATHDATA.TXT");

                nTotalLine = ocsLayoutLines.Count() / 2 + mapDataLines.Count() + cidLines.Count() + collisionLines.Count() + flowControlLines.Count() + teachingLines.Count() + pathDataLines.Count();

                // *******************************************************
                // Read Header Info

                bool bReadStart = false;
                foreach (string line in ocsLayoutLines)
                {
                    if (line.Length < 1) continue;
                    if (line.ElementAt(0) != '[' && !bReadStart) continue;
                    if (line.ElementAt(0) == '[' && bReadStart)
                        break;

                    if (!bReadStart)
                    {
                        if (line == "[VERSION]")
                            bReadStart = true;
                        continue;
                    }

                    IncLoadPercent();
                    // 헤더정보 읽기 시작
                    try
                    {
                        if (line.IndexOf("PROJECT = ") == 0)
                        {
                            string tmpLine = line.Replace("PROJECT = ", "");
                            PrjName = tmpLine;
                            continue;
                        }
                        else if (line.IndexOf("LINENAME = ") == 0)
                        {
                            string tmpLine = line.Replace("LINENAME = ", "");
                            LineName = tmpLine;
                            continue;
                        }
                        else if (line.IndexOf("VERSION = ") == 0)
                        {
                            string tmpLine = line.Replace("VERSION = ", "");
                            MapVersion = tmpLine;
                            continue;
                        }
                        else if (line.IndexOf("BUILD_DATE = ") == 0)
                        {
                            string tmpLine = line.Replace("BUILD_DATE = ", "");
                            if (tmpLine.Length != 14)
                                continue;
                            int nYear = Int32.Parse(tmpLine.Substring(0, 4));
                            int nMonth = Int32.Parse(tmpLine.Substring(4, 2));
                            int nDay = Int32.Parse(tmpLine.Substring(6, 2));
                            int nHour = Int32.Parse(tmpLine.Substring(8, 2));
                            int nMin = Int32.Parse(tmpLine.Substring(10, 2));
                            int nSec = Int32.Parse(tmpLine.Substring(12, 2));
                            BuildDate = new DateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
                            continue;
                        }
                    }
                    catch (Exception)
                    {
                        continue;
                    }
                }

                int nFailCnt_Node = 0;
                int nFailCnt_NodeMapData = 0;
                int nFailCnt_Link = 0;
                int nFailCnt_Station = 0;
                int nFailCnt_Cid = 0;
                int nFailCnt_Collision = 0;
                int nFailCnt_FlowControl = 0;
                int nFailCnt_Teaching = 0;
                int nFailCnt_PathData = 0;

                // *******************************************************
                // Read HID 
                bReadStart = false;
                int nFailCnt_HID = 0;

                foreach (string line in ocsLayoutLines)
                {

                    if (line.Length < 1) continue;
                    if (line.ElementAt(0) != '[' && !bReadStart) continue;
                    if (line.ElementAt(0) == '[' && bReadStart)
                        break;

                    if (!bReadStart)
                    {
                        if (line == "[HID]")
                            bReadStart = true;
                        continue;
                    }
                    IncLoadPercent();
                    if (DObjHID.ParseLayoutFileLine(line, arrHID) == false)
                        nFailCnt_HID++;
                }

                // *******************************************************
                // Read Node XYZ Layout Position 
                bReadStart = false;
                foreach (string line in ocsLayoutLines)
                {

                    if (line.Length < 1) continue;
                    if (line.ElementAt(0) != '[' && !bReadStart) continue;
                    if (line.ElementAt(0) == '[' && bReadStart) break;

                    if (!bReadStart)
                    {
                        if (line == "[NODE]")
                            bReadStart = true;
                        continue;
                    }

                    IncLoadPercent();
                    if (DObjNode.ParseLayoutFileLine(line, arrNode) == false)
                        nFailCnt_Node++;
                }
                //Trace.Write("\n\n");
                //foreach (DObjBase obj in arrNode)
                //    Trace.Write(String.Format("{0},  ", obj.GetID_Int()));
                // 모든 노드 소팅
                arrNode.Sort(delegate (DObjBase a, DObjBase b) { return a.CompareTo(b); });

                //Trace.Write("\n\n");
                //foreach (DObjBase obj in arrNode)
                //    Trace.Write(String.Format("{0},  ", obj.GetID_Int()));

                // *******************************************************
                // Read Node Map Data           
                foreach (string line in mapDataLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjNode.ParseMapDataFile(line, arrNode) == false)
                        nFailCnt_NodeMapData++;
                }

                // *******************************************************
                // Read Link
                bReadStart = false;
                foreach (string line in ocsLayoutLines)
                {
                    if (line.Length < 1) continue;


                    if (line.ElementAt(0) != '[' && !bReadStart) continue;
                    if (line.ElementAt(0) == '[' && bReadStart) break;

                    if (!bReadStart)
                    {
                        if (line == "[LINK]")
                            bReadStart = true;
                        continue;
                    }
                    IncLoadPercent();
                    if (DObjLink.ParseLayoutFileLine(line, arrLink) == false)
                        nFailCnt_Link++;
                }

                // *******************************************************
                // Read Station ---> 시간이 오래걸리는 작업
                bReadStart = false;
                foreach (string line in ocsLayoutLines)
                {
                    if (line.Length < 1) continue;
                    if (line.ElementAt(0) != '[' && !bReadStart) continue;
                    if (line.ElementAt(0) == '[' && bReadStart) break;

                    if (!bReadStart)
                    {
                        if (line == "[STATION]")
                            bReadStart = true;
                        continue;
                    }
                    IncLoadPercent();
                    if (!DObjStation.ParseLayoutFileLine(line, arrStation))
                        nFailCnt_Station++;
                }

                // 모든 스테이션 소팅
                arrStation.Sort(delegate (DObjBase a, DObjBase b) { return a.CompareTo(b); });

                // *******************************************************
                // Read CID
                foreach (string line in cidLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjCID.ParseMapDataCIDLine(line, arrCID) == false)
                        nFailCnt_Cid++;
                }
                foreach (DObjCID cid in DObjMan.arrCID)
                    cid.FindNodeInfo();         // 번호만 알고있는 노드들을 실제 노드객체를 참조하기


                // *******************************************************
                // Read Collisoin
                foreach (string line in collisionLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjCollision.ParseLayoutFileLine(line, arrCollision) == false)
                        nFailCnt_Collision++;
                }


                // *******************************************************
                // Read FlowControl
                foreach (string line in flowControlLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjFlowControl.ParseLayoutFileLine(line, arrFlowControl) == false)
                    {
                        nFailCnt_FlowControl++;
                    }
                }


                // *******************************************************
                // Read Teaching            
                foreach (string line in teachingLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjTeaching.ParseLayoutFileLine(line, arrTeaching) == false)
                        nFailCnt_Teaching++;
                }

                // *******************************************************
                // Read PathData            
                foreach (string line in pathDataLines)
                {
                    if (line.Length < 1) continue;
                    IncLoadPercent();
                    if (DObjPathData.ParseLayoutFileLine(line, arrPathData) == false)
                        nFailCnt_PathData++;
                }


                // *******************************************************
                // Hid들 소속된 link 찾기
                foreach (DObjHID hid in DObjMan.arrHID)
                    hid.FindMyLink(DObjMan.arrLink);

                // *******************************************************
                // Link들 소속된 Collision Info 갱신  ---->  시간이 걸리는 작업
                foreach (DObjLink link in DObjMan.arrLink)
                    link.MakeCollisionInfoText(DObjMan.arrCollision);

                DObjLink.CalcAverageLinkLength();    // 링크 길이 평균 구하기. 나중에 보기 좋은 scale 값을 구할 떄 쓰이기 때문에
                
                arrCID.VisibleAll(false);
                arrHID.VisibleAll(false);
                arrPathData.VisibleAll(false);

                ALL.AddRange(arrNode);
                ALL.AddRange(arrStation);
                ALL.AddRange(arrCID);
                ALL.AddRange(arrCollision);
                ALL.AddRange(arrLink);
                ALL.AddRange(arrHID);
                ALL.AddRange(arrFlowControl);
                ALL.AddRange(arrTeaching);
                ALL.AddRange(arrTeaching);

                strTotalErr.Clear();
                int nErrCntSum = nFailCnt_Node + nFailCnt_NodeMapData + nFailCnt_Link + nFailCnt_Station + nFailCnt_Cid + nFailCnt_Collision + nFailCnt_FlowControl + nFailCnt_Teaching;
                if (nErrCntSum > 0) strTotalErr.Add("\r\n############### Error Count ###############\r\n");
                if (nFailCnt_Node > 0) strTotalErr.Add(String.Format("Read Fail Node Count : {0}", nFailCnt_Node));
                if (nFailCnt_NodeMapData > 0) strTotalErr.Add(String.Format("Read Fail Node Count : {0}", nFailCnt_NodeMapData));
                if (nFailCnt_Link > 0) strTotalErr.Add(String.Format("Read Fail Node Map Count : {0}", nFailCnt_Link));
                if (nFailCnt_Station > 0) strTotalErr.Add(String.Format("Read Fail Station Count : {0}", nFailCnt_Station));
                if (nFailCnt_Cid > 0) strTotalErr.Add(String.Format("Read Fail CID Count : {0}", nFailCnt_Cid));
                if (nFailCnt_Collision > 0) strTotalErr.Add(String.Format("Read Fail Collision Count : {0}", nFailCnt_Collision));
                if (nFailCnt_FlowControl > 0) strTotalErr.Add(String.Format("Read Fail FlowControl Count : {0}", nFailCnt_FlowControl));
                if (nFailCnt_Teaching > 0) strTotalErr.Add(String.Format("Read Fail Teaching Count : {0}", nFailCnt_Teaching));
                if (nFailCnt_PathData > 0) strTotalErr.Add(String.Format("Read Fail PathData Count : {0}", nFailCnt_PathData));

                // 에러 내용 txt로 파일 표시
                File.Delete("Load_Fail_Result.txt");

                if (strTotalErr.Count > 0 || errMsg.Count > 0 || errWarningMsg.Count > 0)
                {
                    strTotalErr.Insert(0, "### Wrong File Path : " + DObjMan.OrgMapPath);
                    strTotalErr.Insert(0, "### Loading Time: " + DateTime.Now.ToLongTimeString());

                    if (errWarningMsg.Count > 0)
                        errWarningMsg.Insert(0, "\r\n############### Warning ###############\r\n");
                    if (errMsg.Count > 0)
                        errMsg.Insert(0, "\r\n############### Error Detail ###############\r\n");

                    strTotalErr.AddRange(errWarningMsg);
                    strTotalErr.AddRange(errMsg);
                }

                //if (strTotalErr.Count > 0)
                //{
                //    File.AppendAllLines("Load_Fail_Result.txt", strTotalErr);
                //    Process.Start("notepad.exe", "Load_Fail_Result.txt");
                //}

                nCurLoadPercent = 1000;
                bLoadResult = strTotalErr.Count == 0;
                bOnLoading = false;
            }
            catch (Exception exp)
            {
                MessageBox.Show("맵 로딩중 에러 발생\n\n" + exp.Message);
                return;
            }
        }

        // CID와 연계된 노드 세부정보까지 미리 파악해 놓음
        //static void MakeNodeDetailText()
        //{
        //    foreach (DObjNode node in DObjMan.arrNode)
        //    {
        //        foreach (DObjFlowControl f in DObjMan.arrFlowControl)
        //        {
        //            if (node.ID == f.StopNodeID || node.ID == f.ResetNodeMain)
        //            {
        //                node.bIsFlowControl = true;
        //                break;
        //            }

        //        }
        //    }
        //}
        static void QuickSortNode(DObjBase []arr, int left, int right)
        {
            DObjMan.arrNode.Sort();
            int L = left;
            int R = right;
            int pivot = arr[(left + right) / 2].GetID_Int();
            while(L<R)
            {
                while (arr[L].GetID_Int() < pivot) L++;
                while (arr[R].GetID_Int() > pivot) R--;

                if(L<=R)
                {
                    if(L<R)
                    {

                    }
                    L++; R--;
                }

            }
        }
    }


    public class DHoverInfo
    {
        public string strInfo = "";
        public Point pos_dc = new Point();
    }

    // 테스트 용도로 추출하기 위해서 
    public class DCustomExtract
    {

        static private List<DObjNode> arrBungiNode = new List<DObjNode>();
        static private List<DObjNode> arrJikJiNode = new List<DObjNode>();

        private static void GetBugiNoBungi()
        {
            arrBungiNode.Clear();
            arrJikJiNode.Clear();

            List<DObjNode> nodeArr = new List<DObjNode>();
            
            foreach (DObjBase b in DObjMan.arrNode)
                nodeArr.Add((DObjNode)b);

            // 분기 노드와 분기 아닌 노드 찾아냄
            arrBungiNode = nodeArr.FindAll(o => o.rightNodeID > 0);
            arrJikJiNode = nodeArr.FindAll(o => o.rightNodeID == 0);
            arrJikJiNode.RemoveAll(o => o.leftNodeID == 0);
        }
        public static bool IsBugiNode(DObjNode node)
        {
            return node.rightNodeID > 0;
        }
        
        public static bool IsHapRuNode(int nodeID)
        {
            int nCnt = 0;
            foreach (DObjBase b in DObjMan.arrNode)
            {
                DObjNode obj = (DObjNode)b;

                if (obj.leftNodeID == nodeID)
                    nCnt++;
                if (obj.rightNodeID == nodeID)
                    nCnt++;

                if (nCnt > 1)   // 연결된 링크가 한개가 아니라는 것은 합류구간이란 뜻
                    return true;
            }
            return false;
        }

        // MTL 투입구간 같이 어느 노드에서 연결되지 않고 홀로 시작하는 노드
        public static bool IsFirstNode(int nodeID)
        {
            int nCnt = 0;
            foreach (DObjBase b in DObjMan.arrNode)
            {
                DObjNode obj = (DObjNode)b;

                if (obj.leftNodeID == nodeID)
                    nCnt++;
                if (obj.rightNodeID == nodeID)
                    nCnt++;
            }
            return nCnt == 0;
        }
      

        // 직선구간 노드만 찾아서 파일 저장
        // 분기 없고, 합류 없고
        // 4개 노드이상 그런 분기x 합류x 노드이고
        // 5000mm 이상 길이일때
        public static void ExtractSpecialNode(string path)
        {
            List<DObjNode> arrResult = new List<DObjNode>();
            const int LENGTH = 12000;   //12m
            GetBugiNoBungi();

            foreach (DObjNode curNode in arrJikJiNode)
            {

                if (IsHapRuNode(curNode.ID)) continue;
                int XPos = curNode.X_mm;
                int YPos = curNode.Y_mm;

                // 현재 노드
                DObjNode nodeA = arrJikJiNode.Find(o => o.ID == curNode.leftNodeID);
                
                if (nodeA == null) continue;            // 직진노드 목록에 없음
                if (nodeA.rightNodeID > 0) continue; // 분기 skip
                if(XPos != nodeA.X_mm && YPos != nodeA.Y_mm) continue;  // 도면상 직진구간만
                if (IsHapRuNode(nodeA.ID)) continue;    // 합류 skip
                

                // 첫번째 노드
                DObjNode nodeB = arrJikJiNode.Find(o => o.ID == nodeA.leftNodeID);

                if (nodeB == null) continue;            // 직진노드 목록에 없음
                if (nodeB.rightNodeID > 0) continue; // 분기 skip
                if (XPos != nodeB.X_mm && YPos != nodeB.Y_mm) continue;  // 도면상 직진구간만
                if (IsHapRuNode(nodeB.ID)) continue;    // 합류 skip

                // 두번째 노드
                DObjNode nodeC = arrJikJiNode.Find(o => o.ID == nodeB.leftNodeID);

                if (nodeC == null) continue;            // 직진노드 목록에 없음
                if (nodeC.rightNodeID > 0) continue; // 분기 skip
                if (XPos != nodeC.X_mm && YPos != nodeC.Y_mm) continue;  // 도면상 직진구간만
                if (IsHapRuNode(nodeC.ID)) continue;    // 합류 skip

                int nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist;
                DObjNode nodeD = null;
                if (nLength > LENGTH)
                {
                    curNode.reserved_data = nLength;
                    arrResult.Add(curNode);
                }
                else
                {
                    // 세번째 노드
                    nodeD = arrJikJiNode.Find(o => o.ID == nodeC.leftNodeID);

                    if (nodeD == null) continue;            // 직진노드 목록에 없음
                    if (nodeD.rightNodeID > 0) continue; // 분기 skip
                    if (XPos != nodeD.X_mm && YPos != nodeD.Y_mm) continue;  // 도면상 직진구간만
                    if (IsHapRuNode(nodeD.ID)) continue;    // 합류 skip

                    nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist;

                    if (nLength > LENGTH)
                    {
                        curNode.reserved_data = nLength;
                        arrResult.Add(curNode);
                    }
                    else
                    {   
                        // 네번째 노드
                        DObjNode nodeE = arrJikJiNode.Find(o => o.ID == nodeD.leftNodeID);

                        if (nodeE == null) continue;            // 직진노드 목록에 없음
                        if (nodeE.rightNodeID > 0) continue; // 분기 skip
                        if (XPos != nodeE.X_mm && YPos != nodeE.Y_mm) continue;  // 도면상 직진구간만
                        if (IsHapRuNode(nodeE.ID)) continue;    // 합류 skip

                        nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist;
                        // nLength 가 5000이 안되도 직선구간으로 보자
                        if (nLength > LENGTH)
                        {
                            curNode.reserved_data = nLength;
                            arrResult.Add(curNode);
                        }
                        else
                        {
                            // 5번째 노드
                            DObjNode nodeF = arrJikJiNode.Find(o => o.ID == nodeE.leftNodeID);

                            if (nodeF == null) continue;            // 직진노드 목록에 없음
                            if (nodeF.rightNodeID > 0) continue; // 분기 skip
                            if (XPos != nodeF.X_mm && YPos != nodeF.Y_mm) continue;  // 도면상 직진구간만
                            if (IsHapRuNode(nodeF.ID)) continue;    // 합류 skip

                            nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist;
                            // nLength 가 5000이 안되도 직선구간으로 보자
                            if (nLength > LENGTH)
                            {
                                curNode.reserved_data = nLength;
                                arrResult.Add(curNode);
                            }
                            else
                            {
                                // 6번째 노드
                                DObjNode nodeG = arrJikJiNode.Find(o => o.ID == nodeF.leftNodeID);

                                if (nodeG == null) continue;            // 직진노드 목록에 없음
                                if (nodeG.rightNodeID > 0) continue; // 분기 skip
                                if (XPos != nodeG.X_mm && YPos != nodeG.Y_mm) continue;  // 도면상 직진구간만
                                if (IsHapRuNode(nodeG.ID)) continue;    // 합류 skip

                                nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist;
                                // nLength 가 5000이 안되도 직선구간으로 보자
                                if (nLength > LENGTH)
                                {
                                    curNode.reserved_data = nLength;
                                    arrResult.Add(curNode);
                                }
                                else
                                {
                                    // 7번째 노드
                                    DObjNode nodeH = arrJikJiNode.Find(o => o.ID == nodeG.leftNodeID);

                                    if (nodeH == null) continue;            // 직진노드 목록에 없음
                                    if (nodeH.rightNodeID > 0) continue; // 분기 skip
                                    if (XPos != nodeH.X_mm && YPos != nodeH.Y_mm) continue;  // 도면상 직진구간만
                                    if (IsHapRuNode(nodeH.ID)) continue;    // 합류 skip

                                    nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist + nodeH.leftNodeDist;
                                    // nLength 가 5000이 안되도 직선구간으로 보자
                                    if (nLength > LENGTH)
                                    {
                                        curNode.reserved_data = nLength;
                                        arrResult.Add(curNode);
                                    }
                                    else
                                    {
                                        // 8번째 노드
                                        DObjNode nodeI = arrJikJiNode.Find(o => o.ID == nodeH.leftNodeID);

                                        if (nodeI == null) continue;            // 직진노드 목록에 없음
                                        if (nodeI.rightNodeID > 0) continue; // 분기 skip
                                        if (XPos != nodeI.X_mm && YPos != nodeI.Y_mm) continue;  // 도면상 직진구간만
                                        if (IsHapRuNode(nodeI.ID)) continue;    // 합류 skip

                                        nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist + nodeH.leftNodeDist + nodeI.leftNodeDist;
                                        // nLength 가 5000이 안되도 직선구간으로 보자
                                        if (nLength > LENGTH)
                                        {
                                            curNode.reserved_data = nLength;
                                            arrResult.Add(curNode);
                                        }
                                        else
                                        {
                                            // 9번째 노드
                                            DObjNode nodeJ = arrJikJiNode.Find(o => o.ID == nodeI.leftNodeID);

                                            if (nodeJ == null) continue;            // 직진노드 목록에 없음
                                            if (nodeJ.rightNodeID > 0) continue; // 분기 skip
                                            if (XPos != nodeJ.X_mm && YPos != nodeJ.Y_mm) continue;  // 도면상 직진구간만
                                            if (IsHapRuNode(nodeJ.ID)) continue;    // 합류 skip

                                            nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist + 
                                                        nodeH.leftNodeDist + nodeI.leftNodeDist + nodeJ.leftNodeDist;
                                            // nLength 가 5000이 안되도 직선구간으로 보자
                                            if (nLength > LENGTH)
                                            {
                                                curNode.reserved_data = nLength;
                                                arrResult.Add(curNode);
                                            }
                                            else
                                            {
                                                // 10번째 노드
                                                DObjNode nodeK = arrJikJiNode.Find(o => o.ID == nodeJ.leftNodeID);

                                                if (nodeK == null) continue;            // 직진노드 목록에 없음
                                                if (nodeK.rightNodeID > 0) continue; // 분기 skip
                                                if (XPos != nodeK.X_mm && YPos != nodeK.Y_mm) continue;  // 도면상 직진구간만
                                                if (IsHapRuNode(nodeK.ID)) continue;    // 합류 skip

                                                nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist +
                                                            nodeH.leftNodeDist + nodeI.leftNodeDist + nodeJ.leftNodeDist + nodeK.leftNodeDist;
                                                // nLength 가 5000이 안되도 직선구간으로 보자
                                                if (nLength > LENGTH)
                                                {
                                                    curNode.reserved_data = nLength;
                                                    arrResult.Add(curNode);
                                                }
                                                else
                                                {
                                                    // 11번째 노드
                                                    DObjNode nodeL = arrJikJiNode.Find(o => o.ID == nodeK.leftNodeID);

                                                    if (nodeL == null) continue;            // 직진노드 목록에 없음
                                                    if (nodeL.rightNodeID > 0) continue; // 분기 skip
                                                    if (XPos != nodeL.X_mm && YPos != nodeL.Y_mm) continue;  // 도면상 직진구간만
                                                    if (IsHapRuNode(nodeL.ID)) continue;    // 합류 skip

                                                    nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist +
                                                                nodeH.leftNodeDist + nodeI.leftNodeDist + nodeJ.leftNodeDist + nodeK.leftNodeDist + nodeL.leftNodeDist;
                                                    // nLength 가 5000이 안되도 직선구간으로 보자
                                                    if (nLength > LENGTH)
                                                    {
                                                        curNode.reserved_data = nLength;
                                                        arrResult.Add(curNode);
                                                    }
                                                    else
                                                    {
                                                        // 12번째 노드
                                                        DObjNode nodeM = arrJikJiNode.Find(o => o.ID == nodeL.leftNodeID);

                                                        if (nodeM == null) continue;            // 직진노드 목록에 없음
                                                        if (nodeM.rightNodeID > 0) continue; // 분기 skip
                                                        if (XPos != nodeM.X_mm && YPos != nodeM.Y_mm) continue;  // 도면상 직진구간만
                                                        if (IsHapRuNode(nodeM.ID)) continue;    // 합류 skip

                                                        nLength = curNode.leftNodeDist + nodeA.leftNodeDist + nodeB.leftNodeDist + nodeC.leftNodeDist + nodeD.leftNodeDist + nodeE.leftNodeDist + nodeF.leftNodeDist + nodeG.leftNodeDist +
                                                                    nodeH.leftNodeDist + nodeI.leftNodeDist + nodeJ.leftNodeDist + nodeK.leftNodeDist + nodeL.leftNodeDist + nodeM.leftNodeDist;
                                                        // nLength 가 5000이 안되도 직선구간으로 보자
                                                        if (nLength > LENGTH)
                                                        {
                                                            curNode.reserved_data = nLength;
                                                            arrResult.Add(curNode);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
            }

            string strTxt = "";
            int nMin = Int32.MaxValue;
            int nMinIdx = -1;
            int i = 0;
            foreach(DObjNode node in arrResult)
            {
                strTxt += String.Format("{0}\n", node.ID);
                if(node.leftNodeDist < nMin)
                {
                    nMin = node.reserved_data;
                    nMinIdx = i;
                }
                i++;
            }

            if (arrResult.Count > 0)
                File.WriteAllText(path, strTxt);
            else
            {
                MessageBox.Show("직진 노드 찾지 못함");
                return;
            }

            MessageBox.Show(String.Format("Nodes are written in file. Count={0}", arrResult.Count));
            if(nMinIdx >= 0)
            {
                MessageBox.Show(String.Format("Min: {0}mm, Node: {1}", nMin, arrResult[nMinIdx].ID));
            }

        }
        
    }
}
