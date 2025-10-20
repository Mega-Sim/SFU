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
    public partial class UAnalysisOutput_Line : Form
    {


        Dictionary<string, string> PreBranch = new Dictionary<string, string>();
        Dictionary<string, string> NextBranch = new Dictionary<string, string>();
        Dictionary<string, bool> MultiRoute = new Dictionary<string, bool>();
        Dictionary<string, bool> JoiningRoute = new Dictionary<string, bool>();

        Dictionary<string, string> PreBranch_2nd = new Dictionary<string, string>();
        Dictionary<string, string> NextBranch_2nd = new Dictionary<string, string>();
        Dictionary<string, bool> MultiRoute_2nd = new Dictionary<string, bool>();
        Dictionary<string, bool> JoiningRoute_2nd = new Dictionary<string, bool>();


        VibMatching VibMatch = null;
        VibMatching VibMatch_2nd = null;
        public double dlimitValue = 0;
        public double dlimitValue_2nd = 0;

        //All Count List
        public double All_Average = 0;
        public double All_Sum = 0;
        public double All_Count = 0;
        public double All_Average_2nd = 0;
        public double All_Sum_2nd = 0;
        public double All_Count_2nd = 0;

        //Spec over List
        public double SpecOver_Average = 0;
        public double SpecOver_Sum = 0;
        public double SpecOver_Count = 0;
        public double SpecOver_Straight_Count = 0;
        public double SpecOver_LR_Count = 0;
        public double SpecOver_N_Count = 0;

        public double SpecOver_Average_2nd = 0;
        public double SpecOver_Sum_2nd = 0;
        public double SpecOver_Count_2nd = 0;
        public double SpecOver_Straight_Count_2nd = 0;
        public double SpecOver_LR_Count_2nd = 0;
        public double SpecOver_N_Count_2nd = 0;

        //MaxData All Count List
        public double Max_All_Sum = 0;
        public double Max_All_Count = 0;
        public double Max_All_Average = 0;
        public double Max_All_Sum_2nd = 0;
        public double Max_All_Count_2nd = 0;
        public double Max_All_Average_2nd = 0;

        //MaxData Spec over List
        public double Max_SpecOver_Average = 0;
        public double Max_SpecOver_Sum = 0;
        public double Max_SpecOver_Count = 0;
        public double Max_SpecOver_Average_2nd = 0;
        public double Max_SpecOver_Sum_2nd = 0;
        public double Max_SpecOver_Count_2nd = 0;

        //Link Data Summary
        public int LinkData_Straight_Count = 0;
        public int LinkData_N_Straight_Count = 0;
        public int LinkData_L_Curve_Count = 0;
        public int LinkData_R_Curve_Count = 0;

        public int LinkData_Straight_Count_2nd = 0;
        public int LinkData_N_Straight_Count_2nd = 0;
        public int LinkData_L_Curve_Count_2nd = 0;
        public int LinkData_R_Curve_Count_2nd = 0;
        
        //Link Type Count
        public string[] PatternTxt =
            new string[] { "None", "N", "L", "R", "S", "B", "V", "STB_L", "STB_R", "N_R", "N_L", "U_L", "U_R", "BAY", "ALL", "Join_N", "Branch_N" };
        public int[] LinkType_Count =  new int[17];
        public int[] SpecOutLinkType_Count = new int[17];

        public int[] LinkType_Count_2nd = new int[17];
        public int[] SpecOutLinkType_Count_2nd = new int[17];

        public int AllLinkCnt;
        public int SpecOutLinkCnt;
        public int SpecOutLinkCnt_2nd;


        public UAnalysisOutput_Line(string[] FileList, double Limit_Vib, double Limit_Vib_2,int Limit_Time)
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
           

            dlimitValue = Limit_Vib;
            dlimitValue_2nd = Limit_Vib_2;
            VibMatch = new VibMatching(FileList, Limit_Vib, Limit_Time);
            VibMatch_2nd = new VibMatching(FileList, Limit_Vib_2, Limit_Time);

            All_Average = Math.Round(VibMatch.All_Average, 3);
            All_Sum = VibMatch.All_Sum;
            All_Count = VibMatch.All_Count;

            SpecOver_Average = Math.Round(VibMatch.SpecOver_Average, 3);
            SpecOver_Sum = VibMatch.SpecOver_Sum;
            SpecOver_Count = VibMatch.SpecOver_Count;
            SpecOver_Straight_Count = VibMatch.SpecOver_Straight_Count;
            SpecOver_LR_Count = VibMatch.SpecOver_LR_Count;
            SpecOver_N_Count = VibMatch.SpecOver_N_Count;

            SpecOver_Average_2nd = Math.Round(VibMatch_2nd.SpecOver_Average, 3);
            SpecOver_Sum_2nd = VibMatch_2nd.SpecOver_Sum;
            SpecOver_Count_2nd = VibMatch_2nd.SpecOver_Count;
            SpecOver_Straight_Count_2nd = VibMatch_2nd.SpecOver_Straight_Count;
            SpecOver_LR_Count_2nd = VibMatch_2nd.SpecOver_LR_Count;
            SpecOver_N_Count_2nd = VibMatch_2nd.SpecOver_N_Count;

            SetUp();
        }
        public void SetUp()
        {
            LinkType_Count.Initialize();
            SpecOutLinkType_Count.Initialize();

            MakeDataSet();
            MultiRoute.Clear();
            PreBranch.Clear();
            NextBranch.Clear();

        }
        private void MakeDataSet()
        {          

            Detail.ColumnCount = 6;
            Detail.Columns[0].Name = "Items";
            Detail.Columns[0].Width = 190;
            Detail.Columns[1].Name = "Value";
            Detail.Columns[1].Width = 100;
            Detail.Columns[2].Name = "Value";
            Detail.Columns[2].Width = 100;


            Summary.ColumnCount = 4;
            Summary.Columns[0].Name = "Items";
            Summary.Columns[0].Width = 190;
            Summary.Columns[1].Name = "Value";
            Summary.Columns[1].Width = 100;
            Summary.Columns[2].Name = "Items";
            Summary.Columns[2].Width = 190;
            Summary.Columns[3].Name = "Value";
            Summary.Columns[3].Width = 100;

            //분기직선 연산
            for (int i = 0; i < VibMatch.BranchStraight.Count(); i++)
            {
                string OnlyLink = VibMatch.BranchStraight[i];
                string[] parse = OnlyLink.Split('_');

                if (!PreBranch.ContainsKey(parse[0]))
                {
                    PreBranch.Add(parse[0], OnlyLink);
                }
                else
                {
                    if (!MultiRoute.ContainsKey(PreBranch[parse[0]]))
                        MultiRoute.Add(PreBranch[parse[0]], true);
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

            // Summary 출력

            for (int i = 0; i < VibMatch.VisitList.Count(); i++)
            {
                //Node
                int node = (int)VibMatch.VisitList[i];

                NodeData Max;
                NodeData Min;
                NodeData Avg;

               
                //Link1, Link2
                for (int k = 0; k < 2; k++)
                {

                    if (VibMatch.Visit[node, k] == 0)
                        break;

                    int size = VibMatch.LinkList[node, k].Count;                    
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

                    double VibRMS = 0;
                    double VibX = 0;
                    double VibY = 0;
                    double VibZ = 0;
                    double Vel = 0;

                    int Offset = 0;
                    int PatternNo = 0;
                    int UBGLV = 0;
                    uint TagID = 0;

                    string nodeStr = node.ToString() + "_" + VibMatch.Visit[node, k].ToString();
                    string Pattern = "";                       

                    NodeData tempNodeData;

                    //summary
                    for (int j = 0; j < size; j++)
                    {
                      
                        string nodeStr_round = nodeStr;

                        nodeStr_round += "_" + (j + 1).ToString();
                        
                        tempNodeData = (NodeData)VibMatch.LinkList[node, k][j];
                        VibRMS += tempNodeData.VibMax;
                        VibX += Math.Abs(tempNodeData.MaxX);
                        VibY += Math.Abs(tempNodeData.MaxY);
                        VibZ += Math.Abs(tempNodeData.MaxZ);
                        Vel += tempNodeData.Vel;
                        Offset += tempNodeData.offset;
                        if (MultiRoute.ContainsKey(nodeStr) && tempNodeData.Pattern == 1)
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
                            tempNodeData.MaxZ == 0 && tempNodeData.Vel == 0 && tempNodeData.offset == 0 &&
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
                            Max.UBGLevel = tempNodeData.UBGLevel;
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
                        VibX = (VibX - Math.Abs(Max.MaxX) - Math.Abs(Min.MaxX)) / (size - NullCnt - 2);
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
                    Avg.Pattern = PatternNo;
                    Avg.TagID = TagID;
                    LinkType_Count[PatternNo]++;
                    AllLinkCnt++;
      

                    if (VibRMS >= dlimitValue)
                    {
                        SpecOutLinkType_Count[PatternNo]++;
                        SpecOutLinkCnt++;
                    }

                    else if (VibRMS >= dlimitValue_2nd)
                    {
                        SpecOutLinkType_Count_2nd[PatternNo]++;
                        SpecOutLinkCnt_2nd++;
                    }

                    //All Data
                    for (int j = 0; j < size; j++)
                    {
                        tempNodeData = (NodeData)VibMatch.LinkList[node, k][j];

                        if (tempNodeData.VibMax > dlimitValue)
                        {
                            Max_SpecOver_Sum += tempNodeData.VibMax;
                            Max_SpecOver_Count++;
                        }
                        if (tempNodeData.VibMax > dlimitValue_2nd)
                        {
                            Max_SpecOver_Sum_2nd += tempNodeData.VibMax;
                            Max_SpecOver_Count_2nd++;
                        }

                        Max_All_Sum += tempNodeData.VibMax;
                        Max_All_Count++;

                        if (tempNodeData.VibMax == 0 && tempNodeData.MaxX == 0 && tempNodeData.MaxY == 0 &&
                            tempNodeData.MaxZ == 0 && tempNodeData.Vel == 0 && tempNodeData.offset == 0)
                        {
                            continue;
                        }
                    }
                }
            }
            Max_All_Average = Max_All_Sum / Max_All_Count;
            Max_SpecOver_Average = Max_SpecOver_Sum / Max_SpecOver_Count;
            Max_All_Average = Math.Round(Max_All_Average, 3);
            Max_SpecOver_Average = Math.Round(Max_SpecOver_Average, 3);

            Max_All_Average_2nd = Max_All_Sum_2nd / Max_All_Count_2nd;
            Max_SpecOver_Average_2nd = Max_SpecOver_Sum_2nd / Max_SpecOver_Count_2nd;
            Max_All_Average_2nd = Math.Round(Max_All_Average_2nd, 3);
            Max_SpecOver_Average_2nd = Math.Round(Max_SpecOver_Average_2nd, 3);


            //1. All Link Average 출력
            this.Summary.Rows.Add("* RMS Average Summary");
            string sr = "All Link Average," + All_Average.ToString() + "(G)" + ",MAX Link Average," + Max_All_Average.ToString() + "(G)";
            string[] str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);


            //2. SpecOver Link Average 출력
            sr = "Spec Over Average(" +dlimitValue.ToString()+ "G),"+ SpecOver_Average.ToString() + "(G)" + ",MAX Link SpecOver Average," + Max_SpecOver_Average.ToString() + "(G)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            sr = "Spec Over Average(" + dlimitValue_2nd.ToString() + "G)," + SpecOver_Average_2nd.ToString() + "(G)" + ",MAX Link SpecOver Average(" + dlimitValue_2nd.ToString() + "G)," + Max_SpecOver_Average_2nd.ToString() + "(G)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);


            //3. SpecOver Ratio 출력
            Double Ratio = SpecOver_Count / All_Count * 100;
            Ratio = Math.Round(Ratio, 2);
            sr = "Spec Over Ratio(" + dlimitValue.ToString() + "G)," + Ratio.ToString() + "(%)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            Ratio = SpecOver_Count_2nd / All_Count_2nd * 100;
            Ratio = Math.Round(Ratio, 2);
            sr = "Spec Over Ratio(" + dlimitValue_2nd.ToString() + "G)," + Ratio.ToString() + "(%)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            this.Summary.Rows.Add();

            //4. SpecOver 개수정보 상세 출력
            this.Summary.Rows.Add(dlimitValue.ToString() + "(G)");
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


            this.Summary.Rows.Add(dlimitValue.ToString() + "(G)");
            sr = "All Count," + All_Count.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Count," + SpecOver_Count_2nd.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Straight Count," + SpecOver_Straight_Count_2nd.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over Curve Count," + SpecOver_LR_Count_2nd.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);
            sr = "Spec Over N Curve Count," + SpecOver_N_Count_2nd.ToString();
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);


            this.Summary.Rows.Add();

            //Link Type 별 개수 출력
            this.Summary.Rows.Add(" * Link Summary");

            sr = "All Link Count," + AllLinkCnt.ToString() + ",SpecOut Link Count("+dlimitValue.ToString() + "G)," + SpecOutLinkCnt.ToString()+ ",SpecOut Link Count(" + dlimitValue_2nd.ToString() + "G)," + SpecOutLinkCnt_2nd.ToString();

            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            this.Summary.Rows.Add();
            sr = "* Link Type All,,* SpecOut Link Type(" + dlimitValue.ToString() + "G),," + "*SpecOut Link Type(" + dlimitValue_2nd.ToString() + "G)";
            str_summary = sr.Split(',');
            this.Summary.Rows.Add(str_summary);

            for (int i = LinkType_Count.Count() - 1; i > (LinkType_Count.Count() - 3); i--)
            {
                sr = PatternTxt[i] + ',' + LinkType_Count[i].ToString() + ',' + PatternTxt[i] + ','
                    + SpecOutLinkType_Count[i].ToString() + PatternTxt[i] + ',' + SpecOutLinkType_Count_2nd[i].ToString();
                str_summary = sr.Split(',');
                this.Summary.Rows.Add(str_summary);
            }
            for (int i = 1; i < LinkType_Count.Count() - 2; i++)
            {
                sr = PatternTxt[i] + ',' + LinkType_Count[i].ToString() + ',' + PatternTxt[i] + ','
                    + SpecOutLinkType_Count[i].ToString();
                str_summary = sr.Split(',');
                this.Summary.Rows.Add(str_summary);
            }
        }

    }
}
