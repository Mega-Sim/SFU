using System;
using System.IO;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VibrationManager
{    

    public struct VibData_
    {
        public int DataCount;
        public uint TagID;
        public int Offset;
        public double Velocity;
        public char SteerF;
        public char SteerR;
        public int UBGLV;
        public int UBGRegion;
        public int OBSLV;
        public int OBSRegion;
        public double RMS;
        public double X;
        public double Y;
        public double Z;
        public int SpeedMode;
    }

    public struct NodeData
    {
        public uint TagID;
        public double VibMax;
        public double MaxX;
        public double MaxY;
        public double MaxZ;
        public int Round;        
        public char SteerF;
        public char SteerR;
        public double Vel;
        public int offset;
        public int Pattern;
        public int UBGLevel;
    }

    class LinkDataQ
    {
        public NodeData[] Data;
        public NodeData maxData;
        public double MaxSpeed;
        public double MinSpeed;        

        public LinkDataQ(int Q_SIZE, double minSpeed = 0, double maxSpeed = 0)
        {
            MaxSpeed = 0;
            MinSpeed = 0;
            
            Data = new NodeData[Q_SIZE];
            MaxSpeed = maxSpeed;
            MinSpeed = minSpeed;
 
        }
    }


    //data matching, 진동 미분값 0.02 이상일 경우 50ms 전부터 데이터 저장/출력
    class VibMatching
    {

        public Dictionary<string, LinkDataQ> Data = new Dictionary<string, LinkDataQ>();
        public Dictionary<int, int> VisitList = new Dictionary<int, int>();
        public Dictionary<int, string> BranchStraight = new Dictionary<int, string>();

        public ArrayList [] ArrList = new ArrayList[1000000];
        public ArrayList [,] LinkList = new ArrayList[1000000,2];

        Queue<NodeData> Buffer = new Queue<NodeData>();

        public int [,] Visit = new int[1000000, 2];
        public bool[] preNodeVisit = new bool[1000000];
        public bool[] NextNodeVisit = new bool[1000000];

        public double maxCal = 0;
        public double minCal = 0;
        public double minCalcnt = 0;
        public double maxCalcnt = 0;
        public double minCalLineCnt = 0;
        public double maxCalLineCnt = 0;
        public double allCal = 0;
        public double allCalcnt = 0;
        public double allCalLineCnt = 0;
        public int FileCnt = 0;

        //All Count
        public double All_Average = 0;
        public double All_Sum = 0;
        public double All_Count = 0;

        //SpecOver Count
        public double SpecOver_Average = 0;
        public double SpecOver_Sum = 0;
        public double SpecOver_Count = 0;
        public double SpecOver_Straight_Count = 0;
        public double SpecOver_LR_Count = 0;
        public double SpecOver_N_Count = 0;

        NodeData NodeData_ = new NodeData();
        NodeData NullData_ = new NodeData();


        public VibMatching(string[] FileList, double Limit_Vib,int Limit_Time)
        {
            VibNodeMatching(FileList, Limit_Vib, Limit_Time);
        }

        public void Initialize()
        {
            //자료구조 초기화
            VisitList.Clear();
            Buffer.Clear();
            ArrList.Initialize();
            LinkList.Initialize();
            Visit.Initialize();
            preNodeVisit.Initialize();
            NextNodeVisit.Initialize();
            Data.Clear();           
           
            //변수 초기화           
            NullData_.VibMax = 0;
            NullData_.MaxX = 0;
            NullData_.MaxY = 0;
            NullData_.MaxZ = 0;
            NullData_.Round = 0;
            NullData_.offset = 0;
            NullData_.Vel = 0;
            NullData_.SteerF = '0';
            NullData_.SteerR = '0';
            NullData_.Pattern = 0;
            NullData_.TagID = 0;

            NodeData_.VibMax = 0;
            NodeData_.MaxX = 0;
            NodeData_.MaxY = 0;
            NodeData_.MaxZ = 0;
            NodeData_.Round = 0;
            NodeData_.offset = 0;
            NodeData_.Vel = 0;
            NodeData_.SteerF = '0';
            NodeData_.SteerR = '0';
            NodeData_.Pattern = 0;
            NodeData_.TagID = 0;

            //모수계산용 변수 초기화
            maxCal = 0;
            minCal = 1e9;
            minCalcnt = 0;
            minCalLineCnt = 0;
            maxCalcnt = 0;
            maxCalLineCnt = 0;
            allCalcnt = 0;
            allCalLineCnt = 0;

            //All, Spec Out 확인용 변수 초기화
            All_Average = 0;
            All_Sum = 0;
            All_Count = 0;
            SpecOver_Average = 0;
            SpecOver_Sum = 0;
            SpecOver_Count = 0;
            SpecOver_Straight_Count = 0;
            SpecOver_LR_Count = 0;
            SpecOver_N_Count = 0;
        }
        public void VibNodeMatching(string[] FileList, double Limit_Vib, int Limit_Time)
        {
            Initialize();            

            int Round = 0;
            LinkDataQ DataClass = null;
            //로직
            foreach (string File in FileList)
            {
                if (File == null)
                    break;

                Round++;
                StreamReader VibDataSr = new StreamReader(File);

                VibDataSr.ReadLine();
                int PreNode = 0;
                int linecnt = 0;
                int limitcnt = 0;

                //bool bAfterFlag = false;
                //string AfterLink = "";
                double maxSpeed = 0;
                double minSpeed = 10000;

                //CSV Data 1개 Data 읽기시작 ~ 끝줄까지
                while (!VibDataSr.EndOfStream)
                {
                    linecnt++;                    
                    All_Count++;
                    allCalLineCnt++;

                    // 한 줄씩 읽어온다. 
                    string line = VibDataSr.ReadLine();
                    
                    // csv 파일은 ',' 를 기준으로 데이터를 분리한다.                 
                    string[] data = line.Split(',');
                    int TempNode = int.Parse(data[1]);

                    try
                    {      
                        //한줄 데이터 파싱                  
                        NodeData_.TagID = uint.Parse(data[1]);
                        NodeData_.offset = int.Parse(data[2]);
                        NodeData_.Vel = double.Parse(data[3]);
                        NodeData_.SteerF = char.Parse(data[4]);
                        NodeData_.SteerR = char.Parse(data[5]);
                        NodeData_.UBGLevel = int.Parse(data[6]);
                        NodeData_.Pattern = int.Parse(data[9]);
                        NodeData_.VibMax = double.Parse(data[10]);
                        NodeData_.MaxX = double.Parse(data[11]);
                        NodeData_.MaxY = double.Parse(data[12]);
                        NodeData_.MaxZ = double.Parse(data[13]);
                        NodeData_.Round = Round;

                        NodeData_.Pattern = int.Parse(data[9]);

                        All_Sum += NodeData_.VibMax;

                        if (Limit_Vib <= NodeData_.VibMax)
                        {
                            limitcnt++;
                            allCalcnt++;
                            SpecOver_Count++;
                            SpecOver_Sum += NodeData_.VibMax;
                        }
                        int ubgPattern = int.Parse(data[7]);

                        if (Limit_Vib <= NodeData_.VibMax && ubgPattern == 1)
                        {
                            SpecOver_Straight_Count++;
                        }
                        else if (Limit_Vib <= NodeData_.VibMax && (ubgPattern == 2 || ubgPattern == 3))
                        {
                            SpecOver_LR_Count++;
                        }
                        else if (Limit_Vib <= NodeData_.VibMax && ubgPattern == 4)
                        {
                            SpecOver_N_Count++;
                        }


                        //Link Data 갱신, 이전 노드와 현재 노드가 다른 경우만 링크 데이터 갱신!
                        string Link = "";
                        
                        if (PreNode != 0 &&  (PreNode != TempNode))
                        {                   
                            DataClass = new VibrationManager.LinkDataQ(Buffer.Count, minSpeed, maxSpeed);
                            Buffer.CopyTo(DataClass.Data, 0);
                            DataClass.maxData = (NodeData)ArrList[PreNode][0];

                            string OnlyLink = PreNode.ToString() + '_' + TempNode.ToString();
                            Link = PreNode.ToString() + '_' + TempNode.ToString() + '_' + Round.ToString();


                            if (preNodeVisit[PreNode] == false ||
                                NextNodeVisit[TempNode] == false)
                            {
                                BranchStraight.Add(BranchStraight.Count(), OnlyLink);
                            }

                            preNodeVisit[PreNode] = true;
                            NextNodeVisit[TempNode] = true;

                            minSpeed = 10000;
                            maxSpeed = 0;

                            if (LinkList[PreNode, 0] == null)
                            {
                                LinkList[PreNode, 0] = new ArrayList();                             

                                if ( Round > 1)
                                {
                                    for(int j=0;j < Round-1; j++)
                                    {
                                        LinkList[PreNode, 0].Add(NullData_);                
                                    }                                    
                                }
                                if(!Data.ContainsKey(Link))
                                {
                                    Data.Add(Link, DataClass);
                                }
                                
                                LinkList[PreNode, 0].Add(ArrList[PreNode][0]);
                                Visit[PreNode, 0] = TempNode;

                            }
                            else
                            {
                                if(Visit[PreNode,0] == TempNode)
                                {
                                    //Link Data
                                    if (Round > LinkList[PreNode, 0].Count)
                                    {
                                        LinkList[PreNode, 0].Add(ArrList[PreNode][0]);
                                        //Queue data
                                        if (!Data.ContainsKey(Link))
                                        {
                                            Data.Add(Link, DataClass);
                                        }
                                    }     
                                    else
                                    {
                                        NodeData Temp1 = (NodeData)LinkList[PreNode, 0][Round - 1];
                                        NodeData Temp2 = (NodeData)ArrList[PreNode][0];

                                        //Queue data
                                        if (!Data.ContainsKey(Link))
                                        {
                                            Data.Add(Link, DataClass);
                                        }                                        
                                        else
                                        {
                                            if (Temp1.VibMax < Temp2.VibMax)
                                                Data[Link] = DataClass;
                                        }

                                        //Link data
                                        if (Temp1.VibMax < Temp2.VibMax)                                        
                                            LinkList[PreNode, 0][Round - 1] = ArrList[PreNode][0];                                
                                    }                                     
                                                                    
                                }
                                else
                                {
                                    if (LinkList[PreNode, 1] == null)
                                    {
                                        LinkList[PreNode, 1] = new ArrayList();  
                                        if (Round > 1)
                                        {
                                            for (int j = 0; j < Round - 1; j++)
                                            {
                                                LinkList[PreNode, 1].Add(NullData_);           
                                            }
                                        }                 
                                        LinkList[PreNode, 1].Add(ArrList[PreNode][0]);                             
                                        Visit[PreNode, 1] = TempNode;

                                        if (!Data.ContainsKey(Link))
                                            Data.Add(Link, DataClass);
                                    }
                                    else
                                    {
                                        //Link Data
                                        if (Round> LinkList[PreNode, 1].Count)
                                        {                            
                                            LinkList[PreNode, 1].Add(ArrList[PreNode][0]);
                                            //Queue data
                                            if (!Data.ContainsKey(Link))
                                            {
                                                Data.Add(Link, DataClass);
                                            }
                                        }
                                        else
                                        {
                                            NodeData Temp1 = (NodeData)LinkList[PreNode, 1][Round - 1];
                                            NodeData Temp2 = (NodeData)ArrList[PreNode][0];
                                           
                                            //Queue data
                                            if (!Data.ContainsKey(Link))
                                            {
                                                Data.Add(Link, DataClass);
                                            }
                                            else
                                            {
                                                if (Temp1.VibMax < Temp2.VibMax)
                                                    Data[Link] = DataClass;
                                            }

                                            //Link Data
                                            if (Temp1.VibMax < Temp2.VibMax)
                                            {
                                                LinkList[PreNode, 1][Round - 1] = ArrList[PreNode][0];
                                            }

                                        }
                                    }
                                }
                            }

                            ArrList[PreNode][0] = NullData_;
                            Buffer.Clear();
                        }



                        Buffer.Enqueue(NodeData_);

                        if (minSpeed > NodeData_.Vel)
                            minSpeed = NodeData_.Vel;
                        if (maxSpeed < NodeData_.Vel)
                            maxSpeed = NodeData_.Vel;

                        //Node Data
                        if (ArrList[TempNode] == null)
                        {
                            ArrList[TempNode] = new ArrayList();
                            ArrList[TempNode].Add(NodeData_);
                            VisitList.Add(VisitList.Count(), TempNode);                                
                        }
                        else
                        {
                            NodeData Temp1 = (NodeData)ArrList[TempNode][0];
                            if (Temp1.VibMax < NodeData_.VibMax)
                            {
                                ArrList[TempNode][0] = NodeData_;   
                            }    
                        }      
                        PreNode = TempNode;

                    }
                    catch(Exception)
                    {

                        MessageBox.Show(TempNode.ToString());
        
                    }
                }
               
                //CSV Data 1개 읽기 완료, 출력 데이터 갱신
                VibDataSr.Close();
                Buffer.Clear();

                double calculate = Math.Round((double)limitcnt / (double)linecnt *(double)100, 3);

                if (maxCal < calculate)
                {
                    maxCal = calculate;
                    maxCalcnt = limitcnt;
                    maxCalLineCnt = linecnt;
                }                    

                if (minCal > calculate)
                {
                    minCal = calculate;
                    minCalcnt = limitcnt;
                    minCalLineCnt = linecnt;
                }                                 
            
            }


            //출력 데이터 계산, 최대 최소 제외, 반올림
            FileCnt = Round;
            allCalcnt = allCalcnt - maxCalcnt - minCalcnt;
            allCalLineCnt = allCalLineCnt - maxCalLineCnt - minCalLineCnt;
            allCal = (double)(allCalcnt) / (double)(allCalLineCnt)*(double)100;
            allCal = Math.Round(allCal, 3);

            All_Average = All_Sum / All_Count;
            All_Average = Math.Round(All_Average, 3);
            SpecOver_Average = SpecOver_Sum / SpecOver_Count;
            SpecOver_Average = Math.Round(SpecOver_Average, 3);

        }        

    }
}
