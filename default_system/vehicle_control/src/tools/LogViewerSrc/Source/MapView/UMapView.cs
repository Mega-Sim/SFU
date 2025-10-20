using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ObjectDrawig;
using System.Diagnostics;
using System.IO;
using MathGeometry2D;
using MapView;
using System.Threading;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Net;
using System.Net.Sockets;
using Msg;
using System.Reflection;

namespace MapView
{

    public enum eKindOf_Grid { Node, Station, CID, Collision, Link, HID, FlowControl, Teaching, PathData }
    public enum eGridCol_Node { ID, Type, X_mm, Y_mm, Z_mm, LeftNodeID, LeftDistance, LeftSpeed, LeftSteering, LeftBranch, RightNodeID, RightDistance, RightSpeed, RightSteering, RightBranch, COUNTOF_ITEM }
    public enum eGridCol_Station { ID, PortType, X_mm, Y_mm, Z_mm, Type, ParentNode, NextNode, Offset, Material, COUNTOF_ITEM }
    public enum eGridCol_CID { ID, Type, ResetNode, StopNode1, StopNode2, ResetNodeType, CIDDir1, CommonStartNode1_1, CommonStartNode1_2, CommonStartNode1_3, CommonStartNode1_4, CommonStartNode1_5, CommonStartNode1_Offset, CIDDir2, CommonStartNode2_1, CommonStartNode2_2, CommonStartNode2_3, CommonStartNode2_4, CommonStartNode2_5, CommonStartNode2_Offset, COUNTOF_ITEM }
    public enum eGridCol_Collision { NodeID, LeftNode, LeftUBG, LeftObstacle, RightNode, RightUBG, RightObstacle, COUNTOF_ITEM }
    public enum eGridCol_Link { ID, FromNode, ToNode, Speed, Distance, HID, COUNTOF_ITEM }
    public enum eGridCol_HID { ID, UseLinkCount, Distance_miter, COUNTOF_ITEM }
    public enum eGridCol_FlowControl { StopNodeID, ResetNodeMain, ResetNodeMainDefaultValue, ResetNodeMainDefaultRatio, ResetNodeSub, ResetNodeSubDefaultValue, ResetNodeSubDefaultRatio, ResetNodeRedundance1, BZStopRedundance1, ResetNodeRedundance1DefaultValue, ResetNodeRedundance1DefaultRatio, COUNTOF_ITEM }
    public enum eGridCol_Teaching { StationID, StationPortType, Hoist, Shift, Rotate, PIOType, PIODir, PIOID, PIOChannel, PIOTimeLevel, AutoRecovery, HandDetectEQ, STBOffSet, LookDown, STBCorrect, COUNTOF_ITEM }
    public enum eGridCol_PathData { LayoutId, LayoutType, LeftNodeIDIn, LeftNodeIDMiddle, LeftNodeIDOut, LeftDriveDirection, RightNodeIDIn, RightNodeIDMiddle, RightNodeIDOut, RightDriveDirection, COUNTOF_ITEM }

    /*
      public LayoutType nLayoutType = LayoutType.MTL;
        public int nLayoutID = 0;
        
        public int nLeftNodeIDIn = 0;
        public int nLeftNodeIDMiddle = 0;              //< MTL(왼쪽), Lifter(아래) 중간 Node Id
        public int nLeftNodeIDOut = 0;                 //< MTL(왼쪽) 진출 Node Id
        public DriveDirection LeftDriveDirection = DriveDirection.Forward;            //< 차량 진행 방향을 표시

        public int nRightNodeIDIn = 0;                 //< MTL(오른쪽), Lifter(위) 진입 Node Id
        public int nRightNodeIDMiddle = 0;             //< MTL(오른쪽), Lifter(위) 중간 Node Id
        public int nRightNodeIDOut = 0;                //< MTL(오른쪽) 진출 Node Id
        public DriveDirection RightDriveDirection = DriveDirection.Forward;           //< 차량 진행 방향을 표시
         */
    public partial class UMapView : Form
    {
		//public static string VERSION = "2.0";
		static string PROGRAM_NAME = "OHT Map Viewer";
		public static bool OPEN_BY_OHTEASYVIEW = false;
        static UMapView thisObj = null;
        DScale m_Scale = new DScale();
        double dProperScale = 1;
        Graphics g = null;
        SolidBrush brush_background = new SolidBrush(Color.FromArgb(0x2D, 0x2D, 0x2D)); 
        Rectangle client_rect = new Rectangle();
        public Point curPos_mm = new Point();       // mouse 움직일 때 항상 마우스의 현재 좌표를 mm 단위로 갖음
        public bool inDrawBox = false;                  // 마우스가 움직이고 있는데, 현재 draw 박스 안에 있는지
        public bool bOnLoading = false;
        OpenFileDialog dlgMapOpen = new OpenFileDialog();
        UVisibleZLevel dlgZlayer = new UVisibleZLevel();
        DataGridView m_CurrentGrid = null;

        public static Font Font_TextInfo = new Font("맑은 고딕", 10F, FontStyle.Regular);
        UFindGridColData dlgFind = new UFindGridColData();
        List<DataGridView> arrGrid = new List<DataGridView>();

        private delegate void DelegateParseMessage(string msg);
        private DelegateParseMessage delegateParse;
        bool m_bMapLoaded = false;
		Point m_ptHoverItemPos_mm = new Point(Int32.MaxValue, Int32.MaxValue);

        //bool m_bWarningExists = false;
        public class UdpState
        {
            public IPEndPoint e;
            public UdpClient u;
        }
        UdpClient listenSock = null;
        IPEndPoint addr = null;
        UdpState m_udpState = new UdpState();
        int m_nMyPortNum = 0;

        public UMapView()
        {
            InitializeComponent();
			SetTitle("");
			delegateParse = new DelegateParseMessage(ParseMessage);

            //this.Text = "MapView " + VERSION;
            this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseWheel);

            this.pictureBox1.BackColor = brush_background.Color;
            this.splitContainer3.BackColor = brush_background.Color;
            this.pictureBox1.BackColor = brush_background.Color;
            this.toolStrip1.BackColor = brush_background.Color;
            this.listView_MapInfo.BackColor = brush_background.Color;
            this.splitContainer2.BackColor = brush_background.Color;
            this.listView_Selected.BackColor = brush_background.Color;
            this.listView_Flags.BackColor = brush_background.Color;
            this.dlgZlayer.BackColor = brush_background.Color;

            arrGrid.Add(dataGridView_node);
            arrGrid.Add(dataGridView_Station);
            arrGrid.Add(dataGridView_CID);
            arrGrid.Add(dataGridView_Collision);
            arrGrid.Add(dataGridView_Link);
            arrGrid.Add(dataGridView_HID);
            arrGrid.Add(dataGridView_FlowControl);
            arrGrid.Add(dataGridView_Teaching);
            arrGrid.Add(dataGridView_PathData);

            foreach (DataGridView grid in arrGrid)
            {
                grid.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_CellDoubleClick);
                grid.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_CellClick);
                grid.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView_KeyDown);
                grid.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView_MouseDown);
            }
           
            m_Scale.nDrawHalfWidth_dc = pictureBox1.Width / 2;
            m_Scale.nDrawHalfHeight_dc = pictureBox1.Height / 2;

            RefreshMapInfo();
            Redraw(false);


            // UDP 초기화
            /// OHTEasyView와 통신하기 위한 소켓 UDP
            // OHT Easy View : 10270 ~ 30개
            // MapView : 10370 ~ 30개
			// 이 부분때문에 젤 처음에 보안 관련 경고가 뜰 수 있다
            // 해당 포트가 사용중이면 exception 발생해서 다음 포트를 검색한다
            for (int i = 0; i <30; i++)
            {
                try
                {
                    listenSock = new UdpClient(10370+i);
                    addr = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 10370+i);
                    m_nMyPortNum = 10370 + i;
                    break;
                }
                catch (Exception)
                {
                    continue;
                } 
            }

            if (listenSock != null)
            {
                m_udpState.e = addr;
                m_udpState.u = listenSock;
                listenSock.BeginReceive(new AsyncCallback(ReceiveCallback), m_udpState);
            }
            else
            {
                if (MessageBox.Show("MapView 프로세스가 할당된 포트번호 개수 보다 더 많이 실행중입니다. 이 경우 OHT Easy View와 통신이 불가능 합니다. 안쓰는 MapView는 종료 후 다시 실행시켜 주세요.", "Warning", MessageBoxButtons.AbortRetryIgnore, MessageBoxIcon.Warning) != DialogResult.Ignore)
                    Application.Exit();
            }
            
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            thisObj = this;
            InitGrid();
            InitListView();

            // 실행시 아규먼트로 맵경로가 온게 있는지 확인
            string[] args = Environment.GetCommandLineArgs();

            if(args.Length >= 2)
            {
                if(File.Exists(args[1]) == false)
                {
                    MessageBox.Show(args[1] + " 파일이 존재하지 않습니다.");
                }
                if(args.Length >=3)
                {
                    if (args[2].ToUpper() == "/MINIZE")
                        this.WindowState = FormWindowState.Minimized;
                }
                MapLoad(args[1]);
            }
            
        }
		public void SetTitle(string strCurPath)
		{
			try
			{
				if (String.IsNullOrEmpty(strCurPath))
					strCurPath = "";

				Version v = Assembly.GetExecutingAssembly().GetName().Version;
				DateTime fileTm = File.GetLastWriteTime(Application.ExecutablePath);
				this.Text = String.Format("{0}/{1}/{2}  ", PROGRAM_NAME, v.ToString(), fileTm.ToShortDateString());

				if (strCurPath.Length > 0)
				{
					strCurPath = strCurPath.Replace(@"\", "/");
					this.Text += String.Format("  - {0}", strCurPath);
				}
			}
			catch
			{
			}
		}
		// grid 초기화. 그리드들이 많다~
		private void InitGrid()
        {
            toolStripComboBox_FindItem.Items.Add("Node");
            toolStripComboBox_FindItem.Items.Add("Station");
            toolStripComboBox_FindItem.Items.Add("CID");
            toolStripComboBox_FindItem.Items.Add("Collision");
            toolStripComboBox_FindItem.Items.Add("Link");
            toolStripComboBox_FindItem.Items.Add("HID");
            toolStripComboBox_FindItem.SelectedIndex = 0;

            TDrawingControl.SetDoubleBuffered(dataGridView_node);
            TDrawingControl.SetDoubleBuffered(dataGridView_Station);
            TDrawingControl.SetDoubleBuffered(dataGridView_CID);
            TDrawingControl.SetDoubleBuffered(dataGridView_Link);
            TDrawingControl.SetDoubleBuffered(dataGridView_Collision);
            TDrawingControl.SetDoubleBuffered(dataGridView_HID);
            TDrawingControl.SetDoubleBuffered(dataGridView_FlowControl);
            TDrawingControl.SetDoubleBuffered(dataGridView_Teaching);
            TDrawingControl.SetDoubleBuffered(dataGridView_PathData);

            // Grid Node **********
            DataGridView grid = dataGridView_node;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_Node.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;


            for (int i = 0; i < (int)eGridCol_Node.COUNTOF_ITEM; i++)
            {
                eGridCol_Node idx = (eGridCol_Node)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid Station **********
            grid = dataGridView_Station;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_Station.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_Station.COUNTOF_ITEM; i++)
            {
                eGridCol_Station idx = (eGridCol_Station)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid CID **********
            grid = dataGridView_CID;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_CID.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_CID.COUNTOF_ITEM; i++)
            {
                eGridCol_CID idx = (eGridCol_CID)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid Collision **********
            grid = dataGridView_Collision;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_Collision.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_Collision.COUNTOF_ITEM; i++)
            {
                eGridCol_Collision idx = (eGridCol_Collision)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid Link **********
            grid = dataGridView_Link;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_Link.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_Link.COUNTOF_ITEM; i++)
            {
                eGridCol_Link idx = (eGridCol_Link)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid HID **********
            grid = dataGridView_HID;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_HID.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;
            
            for (int i = 0; i < (int)eGridCol_HID.COUNTOF_ITEM; i++)
            {
                eGridCol_HID idx = (eGridCol_HID)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid FlowControl **********
            grid = dataGridView_FlowControl;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_FlowControl.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_FlowControl.COUNTOF_ITEM; i++)
            {
                eGridCol_FlowControl idx = (eGridCol_FlowControl)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid Teaching **********
            grid = dataGridView_Teaching;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_Teaching.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_Teaching.COUNTOF_ITEM; i++)
            {
                eGridCol_Teaching idx = (eGridCol_Teaching)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

            // Grid PathData **********
            grid = dataGridView_PathData;
            grid.RowPostPaint += DataGridView_RowPostPaint;
			grid.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			grid.RowHeadersWidth = 50;
            grid.Font = new Font("맑은 고딕", 9);
            grid.ColumnCount = (int)eGridCol_PathData.COUNTOF_ITEM;
            grid.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            grid.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
            grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            grid.ColumnHeadersDefaultCellStyle.Font = grid.Font;

            for (int i = 0; i < (int)eGridCol_PathData.COUNTOF_ITEM; i++)
            {
                eGridCol_PathData idx = (eGridCol_PathData)i;
                grid.Columns[i].Name = idx.ToString();
                grid.Columns[i].HeaderText = idx.ToString();
                //dataGridView_node.Columns[(int)i].DefaultCellStyle.Alignment = i == eAmcRcvIdx.Time_Date ? DataGridViewContentAlignment.MiddleCenter : DataGridViewContentAlignment.MiddleRight;
            }

            grid.AllowUserToResizeRows = false;
            grid.RowTemplate.Height = 20;

        }

        void InitListView()
        {
            listView_Selected.Columns.Add("No.");
            listView_Selected.Columns.Add("Type");
            listView_Selected.Columns.Add("ID");
            listView_Selected.Columns[0].Width = 50;
            listView_Selected.Columns[1].Width = 50;
            listView_Selected.Columns[2].Width = 120;
            listView_Selected.Columns[0].TextAlign = HorizontalAlignment.Center;
            listView_Selected.Columns[1].TextAlign = HorizontalAlignment.Center;
            listView_Selected.Columns[2].TextAlign = HorizontalAlignment.Center;

            listView_Flags.Columns.Add("Type");
            listView_Flags.Columns.Add("ID");
            listView_Flags.Columns.Add("Memo");
            listView_Flags.Columns[0].Width = 50;
            listView_Flags.Columns[1].Width = 60;
            listView_Flags.Columns[2].Width = 200;
            listView_Flags.Columns[0].TextAlign = HorizontalAlignment.Center;
            listView_Flags.Columns[1].TextAlign = HorizontalAlignment.Center;
            listView_Flags.Columns[2].TextAlign = HorizontalAlignment.Left;

            listView_MapInfo.Columns.Add("Info");
            listView_MapInfo.Columns.Add("Val");
            listView_MapInfo.Columns[0].Width = 60;
            listView_MapInfo.Columns[1].Width = 200;
            listView_MapInfo.Columns[0].TextAlign = HorizontalAlignment.Center;
            listView_MapInfo.Columns[1].TextAlign = HorizontalAlignment.Left;
        }
      
        // 그리드 Row헤더쪽에 번호매기기. 응근 유용함
        private void DataGridView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            var grid = sender as DataGridView;
            
            var rowIdx = (e.RowIndex+1).ToString();

            //폰트는 아무거나 지정 가능
            using (StringFormat rightFormat = new StringFormat())
            //using (Font font = new System.Drawing.Font(this.Font, FontStyle.Regular))
            {
                //숫자 매길 거니까, 우측 정렬.
                rightFormat.Alignment = StringAlignment.Far;
                rightFormat.LineAlignment = StringAlignment.Center;

                //로우셀렉터에서 우측 정렬로 숫자를 찍으니,
                //바운드는 좀 줄여야 함.
                var headerBounds = new Rectangle(e.RowBounds.Left, e.RowBounds.Top, grid.RowHeadersWidth - 10, e.RowBounds.Height);

                //숫자를 쓴다.
                e.Graphics.DrawString(rowIdx, grid.Font, SystemBrushes.ControlText, headerBounds, rightFormat);
            }
        }

        // 여러 탭에 들어있는 Grid들 정보 갱신하기
        private void MakeDatagridViewData()
        {
            tabControl_Object.SuspendLayout();
            DataGridView grid = null;
            progressBar1.Value = 50;
            // Data Add  ::  Node ****************
            grid = dataGridView_node;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            //  dynamic 자료형
            dynamic[] arrItems = new dynamic[(int)eGridCol_Node.COUNTOF_ITEM];
            int nSt = Environment.TickCount;

            foreach(DObjNode node in  DObjMan.arrNode)
            {
                arrItems[(int)eGridCol_Node.ID] = node.ID;
                arrItems[(int)eGridCol_Node.Type] = node.nodeType.ToString();
                arrItems[(int)eGridCol_Node.X_mm] = node.X_mm;
                arrItems[(int)eGridCol_Node.Y_mm] = node.Y_mm;
                arrItems[(int)eGridCol_Node.Z_mm] = node.Z_mm;
                arrItems[(int)eGridCol_Node.LeftNodeID] = node.leftNodeID;
                arrItems[(int)eGridCol_Node.LeftDistance] = node.leftNodeDist;
                arrItems[(int)eGridCol_Node.LeftSpeed] = node.leftNodeSpeed;
                arrItems[(int)eGridCol_Node.LeftSteering] = node.leftSteering.ToString();
                arrItems[(int)eGridCol_Node.LeftBranch] = node.leftBranch.ToString();
                arrItems[(int)eGridCol_Node.RightNodeID] = node.rightNodeID;
                arrItems[(int)eGridCol_Node.RightDistance] = node.rightNodeDist;
                arrItems[(int)eGridCol_Node.RightSpeed] = node.rightNodeSpeed;
                arrItems[(int)eGridCol_Node.RightSteering] = node.rightSteering.ToString();
                arrItems[(int)eGridCol_Node.RightBranch] = node.rightBranch.ToString();
                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 100;

            // Data Add  ::  Station ****************
            grid = dataGridView_Station;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_Station.COUNTOF_ITEM];
            foreach (DObjStation st in DObjMan.arrStation)
            {
                arrItems[(int)eGridCol_Station.ID] = st.ID;
                arrItems[(int)eGridCol_Station.PortType] = st.portType.ToString();
                arrItems[(int)eGridCol_Station.X_mm] = st.X_mm;
                arrItems[(int)eGridCol_Station.Y_mm] = st.Y_mm;
                arrItems[(int)eGridCol_Station.Z_mm] = st.Z_mm;
                arrItems[(int)eGridCol_Station.Type] = st.type.ToString();
                arrItems[(int)eGridCol_Station.ParentNode] = st.parentNode.ID;
                arrItems[(int)eGridCol_Station.NextNode] = st.nextNode.ID;
                arrItems[(int)eGridCol_Station.Offset] = st.distance_mm;
                arrItems[(int)eGridCol_Station.Material] = st.materialType.ToString();
                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 300;

            // Data Add  ::  CID ****************
            grid = dataGridView_CID;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_CID.COUNTOF_ITEM];
            foreach (DObjCID cid in DObjMan.arrCID)
            {
                arrItems[(int)eGridCol_CID.ID] = cid.layoutID;
                arrItems[(int)eGridCol_CID.Type] = cid.layoutType.ToString();
                arrItems[(int)eGridCol_CID.ResetNode] = cid.resetNodeID;
                arrItems[(int)eGridCol_CID.StopNode1] = cid.stopNode1ID;
                arrItems[(int)eGridCol_CID.StopNode2] = cid.stopNode2ID;
                arrItems[(int)eGridCol_CID.ResetNodeType] = cid.resetNodeType.ToString();
                arrItems[(int)eGridCol_CID.CIDDir1] = cid.dir1.ToString();
                arrItems[(int)eGridCol_CID.CommonStartNode1_1] = cid.commonStartNode1_1;
                arrItems[(int)eGridCol_CID.CommonStartNode1_2] = cid.commonStartNode1_2;
                arrItems[(int)eGridCol_CID.CommonStartNode1_3] = cid.commonStartNode1_3;
                arrItems[(int)eGridCol_CID.CommonStartNode1_4] = cid.commonStartNode1_4;
                arrItems[(int)eGridCol_CID.CommonStartNode1_5] = cid.commonStartNode1_5;
                arrItems[(int)eGridCol_CID.CommonStartNode1_Offset] = cid.commonStartNode1Offset;

                arrItems[(int)eGridCol_CID.CIDDir2] = cid.dir2.ToString();
                arrItems[(int)eGridCol_CID.CommonStartNode2_1] = cid.commonStartNode2_1;
                arrItems[(int)eGridCol_CID.CommonStartNode2_2] = cid.commonStartNode2_2;
                arrItems[(int)eGridCol_CID.CommonStartNode2_3] = cid.commonStartNode2_3;
                arrItems[(int)eGridCol_CID.CommonStartNode2_4] = cid.commonStartNode2_4;
                arrItems[(int)eGridCol_CID.CommonStartNode2_5] = cid.commonStartNode2_5;
                arrItems[(int)eGridCol_CID.CommonStartNode2_Offset] = cid.commonStartNode2Offset;

                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 500;

            // Data Add  ::  Collision ****************
            grid = dataGridView_Collision;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_Collision.COUNTOF_ITEM];
            foreach (DObjCollision coll in DObjMan.arrCollision)
            {
                arrItems[(int)eGridCol_Collision.NodeID] = coll.node.ID ;
                arrItems[(int)eGridCol_Collision.LeftNode] = coll.leftNode == null ? 0 : coll.leftNode.ID;
                arrItems[(int)eGridCol_Collision.LeftUBG] = coll.leftUBG.ToString();
                arrItems[(int)eGridCol_Collision.LeftObstacle] = coll.leftObstacle.ToString();
                arrItems[(int)eGridCol_Collision.RightNode] = coll.rightNode == null ? 0 :  coll.rightNode.ID;
                arrItems[(int)eGridCol_Collision.RightUBG] = coll.rightUBG.ToString();
                arrItems[(int)eGridCol_Collision.RightObstacle] = coll.rightObstacle.ToString();
                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 700;

            // Data Add  ::  Link ****************
            grid = dataGridView_Link;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_Link.COUNTOF_ITEM];
            foreach (DObjLink link in DObjMan.arrLink)
            {
                arrItems[(int)eGridCol_Link.ID] = link.GetID();
                arrItems[(int)eGridCol_Link.FromNode] = link.NodeA.ID;
                arrItems[(int)eGridCol_Link.ToNode] = link.NodeB.ID;
                arrItems[(int)eGridCol_Link.Speed] = link.Speed;
                if(link.Hid != null)
                    arrItems[(int)eGridCol_Link.HID] = link.Hid.ID;
                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 800;

            // Data Add  ::  HID ****************
            grid = dataGridView_HID;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_HID.COUNTOF_ITEM];
            foreach (DObjHID hid in DObjMan.arrHID)
            {
                arrItems[(int)eGridCol_HID.ID] = hid.ID;
                arrItems[(int)eGridCol_HID.UseLinkCount] = hid.myLinkArray.Count;
                arrItems[(int)eGridCol_HID.Distance_miter] = hid.nTotalLength_mm / 1000.0;
                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 900;

            // Data Add  ::  FlowControl ****************
            grid = dataGridView_FlowControl;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_FlowControl.COUNTOF_ITEM];
            foreach (DObjFlowControl flowCtrl in DObjMan.arrFlowControl)
            {
                arrItems[(int)eGridCol_FlowControl.StopNodeID] = flowCtrl.StopNodeID;
                arrItems[(int)eGridCol_FlowControl.ResetNodeMain] = flowCtrl.ResetNodeMain;
                arrItems[(int)eGridCol_FlowControl.ResetNodeMainDefaultValue] = flowCtrl.ResetNodeMainDefaultValue;
                arrItems[(int)eGridCol_FlowControl.ResetNodeMainDefaultRatio] = flowCtrl.ResetNodeMainDefaultRatio;
                arrItems[(int)eGridCol_FlowControl.ResetNodeSub] = flowCtrl.ResetNodeSub;
                arrItems[(int)eGridCol_FlowControl.ResetNodeSubDefaultValue] = flowCtrl.ResetNodeSubDefaultValue;
                arrItems[(int)eGridCol_FlowControl.ResetNodeSubDefaultRatio] = flowCtrl.ResetNodeSubDefaultRatio;
                arrItems[(int)eGridCol_FlowControl.ResetNodeRedundance1] = flowCtrl.ResetNodeRedundance1;
                arrItems[(int)eGridCol_FlowControl.BZStopRedundance1] = flowCtrl.BZStopRedundance1;
                arrItems[(int)eGridCol_FlowControl.ResetNodeRedundance1DefaultValue] = flowCtrl.ResetNodeRedundance1DefaultValue;
                arrItems[(int)eGridCol_FlowControl.ResetNodeRedundance1DefaultRatio] = flowCtrl.ResetNodeRedundance1DefaultRatio;

                int nIdx = grid.Rows.Add(arrItems);
                grid.Rows[nIdx].Cells[0].Tag = flowCtrl.flowLink;       // 0번째 tag에 link 실제 obj를 넣어놓자
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);
            progressBar1.Value = 950;

            // Data Add  ::  Teaching ****************
            grid = dataGridView_Teaching;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();
            
            arrItems = new dynamic[(int)eGridCol_Teaching.COUNTOF_ITEM];
            foreach (DObjTeaching teach in DObjMan.arrTeaching)
            {
                arrItems[(int)eGridCol_Teaching.StationID] = teach.nStationID;
                arrItems[(int)eGridCol_Teaching.StationPortType] = teach.portType.ToString();
                arrItems[(int)eGridCol_Teaching.Hoist] = teach.Hoist / 10.0;
                arrItems[(int)eGridCol_Teaching.Shift] = teach.Shift / 10.0;
                arrItems[(int)eGridCol_Teaching.Rotate] = teach.Rotate;
                arrItems[(int)eGridCol_Teaching.PIOType] = teach.PIOType.ToString();
                arrItems[(int)eGridCol_Teaching.PIODir] = teach.PIODir.ToString();
                arrItems[(int)eGridCol_Teaching.PIOID] = teach.PIOID;
                arrItems[(int)eGridCol_Teaching.PIOChannel] = teach.PIOChannel;
                arrItems[(int)eGridCol_Teaching.PIOTimeLevel] = teach.PIOTimeLevel;
                arrItems[(int)eGridCol_Teaching.AutoRecovery] = teach.AutoRecovery.ToString();
                arrItems[(int)eGridCol_Teaching.HandDetectEQ] = teach.HandDetectEQ;
                arrItems[(int)eGridCol_Teaching.STBOffSet] = teach.STBOffSet;
                arrItems[(int)eGridCol_Teaching.LookDown] = teach.LookDown;
                arrItems[(int)eGridCol_Teaching.STBCorrect] = teach.STBL_Shift;

                grid.Rows.Add(arrItems);
            }
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);

            // Data Add  ::  Path Data ****************
            grid = dataGridView_PathData;
            TDrawingControl.SuspendDrawing(grid);
            grid.Rows.Clear();

            arrItems = new dynamic[(int)eGridCol_PathData.COUNTOF_ITEM];
            foreach (DObjPathData pathData in DObjMan.arrPathData)
            {
                arrItems[(int)eGridCol_PathData.LayoutId] = pathData.nLayoutID;
                arrItems[(int)eGridCol_PathData.LayoutType] = pathData.nLayoutType.ToString();
                arrItems[(int)eGridCol_PathData.LeftNodeIDIn] = pathData.nLeftNodeIDIn;
                arrItems[(int)eGridCol_PathData.LeftNodeIDMiddle] = pathData.nLeftNodeIDMiddle;
                arrItems[(int)eGridCol_PathData.LeftNodeIDOut] = pathData.nLeftNodeIDOut;
                arrItems[(int)eGridCol_PathData.LeftDriveDirection] = pathData.LeftDriveDirection.ToString();
                arrItems[(int)eGridCol_PathData.RightNodeIDIn] = pathData.nRightNodeIDIn;
                arrItems[(int)eGridCol_PathData.RightNodeIDMiddle] = pathData.nRightNodeIDMiddle;
                arrItems[(int)eGridCol_PathData.RightNodeIDOut] = pathData.nRightNodeIDOut;
                arrItems[(int)eGridCol_PathData.RightDriveDirection] = pathData.RightDriveDirection.ToString();
                
                grid.Rows.Add(arrItems);
            }
            //grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            TDrawingControl.ResumeDrawing(grid);

            progressBar1.Value = 1000;

            tabControl_Object.ResumeLayout();
        }
        
        // 윈도우 크기에 맞게 도면 사이즈 조정
        public void FitWindowAll(bool bSelectedOnly)
        {
            if (DObjMan.arrNode.Count() < 1)
                return;

            DRect_mm rtData_mm = DObjMan.ALL.GetTotalRect_mm(false, bSelectedOnly);
            Point ptCenter = rtData_mm.CenterPoint();
            this.m_Scale.SetCenterX(-ptCenter.X);
            this.m_Scale.SetCenterY(-(-ptCenter.Y));

            double newScale = MathSupport.GetRectScale(rtData_mm, pictureBox1.ClientRectangle) * (bSelectedOnly ? 1 : 0.5);
            newScale = Math.Max(newScale, 1);
            newScale = Math.Min(newScale, 1000);
            m_Scale.SetScale(newScale);
            Redraw(false);
        }

        // 선택된 객체를 focus를 맞추어 보여주고자 할때
        // 센터좌표 x, y 좌표 기준으로 보기 좋은 스케일을 만들어준다.  보기 좋은 스케일을 만들어 주는건 링크들 평균 길이르 가지고함.
        public void SelectedEasyFocusView(bool bUseProperScale)
        {
            DRect_mm rtData_mm = DObjMan.ALL.GetTotalRect_mm(false, true);
            Point ptCenter = rtData_mm.CenterPoint();

            if (bUseProperScale)
                m_Scale.SetScale(dProperScale);
            else
            {
                double newScale = MathSupport.GetRectScale(rtData_mm, pictureBox1.ClientRectangle);
                m_Scale.SetScale(newScale);
            }

            this.m_Scale.SetCenterX(-ptCenter.X);
            this.m_Scale.SetCenterY(-(-ptCenter.Y));
            Redraw(false);
        }
        // 도면상 0,0으로 이동
        public void GotoZero()
        {
            m_Scale.SetCenterX(0);
            m_Scale.SetCenterY(0);
            m_Scale.SetScale(1.0);

            Redraw(false);
        }

        // 도면 특정 위치로 이동 (상대이동)
        public void GotoRel(int x_mm, int y_mm)
        {
            m_Scale.SetCenterX(m_Scale.CenterPos_mm.X + x_mm);
            m_Scale.SetCenterY(m_Scale.CenterPos_mm.Y - y_mm);

            Redraw(false);
        }

        // 도면 특정 위치로 이동 (절대이동)
        public void GotoAbs(int x_mm, int y_mm)
        {
            m_Scale.SetCenterX(x_mm);
            m_Scale.SetCenterY(y_mm);

            Redraw(false);
        }

        // 스크롤바로 줌 조정할때
        public void ZoomIn(bool bIn, int x_dc, int y_dc/*int x_dc = INT_MIN, int y_dc = INT_MIN*/) // INT_MIN값이면 화면 중앙을 중심으로 확대한다.
        {
            if (!inDrawBox) // 마우스가 도면에 있을때만 처리위함
                return;
            int x1_mm = m_Scale.DC2MM_X(x_dc);
            int y1_mm = m_Scale.DC2MM_Y(y_dc);
            if (bIn)
            {
                if (m_Scale.dScale < 0.5)
                    return;

                m_Scale.MultiplyScale(0.9);
            }
            else
            {
                if (m_Scale.dScale > 1000)
                    return;

                m_Scale.MultiplyScale(1.1);
            }
            // 좌표를 입력하면 해당 좌표를 중심으로 확대한다.
            //if (x_dc != DBL_MIN && y_dc != DBL_MIN)
            {
                int x2_mm = m_Scale.DC2MM_X(x_dc);
                int y2_mm = m_Scale.DC2MM_Y(y_dc);
                m_Scale.SetCenterX(m_Scale.CenterPos_mm.X + (x2_mm - x1_mm));
                m_Scale.SetCenterY(m_Scale.CenterPos_mm.Y - (y2_mm - y1_mm));
            }
            Redraw(false);
        }
        public void ZoomPercent(double dPercent, int x_dc, int y_dc/* int x_dc = INT_MIN, int y_dc = INT_MIN*/) // 전체 오브젝트 기준의 스케일 값에서 퍼센트 곱해서 확대/축소한다.
        {
            m_Scale.dScale = 100.0 / dPercent;
            Redraw(false);
        }
        double GetZoomPercent()
        {
            return 100.0 / m_Scale.dScale;
        }

        private void panel1_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            ZoomIn(e.Delta > 0, e.X - this.pictureBox1.Location.X, e.Y - this.pictureBox1.Location.Y);
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {

            g = e.Graphics;
            DObjMan.SetGraphicsScale(e.Graphics, this.m_Scale);
            Redraw(true);


        }

        bool bOnce = true;
        public void Redraw(bool byMessage)
        {
            try
            {
                if (m_bMapLoaded == false)
                    return;

                if (bOnce)
                {
                    bOnce = false;
                    FitWindowAll(false);
                }

                if (!byMessage)
                {
                    pictureBox1.Invalidate();
                    return;
                }

                pictureBox1.SuspendLayout();

                int nStartTick = Environment.TickCount;

                DrawBackground();
                //DObjBase.curMousePos_mm = curPos_mm;
                DObjMan.arrLink.Draw();
                if (this.m_Scale.dScale < DObjBase.VISIBLE_SCALE_NODE_STATION_MUST_HIDE)
                {
                    DObjMan.arrNode.Draw();
                    DObjMan.arrStation.Draw();
                }
                DObjMan.arrCID.Draw();
                DObjMan.arrHID.Draw();
                DObjMan.arrFlowControl.Draw();

                DObjMan.arrNode.DrawFlag();
                DObjMan.arrStation.DrawFlag();
				DObjMan.arrDistance.Draw();

                // 호버링 정보 표시 처리
                DrawHoverItems();

                // 마우스 드래그 영역 표시
                DrawOnSelectingArea();

                // 맵정보 화면 갱신
                //RefreshMapInfo();

                toolStripLabel_RedrawTime.Text = Convert.ToString(Environment.TickCount - nStartTick) + "ms for drawing";
                //Trace.WriteLine(String.Format("drawing time : {0} ms", nElapse));
                pictureBox1.ResumeLayout();
            }
            catch (Exception)
            {
            }
        }

        // 호버링 된 객체 정보 표시
        private void DrawHoverItems()
        {
            DObjArray arr = new DObjArray();
            arr.AddRange(DObjMan.arrNode);
            arr.AddRange(DObjMan.arrStation);

			m_ptHoverItemPos_mm.X = Int32.MaxValue;     // 기본이 int 최대값
			m_ptHoverItemPos_mm.Y = Int32.MaxValue;

			arr = arr.PtInRectArray(curPos_mm, true);
            if (arr.Count == 0)
                return;

            string strInfo = "";
	

			int nXPos = 0, nYPos = 0, nCnt = 0;
            int nRectHeight = 0;
            foreach(DObjBase obj in arr)
            {
                Point pt = obj.GetRect_mm().CenterPoint();
                nXPos += pt.X;
                nYPos += pt.Y;      // 호버링된게 두개 세개 될수도 있어서 그것들 좌표 평균 위치에 적절하게 표현하기 위함

				if(m_ptHoverItemPos_mm.X == Int32.MaxValue)
				{
					m_ptHoverItemPos_mm = pt;   // 호버링 된게 여러개여도 노드(처음만난 객체) 기준으로 잡자
					if (obj.ObjType == DObjBase.eChildObjType.Station)
					{
						DObjStation sobj = (DObjStation)obj;	// stb에 호버링 된 경우 stb의 옵셋은 무시하자
						if (sobj.portType == eStationPortType.LSTB || sobj.portType == eStationPortType.RSTB)
						{
							int nTmpX = m_ptHoverItemPos_mm.X - sobj.X_mm;
							int nTmpY = m_ptHoverItemPos_mm.Y - sobj.Y_mm;
							m_ptHoverItemPos_mm.X -= nTmpX;
							m_ptHoverItemPos_mm.Y -= nTmpY;
						}
					}
				}

                nCnt++;
                if (obj.ObjType == DObjBase.eChildObjType.Node)
                    nRectHeight += 250;
                else if(obj.ObjType == DObjBase.eChildObjType.Station)
                    nRectHeight += 160;

                if (String.IsNullOrEmpty(strInfo))
                    strInfo = obj.GetHoverInfo();
                else
                    strInfo += "\n\n" + obj.GetHoverInfo();
            }
            nXPos = (int)((double)nXPos / nCnt);
            nYPos = (int)((double)nYPos / nCnt);
            // 맑은 고딕 10F 기준 한줄에 45pix 정도 임
            int xdc = m_Scale.MM2DC_X(nXPos);
            int ydc = m_Scale.MM2DC_Y(nYPos);

            Rectangle rt_dc = new Rectangle(xdc+30, ydc-20, 200, nRectHeight);
            g.FillRectangle(Brushes.DarkSlateGray, rt_dc);
            g.DrawRectangle(Pens.White, rt_dc);
            g.DrawString(strInfo, Font_TextInfo, Brushes.MintCream, rt_dc.X+5, rt_dc.Y+ 5);
            
        }
        private void DrawOnSelectingArea()
        {
            if (Control.ModifierKeys == Keys.Alt)
                return;

            if(bOnSelecting)
                g.DrawRectangle(Pens.Aqua, rtOnSelectingBox_dc);
            if(bOnAreaScaling)
                g.DrawRectangle(Pens.HotPink, rtOnAreaScaleBox_dc);
        }
        private void DrawBackground()
        {
            client_rect = new Rectangle(0, 0, 2000, 2000);
            g.FillRectangle(brush_background, client_rect);  //back ground

            
        }
        private void pictureBox1_Resize(object sender, EventArgs e)
        {
            m_Scale.nDrawHalfWidth_dc = pictureBox1.Width/2;
            m_Scale.nDrawHalfHeight_dc = pictureBox1.Height / 2;
            
            int nProperSize_mm = DObjLink.nAverageLinkLength_mm;
            DRect_mm rt = new DRect_mm(-nProperSize_mm / 2, nProperSize_mm / 2, nProperSize_mm * 2, -nProperSize_mm / 2);
            double newScale = MathSupport.GetRectScale(rt, pictureBox1.ClientRectangle);
            if (newScale <= 0)
                return;

            dProperScale = newScale;
            Redraw(false);
        }

        // 좌클릭하고 선택하기 위해서 영역 쫘~ 끌때 필요한 변수들
        Point ptLBtnDownPos_dc = new Point();
        Point ptLBtnMovePos_dc = new Point();
        Rectangle rtOnSelectingBox_dc = new Rectangle();
        bool bLBtnDown = false;
        bool bOnSelecting = false;
		bool bOnDistance = false;   // 현재 distance 모드면
		DObjDist m_CurDistObj = null;	// 현재 그리고 있는 dist 객체

        // 우클릭 하고 영역에 맞게 Scaling 하려고 쫙~ 끌때 필요한 변수들
        Point ptRBtnDownPos_dc = new Point();
        Point ptRBtnMovePos_dc = new Point();
        Rectangle rtOnAreaScaleBox_dc = new Rectangle();
        bool bRBtnDown = false;
        bool bOnAreaScaling = false;
        
        // 마우스 휠 확대 축소 할때 필요한 변수들
        Point ptWheelDownPos_dc = new Point();
        Point ptBackupCenter_mm = new Point();
        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (DObjMan.bOnLoading)
                return;
			
            // LBtn 눌러서 영역 만들때
            if(e.Button == MouseButtons.Left && Control.ModifierKeys != Keys.Alt)   // alt 눌리면 이동이다(휠버튼 같은 효과)
            {
                ptLBtnDownPos_dc.X = e.X;
                ptLBtnDownPos_dc.Y = e.Y;
                bLBtnDown = true;
            }
            // RBtn 눌러서 영역 만들때
            if (e.Button == MouseButtons.Right)
            {
                ptRBtnDownPos_dc.X = e.X;
                ptRBtnDownPos_dc.Y = e.Y;
                bRBtnDown = true;
            }
            if (e.Button == MouseButtons.Left && Control.ModifierKeys == Keys.Alt)   // alt + L버튼 == 휠버튼과 같은 효과
            {
                ptWheelDownPos_dc.X = e.X;
                ptWheelDownPos_dc.Y = e.Y;

                ptBackupCenter_mm = m_Scale.CenterPos_mm;
            }
            // 휠버튼 down시 위치 기억
            if (e.Button == MouseButtons.Middle)    // 휠 이동
            {
                ptWheelDownPos_dc.X = e.X;
                ptWheelDownPos_dc.Y = e.Y;

                ptBackupCenter_mm = m_Scale.CenterPos_mm;
            }
			if (e.Button == MouseButtons.Left && Control.ModifierKeys != Keys.Alt && bOnDistance)    // 거리 재는중
			{
				if (m_CurDistObj == null)	// dist 객체 없으면 생성, 추가
				{
					m_CurDistObj = new DObjDist();
					m_CurDistObj.AddStart();

					DObjMan.arrDistance.Add(m_CurDistObj);	// 점 추가
				}

				if(m_ptHoverItemPos_mm.X == Int32.MaxValue)
					m_CurDistObj.AddPoint(curPos_mm);		// 호버링 된 객체가 없으면 마우스 포스 기준
				else
					m_CurDistObj.AddPoint(m_ptHoverItemPos_mm);   // 호버링된게 있으면 호버링 객체 기준으로 점 추가
			}
			
        }
        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (DObjMan.bOnLoading)
                return;

            bOnSelecting = false;
            bOnAreaScaling = false;
            // 도면 선택 영역 지정중
            if (bLBtnDown && MouseButtons == MouseButtons.Left && m_Scale.dScale < DObjBase.VISIBLE_SCALE_NODE_STATION_MUST_HIDE)
            {
                bOnSelecting = true;
                ptLBtnMovePos_dc.X = e.X;
                ptLBtnMovePos_dc.Y = e.Y;

                rtOnSelectingBox_dc.Width = rtOnSelectingBox_dc.Height = 0;     // init box
                int nWidth = ptLBtnMovePos_dc.X - ptLBtnDownPos_dc.X;
                int nHeight = ptLBtnMovePos_dc.Y - ptLBtnDownPos_dc.Y;

                if (nWidth <= 0 || nHeight <= 0)
                    return;
                rtOnSelectingBox_dc.X = ptLBtnDownPos_dc.X;
                rtOnSelectingBox_dc.Y = ptLBtnDownPos_dc.Y;
                rtOnSelectingBox_dc.Width = nWidth;
                rtOnSelectingBox_dc.Height = nHeight;

                //Redraw(false);
            }
            // 우클릭 영역 지정중
            if (bRBtnDown && MouseButtons == MouseButtons.Right)
            {
                bOnAreaScaling = true;
                ptRBtnMovePos_dc.X = e.X;
                ptRBtnMovePos_dc.Y = e.Y;

                rtOnAreaScaleBox_dc.Width = rtOnAreaScaleBox_dc.Height = 0;     // init box
                int nWidth = ptRBtnMovePos_dc.X - ptRBtnDownPos_dc.X;
                int nHeight = ptRBtnMovePos_dc.Y - ptRBtnDownPos_dc.Y;

                if (nWidth <= 0 || nHeight <= 0)
                    return;
                rtOnAreaScaleBox_dc.X = ptRBtnDownPos_dc.X;
                rtOnAreaScaleBox_dc.Y = ptRBtnDownPos_dc.Y;
                rtOnAreaScaleBox_dc.Width = nWidth;
                rtOnAreaScaleBox_dc.Height = nHeight;

                //Redraw(false);
            }
            // 휠버튼 이동으로 도면 이동 (마우스 없이 쓸수도 있어서 ALT 키 눌른것도 같은 효과)
            if (MouseButtons == MouseButtons.Middle || (MouseButtons == MouseButtons.Left && Control.ModifierKeys == Keys.Alt))
            {
                Point ptOffset_dc = new Point(ptWheelDownPos_dc.X - e.X,  ptWheelDownPos_dc.Y - e.Y);
                Point ptOffset_mm = m_Scale.DC2MM(ptOffset_dc);
                Point ptZero = m_Scale.DC2MM(new Point(0, 0));
                ptOffset_mm = new Point(ptOffset_mm.X - ptZero.X, -ptOffset_mm.Y+ ptZero.Y);
                m_Scale.CenterPos_mm.X = ptBackupCenter_mm.X - ptOffset_mm.X;
                m_Scale.CenterPos_mm.Y = ptBackupCenter_mm.Y - ptOffset_mm.Y;
                //Redraw(false);
                //return;
            }

            // 마우스가 움직일떄 계속해서 마우스 위치 갱신해서 redraw한다.
            // 이유는 특정 station들위에 마우스가 hover되면 그려주는 양식이 바뀌기 때문에
            curPos_mm.X = m_Scale.DC2MM_X(e.X);
            curPos_mm.Y = m_Scale.DC2MM_Y(e.Y);
            DObjBase.curMousePos_mm = curPos_mm;

            Redraw(false);
            //Trace.WriteLine(String.Format("Rbtn: {0}", bRBtnDown && MouseButtons == MouseButtons.Right));
        }
        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (Control.ModifierKeys == Keys.Alt)
            {
                bLBtnDown = false;
                bOnSelecting = false;
            }
            if (bLBtnDown && bOnSelecting)
            {
                if (rtOnSelectingBox_dc.Width <= 0 || rtOnSelectingBox_dc.Height <= 0)
                    return;
                bLBtnDown = false;
                bOnSelecting = false;

                bool bShift = (Control.ModifierKeys & Keys.Shift) != 0;
                bool bControl = (Control.ModifierKeys & Keys.Control) != 0;

                DRect_mm rt_mm = m_Scale.DC2MM(rtOnSelectingBox_dc);

                DObjMan.ALL.SelectArea(rt_mm, bControl, bShift);
                RefreshSelectedList();
                Redraw(false);
            }
            if (bRBtnDown && bOnAreaScaling)
            {
                if (rtOnAreaScaleBox_dc.Width <= 0 || rtOnAreaScaleBox_dc.Height <= 0)
                    return;
                bRBtnDown = false;
                bOnAreaScaling = false;
                
                DRect_mm rtData_mm = m_Scale.DC2MM(rtOnAreaScaleBox_dc);
                
                Point ptCenter = rtData_mm.CenterPoint();
                this.m_Scale.SetCenterX(-ptCenter.X);
                this.m_Scale.SetCenterY(ptCenter.Y);

                double newScale = MathSupport.GetRectScale(rtData_mm, pictureBox1.ClientRectangle);
                newScale = Math.Max(newScale, 1);
                newScale = Math.Min(newScale, 1000);
                m_Scale.SetScale(newScale);

                Redraw(false);
            }
        }

        // 도면에서 우클릭했을 때 팝업 처리
        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right && bOnAreaScaling == false)
                contextMenuStrip_OnDraw.Show((Control)sender, new Point(e.X, e.Y));

        }
        // 더블클릭했을 때
        private void pictureBox1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            // 특정 슽에이션 이나 노드 더블클릭하면 select 하고 하단 표에서 바로 보여준다
            DObjArray arr = new DObjArray();
            arr.AddRange(DObjMan.arrNode);
            arr.AddRange(DObjMan.arrStation);

            DObjMan.arrCID.VisibleAll(false);
            DObjMan.arrHID.VisibleAll(false);

            arr = arr.PtInRectArray(curPos_mm, true);
            if (arr.Count == 0)
                return;

            DObjBase obj = arr.ElementAt(0);
            obj.Selected = !obj.Selected;
            if (obj.ObjType == DObjBase.eChildObjType.Node)
            {
                FindGridItem(eKindOf_Grid.Node, obj.GetID());
                RefreshSelectedList();
            }
            else if(obj.ObjType == DObjBase.eChildObjType.Station)
            {
                FindGridItem(eKindOf_Grid.Station, obj.GetID());
                RefreshSelectedList();
            }
        }
        public void ShowStation(int nStationID)
        {
            if (DObjMan.arrStation.Count == 0)
            {
                MessageBox.Show("Map is not opened.");
                return;
            }

            DObjMan.ALL.SelectAll(false);
            DObjMan.arrCID.VisibleAll(false);
            DObjMan.arrHID.VisibleAll(false);
            
            DObjBase obj = DObjStation.FindStation(nStationID);
            if (obj == null)
            {
                MessageBox.Show("Can not find station.");
                return;
            }

            obj.Selected = true;
            SelectedEasyFocusView(true);
            FindGridItem(eKindOf_Grid.Station, obj.GetID());
            RefreshSelectedList();
            this.Visible = true;
            this.BringToFront();
        }

        public void ShowNode(int nNodeID)
        {
            if (DObjMan.arrNode.Count == 0)
            {
                MessageBox.Show("Map is not opened.");
                return;
            }

            DObjMan.ALL.SelectAll(false);
            DObjMan.arrCID.VisibleAll(false);
            DObjMan.arrHID.VisibleAll(false);

            DObjBase obj = DObjNode.FindNode(nNodeID);
            if (obj == null)
            {
                MessageBox.Show("Can not find node.");
                return;
            }

            obj.Selected = true;
            SelectedEasyFocusView(true);
            FindGridItem(eKindOf_Grid.Node, obj.GetID());
            RefreshSelectedList();
            this.Visible = true;
            this.BringToFront();
        }

        private void RefreshSelectedList()
        {
            listView_Selected.Items.Clear();
            List<DObjBase> arr = DObjMan.ALL.FindAll(o => o.Selected);

            string[] strItems = new string[3];
            int nCnt = 1;
            foreach(DObjBase obj in arr)
            {
                strItems[0] = Convert.ToString(nCnt++);
                strItems[1] = obj.ObjType.ToString();
                strItems[2] = obj.GetID();

                ListViewItem lvi = new ListViewItem(strItems);
                lvi.Tag = obj;  // 테그에 객체를 연결해놓는다
                listView_Selected.Items.Add(lvi);
            }
        }
        private void RefreshFlagList()
        {
            listView_Flags.Items.Clear();
            List<DObjBase> arr = DObjMan.ALL.FindAll(o => o.bDrawFlag);

            string[] strItems = new string[3];
            
            foreach (DObjBase obj in arr)
            {
                strItems[0] = obj.ObjType.ToString();
                strItems[1] = obj.GetID();
                strItems[2] = obj.strFlagMemo;

                ListViewItem lvi = new ListViewItem(strItems);
                lvi.Tag = obj;  // 테그에 객체를 연결해놓는다
                listView_Flags.Items.Add(lvi);
            }
        }

        public void SaveFlagList()
        {
            try
            {
                if(listView_Flags.Items.Count < 1)
                {
                    MessageBox.Show("지정된 Flag가 없습니다.");
                    return;
                }
                SaveFileDialog dlg = new SaveFileDialog();
                dlg.FileName = "*";
                dlg.DefaultExt = "txt";
                dlg.ValidateNames = true;

                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                List<string> contents = new List<string>();
                foreach (ListViewItem item in listView_Flags.Items)
                {
                    string str = "";
                    for (int i = 0; i < 3; i++)
                    {
                        str += item.SubItems[i].Text;
                        if (i != 2)
                            str += "/";
                    }
                    contents.Add(str);
                }

                File.WriteAllLines(dlg.FileName, contents.ToArray());
                MessageBox.Show("Flag 저장 성공!");
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return;
            }
        }
        public void LoadFlagList()
        {
            try
            {
                OpenFileDialog dlg = new OpenFileDialog();
                dlg.FileName = "*";
                dlg.DefaultExt = "txt";
                dlg.ValidateNames = true;

                if (dlg.ShowDialog() != DialogResult.OK)
                    return;

                string errMsg = "";
                foreach(DObjBase obj in DObjMan.ALL)
                {
                    obj.bDrawFlag = false;
                }

                string[] lines = File.ReadAllLines(dlg.FileName);
                int nCnt = 0;
                foreach(string line in lines)
                {
                    string[] data = line.Split('/');
                    if (data.Length < 3)
                    {
                        errMsg += "잘못된 구분자 발견\n";
                        continue;
                    }

                    string type = data[0];
                    string id = data[1];
                    int nID = 0;
                    if (Int32.TryParse(id, out nID) == false)
                        continue;

                    string memo = "";
                    for(int i=2; i< data.Length ; i++)
                    {
                        memo += data[i];
                        if (i != data.Length - 1)
                            memo += "/";
                    }
                    DObjBase obj = null;
                    if (type.ToUpper() == "STATION")
                    {
                        obj = DObjStation.FindStation(nID);
                        if(obj == null)
                        {   errMsg += String.Format("Station ID {0} 를 찾을 수 없음\n", id);
                            continue;
                        }
                    }
                    else if (type.ToUpper() == "NODE")
                    {
                        obj =  DObjNode.FindNode(nID);
                        if (obj == null)
                        {   errMsg += String.Format("Node ID {0} 를 찾을 수 없음\n", id);
                            continue;
                        }
                    }
                    if(obj == null)
                    {
                        errMsg += String.Format("지원하지 않는 타입 {0}\n", type);
                        continue;
                    }
                    nCnt++;
                    obj.bDrawFlag = true;
                    obj.strFlagMemo = memo;
                }
                if(nCnt == 0)
                {
                    MessageBox.Show("Flag 정보를 찾을 수 없습니다.");
                    return;
                }
                if(errMsg != "")
                {
                    MessageBox.Show("에러를 발견했습니다.\n\n" + errMsg);
                }
                RefreshFlagList();
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return;
            }
        }

        int m_nLoadStartTick = 0;

        // 맵 로딩 버튼
        public void buttonMapLoading(object sender, EventArgs e)
        {
			bOnDistance = false;

			dlgMapOpen.Filter = "Map File (*^*^*^*.zip)|*^*^*^*.zip";


            if (dlgMapOpen.ShowDialog() != DialogResult.OK)
                return;

            MapLoad(dlgMapOpen.FileName);
        }
        public void MapLoad(string path)
        {
            try
            {
                dlgMapOpen.FileName = path;
                string strExtractExePath = String.Format("{0}\\reference\\MapExtract.exe", Application.StartupPath);

                if (File.Exists(strExtractExePath) == false)
                {
                    MessageBox.Show("MapExtract.exe 파일이 없기 때문에 맵을 로딩할 수 없습니다.");
                    return;
                }

                //File.WriteAllText(strInfoFilePath, dlg.FileName);
                string thePath = "";
                try
                {
                    // C:\.oht_easy_view_temporary\프로세스ID\ 경로를 생성한다
                    Directory.CreateDirectory(DObjMan.DATA_MAIN_PATH);
                    Process thisProcess = Process.GetCurrentProcess();
                    string dir = DObjMan.DATA_MAIN_PATH + "\\" + thisProcess.Id;
                    try { Directory.Delete(dir, true); }    catch  {}
                    Directory.CreateDirectory(dir);
                    DObjMan.DATA_DIR = dir;
                    

                    // C:\.oht_easy_view_temporary\맵파일이름\Map.Zip 파일로 만듬
                    thePath = dir + @"\Map.zip";
                    
                    File.Copy(path,  thePath , true);
                }
                catch (Exception exp)
                {
                    MessageBox.Show("Map 압축파일을 열 수 없습니다.\n" + exp.Message);
                    return;
                }

                try
                {
                    // map extract 프로세스 시작
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = strExtractExePath;
                    startInfo.WindowStyle = ProcessWindowStyle.Hidden;  // 백그라운드로 실행
                    startInfo.Arguments = thePath;
                    
                    Process pro = Process.Start(startInfo);
                    pro.WaitForExit();
                    if (pro.ExitCode < 0)
                    {
                        string msg = String.Format("맵 압축 풀기 실패. ExitCode={0}\n\n", pro.ExitCode);

                        if (pro.ExitCode == -31)
                            msg += @"MapExtract.exe 의 아규먼트 개수가 적습니다.";
                        else if (pro.ExitCode == -30)
                            msg += @"C:\.oht_easy_view_temporary 폴더로 Map을 임시복사 할 수 없습니다.";
                        else if (pro.ExitCode == -40)
                            msg += @"7zip 파일이 존재해야 되는데 없는 것 같습니다.";
                        else if (pro.ExitCode == -41)
                            msg += @"오류가 있는 패키지 파일. 맵 패키지(Zip)에 필수적인 Dat 파일이 존재하지 않아 맵을 생성 할 수 없습니다.";
                        else if (pro.ExitCode <= -51 && pro.ExitCode > -70)
                            msg += @"오류가 있는 패키지 파일. 맵 패키지(Zip)에 필수적인 Dat 파일이 존재하지 않아 맵을 생성 할 수 없습니다.";
                        else if (pro.ExitCode <= -70)
                            msg += @"맵 Dat 파일 형식이 현재 버전과 달라서 파일을 로딩할 수 없습니다.\n지원하지 않는 버전의 StationEditor에서 추출된 맵파일 같습니다.";
                        
                        MessageBox.Show(msg);
                        return;
                    }
                }
                catch (Exception)
                {
                    MessageBox.Show("맵 압축 해제 프로세스 실패 실패");
                    return;
                }

                m_nLoadStartTick = Environment.TickCount;
                progressBar1.Visible = true;

                //Cursor.Current = Cursors.WaitCursor;
                progressBar1.Value = 0;
                // Thread로 파일 로딩 시작*******************************
                DObjMan.Clear();
                listView_Flags.Items.Clear();
                listView_Selected.Items.Clear();
                this.splitContainer1.Enabled = false;
                foreach (DataGridView grid in arrGrid)
                    grid.Rows.Clear();
                DObjMan.OrgMapPath = dlgMapOpen.FileName;
                RefreshMapInfo();
                DObjMan.LoadLayoutFile_Thread();
                timer_progress.Start();


            }
            catch (Exception ex)
            {
                Debug.Assert(false, "오류 발생\n" + ex.Message);
                throw;
            }
        }
        // 파일 로딩 중에만 프로그래스바 보여주기 위해서 실행되는 타이머
        private void timer_progress_Tick(object sender, EventArgs e)
        {
            // 로딩중~
            if (DObjMan.bOnLoading)
            {
                if(progressBar1.Visible == false)
                    progressBar1.Visible = true;
				//if(Cursor.Current != Cursors.WaitCursor)
				//    Cursor.Current = Cursors.WaitCursor;

				if (progressBar1.Value <= DObjMan.nCurLoadPercent)
					progressBar1.Value = DObjMan.nCurLoadPercent;

				return;
            }
            // 로딩 끝!
            else
            {
                timer_progress.Stop();
                
                //Cursor.Current = Cursors.Default;
                FitWindowAll(false);
                m_bMapLoaded = true;
                DObjBase.VisibleZRange.min = DObjNode.MinZ_mm;
                DObjBase.VisibleZRange.max = DObjNode.MaxZ_mm;
				SetTitle(DObjMan.OrgMapPath);
                //this.Text = "MapView " + VERSION + "  " +  Path.GetFileName(DObjMan.OrgMapPath);
                RefreshMapInfo();
                
                progressBar1.Value = 1000;
                MakeDatagridViewData();
                
                progressBar1.Visible = false;
				progressBar1.Value = 0;
				DObjMan.nCurLoadPercent = 0;
				pictureBox1_Resize(this, null);

                double sec = Math.Round((Environment.TickCount - m_nLoadStartTick)/1000.0, 2);
                //Cursor.Current = Cursors.Default;

                this.splitContainer1.Enabled = true;
                if (this.Visible == false)
                    this.Visible = true;
                if (DObjMan.strTotalErr.Count > 0)
                {
                    foreach(string s in DObjMan.strTotalErr)
                    {
                        textBox_Err.Text += s + "\r\n";
                    }
                    textBox_Err.Visible = true;
                    button_hideErr.Visible = true;
                }
                else
                {
                    textBox_Err.Visible = false;
                    button_hideErr.Visible = false;
                    button_wanring.Visible = false;
                    textBox_Err.Text = "";
                }

                Directory.Delete(DObjMan.DATA_DIR, true);

                //    MessageBox.Show(String.Format("맵에서 문제를 발견했습니다! 자세한 내용은 팝업된 메모장을 확인하세요\n\nElpaseTime: {0} sec", sec), "Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                //else
                //    MessageBox.Show(String.Format("로딩 완료\nElapseTime: {0} sec", sec), "Sucess", MessageBoxButtons.OK, MessageBoxIcon.Information);

            }
        }

        // 우측 분할창 상단 맵정보 갱신
        private void RefreshMapInfo()
        {
            listView_MapInfo.Items.Clear();
            string[] str = new string[2];
            str[0] = "Project";
            str[1] = DObjMan.PrjName;
            listView_MapInfo.Items.Add(new ListViewItem(str));

            str[0] = "Line";
            str[1] = DObjMan.LineName;
            listView_MapInfo.Items.Add(new ListViewItem(str));

            str[0] = "Version";
            str[1] = DObjMan.MapVersion;
            listView_MapInfo.Items.Add(new ListViewItem(str));

            str[0] = "Date";
            str[1] = DObjMan.BuildDate.ToShortDateString() + " " + DObjMan.BuildDate.ToShortTimeString();
            listView_MapInfo.Items.Add(new ListViewItem(str));

            str[0] = "Z Visible";
            str[1] = Convert.ToString(DObjBase.VisibleZRange.min) + " ~ " + Convert.ToString(DObjBase.VisibleZRange.max) + "mm";
            listView_MapInfo.Items.Add(new ListViewItem(str));

            //str[0] = "File Path";
            //str[1] = DObjMan.OrgMapPath;
            //listView_MapInfo.Items.Add(new ListViewItem(str));
            

            DRect_mm rt = DObjMan.ALL.GetTotalRect_mm(false, false);
            toolStripLabel_TotalSize.Text = String.Format("Total Size [m]: {0} x {1} x {2}", rt.Width()/1000.0, rt.Height()/1000.0, (DObjBase.VisibleZRange.max- DObjBase.VisibleZRange.min)/1000.0);
        }



        private int dOldSplitterDistPercent_RWnd = 0;
        private bool bCurExpStat_RWnd = true;

        private int dOldSplitterDistPercent_BWnd = 0;
        private bool bCurExpStat_BWnd = true;

        private void toolStripBtn_MinMaxStatusWnd_Click(object sender, EventArgs e)
        {
            if (bCurExpStat_RWnd)
            {
                dOldSplitterDistPercent_RWnd = this.DisplayRectangle.Width - splitContainer2.SplitterDistance;
                splitContainer2.SplitterDistance = this.DisplayRectangle.Width - 50;
                splitContainer3.Visible = false;
                bCurExpStat_RWnd = false;
                toolStripBtn_MinMaxStatusWnd.Image = imageList2.Images[0];
            }
            else
            {
                splitContainer2.SplitterDistance = this.DisplayRectangle.Width - dOldSplitterDistPercent_RWnd;
                bCurExpStat_RWnd = true;
                splitContainer3.Visible = true;
                toolStripBtn_MinMaxStatusWnd.Image = imageList2.Images[1];
            }
        }

        // 하단 분할창 최소화 최대화 버튼
        private void toolStripBtn_MinMaxListWnd_Click(object sender, EventArgs e)
        {
            if (bCurExpStat_BWnd)
            {
                dOldSplitterDistPercent_BWnd = this.DisplayRectangle.Height - splitContainer1.SplitterDistance;
                splitContainer1.SplitterDistance = this.DisplayRectangle.Height - 80;
                tabControl_Object.Visible = false;
                bCurExpStat_BWnd = false;
                toolStripBtn_MinMaxListWnd.Image = imageList2.Images[2];
            }
            else
            {
                splitContainer1.SplitterDistance = this.DisplayRectangle.Height - dOldSplitterDistPercent_BWnd;
                bCurExpStat_BWnd = true;
                tabControl_Object.Visible = true;
                toolStripBtn_MinMaxListWnd.Image = imageList2.Images[3];
            }
        }
        
        
        // 도면부분 클릭했을 때 포커스 가져오기 위함
        private void pictureBox1_Click(object sender, EventArgs e)
        {
            pictureBox1.Focus();
            
        }

        // 상태정보 갱신하기 위해 상시 계속 도는 타이머
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (DObjMan.bOnLoading)
                return;

            Rectangle rt = this.RectangleToScreen(pictureBox1.DisplayRectangle);
            //Trace.WriteLine(String.Format("{0},{1}, width={2}, height{3}", rt.X, rt.Y, rt.Width, rt.Height));
            //Trace.WriteLine(String.Format("{0} {1}", MousePosition.X, MousePosition.Y));
            inDrawBox = rt.Contains(MousePosition.X, MousePosition.Y);
            //if (inDrawBox)        // 있어도 되는데, ..음
            //{   pictureBox1.Focus();
            //}
            DataGridView grid = null;
            string strSelectedTab = "";
            if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.Node)                  { grid = dataGridView_node; strSelectedTab = "Node"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.Station)          { grid = dataGridView_Station; strSelectedTab = "Station"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.CID)               { grid = dataGridView_CID; strSelectedTab = "CID"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.Collision)        { grid = dataGridView_Collision; strSelectedTab = "Collision"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.Link)              { grid = dataGridView_Link; strSelectedTab = "Link"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.HID)              { grid = dataGridView_HID; strSelectedTab = "HID"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.FlowControl)   { grid = dataGridView_FlowControl; strSelectedTab = "FlowControl"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.Teaching)       { grid = dataGridView_Teaching; strSelectedTab = "Teaching"; }
            else if (tabControl_Object.SelectedIndex == (int)eKindOf_Grid.PathData)       { grid = dataGridView_PathData; strSelectedTab = "Path Data"; }


            if (grid != null)
            {
                if(toolStripLabel_CurTabName.Text != strSelectedTab)
                    toolStripLabel_CurTabName.Text = strSelectedTab;
                if (toolStripLabel_SelectedCellCnt.Text != String.Format("Selected Cnt: {0}", grid.SelectedCells.Count))
                    toolStripLabel_SelectedCellCnt.Text = String.Format("Selected Cnt: {0}", grid.SelectedCells.Count);
            }

            string strScale = "Scale: " + Convert.ToString(Math.Round(m_Scale.dScale, 2));
            if (toolStripLabel_scale.Text != strScale)
                toolStripLabel_scale.Text = strScale;

            toolStripLabel_MousePos.Text = String.Format("Cursor Xy [m]: {0}, {1}", curPos_mm.X/1000.0, curPos_mm.Y/1000.0);

            if (Control.ModifierKeys == Keys.Alt)
            {
                if (Cursor.Current != Cursors.Hand)  // 알트키 눌리면 휠이동
                {
                    Cursor.Current = Cursors.SizeAll;

                }
            }
            else
                if (Cursor.Current != Cursors.Default)
                    Cursor.Current = Cursors.Default;
        }

        // Z 축 표현 버튼 눌렀을때
        private void toolStripBtn_VisibleZAxis_Click(object sender, EventArgs e)
        {
            try
            {

                dlgZlayer.trackBar1.Minimum = 0;
                dlgZlayer.trackBar1.Maximum = DObjNode.MaxZ_mm;
                dlgZlayer.trackBar1.Value = DObjBase.VisibleZRange.min;
                dlgZlayer.textBox_min.Text = Convert.ToString(DObjBase.VisibleZRange.min);
                dlgZlayer.trackBar2.Minimum = 0;
                dlgZlayer.trackBar2.Maximum = DObjNode.MaxZ_mm;
                dlgZlayer.trackBar2.Value = DObjBase.VisibleZRange.max;
                dlgZlayer.textBox_max.Text = Convert.ToString(DObjBase.VisibleZRange.max);

                if (dlgZlayer.ShowDialog() != DialogResult.OK)
                    return;

            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
                return;     //throw;
            }
            
            DObjMan.ALL.SelectAll(false);
            DObjBase.VisibleZRange = dlgZlayer.range;
            Redraw(false);
            RefreshSelectedList();
            RefreshMapInfo();
        }

        // Collision 숨기기, 보기이 버튼
        private void toolStripBtn_DrawCollisionInfo_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawCollisionInfo = !DObjBase.bDrawCollisionInfo; // toggle
            toolStripBtn_DrawCollisionInfo.Image = DObjBase.bDrawCollisionInfo ? imageList3.Images[6] : imageList3.Images[7];

            if (DObjBase.bDrawCollisionInfo)
                DObjBase.bDrawMapSpeedUp = false;
            Redraw(false);
        }
        private void btnMapSpeedUpView_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawMapSpeedUp = !DObjBase.bDrawMapSpeedUp; // toggle
            if (DObjBase.bDrawMapSpeedUp)
                DObjBase.bDrawCollisionInfo = false;

            Redraw(false);
        }

        // 스테이션 ID 보이기 숨기기 버튼
        private void toolStripButton_ShowStationID_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawStationID = !DObjBase.bDrawStationID; // toggle
            toolStripButton_ShowStationID.Image = DObjBase.bDrawStationID ? imageList3.Images[4] : imageList3.Images[5];
            Redraw(false);
        }
        // 스테이션 숨기기/보이기 버튼
        private void toolStripBtn_HideStation_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawStation = !DObjBase.bDrawStation; // toggle
            toolStripBtn_HideStation.Image = DObjBase.bDrawStation ? imageList3.Images[8] : imageList3.Images[9];
            Redraw(false);
        }

        // Node 보이기/숨기기 번튼
        private void toolStripBtn_HideNode_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawNode= !DObjBase.bDrawNode; // toggle
            toolStripBtn_HideNode.Image = DObjBase.bDrawNode ? imageList3.Images[0] : imageList3.Images[1];
            Redraw(false);
        }

        // FitWnd 버튼 눌렀을때
        private void toolStripBtn_FitWnd_Click(object sender, EventArgs e)
        {
            FitWindowAll(false);
        }

        // 찾기 버튼을 누르지 않고 곧장 엔터 눌렀을 때 처리
        private void toolStripTextBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                toolStripBtn_FindData_Click(this, e);
        }


        // 하단 그리드에서 특정항목 찾아서 선택해주기
        public bool FindGridItem(eKindOf_Grid kindof, string id)
        {
            DataGridView grid = null;
            if (bCurExpStat_BWnd == false)  // 최소화 되어있으면 크게한다
                toolStripBtn_MinMaxListWnd_Click(this, null);
            int nColIdx = 0;
            
            if (kindof == eKindOf_Grid.Node)
            {
                grid = dataGridView_node;
                nColIdx = (int)eGridCol_Node.ID;
            }
            else if (kindof == eKindOf_Grid.Station)
            {
                grid = dataGridView_Station;
                nColIdx = (int)eGridCol_Station.ID;
            }
            else if (kindof == eKindOf_Grid.CID)
            {
                grid = dataGridView_CID;
                nColIdx = (int)eGridCol_CID.ID;
            }
            else if (kindof == eKindOf_Grid.Collision)
            {
                grid = dataGridView_Collision;
                nColIdx = (int)eGridCol_Collision.NodeID;
            }
            else if (kindof == eKindOf_Grid.Link)
            {
                grid = dataGridView_Link;
                nColIdx = (int)eGridCol_Link.ID;
            }
            else if (kindof == eKindOf_Grid.HID)
            {
                grid = dataGridView_HID;
                nColIdx = (int)eGridCol_HID.ID;
            }
            else if (kindof == eKindOf_Grid.FlowControl)
            {
                grid = dataGridView_FlowControl;
                nColIdx = (int)eGridCol_FlowControl.StopNodeID;
            }
            else if (kindof == eKindOf_Grid.Teaching)
            {
                grid = dataGridView_Teaching;
                nColIdx = (int)eGridCol_Teaching.StationID;
            }
            else if (kindof == eKindOf_Grid.PathData)
            {
                grid = dataGridView_PathData;
                nColIdx = (int)eGridCol_PathData.LayoutId;
            }
            else
            {
                MessageBox.Show("Error?");
                return false;
            }

            tabControl_Object.SelectedIndex = (int)kindof;

            // 찾기 버튼을 누르면 현재 선택된 셀 기준으로 다음 셀부터 한바퀴 휙 훓는다
            // 왜 이렇게 했냐면, CID 같은 경우는 똑같은 ID를 갖은 cid가 두개존재하는 경우도 있는데
            // 이런걸 처리할 때 행을 Visible 처리하면 시간이 엄청 오래걸리고, Select 처리하자니 그 두 개가 멀리 떨어져 있으면
            // Select 해줘도 찾기 힘들다. 그래서 선택 해주고, 한번더 Find 버튼 누르면 두번째껄 찾게 해주기 위함이다
            int nAddIdx = 0;
            if (grid.SelectedCells.Count > 0)
                nAddIdx = grid.SelectedCells[0].RowIndex;
            grid.ClearSelection();
            bool bFound = false;
            grid.Sort(grid.Columns[0], ListSortDirection.Ascending);
            for (int i = 0; i < grid.Rows.Count; i++)
            {
                int nIdx = i + nAddIdx + 1;
                nIdx %= grid.Rows.Count;
                DataGridViewRow row = grid.Rows[nIdx];
                string str = String.Format("{0}", row.Cells[nColIdx].Value);
                if (str == id)
                {
                    row.Selected = true;
                    if (!bFound)
                    {
                        grid.FirstDisplayedScrollingRowIndex = nIdx;
                        grid.CurrentCell = row.Cells[nColIdx];
                    }
                    bFound = true;
                    //                     break;
                }
            }

            if (!bFound)
            {
                return false;
            }
            return true;
        }
        // 특정 항목 찾기 버튼
        private void toolStripBtn_FindData_Click(object sender, EventArgs e)
        {
            if (toolStripTextBox_find.Text == "")
                return;

            eKindOf_Grid gridType = eKindOf_Grid.Node;
            if (toolStripComboBox_FindItem.Text.ToUpper() == "NODE")                gridType = eKindOf_Grid.Node;
            else if (toolStripComboBox_FindItem.Text.ToUpper() == "STATION")                gridType = eKindOf_Grid.Station;
            else if (toolStripComboBox_FindItem.Text.ToUpper() == "CID")                gridType = eKindOf_Grid.CID;
            else if (toolStripComboBox_FindItem.Text.ToUpper() == "COLLISION")        gridType = eKindOf_Grid.Collision;
            else if (toolStripComboBox_FindItem.Text.ToUpper() == "LINK")                gridType = eKindOf_Grid.Link;
            else if (toolStripComboBox_FindItem.Text.ToUpper() == "HID")                gridType = eKindOf_Grid.HID;
            else
            {
                MessageBox.Show("Error?");
                return;
            }

            if(FindGridItem(gridType, toolStripTextBox_find.Text) == false)
            {
                MessageBox.Show("No Data", "", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        // GridView에서 셀을 더블클릭 할때, 해당 ID를 도면에서 찾아간다
        // 내가 만들었지만 이 함수는 객체지향의 승리다
        private void dataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            try
            {

                DObjBase foundObj = null;
                DObjMan.ALL.SelectAll(false);   //전체 선택 해제
                DObjMan.arrCID.VisibleAll(false);
                DObjMan.arrHID.VisibleAll(false);
                DObjMan.arrFlowControl.VisibleAll(false);

                // 각 그리드별로 더블클릭한 객체 찾기
                if (sender == dataGridView_node)
                {
                    int id = (int)dataGridView_node.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjNode.FindNode(id);
                }
                else if (sender == dataGridView_Station)
                {
                    int id = (int)dataGridView_Station.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjStation.FindStation(id);
                }
                else if (sender == dataGridView_CID)
                {
                    string id = (string)dataGridView_CID.Rows[e.RowIndex].Cells[0].Value;
                    List<DObjBase> objs = DObjMan.arrCID.FindAll(o => (id == o.GetID()));
                    if (objs.Count == 1)
                        foundObj = objs.ElementAt(0);
                    else if (objs.Count > 1)
                    {
                        int nStopNode1 = (int)dataGridView_CID.Rows[e.RowIndex].Cells[(int)eGridCol_CID.StopNode1].Value;
                        int nStopNode2 = (int)dataGridView_CID.Rows[e.RowIndex].Cells[(int)eGridCol_CID.StopNode2].Value;

                        foreach (DObjBase obj in objs)
                        {
                            DObjCID tmpCID = (DObjCID)obj;
                            if (tmpCID.stopNode1ID == nStopNode1 && tmpCID.stopNode2ID == nStopNode2)
                            {
                                foundObj = tmpCID;
                                break;
                            }

                        }
                    }

                    foundObj.Visible = true;
                }
                else if (sender == dataGridView_Collision)
                {
                    int id = (int)dataGridView_Collision.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjMan.arrCollision.Find(o => (id == o.GetID_Int()));
                }
                else if (sender == dataGridView_Link)
                {
                    string id = (string)dataGridView_Link.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjMan.arrLink.Find(o => (id == o.GetID()));
                }
                else if (sender == dataGridView_HID)
                {
                    string id = (string)dataGridView_HID.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjMan.arrHID.Find(o => (id == o.GetID()));
                    foundObj.Visible = true;
                    DObjHID hid = (DObjHID)foundObj;
                    if (hid.myLinkArray.Count == 0)
                    {
                        MessageBox.Show("연결된 링크가 없는 HID 입니다.");
                        return;
                    }
                }
                else if (sender == dataGridView_FlowControl)
                {
                    DObjLink link = (DObjLink)dataGridView_FlowControl.Rows[e.RowIndex].Cells[0].Tag;
                    if (link == null)
                    {
                        MessageBox.Show("해당 링크를 찾을 수 없습니다.");
                        return;
                    }
                    foundObj = link;
                    foundObj.Visible = true;

                }
                else if (sender == dataGridView_Teaching)
                {
                    int id = (int)dataGridView_Teaching.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjStation.FindStation(id);
                }
                else if (sender == dataGridView_PathData)
                {
                    int id = (int)dataGridView_PathData.Rows[e.RowIndex].Cells[0].Value;
                    foundObj = DObjMan.arrPathData.Find(o => o.GetID_Int() == id);
                }

                if (foundObj == null)
                    return;

                foundObj.Selected = true;
                if (foundObj.ObjType == DObjBase.eChildObjType.CID || foundObj.ObjType == DObjBase.eChildObjType.HID)
                {
                    SelectedEasyFocusView(false);        // 선택된 객체들 보기 좋은 View로 표현
                }
                else
                    SelectedEasyFocusView(true);        // 선택된 객체들 보기 좋은 View로 표현
                                                        //FitWindowAll(bSelectedOnly: true);  // 선택된 객체만 Fit Window
            }
            catch (Exception exp)
            {
                Debug.Assert(false, exp.Message);
                return;
            }
        }

        // 그리드에서 특정 셀 클릭 했을 때 처리. Find창 객체에 현재 그리드 알려줌
        private void dataGridView_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if (sender != null)
            {
                m_CurrentGrid = (DataGridView)sender;
                //dlgFind.SetCurrentGrid((DataGridView)sender);
            }

            //if (sender == dataGridView_node)                 dlgFind.SetCurrentGrid(dataGridView_node);
            //else if (sender == dataGridView_Station)         dlgFind.SetCurrentGrid(dataGridView_Station);
            //else if (sender == dataGridView_CID)             dlgFind.SetCurrentGrid(dataGridView_CID);
            //else if (sender == dataGridView_Collision)       dlgFind.SetCurrentGrid(dataGridView_Collision);
            //else if (sender == dataGridView_Link)             dlgFind.SetCurrentGrid(dataGridView_Link);
            //else if (sender == dataGridView_HID)             dlgFind.SetCurrentGrid(dataGridView_HID);
            //else if (sender == dataGridView_FlowControl) dlgFind.SetCurrentGrid(dataGridView_FlowControl);
            //else if (sender == dataGridView_Teaching) dlgFind.SetCurrentGrid(dataGridView_Teaching);
        }

        // 그리드에서 우클릭 했을때 팝업창 처리
        private void dataGridView_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                contextMenu_GridRBtn.Show((Control)sender, new Point(e.X, e.Y));
            }
        }

        // 하단 그리드에서 툴바에서 찾기 버튼
        private void ToolStripMenuItem_Find_Click(object sender, EventArgs e)
        {
            if(m_CurrentGrid == null)
            {
                MessageBox.Show("선택된 Grid가 없습니다.");
                return;
            }
            dlgFind.SetCurrentGrid(m_CurrentGrid);
            dlgFind.Show();
            dlgFind.Visible = true;
            
        }
        // 하단 그리드에서 각탭들 모든 그리드들의 통합 키 이벤트 처리.
        private void dataGridView_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            bool bSpace = (Control.ModifierKeys & Keys.Space) != 0;
            bool bShift = (Control.ModifierKeys & Keys.LShiftKey) != 0;
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                if (m_CurrentGrid == null)
                {
                    MessageBox.Show("선택된 Grid가 없습니다.");
                    return;
                }
                dlgFind.SetCurrentGrid(m_CurrentGrid);
                dlgFind.Show();
                dlgFind.Visible = true;
            }
            
            // 해당 열 전체 선택
            else if (e.Control && e.KeyCode == Keys.Space)
            {
                List<int> selectedCols = new List<int>();
                // 선택된 Cell 중에서 선택된 열만 속아내는 부분 *************************
                foreach (DataGridViewCell cell in m_CurrentGrid.SelectedCells)
                {
                    if (selectedCols.FindIndex(o => o == cell.ColumnIndex) >= 0)
                        continue;
                    selectedCols.Add(cell.ColumnIndex);
                }

                foreach (int nCol in selectedCols)
                {
                    for (int i = 0; i < m_CurrentGrid.Rows.Count; i++)
                    {
                        DataGridViewCell cell = m_CurrentGrid.Rows[i].Cells[nCol];
                        if (cell != null)
                            cell.Selected = true;

                    }
                }
            }
            else if(e.Control && e.Shift && e.KeyCode == Keys.C)
            {
                Clipboard.SetText("xx");
            }
            
            
        }
        

        // 메인 Form에서 키 처리. 단축키 위주
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            // 단축키 처리
            if (e.KeyCode == Keys.A)    //UBG info
                toolStripBtn_DrawCollisionInfo_Click(this, e);
            else if (e.KeyCode == Keys.D)       // station ID
                toolStripButton_ShowStationID_Click(this, e);
            else if (e.KeyCode == Keys.S)
                toolStripBtn_HideStation_Click(this, e);
            else if (e.KeyCode == Keys.N)
                toolStripBtn_HideNode_Click(this, e);
			else if (e.KeyCode == Keys.B)
				btnShowNodeDetail_Click(this, e);
			else if (e.KeyCode == Keys.F4)
                toolStripBtn_FitWnd_Click(this, e);
            else if (e.KeyCode == Keys.F11)
            {
                toolStripBtn_MinMaxStatusWnd_Click(this, e);
                toolStripBtn_MinMaxListWnd_Click(this, e);
            }
            else if (e.KeyCode == Keys.OemMinus || e.KeyCode == Keys.Subtract)
            {
                Point pt = MyTool.GMethod.GetMousePos_onControl(pictureBox1);
                ZoomIn(false, pt.X, pt.Y);
            }
            else if (e.KeyCode == Keys.Oemplus || e.KeyCode == Keys.Add)
            {
                Point pt = MyTool.GMethod.GetMousePos_onControl(pictureBox1);
                ZoomIn(true, pt.X, pt.Y);
            }
            else if (e.KeyCode == Keys.Escape)
            {
				종료ToolStripMenuItem_Click(null, null);	// 거리재는거 있으면 정리

				DObjMan.arrCID.VisibleAll(false);
                DObjMan.arrHID.VisibleAll(false);
                DObjMan.ALL.SelectAll(false);
                RefreshSelectedList();
                Redraw(false);
            }

        }
        // Selected 리스트에서 더블클릭 처리
        private void listView_Selected_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (listView_Selected.SelectedItems.Count <= 0)
                return;

            DObjBase obj = (DObjBase)listView_Selected.SelectedItems[0].Tag;    //해당 객체 도면에서 보여주기
            if (obj == null)
                return;

            DRect_mm rt_mm = obj.GetRect_mm();
            Point pt = rt_mm.CenterPoint();

            m_Scale.SetScale(dProperScale);

            this.m_Scale.SetCenterX(-pt.X);
            this.m_Scale.SetCenterY(-(-pt.Y));
            Redraw(false);

        }
        // selected 리스트에서 키 이벤트 처리
        private void listView_Selected_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)   //삭제 처리
            {
                if (listView_Selected.SelectedItems.Count == 0)
                    return;

                listView_Selected.SuspendLayout();
                while (listView_Selected.SelectedItems.Count > 0)
                {
                    int nIdx = listView_Selected.SelectedItems[0].Index;

                    DObjBase obj = (DObjBase)listView_Selected.Items[nIdx].Tag;
                    obj.Selected = false;
                    listView_Selected.Items.RemoveAt(nIdx);
                }
                Redraw(false);
                RefreshSelectedList();
                listView_Selected.ResumeLayout();
            }
        }

        private void listView_Selected_MouseClick(object sender, MouseEventArgs e)
        {

        }
        // 플래그 리스트에서 더블클릭 처리
        private void listView_Flags_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (listView_Flags.SelectedItems.Count <= 0)
                return;

            DObjBase obj = (DObjBase)listView_Flags.SelectedItems[0].Tag;   //해당 객체로 가기
            if (obj == null)
                return;

            DRect_mm rt_mm = obj.GetRect_mm();
            Point pt = rt_mm.CenterPoint();

            m_Scale.SetScale(dProperScale);

            this.m_Scale.SetCenterX(-pt.X);
            this.m_Scale.SetCenterY(-(-pt.Y));
            Redraw(false);
        }

        // 플래그 리스트에서 키 이벤트 처리
        private void listView_Flags_KeyDown(object sender, KeyEventArgs e)
        {
            // 삭제 키
            if (e.KeyCode == Keys.Delete)
            {
                if (listView_Flags.SelectedItems.Count == 0)
                    return;

                listView_Flags.SuspendLayout();
                while (listView_Flags.SelectedItems.Count > 0)
                {
                    int nIdx = listView_Flags.SelectedItems[0].Index;
                    DObjBase obj = (DObjBase)listView_Flags.Items[nIdx].Tag;
                    obj.bDrawFlag = false;
                    obj.strFlagMemo = "";

                    listView_Flags.Items.RemoveAt(nIdx);
                }
                listView_Flags.ResumeLayout();
                Redraw(false);
            }
        }

        // 플래그 지정 팝업 이벤트
        private void ToolStripMenuItem_Flag_Click(object sender, EventArgs e)
        {
            // 특정 슽에이션 이나 노드 더블클릭하면 select 하고 하단 표에서 바로 보여준다
            DObjArray arr = new DObjArray();
            arr.AddRange(DObjMan.arrNode);
            arr.AddRange(DObjMan.arrStation);
            
            arr = arr.PtInRectArray(curPos_mm, true);
            if (arr.Count == 0)
            {
                MessageBox.Show("원하는 Node, Station에 마우스를 가져가야 Flag 지정이 됩니다.");
                return;
            }
            else if(arr.Count > 1)
            {
                MessageBox.Show("한번에 한개의 객체만 Flag를 지정 할 수 있습니다.");
                return;
            }

            DObjBase obj = arr.ElementAt(0);
            if(obj.bDrawFlag)
            {
                string str = String.Format("{0} {1} 의 Flag를 해제 하시겠습니까?", obj.ObjType.ToString(), obj.GetID());
                if(MessageBox.Show(str, "Qustion", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                    return;
                obj.bDrawFlag = false;
                obj.strFlagMemo = "";

                Redraw(false);
                RefreshFlagList();
                return;
            }

            UFlag dlg = new UFlag();
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            obj.bDrawFlag = true;
            obj.strFlagMemo = dlg.strMemo;

            Redraw(false);
            RefreshFlagList();
        }

        private void listView_Selected_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            if (e.ItemIndex < 0)
                return;

            try
            {
                DObjBase obj = (DObjBase)listView_Selected.Items[e.ItemIndex].Tag;

                curPos_mm = obj.GetRect_mm().CenterPoint();
                DObjBase.curMousePos_mm = curPos_mm;
                Redraw(false);
            }
            catch (Exception)
            {   //throw;
                return;
            }

        }
        
        private void listView_Flags_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            if (e.ItemIndex < 0)
                return;

            try
            {
                DObjBase obj = (DObjBase)listView_Flags.Items[e.ItemIndex].Tag;

                curPos_mm = obj.GetRect_mm().CenterPoint();
                DObjBase.curMousePos_mm = curPos_mm;
                Redraw(false);
            }
            catch (Exception)
            {   //throw;
                return;
            }
        }

        private void UMapView_FormClosing(object sender, FormClosingEventArgs e)
        {
            //if (OPEN_BY_OHTEASYVIEW)        //oht easy view에서 열린 MapView 이면, Closing하지 않고 Hide시킨다.
            //{
            //    e.Cancel = true;
            //    this.Visible = false;
            //}
            if(MessageBox.Show("MapView 종료할까요?", "Exit", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                e.Cancel = true;
                return;
            }
            Process []ps = Process.GetProcesses();
            string thisName = Process.GetCurrentProcess().ProcessName;
            int nCnt = 0;
            foreach (Process p in ps)
            {

                if (thisName == p.ProcessName)
                    nCnt++;
            }
            // 마지막 프로세스면 임시폴더 정리해도 좋다
            if (nCnt <= 1)
            {
                try
                {
                    Directory.Delete(DObjMan.DATA_MAIN_PATH, true); // 임시 폴더 청소한다
                    Directory.CreateDirectory(DObjMan.DATA_MAIN_PATH);
                }
                catch (Exception)
                {
                }
            }
        }

        private void listView_Flags_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                contextMenuStrip_Flag.Show((Control)sender, new Point(e.X, e.Y));
            }
        }

        private void ToolStripMenuItem_FlagSave_Click(object sender, EventArgs e)
        {
            SaveFlagList();
        }

        private void ToolStripMenuItem_FlagLoad_Click(object sender, EventArgs e)
        {
            LoadFlagList();
        }


        
        
        // OHTEasyView 로부터 받은 메시지 전달받은 내용 파싱 *************
        public void ParseMessage(string msg)
        {
            try
            {

                if (msg.Length < 5)
                {
                    return;
                }

                string[] arr = msg.Split('/');
                
                // Show Hide 명령 ******** ********** *********************
                if (arr[0] == Cmd.LOG_REQ_SHOW)
                {
                    if (arr.Length < 2)
                    {
                        return;
                    }
                    if (arr[1] == "1")
                    {
                        //this.Visible = true;
                        this.WindowState = FormWindowState.Minimized;
                        this.Invalidate();  // 한번 작게 했다가 크게 해야 창이 앞으로 나옴
                        this.WindowState = FormWindowState.Normal;
                        this.Show();
                        this.BringToFront();
                    }
                    else if(arr[1] == "0")
                    {
                        this.WindowState = FormWindowState.Minimized;
                        //this.Visible = false;
                        //this.Hide();
                    }
                }
                // 상태 요청 명령  ******** ********** *********************
                else if (arr[0] == Cmd.LOG_REQ_STATUS)
                {
                    if (arr.Length < 2)
                        return;
                    int nPortNum = -1;
                    if (Int32.TryParse(arr[1], out nPortNum) == false)
                        return;


                    string strDate = DObjMan.BuildDate.ToShortDateString() + " " + DObjMan.BuildDate.ToShortTimeString();

                    int curStat = 0;

                    if (progressBar1.Visible)
                        curStat = 3;        // Openning 여는중
                    else if (DObjMan.OrgMapPath == "")
                        curStat = 2;        // EmptyMap 맵 안열린 상태
                    else
                        curStat = 1;        // Standby 대기상태 OK

                    ///   MAP_REP_STATUS / 현재 Status /  P-PRJ / P1L_7F / 0.1577 / 2018-04-27 오전 10:15 / D:\....맵경로/ 나의 포트번호
                    string strRepMsg = String.Format("{0}/{1}/{2}/{3}/{4}/{5}/{6}/{7}", Cmd.MAP_REP_STATUS, curStat,
                        DObjMan.PrjName, DObjMan.LineName, DObjMan.MapVersion, strDate, DObjMan.OrgMapPath, m_nMyPortNum
                        );

                    SendData_toOHTEasyView(nPortNum, strRepMsg);  // 응답하기
                }
                // 노드, 스테이션 찾기 명령  ******** ********** *********************
                else if (arr[0] == Cmd.LOG_REQ_FIND)
                {
                    if (arr.Length < 3)
                    {
                        return;
                    }
                    int nIdx = -1;
                    if (Int32.TryParse(arr[1], out nIdx) == false)
                    {
                        return;
                    }
                    // 자신이 연 맵이랑 다른거면 무시
                    //if(arr[3] != DObjMan.OrgMapPath)
                    //{
                    //    return;
                    //}
                    if (nIdx >= toolStripComboBox_FindItem.Items.Count)
                    {
                        return;
                    }

                    // 한개의 노드나, 스테이션이 아닌, 여러개 찾는 경우
                    // 여러개를 플래그지정한다
                    int nCnt = 1;
                    //string strMsg = "";
                    if(arr[2].IndexOf(',') > 0)
                    {
                        DObjBase []objArr= DObjMan.ALL.FindAll(o => o.Selected).ToArray();
                        foreach (DObjBase aob in objArr)
                            aob.Selected = false;   // 전체 선택 해제

                        string []nodes = arr[2].Split(',');
                        for(int i=0; i<nodes.Length; i++)
                        {
                            if (String.IsNullOrEmpty(nodes[i])) continue;
                            nodes[i] = nodes[i].Replace(" ", "");
                            if (String.IsNullOrEmpty(nodes[i])) continue;
                            int nID = 0;
                            if (Int32.TryParse(nodes[i], out nID) == false)
                                continue;
                            DObjBase obj = null;

                            if(nIdx == 0)
                                obj = DObjNode.FindNode(nID);
                            else if(nIdx == 1)
                                obj = DObjStation.FindStation(nID);

                            if (obj == null)
                                continue;
                            obj.Selected = true;
                            obj.bDrawFlag = true;
                            obj.strFlagMemo = String.Format("{0}{1}", nIdx == 0 ? "N" : "S", nCnt);
                            //strMsg += String.Format("{0}, ", nID);
                            FitWindowAll(true);
                            nCnt++;
                        }

                        Redraw(false);
                        RefreshFlagList();

                        //MessageBox.Show(String.Format("총 {0} 개의 {1} Flag 설정.\n\n{2}", nCnt, nIdx == 0 ? "Node" : "Station", strMsg));

                        this.Visible = true;
                        FormWindowState wndSt = this.WindowState;
                        this.WindowState = FormWindowState.Minimized;
                        this.Invalidate();  // 한번 작게 했다가 크게 해야 창이 앞으로 나옴
                        this.WindowState = wndSt;
                        this.Show();
                        this.BringToFront();
                        this.FitWindowAll(false);
                        return;
                    }

                    // 찾기 명령 받으면, UI에서 직접 입력해서 찾기버튼 누르는거랑 똑같은 과정으로 찾게 함
                    this.toolStripComboBox_FindItem.SelectedIndex = nIdx;   // Node, Station 콤보박스 세팅
                    this.toolStripTextBox_find.Text = arr[2];           // text박스에 해당 ID입력
                    this.toolStripBtn_FindData_Click(null, null);       // 찾기 버튼

                    try
                    {
                        if (nIdx == 0)  //node
                        {
                            DataGridViewCellEventArgs e = new DataGridViewCellEventArgs(0, dataGridView_node.SelectedRows[0].Index);
                            this.dataGridView_CellDoubleClick(dataGridView_node, e);
                        }
                        else if (nIdx == 1)  //station
                        {
                            DataGridViewCellEventArgs e = new DataGridViewCellEventArgs(0, dataGridView_Station.SelectedRows[0].Index);
                            this.dataGridView_CellDoubleClick(dataGridView_Station, e);
                        }
                    }
                    catch (Exception)
                    {

                        //throw;
                    }

                    this.Visible = true;
                    FormWindowState wndStat = this.WindowState;
                    this.WindowState = FormWindowState.Minimized;
                    this.Invalidate();  // 한번 작게 했다가 크게 해야 창이 앞으로 나옴
                    this.WindowState = wndStat;
                    this.Show();
                    this.BringToFront();
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message );
            }
        }

        
        /// ******************************************************************************************
        /// <summary>   
        /// UDP 소켓으로 OHTEasyView에 데이터 전달하는 함수 
        /// </summary>
        public void SendData_toOHTEasyView(int nPortNum, string data)
        {
            byte[] send_buffer = Encoding.Default.GetBytes(data);

            try
            {
                IPEndPoint addr = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPortNum);
                Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                sock.SendTo(send_buffer, addr);       // UDP Send To
            }
            catch (Exception e)
            {
                MessageBox.Show("OHTEasyView에 데이터 전송 실패.\n\n", e.Message);
            }
        }

        // UDP Recv Callback
        public static void ReceiveCallback(IAsyncResult arg)
        {
            try
            {
                UdpClient u = (UdpClient)((UdpState)(arg.AsyncState)).u;
                IPEndPoint e = (IPEndPoint)((UdpState)(arg.AsyncState)).e;

                Byte[] receiveBytes = u.EndReceive(arg, ref e);
                string receiveString = Encoding.Default.GetString(receiveBytes);

                if (receiveString.Length > 5)
                    Program.dlgMapView.Invoke(Program.dlgMapView.delegateParse, receiveString);

                UMapView form = Program.dlgMapView;
                form.listenSock.BeginReceive(new AsyncCallback(ReceiveCallback), form.m_udpState);      // 콜백 다시 ?? 등록
            }
            catch (Exception)
            {

                //throw;
            }
            
        }

        private void button_wanring_Click(object sender, EventArgs e)
        {
            textBox_Err.Visible = true;
            button_hideErr.Visible = true;
            button_wanring.Visible = false;
        }

        private void button_hideErr_Click(object sender, EventArgs e)
        {
            textBox_Err.Visible = false;
            button_hideErr.Visible = false;
            button_wanring.Visible = true;
        }

        private void btn_NodeExtract_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.FileName = "직진노드추출";
            dlg.DefaultExt = "txt";
            dlg.ValidateNames = true;

            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            DCustomExtract.ExtractSpecialNode(dlg.FileName);
        }

        private void btn_LookdownChk_Click(object sender, EventArgs e)
        {
            List<DObjTeaching> wrongList = new List<DObjTeaching>();

            DRange range1 = new DRange();
            DRange range8 = new DRange();
            DRange range8_1 = new DRange();
            DRange range9 = new DRange();

            // 단위는 mm * 10 = 100um
            range1.min = 30000;
            range1.max = 40000;

            range8.min = 16000;
            range8.max = 17000;

            range8_1.min = 6000;
            range8_1.max = 7000;

            range9.min = 1000;
            range9.max = 2000;


            foreach (DObjTeaching obj in DObjMan.arrTeaching)
            {
                switch(obj.LookDown)
                {
                    case 1:
                    {
                        if (range1.IsInRange(obj.Hoist) == false)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        if(obj.portType != eTeachingStationPortType.EQ)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        break;
                    }
                    case 8:
                    {
                        if (range8.IsInRange(obj.Hoist) == false && range8_1.IsInRange(obj.Hoist) == false)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        if (obj.portType != eTeachingStationPortType.EQ && obj.portType != eTeachingStationPortType.Stocker)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        break;
                    }
                    case 9:
                    {
                        if (range9.IsInRange(obj.Hoist) == false)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        if (obj.portType != eTeachingStationPortType.LSTB && obj.portType != eTeachingStationPortType.RSTB)
                        {
                            wrongList.Add(obj);
                            break;
                        }
                        break;
                    }
                    default:
                    {
                        wrongList.Add(obj);
                        break;
                    }
                }
            }

            string allLine = "StationID,PortType,Hoist,Shift,Rotate,LookDown\n";
            // file save
            foreach(DObjTeaching obj in wrongList)
            {
                string line = String.Format("{0},{1},{2},{3},{4},{5},{6}\n", obj.nStationID, obj.portType.ToString(), obj.Hoist/10.0, obj.Shift/10.0, obj.Rotate/10.0, obj.LookDown, obj.STBL_Shift);
                allLine += line;
            }

            string fileName = Path.GetFileNameWithoutExtension(DObjMan.OrgMapPath);
            string DirName = Path.GetDirectoryName(DObjMan.OrgMapPath);

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.InitialDirectory = DirName;
            dlg.FileName = fileName + ".csv";
            if(dlg.ShowDialog() != DialogResult.OK)
            {
                MessageBox.Show("취소되었습니다.");
                return;
            }
            
            File.WriteAllText(dlg.FileName, allLine);
        }

        private void btn_MapSpeedUp_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("맵전체 일괄 증속을 시작합니다.", "증속~!", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;
            DMapSpeedUp MapSpeedMan = new DMapSpeedUp();
            MapSpeedMan.MapSpeedUp();
            
            List<string> allSnakeInfo = new List<string>();

            //DGugan.Rule1Cnt = DGugan.Rule2Cnt = DGugan.Rule3Cnt = 0;
            MapSpeedMan.WriteInfoFile(DObjMan.OrgMapPath + "_Map증속.txt");

            for (int i=0; i< MapSpeedMan.m_GuGanList_All.Count; i++)
            {
                
                DGugan obj = MapSpeedMan.m_GuGanList_All[i];
                
                // 증속된 값 전달
                foreach(DObjNodeEx aaa in obj.Nodes)
                {
                    int nIdx = aaa.nParentNodeArrIdx;
                    DObjNode nn = (DObjNode)DObjMan.arrNode[nIdx];

                    if (aaa.NewSpeed > 0)
                    {        
                        //if (nn.leftNodeSpeed >= aaa.NewSpeed)
                        {
                            //nn.leftNodeSpeed = aaa.NewSpeed;
                            nn.NewSpeed = aaa.NewSpeed;
                            nn.NewSpeed_raw = aaa.NewSpeed_raw;
                            nn.NewCalDist = aaa.NewCalDist;
                        }
                    }
                    //nn.bIsCSnode = aaa.bIsCSnode;
                    //nn.bIsResetNode = aaa.bIsResetNode;
                    //nn.bIsStopNode = aaa.bIsStopNode;
                    //nn.bIsFlowControl = aaa.bIsFlowControl;
                }
            }

            foreach (DObjLink link in DObjMan.arrLink)
                link.MakeMapSpeedUpInfoText(DObjMan.arrCollision);

            //int nTotal = DGugan.Rule1Cnt + DGugan.Rule2Cnt + DGugan.Rule3Cnt;
            MessageBox.Show("증속완료");
            DObjBase.bDrawStation = false;
            DObjBase.bDrawCollisionInfo = false;
            DObjBase.bDrawMapSpeedUp = true;
            

            //SaveFileDialog dlg = new SaveFileDialog();
            //if(dlg.ShowDialog() == DialogResult.OK)
            //{
            //    string path = dlg.FileName;
            //    File.WriteAllLines(path, allSnakeInfo.ToArray());
            //}

        }

        private void 선택된Node중증속된것만엑셀저장ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //NodeID	NodeType	LeftNodeID	LeftNodeDistance	LeftNodeSpeed	LeftSteering	LeftBranch	RightNodeID	RightNodeDistance	RightNodeSpeed	RightSteering	RightBranch	LeftTeaching	RightTeaching	Updated Date
            //1002    Normal  1003    560 1   NoDirection Normal-Diverge  0   0   0   NoDirection Normal-Diverge  0   0

            List<string> allLines = new List<string>();
            string head = "NodeID,NodeType,LeftNodeID,LeftNodeDistance,LeftNodeSpeed,LeftSteering,LeftBranch,RightNodeID,RightNodeDistance,RightNodeSpeed,RightSteering,RightBranch,LeftTeaching,RightTeaching,Updated Date";
            allLines.Add(head);

            foreach (DObjNode obj in DObjMan.arrNode)
            {
                if (!obj.Selected)
                    continue;

                if (obj.NewSpeed <= 0)
                    continue;

                if (obj.NewSpeed == obj.leftNodeSpeed)
                    continue;

                string tmp = String.Format("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},0,0", obj.ID, obj.nodeType.ToString(), obj.leftNodeID, obj.leftNodeDist, obj.NewSpeed, obj.leftSteering.ToString(), obj.leftBranch.ToString().Replace("_", "-"),
                                                                                obj.rightNodeID, obj.rightNodeDist, obj.rightNodeSpeed, obj.rightSteering.ToString(), obj.rightBranch.ToString().Replace("_", "-")
                    );
                allLines.Add(tmp);
            }
            if (allLines.Count == 0)
            {
                MessageBox.Show("선택된 항목이 없음");
                return;
            }

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.DefaultExt = "csv";
            dlg.Filter = "Excel files (*.csv)|*.csv|All files (*.*)|*.*";


            if (dlg.ShowDialog() == DialogResult.OK)
            {
                File.WriteAllLines(dlg.FileName, allLines.ToArray());

                string arg = "/select, \"" + dlg.FileName + "\"";
                Process.Start("explorer.exe", arg);
            }
        }

        private void 증속된모든Node엑셀저장ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //NodeID	NodeType	LeftNodeID	LeftNodeDistance	LeftNodeSpeed	LeftSteering	LeftBranch	RightNodeID	RightNodeDistance	RightNodeSpeed	RightSteering	RightBranch	LeftTeaching	RightTeaching	Updated Date
            //1002    Normal  1003    560 1   NoDirection Normal-Diverge  0   0   0   NoDirection Normal-Diverge  0   0

            List<string> allLines = new List<string>();
            string head = "NodeID,NodeType,LeftNodeID,LeftNodeDistance,LeftNodeSpeed,LeftSteering,LeftBranch,RightNodeID,RightNodeDistance,RightNodeSpeed,RightSteering,RightBranch,LeftTeaching,RightTeaching,Updated Date";
            allLines.Add(head);

            foreach (DObjNode obj in DObjMan.arrNode)
            {
                //if (!obj.Selected)
                    //continue;

                if (obj.NewSpeed == 0)
                    continue;

                if (obj.NewSpeed == obj.leftNodeSpeed)
                    continue;

                string tmp = String.Format("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},0,0", obj.ID, obj.nodeType.ToString(), obj.leftNodeID, obj.leftNodeDist, obj.NewSpeed, obj.leftSteering.ToString(), obj.leftBranch.ToString().Replace("_", "-"),
                                                                                obj.rightNodeID, obj.rightNodeDist, obj.rightNodeSpeed, obj.rightSteering.ToString(), obj.rightBranch.ToString().Replace("_", "-")
                                                       );
                allLines.Add(tmp);
            }
            if (allLines.Count == 0)
            {
                MessageBox.Show("저장할 항목이 없음");
                return;
            }

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.DefaultExt = "csv";
            dlg.Filter = "Excel files (*.csv)|*.csv|All files (*.*)|*.*";
            
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                File.WriteAllLines(dlg.FileName, allLines.ToArray());

                string arg = "/select, \"" + dlg.FileName + "\"";
                Process.Start("explorer.exe", arg);
            }
        }

        private void 선택된항목중증속된것원복ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (DObjNode obj in DObjMan.arrNode)
            {
                if (!obj.Selected)
                    continue;

                obj.NewSpeed = 0;
                obj.NewSpeed_raw = 0;
                obj.NewCalDist = 0;
            }

            foreach (DObjLink link in DObjMan.arrLink)
                link.MakeMapSpeedUpInfoText(DObjMan.arrCollision);

            Redraw(false);

        }

        private void 선택된항목증속값직접입력ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int nCnt = 0;
            List<DObjBase> arr = DObjMan.arrNode.FindAll(o => o.Selected);
            int nAllCnt = arr.Count;
            string msg = String.Format("선택된 노드 {0}개 의 증속값을 직접 입력하시겠습니까?\n\n", nAllCnt);

            foreach (DObjNode obj in arr)
            {
                if (nCnt > 30)
                {
                    msg += "...";
                    break;
                }

                msg += Convert.ToString(obj.ID) + ", ";
                nCnt++;
            }

            if (MessageBox.Show(msg, "", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) != DialogResult.OK)
                return;

            UKeyIn dlg = new UKeyIn();
            dlg.label1.Text = "속도를 입력하세요. mm/s (ex:3000)";
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            int nVal = 0;
            if (Int32.TryParse(dlg.strVal, out nVal) == false)
            {
                MessageBox.Show("정수를 입력하세요", "입력잘못", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (nVal < 700 || nVal > 6000)
            {
                MessageBox.Show("값 범위 안맞음...", "Range", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            foreach (DObjNode obj in arr)
            {
                obj.NewSpeed = nVal;
                obj.NewSpeed_raw = -1;
                obj.NewCalDist = 0;
            }

            foreach (DObjLink link in DObjMan.arrLink)
                link.MakeMapSpeedUpInfoText(DObjMan.arrCollision);
            Redraw(false);
        }

        private void btnShowNodeDetail_Click(object sender, EventArgs e)
        {
            DObjBase.bDrawNodeDetail = !DObjBase.bDrawNodeDetail;
            btnShowNodeDetail.Image = DObjBase.bDrawNodeDetail ? imageList3.Images[2] : imageList3.Images[3];
            Redraw(false);
        }

        private void 증속된값으로OcsLayout추출ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //DObjMan.OrgMapPath;
        }
		
		private void 시작ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			bOnDistance = true;
			m_CurDistObj = new DObjDist();
			m_CurDistObj.AddStart();
			DObjMan.arrDistance.Add(m_CurDistObj);
		}

		private void 종료ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (m_CurDistObj != null)   // 거리재는거 있으면 마무리하기
			{
				m_CurDistObj.AddFinish();
				if(m_CurDistObj.PointCount() < 2)	// 한점짜리는 지운다
				{
					int n = DObjMan.arrDistance.FindIndex(o => o == m_CurDistObj);
					if (n >= 0)
						DObjMan.arrDistance.RemoveAt(n);
				}
				m_CurDistObj = null;
				bOnDistance = false;
			}

			// 점한개 이하짜리 객체는 정리한다
			for(int i=DObjMan.arrDistance.Count - 1; i>=0;i--)
			{
				DObjDist obj = (DObjDist)DObjMan.arrDistance[i];
				if (obj.PointCount() <= 1)
					DObjMan.arrDistance.RemoveAt(i);
			}
			Redraw(false);
		}

		private void 전체삭제ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			DObjMan.arrDistance.Clear();
			Redraw(false);
		}
	}

	public static class TDrawingControl

    {

        [DllImport("user32.dll")]

        public static extern int SendMessage(IntPtr hWnd, Int32 wMsg, bool wParam, Int32 lParam);



        private const int WM_SETREDRAW = 11;



        /// <summary>
        /// Some controls, such as the DataGridView, do not allow setting the DoubleBuffered property.
        /// It is set as a protected property. This method is a work-around to allow setting it.
        /// Call this in the constructor just after InitializeComponent().
        /// </summary>
        /// <param name="control">The Control on which to set DoubleBuffered to true.</param>
        public static void SetDoubleBuffered(Control control)
        {
            // if not remote desktop session then enable double-buffering optimization
            if (!System.Windows.Forms.SystemInformation.TerminalServerSession)
            {

                // set instance non-public property with name "DoubleBuffered" to true
                typeof(Control).InvokeMember("DoubleBuffered",
                                             System.Reflection.BindingFlags.SetProperty |
                                                System.Reflection.BindingFlags.Instance |
                                                System.Reflection.BindingFlags.NonPublic,
                                             null,
                                             control,
                                             new object[] { true });
            }
        }

        /// <summary>
        /// Suspend drawing updates for the specified control. After the control has been updated
        /// call DrawingControl.ResumeDrawing(Control control).
        /// </summary>
        /// <param name="control">The control to suspend draw updates on.</param>
        public static void SuspendDrawing(Control control)
        {
            SendMessage(control.Handle, WM_SETREDRAW, false, 0);
        }

        /// <summary>
        /// Resume drawing updates for the specified control.
        /// </summary>
        /// <param name="control">The control to resume draw updates on.</param>
        public static void ResumeDrawing(Control control)
        {
            SendMessage(control.Handle, WM_SETREDRAW, true, 0);
            control.Refresh();
        }

       
    }

}

