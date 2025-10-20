using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Reflection;


namespace VibrationManager
{ 
    public partial class UAnalysisOutput : System.Windows.Forms.Form
    {

        public UMapViewMan m_dlgMapMan = new UMapViewMan();

        Queue<NodeData> que = new Queue<NodeData>();
        Queue<int> queNode = new Queue<int>();
        Queue<string> queNodeStr = new Queue<string>();
        Queue<double> queMaxMinVel = new Queue<double>();

        Dictionary<string, string> PreBranch = new Dictionary<string, string>();
        Dictionary<string, string> NextBranch = new Dictionary<string, string>();
        Dictionary<string, bool> MultiRoute = new Dictionary<string, bool>();
        Dictionary<string, bool> JoiningRoute = new Dictionary<string, bool>();

        VibMatching VibMatch = null;
        public double dlimitValue = 0;

        //All Count List
        public double All_Average = 0;
        public double All_Sum = 0;
        public double All_Count = 0;

        //Spec over List
        public double SpecOver_Average = 0;
        public double SpecOver_Sum = 0;
        public double SpecOver_Count = 0;
        public double SpecOver_Straight_Count = 0;
        public double SpecOver_LR_Count = 0;
        public double SpecOver_N_Count = 0;

        //MaxData All Count List
        public double Max_All_Sum = 0;
        public double Max_All_Count = 0;
        public double Max_All_Average = 0;

        //MaxData Spec over List
        public double Max_SpecOver_Average = 0;
        public double Max_SpecOver_Sum = 0;
        public double Max_SpecOver_Count = 0;

        //Link Data Summary
        public int LinkData_Straight_Count = 0;
        public int LinkData_N_Straight_Count = 0;
        public int LinkData_L_Curve_Count = 0;
        public int LinkData_R_Curve_Count = 0;

        //Link Type Count
        public string[] PatternTxt = 
            new string[] { "None", "N", "L", "R", "S", "B", "V", "STB_L", "STB_R", "N_R", "N_L", "U_L", "U_R", "BAY", "ALL", "Join_N","Branch_N" };
        public int[] LinkType_Count = 
            new int[17];
        public int[] SpecOutLinkType_Count = new int[17];

        public int AllLinkCnt;
        public int SpecOutLinkCnt;

        public string[] LinkList_MapView = new string[1000]; // MapView 통신용
        public int MapViewIndex = 0;

        public UAnalysisOutput(string[] FileList, double Limit_Vib,int Limit_Time)
        {
            Assembly assemObj = Assembly.GetExecutingAssembly();
            Version v = assemObj.GetName().Version; // 현재 실행되는 어셈블리..dll의 버전 가져오기

            int majorV = v.Major; // 주버전
            int minorV = v.Minor; // 부버전
            int buildV = v.Build; // 빌드번호
            int revisionV = v.Revision; // 수정번호
            string Version = "(Ver " + majorV.ToString() + '.' + minorV.ToString() + '.' + buildV.ToString() + '.' + revisionV.ToString() + ')';
            this.Text = "Vibration Manager " + Version;

            InitializeComponent();
            VibMatch = new VibMatching(FileList, Limit_Vib, Limit_Time);

            dlimitValue = Limit_Vib;                      
            label1.Text = "Spec Over"+'('+Limit_Vib.ToString()+"G)";

            MaxCal.Text = VibMatch.maxCal.ToString()+"%("+ VibMatch.maxCalcnt.ToString() + "/" + 
                VibMatch.maxCalLineCnt.ToString()+")";

            MinCal.Text = VibMatch.minCal.ToString()+"%("+ VibMatch.minCalcnt.ToString() + "/" +
                VibMatch.minCalLineCnt.ToString() + ")";

            AllCal.Text = VibMatch.allCal.ToString() + "%(" + VibMatch.allCalcnt.ToString() + "/" +
                VibMatch.allCalLineCnt.ToString() + ")";

            All_Average = Math.Round(VibMatch.All_Average,3);
            All_Sum = VibMatch.All_Sum;
            All_Count = VibMatch.All_Count;

            SpecOver_Average = Math.Round(VibMatch.SpecOver_Average,3);
            SpecOver_Sum = VibMatch.SpecOver_Sum;
            SpecOver_Count = VibMatch.SpecOver_Count;
            SpecOver_Straight_Count = VibMatch.SpecOver_Straight_Count;
            SpecOver_LR_Count = VibMatch.SpecOver_LR_Count;
            SpecOver_N_Count = VibMatch.SpecOver_N_Count;

            LinkList_MapView.Initialize();
            SetUp();

        }
        public void SetUp()
        {
            LinkType_Count.Initialize();
            SpecOutLinkType_Count.Initialize();
            que.Clear();
            queNode.Clear();
            queNodeStr.Clear();
            queMaxMinVel.Clear();
            MakeDataSet();
            MultiRoute.Clear();
            PreBranch.Clear();
            NextBranch.Clear();

        }

        public static UAnalysisOutput GetThis()
        {
            return Program.StartPage.MainForm;
        }
        private void MakeDataSet()
        {

            NodeList.ColumnCount = 10;
            NodeList.Columns[0].Name = "Link";
            NodeList.Columns[1].Name = "Offset";
            NodeList.Columns[1].Width = 60;
            NodeList.Columns[2].Name = "Pattern";
            NodeList.Columns[2].Width = 60;
            NodeList.Columns[3].Name = "UBGLV";
            NodeList.Columns[3].Width = 30;
            NodeList.Columns[4].Name = "Vel";
            NodeList.Columns[4].Width = 100;
            NodeList.Columns[5].Name = "RMS";
            NodeList.Columns[5].Width = 60;
            NodeList.Columns[6].Name = "X";
            NodeList.Columns[6].Width = 60;
            NodeList.Columns[7].Name = "Y";
            NodeList.Columns[7].Width = 60;
            NodeList.Columns[8].Name = "Z";
            NodeList.Columns[8].Width = 60;
            NodeList.Columns[9].Name = "Vel Diff";
            NodeList.Columns[9].Width = 60;


            AllList.ColumnCount = 10;
            AllList.Columns[0].Name = "Link";
            AllList.Columns[1].Name = "Round";
            AllList.Columns[1].Width = 60;
            AllList.Columns[2].Name = "Offset";
            AllList.Columns[2].Width = 60;
            AllList.Columns[3].Name = "Pattern";
            AllList.Columns[3].Width = 60;
            AllList.Columns[4].Name = "UBGLV";
            AllList.Columns[4].Width = 60;
            AllList.Columns[5].Name = "Vel";
            AllList.Columns[5].Width = 80;
            AllList.Columns[6].Name = "RMS";
            AllList.Columns[6].Width = 60;
            AllList.Columns[7].Name = "X";
            AllList.Columns[7].Width = 60;
            AllList.Columns[8].Name = "Y";
            AllList.Columns[8].Width = 60;
            AllList.Columns[9].Name = "Z";
            AllList.Columns[9].Width = 60;


            EventList.ColumnCount = 10;
            EventList.Columns[0].Name = "Link";
            EventList.Columns[1].Name = "Round";
            EventList.Columns[1].Width = 60;
            EventList.Columns[2].Name = "Offset";
            EventList.Columns[2].Width = 60;
            EventList.Columns[3].Name = "Pattern";
            EventList.Columns[3].Width = 60;
            EventList.Columns[4].Name = "UBGLV";
            EventList.Columns[4].Width = 60;
            EventList.Columns[5].Name = "Vel";
            EventList.Columns[5].Width = 80;
            EventList.Columns[6].Name = "RMS";
            EventList.Columns[6].Width = 60;
            EventList.Columns[7].Name = "X";
            EventList.Columns[7].Width = 60;
            EventList.Columns[8].Name = "Y";
            EventList.Columns[8].Width = 60;
            EventList.Columns[9].Name = "Z";
            EventList.Columns[9].Width = 60;


            Summary.ColumnCount = 4;
            Summary.Columns[0].Name = "Items";
            Summary.Columns[0].Width = 190;
            Summary.Columns[1].Name = "Value";
            Summary.Columns[1].Width = 100;
            Summary.Columns[2].Name = "Items";
            Summary.Columns[2].Width = 190;
            Summary.Columns[3].Name = "Value";
            Summary.Columns[3].Width = 100;


            NodeData tempNodeData = new NodeData();
            
            int FileCnt = VibMatch.FileCnt;
            AllLinkCnt = 0;
            SpecOutLinkCnt = 0;

            for (int i = 0; i < VibMatch.BranchStraight.Count(); i++)
            {
                string OnlyLink = VibMatch.BranchStraight[i];
                string[] parse = OnlyLink.Split('_');
                
                if(!PreBranch.ContainsKey(parse[0]))
                {
                    PreBranch.Add(parse[0], OnlyLink);
                }
                else
                {
                    if(!MultiRoute.ContainsKey(PreBranch[parse[0]]))
                        MultiRoute.Add(PreBranch[parse[0]],true);
                    if (!MultiRoute.ContainsKey(OnlyLink))
                        MultiRoute.Add(OnlyLink, true);
                }

                if (!NextBranch.ContainsKey(parse[1]))
                {

                    NextBranch.Add(parse[1], OnlyLink);
                }
                else
                {
                    if (!JoiningRoute.ContainsKey(NextBranch[parse[1]]))
                        JoiningRoute.Add(NextBranch[parse[1]], true);
                    if (!JoiningRoute.ContainsKey(OnlyLink))
                        JoiningRoute.Add(OnlyLink, true);
                }

            }

            for (int i = 0; i < VibMatch.VisitList.Count(); i++)
            {
                //Node
                int node = (int)VibMatch.VisitList[i];                
                //int size = VibMatch.LinkList[node, 0].Count;

                NodeData Max;
                NodeData Min;
                NodeData Avg;
                double MaxMinVel;

                //Link1, Link2
                for (int k = 0; k < 2; k++)
                {
                    
                    if (VibMatch.Visit[node, k] == 0)
                        break;
               
                    int size = VibMatch.LinkList[node, k].Count;

                    //if (size != FileCnt)
                    //    break;

                    int NullCnt = 0;

                    Max.VibMax = 0;
                    Min.VibMax = 1e9;
                    Max.MaxX = 0;
                    Min.MaxX = 1e9;
                    Max.MaxY = 0;
                    Min.MaxY = 1e9;
                    Max.MaxZ = 0;
                    Min.MaxZ = 1e9;
                    Max.Vel = 0;
                    Min.Vel = 1e9;
                    Max.offset = 0;
                    Min.offset = 100000;
                    Max.UBGLevel = 0;
                    Min.UBGLevel = 100000;
                    MaxMinVel = 0;

                    double VibRMS = 0;
                    double VibX = 0;
                    double VibY = 0;
                    double VibZ = 0;
                    double Vel = 0;
                    double maxVel = 0;
                    double minVel = 0;
                    
                    int Offset = 0;
                    int PatternNo = 0;
                    int UBGLV = 0;
                    uint TagID = 0;

                    string nodeStr =  node.ToString() + "_" + VibMatch.Visit[node, k].ToString();
                    string Pattern = "";

                    int maxminCnt = 0;

                    //summary
                    for (int j = 0; j < size; j++)
                    {                                   
                        string nodeStr_round = nodeStr;
                        
                        nodeStr_round += "_" + (j + 1).ToString();         

                        if(VibMatch.Data.ContainsKey(nodeStr_round))
                        {
                            minVel = VibMatch.Data[nodeStr_round].MinSpeed;
                            maxVel = VibMatch.Data[nodeStr_round].MaxSpeed;
                            MaxMinVel += maxVel - minVel;
                            maxminCnt++;
                        }

                        tempNodeData = (NodeData)VibMatch.LinkList[node, k][j];
                        VibRMS += tempNodeData.VibMax;
                        VibX += Math.Abs(tempNodeData.MaxX);
                        VibY += Math.Abs(tempNodeData.MaxY);
                        VibZ += Math.Abs(tempNodeData.MaxZ);
                        Vel += tempNodeData.Vel;
                        Offset += tempNodeData.offset;
                        if(MultiRoute.ContainsKey(nodeStr) && tempNodeData.Pattern ==1)
                        {
                            tempNodeData.Pattern = 16;
                        }
                        if (JoiningRoute.ContainsKey(nodeStr) && tempNodeData.Pattern == 1)
                        {
                            tempNodeData.Pattern = 15;
                        }                        
                        Pattern = PatternTxt[tempNodeData.Pattern];
                        PatternNo = tempNodeData.Pattern;
                        UBGLV += tempNodeData.UBGLevel;
                        TagID = tempNodeData.TagID;
                        
                        if (tempNodeData.VibMax == 0 && tempNodeData.MaxX == 0 && tempNodeData.MaxY == 0 &&
                            tempNodeData.MaxZ == 0 && tempNodeData.Vel==0 && tempNodeData.offset==0 &&
                            tempNodeData.Pattern == 0)
                        {
                            NullCnt++;
                            continue;
                        }
                          
                        if (Max.VibMax < tempNodeData.VibMax)
                        {
                            Max.VibMax = tempNodeData.VibMax;
                            Max.MaxX = tempNodeData.MaxX;
                            Max.MaxY = tempNodeData.MaxY;
                            Max.MaxZ = tempNodeData.MaxZ;
                            Max.Vel = tempNodeData.Vel;
                            Max.offset = tempNodeData.offset;
                            Max.UBGLevel= tempNodeData.UBGLevel;
                            Max.TagID = tempNodeData.TagID;
                        }

                        if (Min.VibMax > tempNodeData.VibMax)
                        {
                            Min.VibMax = tempNodeData.VibMax;
                            Min.MaxX = tempNodeData.MaxX;
                            Min.MaxY = tempNodeData.MaxY;
                            Min.MaxZ = tempNodeData.MaxZ;
                            Min.Vel = tempNodeData.Vel;
                            Min.offset = tempNodeData.offset;
                            Min.UBGLevel = tempNodeData.UBGLevel;
                            Min.TagID = tempNodeData.TagID;
                        }                                 
                    }

                    //AVG
                    if (size > 2)
                    {
                        VibRMS = (VibRMS - Max.VibMax - Min.VibMax) / (size - NullCnt - 2);
                        VibX = (VibX - Math.Abs(Max.MaxX) - Math.Abs(Min.MaxX)) / (size - NullCnt-  2);
                        VibY = (VibY - Math.Abs(Max.MaxY) - Math.Abs(Min.MaxY)) / (size - NullCnt - 2);
                        VibZ = (VibZ - Math.Abs(Max.MaxZ) - Math.Abs(Min.MaxZ)) / (size - NullCnt - 2);
                        Vel = (Vel - Max.Vel - Min.Vel) / (size - NullCnt - 2);
                        Offset = (Offset - Max.offset - Min.offset) / (size - NullCnt - 2);
                        UBGLV = (UBGLV - Max.UBGLevel - Min.UBGLevel) / (size - NullCnt - 2);
                        
                    }
                    else
                    {
                        VibRMS /= (size - NullCnt);
                        VibX /= (size - NullCnt);
                        VibY /= (size - NullCnt);
                        VibZ /= (size - NullCnt);
                        Vel /= (size - NullCnt);
                        Offset /= (size - NullCnt);
                        UBGLV /= (size - NullCnt);
                    }
                   

                    Avg.MaxX = Math.Round(VibX, 3);
                    Avg.MaxY = Math.Round(VibY, 3);
                    Avg.MaxZ = Math.Round(VibZ, 3);
                    Avg.VibMax = Math.Round(VibRMS, 3); 
                    Avg.Round = 0;
                    Avg.Vel = Math.Round(Vel, 2);
                    Avg.offset = Offset;
                    Avg.SteerF = '0';
                    Avg.SteerR = '0';
                    Avg.UBGLevel = UBGLV;
                    Avg.Pattern =  PatternNo;
                    Avg.TagID = TagID;   
                    LinkType_Count[PatternNo]++;
                    AllLinkCnt++;
                    MaxMinVel /= maxminCnt;

                    if (VibRMS >= dlimitValue)
                    {
                        SpecOutLinkType_Count[PatternNo]++;
                        que.Enqueue(Avg);
                        queNodeStr.Enqueue(nodeStr);
                        queMaxMinVel.Enqueue(MaxMinVel);
                        SpecOutLinkCnt++;
                    }

                    if(MapViewIndex<1000)
                        LinkList_MapView[MapViewIndex++] = nodeStr + '_' + Avg.VibMax.ToString();

                    //All Data
                    for (int j = 0; j < size; j++)
                    {
                        tempNodeData = (NodeData)VibMatch.LinkList[node, k][j];

                        if(tempNodeData.VibMax> dlimitValue)
                        {
                            Max_SpecOver_Sum += tempNodeData.VibMax;
                            Max_SpecOver_Count++;
                        }

                        Max_All_Sum += tempNodeData.VibMax;
                        Max_All_Count++;

                        if (tempNodeData.VibMax == 0 && tempNodeData.MaxX == 0 && tempNodeData.MaxY == 0 &&
                            tempNodeData.MaxZ == 0 && tempNodeData.Vel == 0 && tempNodeData.offset == 0)
                        {
                            continue;
                        }

                        string strVib = nodeStr + ',' +
                                        tempNodeData.Round.ToString() + ',' +
                                        tempNodeData.offset.ToString() + ',' +
                                        PatternTxt[tempNodeData.Pattern].ToString() + ',' +
                                        tempNodeData.UBGLevel.ToString() + ',' +
                                        tempNodeData.Vel.ToString() + ',' +
                                        tempNodeData.VibMax.ToString() + ',' +
                                        tempNodeData.MaxX.ToString() + ',' +
                                        tempNodeData.MaxY.ToString() + ',' +
                                        tempNodeData.MaxZ.ToString();

                        string[] str = strVib.Split(',');
                        this.AllList.Rows.Add(str);
                    }
                }
            }
            Max_All_Average = Max_All_Sum / Max_All_Count;
            Max_SpecOver_Average = Max_SpecOver_Sum / Max_SpecOver_Count;
            Max_All_Average = Math.Round(Max_All_Average, 3);
            Max_SpecOver_Average = Math.Round(Max_SpecOver_Average,3);
            int QueCnt = 0;
            while (que.Count != 0)
            {
               
                    
                NodeData QueTemp = que.Dequeue();
                string NodeTemp = queNodeStr.Dequeue();
                double MaxMinTemp = queMaxMinVel.Dequeue();
                MaxMinTemp  = Math.Round(MaxMinTemp, 0);
                string strVib = NodeTemp + ',' + 
                        QueTemp.offset.ToString() + ',' +
                        PatternTxt[QueTemp.Pattern] + ',' +
                        QueTemp.UBGLevel.ToString() + ',' +
                        QueTemp.Vel.ToString()  + ',' +
                        QueTemp.VibMax.ToString() + ',' +
                        QueTemp.MaxX.ToString() + ',' +
                        QueTemp.MaxY.ToString() + ',' +
                        QueTemp.MaxZ.ToString() + ','+
                        MaxMinTemp.ToString();

                

                string[] str = strVib.Split(',');
                this.NodeList.Rows.Add(str);
                if(MaxMinTemp > 1000)
                {
                    this.NodeList.Rows[QueCnt].DefaultCellStyle.BackColor = Color.Orange;
                }                
                QueCnt++;
            }




            //Summary 출력


            //1. All Link Average 출력
            this.Summary.Rows.Add("* RMS Average Summary");
            string sr = "All Link Average," + All_Average.ToString() + "(G)"+  ",MAX Link Average," + Max_All_Average.ToString()+ "(G)";
            string[] str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);


            //2. SpecOver Link Average 출력
            sr = "Spec Over Average," + SpecOver_Average.ToString() + "(G)" + ",MAX Link SpecOver Average," + Max_SpecOver_Average.ToString()+ "(G)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);


            //3. SpecOver Ratio 출력
            Double Ratio = SpecOver_Count / All_Count * 100;
            Ratio = Math.Round(Ratio, 2);
            sr = "Spec Over Ratio," + Ratio.ToString() + "(%)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            this.Summary.Rows.Add();

            //4. SpecOver 개수정보 상세 출력
            sr = "All Count," + All_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Count," + SpecOver_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Straight Count," + SpecOver_Straight_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Curve Count," + SpecOver_LR_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over N Curve Count," + VibMatch.SpecOver_N_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            this.Summary.Rows.Add();

            //Link Type 별 개수 출력
            this.Summary.Rows.Add(" * Link Summary");

            sr = "All Link Count," + AllLinkCnt.ToString() + ",SpecOut Link Count,"+ SpecOutLinkCnt.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            this.Summary.Rows.Add();
            sr = "* Link Type All,,* SpecOut Link Type";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            for (int i = LinkType_Count.Count()-1; i > (LinkType_Count.Count() - 3); i--)
            {
                sr = PatternTxt[i] + ',' + LinkType_Count[i].ToString() + ',' + PatternTxt[i] + ','
                    + SpecOutLinkType_Count[i].ToString();
                str_summary = sr.Split(',');
                this.Summary.Rows.Add(str_summary);
            }
            for (int i=1;i< LinkType_Count.Count()-2; i++)
            {
                sr =  PatternTxt[i]+ ',' + LinkType_Count[i].ToString() + ','+ PatternTxt[i] +','
                    + SpecOutLinkType_Count[i].ToString();
                str_summary = sr.Split(',');
                this.Summary.Rows.Add(str_summary);
            }
        }

        private void NodeList_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string temp = this.NodeList.CurrentRow.Cells["Link"].Value.ToString();
            this.DetailLink.Text = temp.ToString();         
            this.DetailAvg.Text = this.NodeList.CurrentRow.Cells["RMS"].Value.ToString();
            this.DetailOffset.Text = this.NodeList.CurrentRow.Cells["Offset"].Value.ToString();
            this.DetailVel.Text = this.NodeList.CurrentRow.Cells["Vel"].Value.ToString();
            this.DetailDetectLevel.Text = this.NodeList.CurrentRow.Cells["UBGLV"].Value.ToString();
            this.DetailLinkType.Text = this.NodeList.CurrentRow.Cells["Pattern"].Value.ToString();
            this.EventList.Rows.Clear();
            int cnt = AllList.Rows.Count-1;

            double min = 1000000;
            double max = 0;

            for (int i = 0; i< cnt; i++)
            {
               string temp2 =  this.AllList.Rows[i].Cells["Link"].Value.ToString();

               if(temp == temp2)
                {
                    string strVib = temp2 + ',' +
                    this.AllList.Rows[i].Cells["Round"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["Offset"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["Pattern"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["UBGLV"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["Vel"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["RMS"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["X"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["Y"].Value.ToString() + ',' +
                    this.AllList.Rows[i].Cells["Z"].Value.ToString();

                    double dTemp = double.Parse(this.AllList.Rows[i].Cells["RMS"].Value.ToString());

                    if (dTemp > max)
                        max = dTemp;

                    if (dTemp < min)
                        min = dTemp;
                    string[] str = strVib.Split(',');

                    this.EventList.Rows.Add(str);
                }

            }

            max = Math.Round(max, 3);
            min = Math.Round(min, 3);
            this.DetailMax.Text = max.ToString();
            this.DetailMin.Text = min.ToString();

        }

        private void EventList_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string temp = this.EventList.CurrentRow.Cells["Link"].Value.ToString();
            string temp2 = this.EventList.CurrentRow.Cells["Round"].Value.ToString();

            string[] arr = temp.Split('_');

            int node1 = int.Parse(arr[0]);
            int node2 = int.Parse(arr[1]);
            int round = int.Parse(temp2);
            
            double MaxSpeed = 0;
            double MinSpeed = 0;

            temp += "_";
            temp += temp2;

            if (VibMatch.Visit[node1, 0] == node2)
            {

                NodeData[] Data = (NodeData[])VibMatch.Data[temp].Data;
                MaxSpeed = VibMatch.Data[temp].MaxSpeed;
                MinSpeed = VibMatch.Data[temp].MinSpeed;
                UPlot PlotFrm = new UPlot(Data, MaxSpeed, MinSpeed);
             
            }
            else if(VibMatch.Visit[node1, 1] == node2)
            {
                NodeData[] Data = (NodeData[])VibMatch.Data[temp].Data;
                MaxSpeed = VibMatch.Data[temp].MaxSpeed;
                MinSpeed = VibMatch.Data[temp].MinSpeed;
                UPlot PlotFrm = new UPlot(Data, MaxSpeed, MinSpeed);
            }
            
        }

        private void AllList_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string temp = this.AllList.CurrentRow.Cells["Link"].Value.ToString();
            string temp2 = this.AllList.CurrentRow.Cells["Round"].Value.ToString();

            string[] arr = temp.Split('_');

            int node1 = int.Parse(arr[0]);
            int node2 = int.Parse(arr[1]);
            int round = int.Parse(temp2);

            double MaxSpeed = 0;
            double MinSpeed = 0;


            temp += "_";
            temp += temp2;

            if (VibMatch.Visit[node1, 0] == node2)
            {

                NodeData[] Data = (NodeData[])VibMatch.Data[temp].Data;
                MaxSpeed = VibMatch.Data[temp].MaxSpeed;
                MinSpeed = VibMatch.Data[temp].MinSpeed;
                UPlot PlotFrm = new UPlot(Data, MaxSpeed, MinSpeed);

            }
            else if (VibMatch.Visit[node1, 1] == node2)
            {

                NodeData[] Data = (NodeData[])VibMatch.Data[temp].Data;
                MaxSpeed = VibMatch.Data[temp].MaxSpeed;
                MinSpeed = VibMatch.Data[temp].MinSpeed;
                UPlot PlotFrm = new UPlot(Data, MaxSpeed, MinSpeed);
            }
        }

        private void NodeList_SortCompare(object sender, DataGridViewSortCompareEventArgs e)
        {
            if(!e.Column.Name.Equals("Pattern") && !e.Column.Name.Equals("Link"))
            {
                double a = double.Parse(e.CellValue1.ToString()), b = double.Parse(e.CellValue2.ToString());
                e.SortResult = a.CompareTo(b);
                e.Handled = true;
            }
            
        }
        
        private void toolStripButton1_Click_1(object sender, EventArgs e)
        {
            try
            {
                MyTools.GMethod.MoveForm_ParentCenter(m_dlgMapMan, this);
                m_dlgMapMan.Show();
                m_dlgMapMan.BringToFront();
                m_dlgMapMan.timer1.Start();
            }
            catch (Exception)
            {
            }
        }
    }

}
