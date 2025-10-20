using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using MathGeometry2D;
using ObjectDrawig;
using System.IO;
namespace MapView
{
    // Node 상속받은 Snake용 Ex 클래스 정의
    // 해당 노드가 합류인지, 분기인지, 기타 정보를 담고 있는다
    public class DObjNodeEx : DObjNode
    {
        public bool bIsHapryu = false;
        public int nHapruyMinSpd = 0;
        public bool bIsBungi = false;
        public bool bIsFirstNode = false;
        public bool bIsLastNode = false;
        //public bool bIsCSnode = false;
        //public bool bIsStopNode = false;
        //public bool bIsResetNode = false;
        //public bool bIsFlowControl = false;

        //public int NewSpeed = 0;
        //public int NewSpeed_raw = 0;
        //public int NewCalDist = 0;

        public int nParentNodeArrIdx = 0;
        
        public bool IsPureNode()
        {
            return!bIsCSnode && !bIsStopNode;
            //return !bIsHapryu && !bIsBungi && !bIsFirstNode && !bIsLastNode && !bIsCSnode && !bIsStopNode && !bIsResetNode;
        }
    }

    public class DGugan
    {
        public List<DObjNodeEx> Nodes = new List<DObjNodeEx>();
        public DGugan() { }

        // Snake 가운데 노드중 N조향인 노드 개수와 N조향의 총 길이
        public void GetNoDirNodeCnt(out int Cnt, out int Distance)
        {
            int nCnt = 0;
            int nDist = 0;
            for (int i = 0; i < Nodes.Count - 1; i++)
            {
                DObjNodeEx nodeA = Nodes[i];
                DObjNodeEx nodeB = Nodes[i + 1];

                bool bLeftDir = nodeA.leftNodeID == nodeB.ID;
                
                eSteering st = bLeftDir ? nodeA.leftSteering : nodeA.rightSteering;

                if (nCnt > 0 && !nodeA.IsPureNode() && st != eSteering.NoDirection)    // 연속되는 N 조향은 끝
                    break;

                if (st == eSteering.NoDirection && nodeA.IsPureNode())
                {
                    nDist += (bLeftDir ? nodeA.leftNodeDist : nodeA.rightNodeDist);
                    nCnt++;
                }
            }
            Cnt = nCnt;
            Distance = nDist;
        }

        public static int Rule1Cnt = 0;
        public static int Rule2Cnt = 0;
        public static int Rule3Cnt = 0;

        public void SpeedUp()
        {
            if (Nodes.Count < 2)
                return;
            
            int nSteerCnt = 0;
            int nSteerDist = 0;

            //N조향 개수와, 그 길이를 알아낸다
            GetNoDirNodeCnt(out nSteerCnt, out nSteerDist);

            if (nSteerCnt == 0)
                return;

            int Cnt = SpeedUp_Rule3();
            Rule3Cnt += Cnt;
            if (Cnt > 0)
                return;
            
            Cnt = SpeedUp_Rule1();
            Rule1Cnt += Cnt;

            if (Cnt > 0)
                return;

            Cnt = SpeedUp_Rule2();
            Rule2Cnt += Cnt;

            if (Cnt > 0)
                return;
        }

        // 조건:
        // 시작노드: 합류Reset노드 + CommonStart 아님 + N조향
        // 으로 시작하면 이 노드부터 구간끝까지, CID관련된 노드만 아니면 속도5
        public int SpeedUp_Rule1()
        {
            int nUpCnt = 0;
            bool bSpeedUpStart = false;

            for (int i=0; i<Nodes.Count; i++)
            {
                bool bLeftDir = true;
                if (i == 0)
                {
                     bLeftDir = Nodes[0].leftNodeID == Nodes[1].ID;
                }
                

                eSteering steer = bLeftDir ? Nodes[i].leftSteering : Nodes[i].rightSteering;
                if (Nodes[i].bIsResetNode && steer == eSteering.NoDirection && !Nodes[i].bIsCSnode)
                {
                    bSpeedUpStart = true;
                }

                //if(Nodes[i].bIsCSnode || Nodes[i].bIsStopNode)
                //{
                //    bSpeedUpStart = false;
                //}

                if(bSpeedUpStart && steer == eSteering.NoDirection && !Nodes[i].bIsCSnode && !Nodes[i].bIsStopNode)
                {
                    // firedoor case skip
                    if (Nodes[i].bIsFlowControl && !Nodes[i].bIsCSnode && !Nodes[i].bIsStopNode && !Nodes[i].bIsResetNode)
                        continue;

                    // normal node only
                    if (Nodes[i].nodeType != eNodeType.Normal)
                        continue;

                    Nodes[i].NewSpeed = 5000;
                    nUpCnt++;
                }
            }
            return nUpCnt;
        }

        // 조건:
        // 조향L or R -> N : 속도5
        public int SpeedUp_Rule2()
        {
            int nUpCnt = 0;
            for (int i = 0; i < Nodes.Count; i++)
            {
                bool bLeftDir = true;
                if (i == 0)
                {
                    bLeftDir = Nodes[0].leftNodeID == Nodes[1].ID;
                }
                DObjNodeEx Cur = Nodes[i];
                eSteering steer = bLeftDir ? Cur.leftSteering : Cur.rightSteering;
                
                if(steer == eSteering.NoDirection && i > 0 && !Cur.bIsCSnode && !Cur.bIsFlowControl && !Cur.bIsResetNode && !Cur.bIsStopNode)
                {
                    eSteering prevSteer;
                    if (i - 1 == 0)
                        prevSteer = bLeftDir ? Nodes[0].leftSteering : Nodes[0].rightSteering;
                    else
                        prevSteer = Nodes[i - 1].leftSteering;

                    if(prevSteer != eSteering.NoDirection)
                    {
                        for(int k=i; k<Nodes.Count; k++)
                        {
                            // firedoor case skip
                            if (Nodes[k].bIsFlowControl && !Nodes[k].bIsCSnode && !Nodes[k].bIsStopNode && !Nodes[k].bIsResetNode)
                                continue;

                            if (Nodes[k].bIsCSnode || Nodes[k].bIsStopNode || Nodes[k].bIsResetNode || Nodes[k].bIsBungi || Nodes[k].bIsHapryu)
                                break;

                            if (Nodes[k].leftSteering != eSteering.NoDirection)
                                continue;

                            // normal node only
                            if (Nodes[i].nodeType != eNodeType.Normal)
                                continue;

                            Nodes[k].NewSpeed = 5000;
                            nUpCnt++;
                        }
                        return nUpCnt;
                    }
                }
            }
            return nUpCnt;
        }

        // 식계산 해서 넣는 방법
        public int SpeedUp_Rule3()
        {
            // 시작 : 분기, 합류, 조향O, 조향x
            // 중간 : N
            // 끝 전: CIDx, CIDo, 조향 O, 조향x
            // 끝 : 합류, 분기

            int nUpCnt = 0;

            int nDist = 0;
            bool bPureNodeStart = false;
            List<int> pureNodeList = new List<int>();
                        
            for (int i = 0; i < Nodes.Count - 1; i++)
            {
                DObjNodeEx node = Nodes[i];
                DObjNodeEx nextNode = Nodes[i + 1];
                bool bLeftDir = node.leftNodeID == nextNode.ID;


                eSteering st = bLeftDir ? node.leftSteering : node.rightSteering;

                

                bool bPureNode = node.IsPureNode() && (st == eSteering.NoDirection);

                if (!bPureNodeStart && bPureNode)
                {
                    bPureNodeStart = true;  // CID도, 합류도, 분기도 아닌 순수한 N조향 노드 시작
                    nDist = 0;
                    pureNodeList.Clear();
                }

                if (bPureNode)
                {
                    nDist += bLeftDir ? node.leftNodeDist : node.rightNodeDist; // Pure노드의 거리 합
                    pureNodeList.Add(i);
                }

                if (bPureNodeStart && !bPureNode)
                {
                    bPureNodeStart = false; // Pure Node가 계속 나오다가, 아닌노드가 등장. 
                    nUpCnt += SpeedUp_Algo(pureNodeList, nDist);      //-> 알고리즘 적용하여 증속
                }
            }
            return nUpCnt;
        }

        public int SpeedUp_Algo(List<int> PureNodeList, int nDist)
        {
            if (PureNodeList.Count == 0)
                return 0;

            if (PureNodeList[0] == 0)   // 이상한 경우
                return 0;

            int nUpCnt = 0;

            double dDist_m = nDist / 1000.0;
            double dEnterSpeed_mps = 0;
            double dOutSpeed_mps = 0;
            double dAcc_mps2 = 2;
            double dAcc2_mps2 = 1;
            double dDeAcc_mps2 = 3;
            double dConVelMove_sec = 1;
            double dSpd33_mps = 3.3;

            int nNodeIdx = PureNodeList[0] - 1;
            if (Nodes[nNodeIdx].bIsHapryu)
                dEnterSpeed_mps = Nodes[nNodeIdx].nHapruyMinSpd;
            else
            {
                bool bLeftDir = Nodes[nNodeIdx].leftNodeID == Nodes[nNodeIdx + 1].ID;
                dEnterSpeed_mps = bLeftDir ? Nodes[nNodeIdx].leftNodeSpeed : Nodes[nNodeIdx].rightNodeSpeed;
            }

            int nLastIdx = PureNodeList[PureNodeList.Count - 1] + 1;
            if (nLastIdx >= Nodes.Count - 1)
                return 0;

            bool bCID = Nodes[nLastIdx].bIsCSnode || Nodes[nLastIdx].bIsStopNode || Nodes[nLastIdx].bIsResetNode;
            bool bJoHyang = Nodes[nLastIdx].leftSteering != eSteering.NoDirection;

            // Execel 내에 있는 구간내 Max 속도 구하는 식
            double dAdaptedSpeed = 0;
            double dAnswer = 0;

            //if (bJoHyang)
            //{
            //    dAdaptedSpeed = 5;  // 5 m/s
            //}
            //else
            {
                int nStIdx = Nodes[PureNodeList[0] - 1].bIsBungi ? PureNodeList[0] : PureNodeList[0] - 1;
                int nEndIdx = Nodes[PureNodeList.Last() + 1].bIsBungi ? PureNodeList.Last() + 1 : PureNodeList.Last();

                dDist_m = 0;
                for (int i = nStIdx; i <= nEndIdx; i++)
                    dDist_m += Nodes[i].leftNodeSpeed;

                dDist_m /= 1000.0;

                dEnterSpeed_mps /= 1000.0;

                dOutSpeed_mps = Nodes[Nodes.Count - 2].leftNodeSpeed;   // 마지막 하나 전 노드의 속도가 진출속도
                dOutSpeed_mps /= 1000.0;


                dAnswer = (-dConVelMove_sec + Math.Sqrt(dConVelMove_sec * dConVelMove_sec + (1 / dAcc2_mps2 + 1 / dDeAcc_mps2) * (dEnterSpeed_mps * dEnterSpeed_mps / dAcc_mps2 - dSpd33_mps * dSpd33_mps / dAcc_mps2 + dSpd33_mps * dSpd33_mps / dAcc2_mps2 + dOutSpeed_mps * dOutSpeed_mps / dDeAcc_mps2 + 2 * dDist_m))) / (1 / dAcc2_mps2 + 1 / dDeAcc_mps2);

                if (dAnswer >= 3.5 && dAnswer < 4.5)
                    dAdaptedSpeed = 4;
                else if (dAnswer >= 4.5)
                    dAdaptedSpeed = 5;
                else
                    dAdaptedSpeed = 3.3;

                //dAdaptedSpeed = dAnswer > 3.5 ? 4 : 3.3;
            }
           
            foreach(int idx in PureNodeList)
            {
                if (idx == 0)
                    continue;   // 이상한 경우
                if(idx == Nodes.Count-1)
                    continue;   // 이상한 경우

                if (Nodes[idx].bIsFlowControl && (!Nodes[idx].bIsCSnode && !Nodes[idx].bIsStopNode&& !Nodes[idx].bIsResetNode ))
                    continue;

                if (Nodes[idx].nodeType != eNodeType.Normal)
                    continue;

                Nodes[idx].NewSpeed_raw = (int)(dAnswer * 1000);
                Nodes[idx].NewSpeed = (int)(dAdaptedSpeed * 1000);     // Up된 속도 적용
                Nodes[idx].NewCalDist = (int)(dDist_m * 1000);
                nUpCnt++;
            }
            return nUpCnt;
        }
        
        public void ApplyUpSpeed()
        {
            foreach(DObjNodeEx obj in Nodes)
            {
                if (obj.NewSpeed > 0)
                {
                    obj.leftNodeSpeed = obj.NewSpeed;
                    obj.NewSpeed = 0;
                }
            }
        }

        public List<string> GetInfo_String()
        {
            List<string> allLines = new List<string>();

            for(int i=0; i<Nodes.Count-1; i++)
            {
                DObjNodeEx nodeA = Nodes[i];
                DObjNodeEx nodeB = Nodes[i+1];

                bool bLeftDir = nodeA.leftNodeID == nodeB.ID;

                string strNodeInfo = "N: " + Convert.ToString(nodeA.ID);
                strNodeInfo += ", Speed: " + Convert.ToString(bLeftDir ? nodeA.leftNodeSpeed : nodeA.rightNodeSpeed);
                if (nodeA.NewSpeed > 0)
                    strNodeInfo += String.Format("--> NewSpeed: {0} ({1})", nodeA.NewSpeed, nodeA.NewSpeed_raw);

                strNodeInfo += ", Steer: " + (bLeftDir ? nodeA.leftSteering.ToString() : nodeA.rightSteering.ToString());

                if (nodeA.bIsResetNode)
                    strNodeInfo += ", ResetNode";

                if (nodeA.bIsStopNode)
                    strNodeInfo += ", StopNode";

                if (nodeA.bIsCSnode)
                    strNodeInfo += ", CommonStartNode";

                if (nodeA.bIsHapryu)
                    strNodeInfo += ", Hapryu-MinSpd: " + Convert.ToString(nodeA.nHapruyMinSpd);

                if (nodeA.bIsBungi)
                    strNodeInfo += ", BunGiNode";

                

                allLines.Add(strNodeInfo);

                // 마지막 노드 처리
                if(i == Nodes.Count - 2)
                {
                    strNodeInfo = "N: " + Convert.ToString(nodeB.ID);
                    if (nodeB.bIsResetNode)
                        strNodeInfo += ", ResetNode";

                    if (nodeB.bIsStopNode)
                        strNodeInfo += ", StopNode";

                    if (nodeB.bIsCSnode)
                        strNodeInfo += ", CommonStartNode";

                    if (nodeB.bIsHapryu)
                        strNodeInfo += ", Hapryu-MinSpd: " + Convert.ToString(nodeB.nHapruyMinSpd);

                    if (nodeB.bIsBungi)
                        strNodeInfo += ", BunGiNode";

                    allLines.Add(strNodeInfo);
                }
                
            }
            return allLines;
        }
    }

    public class DMapSpeedUp
    {
        public static int MAX_SPEED = 5000;

        List<DObjNodeEx> NodeAll = new List<DObjNodeEx>();
        public List<DGugan> m_GuGanList_All = new List<DGugan>();
        
        // Steer to NonSteer to Steer 구간 
        // [조향] -> [N조향] -> [조향]   ==> 5 m/s 
        public List<DGugan> m_GuganList_SNS = new List<DGugan>();

        // (합류)Reset Node Start NonSteer 
        // (합류)Reset 노드로 시작해서 끝날때 까지 CID 만나지 않고 계속 N 유지. ==> 5m/s 적용
        public List<DGugan> m_GuganList_RStartN = new List<DGugan>();

        // (분기) Node Start NonSteer 
        // (분기) 노드로 시작해서 끝날때 까지 CID 만나지 않고 계속 N 유지. ==> 5m/s 적용
        public List<DGugan> m_GuganList_BungiStartN = new List<DGugan>();

        // N Steer + CID 구간
        // [N조향] -> [N조향+CID구간] 인 경우 ==> 속도 계산식으로 구해야함
        public List<DGugan> m_GuganList_NCid = new List<DGugan>();

        // 합류N -> 조향 -> N 구간. CID 없음
        // 유턴구간에 패턴. N조향인 구간만 5 m/s 적용구간
        public List<DGugan> m_GuganList_NSN = new List<DGugan>();

        // Complex Gugan : 복잡 구간
        // 한개의 구간에 [L조향] -> [N조향] -> [R조향] 같이 3번의 조향 변경 보다 더많은 경우. 특이한 구간. 많지는 않음.
        // 주로 맵 외곽쪽에 존재
        public List<DGugan> m_GuganList_Complex = new List<DGugan>();


        // 증속이 필요없는 구간
        public List<DGugan> m_GuganList_Skip = new List<DGugan>();

        // 조향이 두번만 바뀌는 구간
        // (분기or합류) 조향 -> N조향 -> 분기로 끝 : 5m/s 대상
        public List<DGugan> m_GuganList_DubleS = new List<DGugan>();


        // 그외 예외 구간...
        public List<DGugan> m_GuganList_Undefined = new List<DGugan>();


        public void GetAllNode()
        {
            for(int i =0; i< DObjMan.arrNode.Count; i++) 
            {
                DObjNode node = (DObjNode)DObjMan.arrNode[i];

                DObjNodeEx nodeEx = new DObjNodeEx();
                nodeEx.CopyFrom(node);
                nodeEx.nParentNodeArrIdx = i;

                //nodeEx = (DObjNodeEx)node;

                nodeEx.bIsHapryu = DMapSpeedUp.IsHapRuNode(node.ID, out nodeEx.nHapruyMinSpd);
                nodeEx.bIsBungi = DCustomExtract.IsBugiNode(node);
                nodeEx.bIsFirstNode = DCustomExtract.IsFirstNode(node.ID);
                nodeEx.bIsLastNode = node.leftNodeID == 0;

                foreach(DObjCID cid in DObjMan.arrCID)
                {
                    
                    if (node.ID == cid.commonStartNode1_1)
                        nodeEx.bIsCSnode = true;
                    else  if (node.ID == cid.commonStartNode1_2)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode1_3)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode1_4)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode1_5)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode2_1)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode2_2)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode2_3)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode2_4)
                        nodeEx.bIsCSnode = true;
                    else if (node.ID == cid.commonStartNode2_5)
                        nodeEx.bIsCSnode = true;

                    if (node.ID == cid.stopNode1ID)
                        nodeEx.bIsStopNode = true;
                    else if (node.ID == cid.stopNode2ID)
                        nodeEx.bIsStopNode = true;

                    if (node.ID == cid.resetNodeID)
                        nodeEx.bIsResetNode = true;
                }

                foreach(DObjFlowControl f in DObjMan.arrFlowControl)
                {
                    if (node.ID == f.StopNodeID || node.ID == f.ResetNodeMain)
                        nodeEx.bIsFlowControl = true;
                }

                NodeAll.Add(nodeEx);
            }
        }

        // 구간 정보 저장
        public void WriteInfoFile(string path)
        {
            List<string> sns = new List<string>();
            sns.Add("\n### Steer -> nonSteer -> Steer 구간: 5m/s 증속대상구간,  " + Convert.ToString(m_GuganList_SNS.Count));

            for (int i = 0; i < m_GuganList_SNS.Count; i++)
            {
                sns.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_SNS[i].Nodes.Count));
                sns.AddRange(m_GuganList_SNS[i].GetInfo_String());
            }
            

            List<string> rsn = new List<string>();
            rsn.Add("\n### 합류 -> nonSteer (no CID):  5m/s 증속대상구간,  " + Convert.ToString(m_GuganList_RStartN.Count));

            for (int i = 0; i < m_GuganList_RStartN.Count; i++)
            {
                rsn.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_RStartN[i].Nodes.Count));
                rsn.AddRange(m_GuganList_RStartN[i].GetInfo_String());
            }


            List<string> bsn = new List<string>();
            bsn.Add("\n### 분기 -> nonSteer (no CID):  5m/s 증속대상구간,  " + Convert.ToString(m_GuganList_BungiStartN.Count));

            for (int i = 0; i < m_GuganList_BungiStartN.Count; i++)
            {
                bsn.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_BungiStartN[i].Nodes.Count));
                bsn.AddRange(m_GuganList_BungiStartN[i].GetInfo_String());
            }
            

            List<string> cid = new List<string>();
            cid.Add("\n### NonSteer  -> nonSteer CID:  계산 증속 대상구간,  " + Convert.ToString(m_GuganList_NCid.Count));

            for (int i = 0; i < m_GuganList_NCid.Count; i++)
            {
                cid.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_NCid[i].Nodes.Count));
                cid.AddRange(m_GuganList_NCid[i].GetInfo_String());
            }


            //List<string> nsn = new List<string>();
            //nsn.Add("\n### 합류 NonSteer  -> 조향 -> NonSteer:  턴 구간 -> 5m/s 증속  " + Convert.ToString(m_GuganList_NSN.Count));

            //for (int i = 0; i < m_GuganList_NSN.Count; i++)
            //{
            //    nsn.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_NSN[i].Nodes.Count));
            //    nsn.AddRange(m_GuganList_NSN[i].GetInfo_String());
            //}

            List<string> comp = new List<string>();
            comp.Add("\n### Complex 구간. 조향이 2번이상 바뀌는 구간. 따로 처리 필요,  " + Convert.ToString(m_GuganList_Complex.Count));

            for (int i = 0; i < m_GuganList_Complex.Count; i++)
            {
                comp.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_Complex[i].Nodes.Count));
                comp.AddRange(m_GuganList_Complex[i].GetInfo_String());
            }

            List<string> skip = new List<string>();
            skip.Add("\n### Skip 구간  " + Convert.ToString(m_GuganList_Skip.Count));

            for (int i = 0; i < m_GuganList_Skip.Count; i++)
            {
                skip.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_Skip[i].Nodes.Count));
                skip.AddRange(m_GuganList_Skip[i].GetInfo_String());
            }

            List<string> dbl = new List<string>();
            dbl.Add("\n### (분기or합류) 조향 -> N조향 -> 그대로 끝(분기) -> 5m/s 증속 대상,    " + Convert.ToString(m_GuganList_DubleS.Count));

            for (int i = 0; i < m_GuganList_DubleS.Count; i++)
            {
                dbl.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_DubleS[i].Nodes.Count));
                dbl.AddRange(m_GuganList_DubleS[i].GetInfo_String());
            }

            List<string> und = new List<string>();
            und.Add("\n### 예외 구간. 왜 예외가 났는지...  " + Convert.ToString(m_GuganList_Undefined.Count));

            for (int i = 0; i < m_GuganList_Undefined.Count; i++)
            {
                und.Add(String.Format("\n{0} Gugan,  Cnt  {1}", i, m_GuganList_Undefined[i].Nodes.Count));
                und.AddRange(m_GuganList_Undefined[i].GetInfo_String());
            }

            List<string> total = new List<string>();
            total.AddRange(sns);
            total.AddRange(rsn);
            total.AddRange(bsn); 
            total.AddRange(cid);
            //total.AddRange(nsn);
            total.AddRange(comp);
            total.AddRange(skip);
            total.AddRange(dbl);
            total.AddRange(und);
            File.WriteAllLines(path, total);

        }

        public void MapSpeedUp()
        {
            m_GuGanList_All.Clear();
            m_GuganList_Complex.Clear();
            m_GuganList_NCid.Clear();
            m_GuganList_NSN.Clear();
            m_GuganList_RStartN.Clear();
            m_GuganList_BungiStartN.Clear();
            m_GuganList_SNS.Clear();
            m_GuganList_Undefined.Clear();
            m_GuganList_Skip.Clear();

            // 합류인지, 분기인지, 등등, 전체를 알아야만 볼수있는 정보들을 DObjNodeEx 객체에 미리 조사해서 넣는다.
            // 이후에 계산을 빨리 하기 위함
            GetAllNode();

            // 모든 링크를 구간별로 분리해서 m_GuGanList_All 에 다 넣음
            // 구간이란 합류나 분기로 시작해서, 합류나 분기로 끝나는 한개의 긴 선이다.
            m_GuGanList_All = FindAllSanke();

            // m_GuGanList_All 의 전체 구간에서 m_GuganList_Complex 등 각 구간의 타입별로 분리시켜서 리스트에 집어 넣는다    
            foreach(DGugan gugan in m_GuGanList_All)
                SplitGuganType(gugan);            
            
            // 분리된 리스트 별로 텍스트 파일 저장
            //WriteInfoFile(DObjMan.OrgMapPath + "_Map증속.txt");

            // 각 구간별로 증속 수행
            GuganSpeedUp();
        }

        void GuganSpeedUp()
        {
            // **************************************************************************************************************
            // Steer -> nonSteer -> Steer 구간: 5m/s 증속대상구간
            foreach (DGugan gugan in m_GuganList_SNS)
            {
                for(int i=1; i < gugan.Nodes.Count-1; i++)
                {
                    if(gugan.Nodes[i].ID == 20135)
                    {
                        gugan.Nodes[i].ID = gugan.Nodes[i].ID;
                    }
                    if (gugan.Nodes[i].nodeType != eNodeType.Normal)
                        continue;

                    // N조향에서 증속하는데, 원래 속도가 3m/s 미만 이면 증속안함, FlowControl 노드여도 증속 안함
                    if (gugan.Nodes[i].leftSteering == eSteering.NoDirection && gugan.Nodes[i].leftNodeSpeed >= 3000 && !gugan.Nodes[i].bIsFlowControl)
                    {
                        gugan.Nodes[i].NewSpeed = DMapSpeedUp.MAX_SPEED;
                        gugan.Nodes[i].NewCalDist = 0;
                        gugan.Nodes[i].NewSpeed_raw = 0;
                    }
                }
            }

            // **************************************************************************************************************
            // 분기 -> nonSteer (no CID):  5m/s 증속대상구간
            foreach (DGugan gugan in m_GuganList_BungiStartN)
            {
                for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                {
                    if (gugan.Nodes[i].nodeType != eNodeType.Normal)
                        continue;

                    // N조향에서 증속하는데, 원래 속도가 3m/s 미만 이면 증속안함, FlowControl 노드여도 증속 안함
                    if (gugan.Nodes[i].leftSteering == eSteering.NoDirection && gugan.Nodes[i].leftNodeSpeed >= 3000 && !gugan.Nodes[i].bIsFlowControl)
                    {
                        gugan.Nodes[i].NewSpeed = DMapSpeedUp.MAX_SPEED;
                        gugan.Nodes[i].NewCalDist = 0;
                        gugan.Nodes[i].NewSpeed_raw = 0;
                    }
                }
            }

            // **************************************************************************************************************
            // 합류 -> nonSteer (no CID):  5m/s 증속대상구간
            foreach (DGugan gugan in m_GuganList_RStartN)
            {
                for (int i = 0; i < gugan.Nodes.Count - 1; i++)
                {
                    int nSpeed = gugan.Nodes[i].GetSpeed(gugan.Nodes[i].ID);
                    // N조향에서 증속하는데, 원래 속도가 3m/s 미만 이면 증속안함, FlowControl 노드여도 증속 안함
                    if (gugan.Nodes[i].leftSteering == eSteering.NoDirection && nSpeed >= 3000)
                    {
                        // 중간에Flow Control 나오는건 Skip
                        if (i > 0 && gugan.Nodes[i].bIsFlowControl)
                            continue;
                        if (gugan.Nodes[i].nodeType != eNodeType.Normal)
                            continue;
                        gugan.Nodes[i].NewSpeed = DMapSpeedUp.MAX_SPEED;
                        gugan.Nodes[i].NewCalDist = 0;
                        gugan.Nodes[i].NewSpeed_raw = 0;
                    }
                }
            }

            //foreach (DGugan gugan in m_GuganList_NSN)
            //{
            //}

            // **************************************************************************************************************
            //(분기or합류)조향->N조향->그대로 끝(분기)-> 5m / s 증속 대상
            foreach (DGugan gugan in m_GuganList_DubleS)
            {
                for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                {
                    // N조향에서 증속하는데, 원래 속도가 3m/s 미만 이면 증속안함, FlowControl 노드여도 증속 안함
                    if (gugan.Nodes[i].leftSteering == eSteering.NoDirection && gugan.Nodes[i].leftNodeSpeed >= 3000)
                    {
                        // 중간에Flow Control 나오는건 Skip
                        if (i > 0 && gugan.Nodes[i].bIsFlowControl)
                            continue;

                        if (gugan.Nodes[i].nodeType != eNodeType.Normal)
                            continue;

                        gugan.Nodes[i].NewSpeed = DMapSpeedUp.MAX_SPEED;
                        gugan.Nodes[i].NewCalDist = 0;
                        gugan.Nodes[i].NewSpeed_raw = 0;
                    }
                }
            }

            // **************************************************************************************************************
            // NonSteer  -> nonSteer CID:  계산 증속 대상구간
            foreach (DGugan gugan in m_GuganList_NCid)
            {
                SpeedUpCal_Gugan(gugan);
            }

            foreach (DGugan gugan in m_GuganList_Complex)
            {
                SpeedUp_ComplexGugan(gugan);
            }
        }

        void SpeedUp_ComplexGugan(DGugan gugan)
        {
            List<DGugan> simpleGuganList = new List<DGugan>();

            DGugan newGugan = new DGugan();

            for(int i=0; i<gugan.Nodes.Count-1; i++)
            {
                // 조향을 만나면 구간을 새로 생성
                // 조향 -> N -> 조향
                // 으로 구간을 세분화시켜서 simpleGuganList 에 저장
                
                eSteering st = i == 0 ? gugan.Nodes[i].GetSteer(gugan.Nodes[i + 1].ID) : gugan.Nodes[i].leftSteering;
                if(st != eSteering.NoDirection)
                {
                    //if(newGugan.Nodes.Count > 2)
                    {
                        newGugan.Nodes.Add(gugan.Nodes[i]);
                        simpleGuganList.Add(newGugan);
                    }

                    newGugan = new DGugan();
                    newGugan.Nodes.Add(gugan.Nodes[i]);
                    continue;
                }

                newGugan.Nodes.Add(gugan.Nodes[i]);
            }
            // 마지막 노드 추가
            newGugan.Nodes.Add(gugan.Nodes[gugan.Nodes.Count - 1]);

            // 마지막 구간 추가
            if (simpleGuganList.Last() != newGugan)
                simpleGuganList.Add(newGugan);

            // 최소 3개이상 노드가 나와야 뭘 해봄
            for(int i= simpleGuganList.Count-1; i>=0; i--)
            {
                if (simpleGuganList[i].Nodes.Count < 3)
                    simpleGuganList.RemoveAt(i);
            }

            // 증속 적용 *****
            foreach(DGugan gg in simpleGuganList)
            {
                bool bDid = false;
                for(int i=1; i<gg.Nodes.Count; i++)
                {
                    // N조향에 CommonStop이 있으면 계산식으로 넘긴다
                    if(gg.Nodes[i].leftSteering == eSteering.NoDirection && gg.Nodes[i].bIsCSnode)
                    {
                        SpeedUpCal_Gugan(gg);
                        bDid = true;
                        break;
                    }
                }

                if (bDid)
                    continue;
                
                // 그게 아니라면 5m/s 적용
                for (int i = 1; i < gg.Nodes.Count; i++)
                {
                    if (gg.Nodes[i].leftSteering == eSteering.NoDirection && gg.Nodes[i].nodeType == eNodeType.Normal)
                    {
                        gg.Nodes[i].NewSpeed = MAX_SPEED;
                        gg.Nodes[i].NewCalDist = 0;
                        gg.Nodes[i].NewSpeed_raw = 0;
                    }
                }
            }

        }

        

        // CID를 만날때 속도 구하는 공식 적용
        void SpeedUpCal_Gugan(DGugan gugan)
        {

            int nDist = 0;
            List<int> tagetNodeIdx = new List<int>();

            bool bHapRuy = gugan.Nodes[0].bIsHapryu;

            for (int i = 0; i < gugan.Nodes.Count - 1; i++)
            {
                if (gugan.Nodes[i].IsPureNode() == false || gugan.Nodes[i].leftSteering != eSteering.NoDirection)
                    continue;

                nDist += gugan.Nodes[i].leftNodeDist;
                tagetNodeIdx.Add(i);
            }

            if (tagetNodeIdx.Count == 0)
                return;
            
            double dDist_m = nDist / 1000.0;
            double dEnterSpeed_mps = 0;
            double dOutSpeed_mps = 0;
            double dAcc_mps2 = 2;
            double dAcc2_mps2 = 1;
            double dDeAcc_mps2 = 3;
            double dConVelMove_sec = 1;
            double dSpd33_mps = 3.3;

            if (gugan.Nodes[0].bIsHapryu)
                dEnterSpeed_mps = gugan.Nodes[0].nHapruyMinSpd;
            else
                dEnterSpeed_mps = gugan.Nodes[0].GetSpeed(gugan.Nodes[1].ID);

            dEnterSpeed_mps /= 1000.0;

            // Execel 내에 있는 구간내 Max 속도 구하는 식
            double dAdaptedSpeed = 0;
            double dAnswer = 0;

            dOutSpeed_mps = gugan.Nodes[gugan.Nodes.Count - 1].leftNodeSpeed;   // 마지막 하나 전 노드의 속도가 진출속도
            dOutSpeed_mps /= 1000.0;

            dAnswer = (-dConVelMove_sec + Math.Sqrt(dConVelMove_sec * dConVelMove_sec + (1 / dAcc2_mps2 + 1 / dDeAcc_mps2) * (dEnterSpeed_mps * dEnterSpeed_mps / dAcc_mps2 - dSpd33_mps * dSpd33_mps / dAcc_mps2 + dSpd33_mps * dSpd33_mps / dAcc2_mps2 + dOutSpeed_mps * dOutSpeed_mps / dDeAcc_mps2 + 2 * dDist_m))) / (1 / dAcc2_mps2 + 1 / dDeAcc_mps2);

            if (dAnswer >= 3.5 && dAnswer < 4.5)
                dAdaptedSpeed = 4;
            else if (dAnswer >= 4.5)
                dAdaptedSpeed = 5;
            else
                dAdaptedSpeed = 3.3;

            foreach (int idx in tagetNodeIdx)
            {
                //   if (gugan.Nodes[idx].bIsFlowControl && (!gugan.Nodes[idx].bIsCSnode && !gugan.Nodes[idx].bIsStopNode && !gugan.Nodes[idx].bIsResetNode))
                //   continue;

                if (gugan.Nodes[idx].GetSteer(gugan.Nodes[idx+1].ID) != eSteering.NoDirection || gugan.Nodes[idx].GetSpeed(gugan.Nodes[idx + 1].ID) < 3000)
                     continue;

                if (gugan.Nodes[idx].nodeType != eNodeType.Normal)
                   continue;

                gugan.Nodes[idx].NewSpeed_raw = (int)(dAnswer * 1000);
                gugan.Nodes[idx].NewSpeed = (int)(dAdaptedSpeed * 1000);     // Up된 속도 적용
                gugan.Nodes[idx].NewCalDist = (int)(dDist_m * 1000);
            }
        }
        // m_GuGanList_All 의 전체 구간에서 m_GuganList_Complex 등 각 구간의 타입별로 분리시켜서 리스트에 집어 넣는다
        void SplitGuganType(DGugan gugan)
        {
            //  한 구간에 속한 모든 노드의 속도가 1 m/s 이하면 그 노드는 Skip 한다.
            // 1m/s 이하의 구간은 손대지 않는다
            
            bool bSkipGugan = true;
            if(gugan.Nodes[0].bIsFirstNode) // MTL같이 이전 노드 없이 그냥 시작 노드인 경우, 증속 안함
            {
                m_GuganList_Skip.Add(gugan);
                return;
            }
            if(gugan.Nodes.Count > 2)   // 아주 짧은 구간은 아닐때
            {
                for (int i = 0; i < gugan.Nodes.Count - 1; i++)
                {
                    if (gugan.Nodes[i].GetSpeed(gugan.Nodes[i + 1].ID) >= 3000 && gugan.Nodes[i].GetSteer(gugan.Nodes[i + 1].ID) == eSteering.NoDirection && !gugan.Nodes[i].bIsCSnode)
                    {
                        bSkipGugan = false;
                        break;
                    }
                }
                if (bSkipGugan)
                {
                    m_GuganList_Skip.Add(gugan);
                    return;
                }
            }
            else
            {   // 아주 짧은 구간 2개 노드짜리
                if(gugan.Nodes[0].GetSpeed(gugan.Nodes[1].ID) < 3000 || gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID) != eSteering.NoDirection)
                {
                    m_GuganList_Skip.Add(gugan);
                    return;
                }
            }
            

            eSteering steer = eSteering.NoDirection;

            // Complex Gugan : 복잡 구간
            // 한개의 구간에 [L조향] -> [N조향] -> [R조향] 같이 3번의 조향 변경 보다 더많은 경우. 특이한 구간. 많지는 않음.
            // 주로 맵 외곽쪽에 존재
            if (true)
            {
                int nGuganCnt = 0;
                steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
                for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                {
                    bool bDiff = gugan.Nodes[i-1].GetSteer(gugan.Nodes[i].ID) != gugan.Nodes[i].GetSteer(gugan.Nodes[i + 1].ID);
                    if (bDiff)
                    {
                        nGuganCnt++;
                        if(nGuganCnt == 3)
                        {
                            m_GuganList_Complex.Add(gugan);
                            return;
                        }
                    }
                }
            }

            // Steer -> NonSteer -> Steer 구간 *********************************************
            // [조향] -> [N조향] -> [조향]   ==> 5 m/s 
            steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            if (steer != eSteering.NoDirection)
            {                
                bool bNSteerStarted = false;
                bool bSteerChanged = false;
                for (int i = 1; i < gugan.Nodes.Count-1; i++)
                {                
                    // N조향 나올때까지 continue
                    if(bNSteerStarted == false)
                        bNSteerStarted = gugan.Nodes[i].leftSteering == eSteering.NoDirection;

                    if (bNSteerStarted == false)
                        continue;

                    // N조향에서 (LR)조향으로 바뀌면 OK. 추가하고 끝내기
                    if (bSteerChanged == false)
                        bSteerChanged = gugan.Nodes[i].leftSteering != eSteering.NoDirection;

                    // N조향일때 CID를 먼저 만나면 나가리
                    bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;

                    if (bIsCIDNode && !bSteerChanged)
                        break;
                        
                    if (bSteerChanged == false)
                        continue;
                    
                    m_GuganList_SNS.Add(gugan);
                    return;
                   
                }
            }

            // 합류 -> NonSteer -> Steer 구간 *********************************************
            // 위에 조건하고 비슷한데 추가  ==> 5 m/s 
            steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            if (steer == eSteering.NoDirection && gugan.Nodes[0].bIsHapryu)
            {
                bool bChanged = false;
                bool bSteerChanged = false;
                for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                {
                    // N조향 끝날때까지
                    if (bChanged == false)
                        bChanged = gugan.Nodes[i].leftSteering != eSteering.NoDirection;

                    // N조향일때 CID를 먼저 만나면 나가리
                    bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;

                    if (bChanged == false && bIsCIDNode)
                        break;

                    if (bChanged == false)
                        continue;

                    // N조향에서 (LR)조향으로 바뀌면 OK. 추가하고 끝내기
                    if (bSteerChanged == false)
                        bSteerChanged = gugan.Nodes[i].leftSteering != eSteering.NoDirection;

                   

                    if (bIsCIDNode && !bSteerChanged)
                        break;

                    if (bSteerChanged == false)
                        continue;

                    m_GuganList_SNS.Add(gugan);
                    return;
                }
            }

            // 분기 LR조향 -> N조향 -> LR조향 구간 -> N조향 -> CID*********************************************
            // 턴 구간  ==> 5 m/s 
            //steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            //if (steer != eSteering.NoDirection && gugan.Nodes[0].bIsBungi)
            //{
            //    bool bChanged1 = false;
            //    bool bChanged2 = false;
            //    for (int i = 1; i < gugan.Nodes.Count - 1; i++)
            //    {
            //        // CID를 먼저 만나면 나가리
            //        bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;

            //        if (bIsCIDNode)
            //            break;

            //        // 조향 나올때까지 continue
            //        if (bChanged1 == false)
            //            bChanged1 = gugan.Nodes[i].leftSteering != eSteering.NoDirection;

            //        if (bChanged1 == false)
            //            continue;

            //        // 조향에서 N조향으로 바뀌면 OK. 추가하고 끝내기
            //        if (bChanged2 == false)
            //            bChanged2 = gugan.Nodes[i].leftSteering == eSteering.NoDirection;
                    
            //        if (bChanged2 == false)
            //            continue;

            //        //if (i == gugan.Nodes.Count - 2)
            //        {
            //            m_GuganList_NSN.Add(gugan);
            //            return;
            //        }

            //    }
            //}


            // (합류)Reset Node Start NonSteer 
            // (합류)Reset 노드로 시작해서 끝날때 까지 CID 만나지 않고 계속 N 유지. ==> 5m/s 적용
            if (gugan.Nodes.Count == 2)
            {
                // 노드 두개짜리 짧은 구간은 조건이 단순
                if (gugan.Nodes[0].bIsHapryu && gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID) == eSteering.NoDirection)
                {
                    m_GuganList_RStartN.Add(gugan);
                    return;
                }

            }
            else
            {
                steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
                if (gugan.Nodes[0].bIsHapryu && steer == eSteering.NoDirection)
                {
                    for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                    {
                        // N조향 아니면 나가리
                        if (gugan.Nodes[i].leftSteering != eSteering.NoDirection)
                            break;

                        // N조향일때 CID를 먼저 만나면 나가리
                        bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;

                        if (bIsCIDNode)
                            break;

                        if (i == gugan.Nodes.Count - 2)
                        {
                            m_GuganList_RStartN.Add(gugan);
                            return;
                        }
                    }
                }
            }

            // (분기) Node Start NonSteer 
            // (분기) 노드로 시작해서 끝날때 까지 CID 만나지 않고 계속 N 유지. ==> 5m/s 적용
            steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            if (gugan.Nodes[0].bIsBungi)
            {
                for (int i = 1; i < gugan.Nodes.Count - 1; i++)
                {
                    // N조향 아니면 나가리
                    if (gugan.Nodes[i].leftSteering != eSteering.NoDirection)
                        break;

                    // N조향일때 CID를 먼저 만나면 나가리
                    bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;

                    if (bIsCIDNode)
                        break;

                    if (i == gugan.Nodes.Count - 2)
                    {
                        m_GuganList_BungiStartN.Add(gugan);
                        return;
                    }
                }
            }

            

            // N Steer + CID 구간
            // [N조향] -> [N조향+CID구간] 인 경우 ==> 속도 계산식으로 구해야함
            steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            //if (steer != eSteering.NoDirection)
            {
                for (int i = 1; i < gugan.Nodes.Count-1; i++)
                {
                    bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;
                    if (bIsCIDNode && gugan.Nodes[i].leftSteering == eSteering.NoDirection)
                    {
                        m_GuganList_NCid.Add(gugan);
                        return;
                    }
                }
            }

            // Double Steer
            // [분기or합류] 조향있음 -> N조향 -> 그대로 끝(분기로끝)
            // N조향 구간은 5 m/s 증속
            steer = gugan.Nodes[0].GetSteer(gugan.Nodes[1].ID);
            if (steer != eSteering.NoDirection)
            {
                bool bChanged = false;
                for (int i = 1; i < gugan.Nodes.Count; i++)
                {
                    bool bIsCIDNode = gugan.Nodes[i].bIsCSnode || gugan.Nodes[i].bIsStopNode || gugan.Nodes[i].bIsResetNode;
                    if (bIsCIDNode && i != gugan.Nodes.Count - 1)
                        break;


                    // 조향에서 -> N조향 바뀔때까지 지켜봄
                    if (!bChanged)
                    {
                        bChanged = steer != gugan.Nodes[i].leftSteering;
                        if(bChanged)
                        {
                            // 바뀌긴 했는데 N조향 아니면 나가리
                            if (gugan.Nodes[i].leftSteering != eSteering.NoDirection)
                                break;
                        }
                    }

                    if (!bChanged)
                        continue;

                    if (gugan.Nodes[i].leftSteering != eSteering.NoDirection && i != gugan.Nodes.Count - 1)
                        break;

                    // N조향이 끝까지 유지되야함
                    if(i == gugan.Nodes.Count - 1)
                    {
                        m_GuganList_DubleS.Add(gugan);
                        return;
                    }
                }
            }

            


            // 아무대도 소속되지 않는 예외구간
            m_GuganList_Undefined.Add(gugan);

        }


        // 모든 링크를 [Snake]로 분리:
        //   --> 시작노드(합류or분기or시작) -> 직진 -> 끝노드(합류or분기or끝) 로 분리
        List<DGugan> FindAllSanke()
        {
            
            List<DGugan> snakeList = new List<DGugan>();
            
            foreach(DObjNodeEx node in NodeAll)
            {
                // 합류나 첫 노드일 경우
                if(node.bIsHapryu || node.bIsFirstNode)
                {
                    // Snake 시작 노드를 만족하는 링크를 찾았다면 해당 링크부터 한개의 Snake를 찾는다
                    DGugan snake = FindSnake_Single(node);
                    snakeList.Add(snake);
                    continue;
                }

                // 분기노드 일경우, 양갈래 모두 snake 찾는다
                if(node.bIsBungi)
                {
                    DGugan snakeL = FindSnake_Bungi(node, true);
                    DGugan snakeR = FindSnake_Bungi(node, false);
                    snakeList.Add(snakeL);
                    snakeList.Add(snakeR);
                    continue;
                }
            }
            return snakeList;
        }

        // 한방향으로 시작하는 Snake일경우, Snake 찾기
        DGugan FindSnake_Single(DObjNodeEx startNode)
        {
            DGugan bem = new DGugan();
            bem.Nodes.Add(startNode);  // 시작링크 일단 추가
            bem.Nodes.Add(NodeAll.Find(n => n.ID == startNode.leftNodeID));

            while (true)
            {
                DObjNodeEx lastNode = bem.Nodes[bem.Nodes.Count - 1];

                // 마지막 노드를 만족하는 조건이 나올때까지 계속 검색
                bool bFoundLastNode = lastNode.bIsBungi || lastNode.bIsHapryu || lastNode.bIsLastNode;

                if (bFoundLastNode)  // 분기나 합류인 마지막 노드까지 찾았음으로 루프 탈출
                    break;

                DObjNodeEx nextNode = NodeAll.Find(o => o.ID == lastNode.leftNodeID);

                if (nextNode == null)   // next 노드를 못찾은 경우???
                    break;

                bem.Nodes.Add(nextNode); // 다음 노드 추가
            }
            
            return bem;
        }

        // 분기에서 양방향으로 생기는 Snake일 경우, 좌측 우측 결정해줘서 찾기
        DGugan FindSnake_Bungi(DObjNodeEx startNode, bool toLeft)
        {
            DGugan bem = new DGugan();
            bem.Nodes.Add(startNode);  // 시작링크 일단 추가

            if(toLeft)
                bem.Nodes.Add(NodeAll.Find(n => n.ID == startNode.leftNodeID));
            else
                bem.Nodes.Add(NodeAll.Find(n => n.ID == startNode.rightNodeID));

            while (true)
            {
                DObjNodeEx lastNode = bem.Nodes[bem.Nodes.Count - 1];

                // 마지막 노드를 만족하는 조건이 나올때까지 계속 검색
                bool bFoundLastNode = lastNode.bIsBungi || lastNode.bIsHapryu || lastNode.bIsLastNode;

                if (bFoundLastNode)  // 분기나 합류인 마지막 노드까지 찾았음으로 루프 탈출
                    break;

                DObjNodeEx nextNode = NodeAll.Find(o => o.ID == lastNode.leftNodeID);

                if (nextNode == null)   // next 노드를 못찾은 경우???
                    break;

                bem.Nodes.Add(nextNode); // 다음 노드 추가
            }

            return bem;
        }
        
        public static bool IsHapRuNode(int nodeID, out int MinSpeed)
        {
            int nCnt = 0;
            int[] arrSpeed = new int[5];
            MinSpeed = 0;

            foreach (DObjBase b in DObjMan.arrNode)
            {
                DObjNode obj = (DObjNode)b;

                if (obj.leftNodeID == nodeID)
                {
                    arrSpeed[nCnt] = obj.leftNodeSpeed;
                    nCnt++;
                }
                if (obj.rightNodeID == nodeID)
                {
                    arrSpeed[nCnt] = obj.rightNodeSpeed;
                    nCnt++;
                }

                if (nCnt > 1)   // 연결된 링크가 한개가 아니라는 것은 합류구간이란 뜻
                {
                    MinSpeed = Math.Min(arrSpeed[0], arrSpeed[1]);
                    return true;
                }
            }
            return false;
        }
    }
}
