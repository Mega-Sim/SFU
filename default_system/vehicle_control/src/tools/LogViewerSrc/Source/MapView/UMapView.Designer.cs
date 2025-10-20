namespace MapView
{
    partial class UMapView
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UMapView));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.button_wanring = new System.Windows.Forms.Button();
			this.button_hideErr = new System.Windows.Forms.Button();
			this.textBox_Err = new System.Windows.Forms.TextBox();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.splitContainer3 = new System.Windows.Forms.SplitContainer();
			this.listView_MapInfo = new System.Windows.Forms.ListView();
			this.tabControl_Select_and_Flag = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.listView_Selected = new System.Windows.Forms.ListView();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.listView_Flags = new System.Windows.Forms.ListView();
			this.listView_CurSelInfo = new System.Windows.Forms.ListView();
			this.toolStrip3 = new System.Windows.Forms.ToolStrip();
			this.toolStripBtn_MinMaxStatusWnd = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripBtn_FitWnd = new System.Windows.Forms.ToolStripButton();
			this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
			this.분기노드추출ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.lookDown패턴검증ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.맵증속ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.증속수행ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.증속결과View에표시ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
			this.전체결과저장ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.선택된노드만저장ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.선택된노드원복ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.선택된노드증속값직접입력ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
			this.증속된값으로OcsLayout추출ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
			this.toolStripBtn_VisibleZAxis = new System.Windows.Forms.ToolStripButton();
			this.toolStripBtn_DrawCollisionInfo = new System.Windows.Forms.ToolStripButton();
			this.toolStripButton_ShowStationID = new System.Windows.Forms.ToolStripButton();
			this.toolStripBtn_HideStation = new System.Windows.Forms.ToolStripButton();
			this.toolStripBtn_HideNode = new System.Windows.Forms.ToolStripButton();
			this.btnShowNodeDetail = new System.Windows.Forms.ToolStripButton();
			this.tabControl_Object = new System.Windows.Forms.TabControl();
			this.tabPage_node = new System.Windows.Forms.TabPage();
			this.dataGridView_node = new System.Windows.Forms.DataGridView();
			this.tabPage_station = new System.Windows.Forms.TabPage();
			this.dataGridView_Station = new System.Windows.Forms.DataGridView();
			this.tabPage_CID = new System.Windows.Forms.TabPage();
			this.dataGridView_CID = new System.Windows.Forms.DataGridView();
			this.tabPage_Collision = new System.Windows.Forms.TabPage();
			this.dataGridView_Collision = new System.Windows.Forms.DataGridView();
			this.tabPage_link = new System.Windows.Forms.TabPage();
			this.dataGridView_Link = new System.Windows.Forms.DataGridView();
			this.tabPage_HID = new System.Windows.Forms.TabPage();
			this.dataGridView_HID = new System.Windows.Forms.DataGridView();
			this.tabPage_FlowControl = new System.Windows.Forms.TabPage();
			this.dataGridView_FlowControl = new System.Windows.Forms.DataGridView();
			this.tabPage_Teaching = new System.Windows.Forms.TabPage();
			this.dataGridView_Teaching = new System.Windows.Forms.DataGridView();
			this.tabPage_PathData = new System.Windows.Forms.TabPage();
			this.dataGridView_PathData = new System.Windows.Forms.DataGridView();
			this.toolStrip2 = new System.Windows.Forms.ToolStrip();
			this.toolStripBtn_MinMaxListWnd = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripLabel_CurTabName = new System.Windows.Forms.ToolStripLabel();
			this.toolStripLabel_SelectedCellCnt = new System.Windows.Forms.ToolStripLabel();
			this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
			this.toolStripComboBox_FindItem = new System.Windows.Forms.ToolStripComboBox();
			this.toolStripTextBox_find = new System.Windows.Forms.ToolStripTextBox();
			this.toolStripBtn_FindData = new System.Windows.Forms.ToolStripButton();
			this.toolStripLabel_scale = new System.Windows.Forms.ToolStripLabel();
			this.toolStripLabel_RedrawTime = new System.Windows.Forms.ToolStripLabel();
			this.toolStripLabel_TotalSize = new System.Windows.Forms.ToolStripLabel();
			this.toolStripLabel_MousePos = new System.Windows.Forms.ToolStripLabel();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.timer_progress = new System.Windows.Forms.Timer(this.components);
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.contextMenu_GridRBtn = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ToolStripMenuItem_Find = new System.Windows.Forms.ToolStripMenuItem();
			this.contextMenuStrip_OnDraw = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ToolStripMenuItem_Flag = new System.Windows.Forms.ToolStripMenuItem();
			this.거리측정ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.시작ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.종료ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.전체삭제ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.contextMenuStrip_Flag = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ToolStripMenuItem_FlagSave = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItem_FlagLoad = new System.Windows.Forms.ToolStripMenuItem();
			this.imageList2 = new System.Windows.Forms.ImageList(this.components);
			this.imageList3 = new System.Windows.Forms.ImageList(this.components);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
			this.splitContainer3.Panel1.SuspendLayout();
			this.splitContainer3.Panel2.SuspendLayout();
			this.splitContainer3.SuspendLayout();
			this.tabControl_Select_and_Flag.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.toolStrip3.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.tabControl_Object.SuspendLayout();
			this.tabPage_node.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_node)).BeginInit();
			this.tabPage_station.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Station)).BeginInit();
			this.tabPage_CID.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_CID)).BeginInit();
			this.tabPage_Collision.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Collision)).BeginInit();
			this.tabPage_link.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Link)).BeginInit();
			this.tabPage_HID.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_HID)).BeginInit();
			this.tabPage_FlowControl.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_FlowControl)).BeginInit();
			this.tabPage_Teaching.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Teaching)).BeginInit();
			this.tabPage_PathData.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_PathData)).BeginInit();
			this.toolStrip2.SuspendLayout();
			this.contextMenu_GridRBtn.SuspendLayout();
			this.contextMenuStrip_OnDraw.SuspendLayout();
			this.contextMenuStrip_Flag.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.BackColor = System.Drawing.Color.White;
			this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.tabControl_Object);
			this.splitContainer1.Panel2.Controls.Add(this.toolStrip2);
			this.splitContainer1.Size = new System.Drawing.Size(1008, 696);
			this.splitContainer1.SplitterDistance = 343;
			this.splitContainer1.TabIndex = 0;
			this.splitContainer1.TabStop = false;
			// 
			// splitContainer2
			// 
			this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.button_wanring);
			this.splitContainer2.Panel1.Controls.Add(this.button_hideErr);
			this.splitContainer2.Panel1.Controls.Add(this.textBox_Err);
			this.splitContainer2.Panel1.Controls.Add(this.progressBar1);
			this.splitContainer2.Panel1.Controls.Add(this.pictureBox1);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.splitContainer3);
			this.splitContainer2.Panel2.Controls.Add(this.toolStrip3);
			this.splitContainer2.Panel2.Controls.Add(this.toolStrip1);
			this.splitContainer2.Size = new System.Drawing.Size(1008, 343);
			this.splitContainer2.SplitterDistance = 727;
			this.splitContainer2.TabIndex = 0;
			this.splitContainer2.TabStop = false;
			// 
			// button_wanring
			// 
			this.button_wanring.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.button_wanring.Image = ((System.Drawing.Image)(resources.GetObject("button_wanring.Image")));
			this.button_wanring.Location = new System.Drawing.Point(-2, -2);
			this.button_wanring.Name = "button_wanring";
			this.button_wanring.Size = new System.Drawing.Size(36, 33);
			this.button_wanring.TabIndex = 5;
			this.button_wanring.UseVisualStyleBackColor = true;
			this.button_wanring.Visible = false;
			this.button_wanring.Click += new System.EventHandler(this.button_wanring_Click);
			// 
			// button_hideErr
			// 
			this.button_hideErr.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.button_hideErr.Location = new System.Drawing.Point(487, 268);
			this.button_hideErr.Name = "button_hideErr";
			this.button_hideErr.Size = new System.Drawing.Size(137, 27);
			this.button_hideErr.TabIndex = 5;
			this.button_hideErr.Text = "Hide";
			this.button_hideErr.UseVisualStyleBackColor = true;
			this.button_hideErr.Visible = false;
			this.button_hideErr.Click += new System.EventHandler(this.button_hideErr_Click);
			// 
			// textBox_Err
			// 
			this.textBox_Err.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.textBox_Err.Font = new System.Drawing.Font("돋움", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Err.Location = new System.Drawing.Point(52, 25);
			this.textBox_Err.Multiline = true;
			this.textBox_Err.Name = "textBox_Err";
			this.textBox_Err.Size = new System.Drawing.Size(572, 237);
			this.textBox_Err.TabIndex = 4;
			this.textBox_Err.Visible = false;
			// 
			// progressBar1
			// 
			this.progressBar1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.progressBar1.Location = new System.Drawing.Point(0, 324);
			this.progressBar1.Maximum = 1000;
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(723, 15);
			this.progressBar1.Step = 1;
			this.progressBar1.TabIndex = 3;
			this.progressBar1.Visible = false;
			// 
			// pictureBox1
			// 
			this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox1.Cursor = System.Windows.Forms.Cursors.Arrow;
			this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox1.Location = new System.Drawing.Point(0, 0);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(723, 339);
			this.pictureBox1.TabIndex = 2;
			this.pictureBox1.TabStop = false;
			this.pictureBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
			this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
			this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
			this.pictureBox1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseClick);
			this.pictureBox1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDoubleClick);
			this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
			this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
			this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
			this.pictureBox1.Resize += new System.EventHandler(this.pictureBox1_Resize);
			// 
			// splitContainer3
			// 
			this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer3.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer3.Location = new System.Drawing.Point(0, 62);
			this.splitContainer3.Name = "splitContainer3";
			this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this.splitContainer3.Panel1.Controls.Add(this.listView_MapInfo);
			// 
			// splitContainer3.Panel2
			// 
			this.splitContainer3.Panel2.Controls.Add(this.tabControl_Select_and_Flag);
			this.splitContainer3.Panel2.Controls.Add(this.listView_CurSelInfo);
			this.splitContainer3.Size = new System.Drawing.Size(273, 277);
			this.splitContainer3.SplitterDistance = 170;
			this.splitContainer3.TabIndex = 1;
			this.splitContainer3.TabStop = false;
			// 
			// listView_MapInfo
			// 
			this.listView_MapInfo.BackColor = System.Drawing.Color.White;
			this.listView_MapInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_MapInfo.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_MapInfo.ForeColor = System.Drawing.SystemColors.Info;
			this.listView_MapInfo.FullRowSelect = true;
			this.listView_MapInfo.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
			this.listView_MapInfo.Location = new System.Drawing.Point(0, 0);
			this.listView_MapInfo.Name = "listView_MapInfo";
			this.listView_MapInfo.Size = new System.Drawing.Size(273, 170);
			this.listView_MapInfo.TabIndex = 2;
			this.listView_MapInfo.UseCompatibleStateImageBehavior = false;
			this.listView_MapInfo.View = System.Windows.Forms.View.Details;
			// 
			// tabControl_Select_and_Flag
			// 
			this.tabControl_Select_and_Flag.Controls.Add(this.tabPage1);
			this.tabControl_Select_and_Flag.Controls.Add(this.tabPage2);
			this.tabControl_Select_and_Flag.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl_Select_and_Flag.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.tabControl_Select_and_Flag.Location = new System.Drawing.Point(0, 0);
			this.tabControl_Select_and_Flag.Name = "tabControl_Select_and_Flag";
			this.tabControl_Select_and_Flag.SelectedIndex = 0;
			this.tabControl_Select_and_Flag.Size = new System.Drawing.Size(273, 103);
			this.tabControl_Select_and_Flag.TabIndex = 1;
			// 
			// tabPage1
			// 
			this.tabPage1.BackColor = System.Drawing.Color.White;
			this.tabPage1.Controls.Add(this.listView_Selected);
			this.tabPage1.Location = new System.Drawing.Point(4, 24);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage1.Size = new System.Drawing.Size(265, 75);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "Selected";
			// 
			// listView_Selected
			// 
			this.listView_Selected.BackColor = System.Drawing.Color.White;
			this.listView_Selected.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_Selected.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_Selected.ForeColor = System.Drawing.SystemColors.Control;
			this.listView_Selected.FullRowSelect = true;
			this.listView_Selected.Location = new System.Drawing.Point(3, 3);
			this.listView_Selected.Name = "listView_Selected";
			this.listView_Selected.Size = new System.Drawing.Size(259, 69);
			this.listView_Selected.TabIndex = 0;
			this.listView_Selected.UseCompatibleStateImageBehavior = false;
			this.listView_Selected.View = System.Windows.Forms.View.Details;
			this.listView_Selected.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.listView_Selected_ItemSelectionChanged);
			this.listView_Selected.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_Selected_KeyDown);
			this.listView_Selected.MouseClick += new System.Windows.Forms.MouseEventHandler(this.listView_Selected_MouseClick);
			this.listView_Selected.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView_Selected_MouseDoubleClick);
			// 
			// tabPage2
			// 
			this.tabPage2.BackColor = System.Drawing.Color.White;
			this.tabPage2.Controls.Add(this.listView_Flags);
			this.tabPage2.Location = new System.Drawing.Point(4, 24);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage2.Size = new System.Drawing.Size(265, 75);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "Flags";
			// 
			// listView_Flags
			// 
			this.listView_Flags.BackColor = System.Drawing.Color.White;
			this.listView_Flags.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_Flags.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_Flags.ForeColor = System.Drawing.SystemColors.Control;
			this.listView_Flags.FullRowSelect = true;
			this.listView_Flags.Location = new System.Drawing.Point(3, 3);
			this.listView_Flags.Name = "listView_Flags";
			this.listView_Flags.Size = new System.Drawing.Size(259, 69);
			this.listView_Flags.TabIndex = 1;
			this.listView_Flags.UseCompatibleStateImageBehavior = false;
			this.listView_Flags.View = System.Windows.Forms.View.Details;
			this.listView_Flags.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.listView_Flags_ItemSelectionChanged);
			this.listView_Flags.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_Flags_KeyDown);
			this.listView_Flags.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView_Flags_MouseDoubleClick);
			this.listView_Flags.MouseDown += new System.Windows.Forms.MouseEventHandler(this.listView_Flags_MouseDown);
			// 
			// listView_CurSelInfo
			// 
			this.listView_CurSelInfo.BackColor = System.Drawing.SystemColors.Desktop;
			this.listView_CurSelInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_CurSelInfo.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_CurSelInfo.Location = new System.Drawing.Point(0, 0);
			this.listView_CurSelInfo.Name = "listView_CurSelInfo";
			this.listView_CurSelInfo.Size = new System.Drawing.Size(273, 103);
			this.listView_CurSelInfo.TabIndex = 0;
			this.listView_CurSelInfo.UseCompatibleStateImageBehavior = false;
			// 
			// toolStrip3
			// 
			this.toolStrip3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			this.toolStrip3.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip3.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripBtn_MinMaxStatusWnd,
            this.toolStripSeparator1,
            this.toolStripBtn_FitWnd,
            this.toolStripDropDownButton1});
			this.toolStrip3.Location = new System.Drawing.Point(0, 37);
			this.toolStrip3.Name = "toolStrip3";
			this.toolStrip3.Size = new System.Drawing.Size(273, 25);
			this.toolStrip3.TabIndex = 1;
			this.toolStrip3.Text = "toolStrip3";
			// 
			// toolStripBtn_MinMaxStatusWnd
			// 
			this.toolStripBtn_MinMaxStatusWnd.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_MinMaxStatusWnd.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_MinMaxStatusWnd.Image")));
			this.toolStripBtn_MinMaxStatusWnd.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_MinMaxStatusWnd.Name = "toolStripBtn_MinMaxStatusWnd";
			this.toolStripBtn_MinMaxStatusWnd.Size = new System.Drawing.Size(23, 22);
			this.toolStripBtn_MinMaxStatusWnd.Text = "toolStripButton1";
			this.toolStripBtn_MinMaxStatusWnd.ToolTipText = "Expand / Collapse [F11]";
			this.toolStripBtn_MinMaxStatusWnd.Click += new System.EventHandler(this.toolStripBtn_MinMaxStatusWnd_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
			// 
			// toolStripBtn_FitWnd
			// 
			this.toolStripBtn_FitWnd.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_FitWnd.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_FitWnd.Image")));
			this.toolStripBtn_FitWnd.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_FitWnd.Name = "toolStripBtn_FitWnd";
			this.toolStripBtn_FitWnd.Size = new System.Drawing.Size(23, 22);
			this.toolStripBtn_FitWnd.Text = "toolStripButton1";
			this.toolStripBtn_FitWnd.ToolTipText = "Fit Window [F4]";
			this.toolStripBtn_FitWnd.Click += new System.EventHandler(this.toolStripBtn_FitWnd_Click);
			// 
			// toolStripDropDownButton1
			// 
			this.toolStripDropDownButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.toolStripDropDownButton1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.분기노드추출ToolStripMenuItem,
            this.lookDown패턴검증ToolStripMenuItem,
            this.맵증속ToolStripMenuItem});
			this.toolStripDropDownButton1.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
			this.toolStripDropDownButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripDropDownButton1.Image")));
			this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
			this.toolStripDropDownButton1.Size = new System.Drawing.Size(68, 22);
			this.toolStripDropDownButton1.Text = "부가기능";
			// 
			// 분기노드추출ToolStripMenuItem
			// 
			this.분기노드추출ToolStripMenuItem.Name = "분기노드추출ToolStripMenuItem";
			this.분기노드추출ToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.분기노드추출ToolStripMenuItem.Text = "분기노드 추출";
			// 
			// lookDown패턴검증ToolStripMenuItem
			// 
			this.lookDown패턴검증ToolStripMenuItem.Name = "lookDown패턴검증ToolStripMenuItem";
			this.lookDown패턴검증ToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.lookDown패턴검증ToolStripMenuItem.Text = "LookDown 패턴 검증";
			// 
			// 맵증속ToolStripMenuItem
			// 
			this.맵증속ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.증속수행ToolStripMenuItem,
            this.증속결과View에표시ToolStripMenuItem,
            this.toolStripMenuItem2,
            this.전체결과저장ToolStripMenuItem,
            this.선택된노드만저장ToolStripMenuItem,
            this.선택된노드원복ToolStripMenuItem,
            this.선택된노드증속값직접입력ToolStripMenuItem,
            this.toolStripMenuItem3,
            this.증속된값으로OcsLayout추출ToolStripMenuItem});
			this.맵증속ToolStripMenuItem.Name = "맵증속ToolStripMenuItem";
			this.맵증속ToolStripMenuItem.Size = new System.Drawing.Size(188, 22);
			this.맵증속ToolStripMenuItem.Text = "맵 증속";
			// 
			// 증속수행ToolStripMenuItem
			// 
			this.증속수행ToolStripMenuItem.Name = "증속수행ToolStripMenuItem";
			this.증속수행ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.증속수행ToolStripMenuItem.Text = "실행";
			this.증속수행ToolStripMenuItem.Click += new System.EventHandler(this.btn_MapSpeedUp_Click);
			// 
			// 증속결과View에표시ToolStripMenuItem
			// 
			this.증속결과View에표시ToolStripMenuItem.Name = "증속결과View에표시ToolStripMenuItem";
			this.증속결과View에표시ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.증속결과View에표시ToolStripMenuItem.Text = "증속결과 View에 표시";
			this.증속결과View에표시ToolStripMenuItem.Click += new System.EventHandler(this.btnMapSpeedUpView_Click);
			// 
			// toolStripMenuItem2
			// 
			this.toolStripMenuItem2.Enabled = false;
			this.toolStripMenuItem2.Name = "toolStripMenuItem2";
			this.toolStripMenuItem2.Size = new System.Drawing.Size(238, 22);
			this.toolStripMenuItem2.Text = "--------------";
			// 
			// 전체결과저장ToolStripMenuItem
			// 
			this.전체결과저장ToolStripMenuItem.Name = "전체결과저장ToolStripMenuItem";
			this.전체결과저장ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.전체결과저장ToolStripMenuItem.Text = "전체 결과 저장";
			this.전체결과저장ToolStripMenuItem.Click += new System.EventHandler(this.증속된모든Node엑셀저장ToolStripMenuItem_Click);
			// 
			// 선택된노드만저장ToolStripMenuItem
			// 
			this.선택된노드만저장ToolStripMenuItem.Name = "선택된노드만저장ToolStripMenuItem";
			this.선택된노드만저장ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.선택된노드만저장ToolStripMenuItem.Text = "선택된 노드만 저장";
			this.선택된노드만저장ToolStripMenuItem.Click += new System.EventHandler(this.선택된Node중증속된것만엑셀저장ToolStripMenuItem_Click);
			// 
			// 선택된노드원복ToolStripMenuItem
			// 
			this.선택된노드원복ToolStripMenuItem.Name = "선택된노드원복ToolStripMenuItem";
			this.선택된노드원복ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.선택된노드원복ToolStripMenuItem.Text = "선택된 노드 원복";
			this.선택된노드원복ToolStripMenuItem.Click += new System.EventHandler(this.선택된항목중증속된것원복ToolStripMenuItem_Click);
			// 
			// 선택된노드증속값직접입력ToolStripMenuItem
			// 
			this.선택된노드증속값직접입력ToolStripMenuItem.Name = "선택된노드증속값직접입력ToolStripMenuItem";
			this.선택된노드증속값직접입력ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.선택된노드증속값직접입력ToolStripMenuItem.Text = "선택된 노드 증속값 직접 입력";
			this.선택된노드증속값직접입력ToolStripMenuItem.Click += new System.EventHandler(this.선택된항목증속값직접입력ToolStripMenuItem_Click);
			// 
			// toolStripMenuItem3
			// 
			this.toolStripMenuItem3.Enabled = false;
			this.toolStripMenuItem3.Name = "toolStripMenuItem3";
			this.toolStripMenuItem3.Size = new System.Drawing.Size(238, 22);
			this.toolStripMenuItem3.Text = "-------------";
			// 
			// 증속된값으로OcsLayout추출ToolStripMenuItem
			// 
			this.증속된값으로OcsLayout추출ToolStripMenuItem.Name = "증속된값으로OcsLayout추출ToolStripMenuItem";
			this.증속된값으로OcsLayout추출ToolStripMenuItem.Size = new System.Drawing.Size(238, 22);
			this.증속된값으로OcsLayout추출ToolStripMenuItem.Text = "증속된 값으로 OcsLayout 추출";
			this.증속된값으로OcsLayout추출ToolStripMenuItem.Click += new System.EventHandler(this.증속된값으로OcsLayout추출ToolStripMenuItem_Click);
			// 
			// toolStrip1
			// 
			this.toolStrip1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
			this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripBtn_VisibleZAxis,
            this.toolStripBtn_DrawCollisionInfo,
            this.toolStripButton_ShowStationID,
            this.toolStripBtn_HideStation,
            this.toolStripBtn_HideNode,
            this.btnShowNodeDetail});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(273, 37);
			this.toolStrip1.TabIndex = 0;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// toolStripButton1
			// 
			this.toolStripButton1.BackColor = System.Drawing.Color.DarkGray;
			this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.toolStripButton1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.toolStripButton1.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButton1.Name = "toolStripButton1";
			this.toolStripButton1.Size = new System.Drawing.Size(66, 34);
			this.toolStripButton1.Text = "MapLoad";
			this.toolStripButton1.Click += new System.EventHandler(this.buttonMapLoading);
			// 
			// toolStripBtn_VisibleZAxis
			// 
			this.toolStripBtn_VisibleZAxis.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_VisibleZAxis.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_VisibleZAxis.Image")));
			this.toolStripBtn_VisibleZAxis.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripBtn_VisibleZAxis.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_VisibleZAxis.Name = "toolStripBtn_VisibleZAxis";
			this.toolStripBtn_VisibleZAxis.Size = new System.Drawing.Size(34, 34);
			this.toolStripBtn_VisibleZAxis.Text = "toolStripButton1";
			this.toolStripBtn_VisibleZAxis.ToolTipText = "Layer";
			this.toolStripBtn_VisibleZAxis.Click += new System.EventHandler(this.toolStripBtn_VisibleZAxis_Click);
			// 
			// toolStripBtn_DrawCollisionInfo
			// 
			this.toolStripBtn_DrawCollisionInfo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_DrawCollisionInfo.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_DrawCollisionInfo.Image")));
			this.toolStripBtn_DrawCollisionInfo.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripBtn_DrawCollisionInfo.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_DrawCollisionInfo.Name = "toolStripBtn_DrawCollisionInfo";
			this.toolStripBtn_DrawCollisionInfo.Size = new System.Drawing.Size(34, 34);
			this.toolStripBtn_DrawCollisionInfo.Text = "toolStripButton2";
			this.toolStripBtn_DrawCollisionInfo.ToolTipText = "Speed/Steer/UBG [A]";
			this.toolStripBtn_DrawCollisionInfo.Click += new System.EventHandler(this.toolStripBtn_DrawCollisionInfo_Click);
			// 
			// toolStripButton_ShowStationID
			// 
			this.toolStripButton_ShowStationID.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripButton_ShowStationID.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_ShowStationID.Image")));
			this.toolStripButton_ShowStationID.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripButton_ShowStationID.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButton_ShowStationID.Name = "toolStripButton_ShowStationID";
			this.toolStripButton_ShowStationID.Size = new System.Drawing.Size(34, 34);
			this.toolStripButton_ShowStationID.Text = "toolStripButton2";
			this.toolStripButton_ShowStationID.ToolTipText = "Staion ID [D]";
			this.toolStripButton_ShowStationID.Click += new System.EventHandler(this.toolStripButton_ShowStationID_Click);
			// 
			// toolStripBtn_HideStation
			// 
			this.toolStripBtn_HideStation.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_HideStation.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_HideStation.Image")));
			this.toolStripBtn_HideStation.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripBtn_HideStation.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_HideStation.Name = "toolStripBtn_HideStation";
			this.toolStripBtn_HideStation.Size = new System.Drawing.Size(34, 34);
			this.toolStripBtn_HideStation.Text = "toolStripButton1";
			this.toolStripBtn_HideStation.ToolTipText = "Station [S]";
			this.toolStripBtn_HideStation.Click += new System.EventHandler(this.toolStripBtn_HideStation_Click);
			// 
			// toolStripBtn_HideNode
			// 
			this.toolStripBtn_HideNode.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_HideNode.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_HideNode.Image")));
			this.toolStripBtn_HideNode.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripBtn_HideNode.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_HideNode.Name = "toolStripBtn_HideNode";
			this.toolStripBtn_HideNode.Size = new System.Drawing.Size(34, 34);
			this.toolStripBtn_HideNode.Text = "toolStripButton1";
			this.toolStripBtn_HideNode.ToolTipText = "Node [N]";
			this.toolStripBtn_HideNode.Click += new System.EventHandler(this.toolStripBtn_HideNode_Click);
			// 
			// btnShowNodeDetail
			// 
			this.btnShowNodeDetail.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btnShowNodeDetail.Image = ((System.Drawing.Image)(resources.GetObject("btnShowNodeDetail.Image")));
			this.btnShowNodeDetail.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btnShowNodeDetail.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btnShowNodeDetail.Name = "btnShowNodeDetail";
			this.btnShowNodeDetail.Size = new System.Drawing.Size(34, 34);
			this.btnShowNodeDetail.Text = "toolStripButton1";
			this.btnShowNodeDetail.ToolTipText = "Show Node Detail [B]";
			this.btnShowNodeDetail.Click += new System.EventHandler(this.btnShowNodeDetail_Click);
			// 
			// tabControl_Object
			// 
			this.tabControl_Object.Alignment = System.Windows.Forms.TabAlignment.Left;
			this.tabControl_Object.Controls.Add(this.tabPage_node);
			this.tabControl_Object.Controls.Add(this.tabPage_station);
			this.tabControl_Object.Controls.Add(this.tabPage_CID);
			this.tabControl_Object.Controls.Add(this.tabPage_Collision);
			this.tabControl_Object.Controls.Add(this.tabPage_link);
			this.tabControl_Object.Controls.Add(this.tabPage_HID);
			this.tabControl_Object.Controls.Add(this.tabPage_FlowControl);
			this.tabControl_Object.Controls.Add(this.tabPage_Teaching);
			this.tabControl_Object.Controls.Add(this.tabPage_PathData);
			this.tabControl_Object.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl_Object.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.tabControl_Object.Location = new System.Drawing.Point(0, 25);
			this.tabControl_Object.Multiline = true;
			this.tabControl_Object.Name = "tabControl_Object";
			this.tabControl_Object.SelectedIndex = 0;
			this.tabControl_Object.Size = new System.Drawing.Size(1004, 320);
			this.tabControl_Object.TabIndex = 1;
			// 
			// tabPage_node
			// 
			this.tabPage_node.Controls.Add(this.dataGridView_node);
			this.tabPage_node.Location = new System.Drawing.Point(52, 4);
			this.tabPage_node.Name = "tabPage_node";
			this.tabPage_node.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage_node.Size = new System.Drawing.Size(948, 312);
			this.tabPage_node.TabIndex = 0;
			this.tabPage_node.Text = "Node";
			this.tabPage_node.UseVisualStyleBackColor = true;
			// 
			// dataGridView_node
			// 
			this.dataGridView_node.AllowUserToAddRows = false;
			this.dataGridView_node.AllowUserToDeleteRows = false;
			this.dataGridView_node.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_node.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_node.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_node.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_node.Location = new System.Drawing.Point(3, 3);
			this.dataGridView_node.Name = "dataGridView_node";
			this.dataGridView_node.ReadOnly = true;
			this.dataGridView_node.RowTemplate.Height = 23;
			this.dataGridView_node.ShowCellErrors = false;
			this.dataGridView_node.ShowCellToolTips = false;
			this.dataGridView_node.ShowEditingIcon = false;
			this.dataGridView_node.ShowRowErrors = false;
			this.dataGridView_node.Size = new System.Drawing.Size(942, 306);
			this.dataGridView_node.TabIndex = 0;
			// 
			// tabPage_station
			// 
			this.tabPage_station.Controls.Add(this.dataGridView_Station);
			this.tabPage_station.Location = new System.Drawing.Point(52, 4);
			this.tabPage_station.Name = "tabPage_station";
			this.tabPage_station.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage_station.Size = new System.Drawing.Size(948, 312);
			this.tabPage_station.TabIndex = 1;
			this.tabPage_station.Text = "Station";
			this.tabPage_station.UseVisualStyleBackColor = true;
			// 
			// dataGridView_Station
			// 
			this.dataGridView_Station.AllowUserToAddRows = false;
			this.dataGridView_Station.AllowUserToDeleteRows = false;
			this.dataGridView_Station.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_Station.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Station.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Station.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_Station.Location = new System.Drawing.Point(3, 3);
			this.dataGridView_Station.Name = "dataGridView_Station";
			this.dataGridView_Station.ReadOnly = true;
			this.dataGridView_Station.RowTemplate.Height = 23;
			this.dataGridView_Station.ShowCellErrors = false;
			this.dataGridView_Station.ShowCellToolTips = false;
			this.dataGridView_Station.ShowEditingIcon = false;
			this.dataGridView_Station.ShowRowErrors = false;
			this.dataGridView_Station.Size = new System.Drawing.Size(942, 306);
			this.dataGridView_Station.TabIndex = 1;
			// 
			// tabPage_CID
			// 
			this.tabPage_CID.Controls.Add(this.dataGridView_CID);
			this.tabPage_CID.Location = new System.Drawing.Point(52, 4);
			this.tabPage_CID.Name = "tabPage_CID";
			this.tabPage_CID.Size = new System.Drawing.Size(948, 312);
			this.tabPage_CID.TabIndex = 2;
			this.tabPage_CID.Text = "CID";
			this.tabPage_CID.UseVisualStyleBackColor = true;
			// 
			// dataGridView_CID
			// 
			this.dataGridView_CID.AllowUserToAddRows = false;
			this.dataGridView_CID.AllowUserToDeleteRows = false;
			this.dataGridView_CID.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_CID.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_CID.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_CID.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_CID.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_CID.Name = "dataGridView_CID";
			this.dataGridView_CID.ReadOnly = true;
			this.dataGridView_CID.RowTemplate.Height = 23;
			this.dataGridView_CID.ShowCellErrors = false;
			this.dataGridView_CID.ShowCellToolTips = false;
			this.dataGridView_CID.ShowEditingIcon = false;
			this.dataGridView_CID.ShowRowErrors = false;
			this.dataGridView_CID.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_CID.TabIndex = 1;
			// 
			// tabPage_Collision
			// 
			this.tabPage_Collision.Controls.Add(this.dataGridView_Collision);
			this.tabPage_Collision.Location = new System.Drawing.Point(52, 4);
			this.tabPage_Collision.Name = "tabPage_Collision";
			this.tabPage_Collision.Size = new System.Drawing.Size(948, 312);
			this.tabPage_Collision.TabIndex = 3;
			this.tabPage_Collision.Text = "Collision";
			this.tabPage_Collision.UseVisualStyleBackColor = true;
			// 
			// dataGridView_Collision
			// 
			this.dataGridView_Collision.AllowUserToAddRows = false;
			this.dataGridView_Collision.AllowUserToDeleteRows = false;
			this.dataGridView_Collision.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_Collision.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Collision.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Collision.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_Collision.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_Collision.Name = "dataGridView_Collision";
			this.dataGridView_Collision.ReadOnly = true;
			this.dataGridView_Collision.RowTemplate.Height = 23;
			this.dataGridView_Collision.ShowCellErrors = false;
			this.dataGridView_Collision.ShowCellToolTips = false;
			this.dataGridView_Collision.ShowEditingIcon = false;
			this.dataGridView_Collision.ShowRowErrors = false;
			this.dataGridView_Collision.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_Collision.TabIndex = 1;
			// 
			// tabPage_link
			// 
			this.tabPage_link.Controls.Add(this.dataGridView_Link);
			this.tabPage_link.Location = new System.Drawing.Point(52, 4);
			this.tabPage_link.Name = "tabPage_link";
			this.tabPage_link.Size = new System.Drawing.Size(948, 312);
			this.tabPage_link.TabIndex = 4;
			this.tabPage_link.Text = "Link";
			this.tabPage_link.UseVisualStyleBackColor = true;
			// 
			// dataGridView_Link
			// 
			this.dataGridView_Link.AllowUserToAddRows = false;
			this.dataGridView_Link.AllowUserToDeleteRows = false;
			this.dataGridView_Link.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_Link.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Link.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Link.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_Link.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_Link.Name = "dataGridView_Link";
			this.dataGridView_Link.ReadOnly = true;
			this.dataGridView_Link.RowTemplate.Height = 23;
			this.dataGridView_Link.ShowCellErrors = false;
			this.dataGridView_Link.ShowCellToolTips = false;
			this.dataGridView_Link.ShowEditingIcon = false;
			this.dataGridView_Link.ShowRowErrors = false;
			this.dataGridView_Link.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_Link.TabIndex = 1;
			// 
			// tabPage_HID
			// 
			this.tabPage_HID.Controls.Add(this.dataGridView_HID);
			this.tabPage_HID.Location = new System.Drawing.Point(52, 4);
			this.tabPage_HID.Name = "tabPage_HID";
			this.tabPage_HID.Size = new System.Drawing.Size(948, 312);
			this.tabPage_HID.TabIndex = 5;
			this.tabPage_HID.Text = "HID";
			this.tabPage_HID.UseVisualStyleBackColor = true;
			// 
			// dataGridView_HID
			// 
			this.dataGridView_HID.AllowUserToAddRows = false;
			this.dataGridView_HID.AllowUserToDeleteRows = false;
			this.dataGridView_HID.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_HID.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_HID.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_HID.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_HID.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_HID.Name = "dataGridView_HID";
			this.dataGridView_HID.ReadOnly = true;
			this.dataGridView_HID.RowTemplate.Height = 23;
			this.dataGridView_HID.ShowCellErrors = false;
			this.dataGridView_HID.ShowCellToolTips = false;
			this.dataGridView_HID.ShowEditingIcon = false;
			this.dataGridView_HID.ShowRowErrors = false;
			this.dataGridView_HID.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_HID.TabIndex = 1;
			// 
			// tabPage_FlowControl
			// 
			this.tabPage_FlowControl.Controls.Add(this.dataGridView_FlowControl);
			this.tabPage_FlowControl.Location = new System.Drawing.Point(52, 4);
			this.tabPage_FlowControl.Name = "tabPage_FlowControl";
			this.tabPage_FlowControl.Size = new System.Drawing.Size(948, 312);
			this.tabPage_FlowControl.TabIndex = 6;
			this.tabPage_FlowControl.Text = "FlowControl";
			this.tabPage_FlowControl.UseVisualStyleBackColor = true;
			// 
			// dataGridView_FlowControl
			// 
			this.dataGridView_FlowControl.AllowUserToAddRows = false;
			this.dataGridView_FlowControl.AllowUserToDeleteRows = false;
			this.dataGridView_FlowControl.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_FlowControl.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_FlowControl.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_FlowControl.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_FlowControl.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_FlowControl.Name = "dataGridView_FlowControl";
			this.dataGridView_FlowControl.ReadOnly = true;
			this.dataGridView_FlowControl.RowTemplate.Height = 23;
			this.dataGridView_FlowControl.ShowCellErrors = false;
			this.dataGridView_FlowControl.ShowCellToolTips = false;
			this.dataGridView_FlowControl.ShowEditingIcon = false;
			this.dataGridView_FlowControl.ShowRowErrors = false;
			this.dataGridView_FlowControl.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_FlowControl.TabIndex = 1;
			// 
			// tabPage_Teaching
			// 
			this.tabPage_Teaching.Controls.Add(this.dataGridView_Teaching);
			this.tabPage_Teaching.Location = new System.Drawing.Point(52, 4);
			this.tabPage_Teaching.Name = "tabPage_Teaching";
			this.tabPage_Teaching.Size = new System.Drawing.Size(948, 312);
			this.tabPage_Teaching.TabIndex = 7;
			this.tabPage_Teaching.Text = "Teaching";
			this.tabPage_Teaching.UseVisualStyleBackColor = true;
			// 
			// dataGridView_Teaching
			// 
			this.dataGridView_Teaching.AllowUserToAddRows = false;
			this.dataGridView_Teaching.AllowUserToDeleteRows = false;
			this.dataGridView_Teaching.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_Teaching.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Teaching.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Teaching.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_Teaching.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_Teaching.Name = "dataGridView_Teaching";
			this.dataGridView_Teaching.ReadOnly = true;
			this.dataGridView_Teaching.RowTemplate.Height = 23;
			this.dataGridView_Teaching.ShowCellErrors = false;
			this.dataGridView_Teaching.ShowCellToolTips = false;
			this.dataGridView_Teaching.ShowEditingIcon = false;
			this.dataGridView_Teaching.ShowRowErrors = false;
			this.dataGridView_Teaching.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_Teaching.TabIndex = 1;
			// 
			// tabPage_PathData
			// 
			this.tabPage_PathData.Controls.Add(this.dataGridView_PathData);
			this.tabPage_PathData.Location = new System.Drawing.Point(52, 4);
			this.tabPage_PathData.Name = "tabPage_PathData";
			this.tabPage_PathData.Size = new System.Drawing.Size(948, 312);
			this.tabPage_PathData.TabIndex = 8;
			this.tabPage_PathData.Text = "Path Data";
			this.tabPage_PathData.UseVisualStyleBackColor = true;
			// 
			// dataGridView_PathData
			// 
			this.dataGridView_PathData.AllowUserToAddRows = false;
			this.dataGridView_PathData.AllowUserToDeleteRows = false;
			this.dataGridView_PathData.BackgroundColor = System.Drawing.SystemColors.Control;
			this.dataGridView_PathData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_PathData.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_PathData.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_PathData.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_PathData.Name = "dataGridView_PathData";
			this.dataGridView_PathData.ReadOnly = true;
			this.dataGridView_PathData.RowTemplate.Height = 23;
			this.dataGridView_PathData.ShowCellErrors = false;
			this.dataGridView_PathData.ShowCellToolTips = false;
			this.dataGridView_PathData.ShowEditingIcon = false;
			this.dataGridView_PathData.ShowRowErrors = false;
			this.dataGridView_PathData.Size = new System.Drawing.Size(948, 312);
			this.dataGridView_PathData.TabIndex = 1;
			// 
			// toolStrip2
			// 
			this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripBtn_MinMaxListWnd,
            this.toolStripSeparator2,
            this.toolStripLabel_CurTabName,
            this.toolStripLabel_SelectedCellCnt,
            this.toolStripSeparator3,
            this.toolStripLabel1,
            this.toolStripComboBox_FindItem,
            this.toolStripTextBox_find,
            this.toolStripBtn_FindData,
            this.toolStripLabel_scale,
            this.toolStripLabel_RedrawTime,
            this.toolStripLabel_TotalSize,
            this.toolStripLabel_MousePos});
			this.toolStrip2.Location = new System.Drawing.Point(0, 0);
			this.toolStrip2.Name = "toolStrip2";
			this.toolStrip2.Size = new System.Drawing.Size(1004, 25);
			this.toolStrip2.TabIndex = 0;
			this.toolStrip2.Text = "toolStrip2";
			// 
			// toolStripBtn_MinMaxListWnd
			// 
			this.toolStripBtn_MinMaxListWnd.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_MinMaxListWnd.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_MinMaxListWnd.Image")));
			this.toolStripBtn_MinMaxListWnd.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_MinMaxListWnd.Name = "toolStripBtn_MinMaxListWnd";
			this.toolStripBtn_MinMaxListWnd.Size = new System.Drawing.Size(23, 22);
			this.toolStripBtn_MinMaxListWnd.Text = "toolStripButton1";
			this.toolStripBtn_MinMaxListWnd.ToolTipText = "Expand / Collapse";
			this.toolStripBtn_MinMaxListWnd.Click += new System.EventHandler(this.toolStripBtn_MinMaxListWnd_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			// 
			// toolStripLabel_CurTabName
			// 
			this.toolStripLabel_CurTabName.AutoSize = false;
			this.toolStripLabel_CurTabName.BackColor = System.Drawing.SystemColors.Desktop;
			this.toolStripLabel_CurTabName.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.toolStripLabel_CurTabName.Name = "toolStripLabel_CurTabName";
			this.toolStripLabel_CurTabName.Size = new System.Drawing.Size(150, 22);
			this.toolStripLabel_CurTabName.Text = "Cur Tab Name";
			// 
			// toolStripLabel_SelectedCellCnt
			// 
			this.toolStripLabel_SelectedCellCnt.AutoSize = false;
			this.toolStripLabel_SelectedCellCnt.Name = "toolStripLabel_SelectedCellCnt";
			this.toolStripLabel_SelectedCellCnt.Size = new System.Drawing.Size(130, 22);
			this.toolStripLabel_SelectedCellCnt.Text = "Selected Cnt";
			// 
			// toolStripSeparator3
			// 
			this.toolStripSeparator3.Name = "toolStripSeparator3";
			this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
			// 
			// toolStripLabel1
			// 
			this.toolStripLabel1.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.toolStripLabel1.Name = "toolStripLabel1";
			this.toolStripLabel1.Size = new System.Drawing.Size(30, 22);
			this.toolStripLabel1.Text = "Find";
			// 
			// toolStripComboBox_FindItem
			// 
			this.toolStripComboBox_FindItem.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.toolStripComboBox_FindItem.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.toolStripComboBox_FindItem.Name = "toolStripComboBox_FindItem";
			this.toolStripComboBox_FindItem.Size = new System.Drawing.Size(121, 25);
			// 
			// toolStripTextBox_find
			// 
			this.toolStripTextBox_find.BackColor = System.Drawing.SystemColors.ActiveCaption;
			this.toolStripTextBox_find.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.toolStripTextBox_find.Name = "toolStripTextBox_find";
			this.toolStripTextBox_find.Size = new System.Drawing.Size(100, 25);
			this.toolStripTextBox_find.KeyDown += new System.Windows.Forms.KeyEventHandler(this.toolStripTextBox1_KeyDown);
			// 
			// toolStripBtn_FindData
			// 
			this.toolStripBtn_FindData.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.toolStripBtn_FindData.Image = ((System.Drawing.Image)(resources.GetObject("toolStripBtn_FindData.Image")));
			this.toolStripBtn_FindData.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripBtn_FindData.Name = "toolStripBtn_FindData";
			this.toolStripBtn_FindData.Size = new System.Drawing.Size(23, 22);
			this.toolStripBtn_FindData.Text = "Find";
			this.toolStripBtn_FindData.Click += new System.EventHandler(this.toolStripBtn_FindData_Click);
			// 
			// toolStripLabel_scale
			// 
			this.toolStripLabel_scale.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
			this.toolStripLabel_scale.Margin = new System.Windows.Forms.Padding(0, 1, 10, 2);
			this.toolStripLabel_scale.Name = "toolStripLabel_scale";
			this.toolStripLabel_scale.Size = new System.Drawing.Size(33, 22);
			this.toolStripLabel_scale.Text = "scale";
			// 
			// toolStripLabel_RedrawTime
			// 
			this.toolStripLabel_RedrawTime.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
			this.toolStripLabel_RedrawTime.Margin = new System.Windows.Forms.Padding(0, 1, 10, 2);
			this.toolStripLabel_RedrawTime.Name = "toolStripLabel_RedrawTime";
			this.toolStripLabel_RedrawTime.Size = new System.Drawing.Size(46, 22);
			this.toolStripLabel_RedrawTime.Text = "Redraw";
			// 
			// toolStripLabel_TotalSize
			// 
			this.toolStripLabel_TotalSize.Name = "toolStripLabel_TotalSize";
			this.toolStripLabel_TotalSize.Size = new System.Drawing.Size(89, 22);
			this.toolStripLabel_TotalSize.Text = "Total Size [m]: ";
			// 
			// toolStripLabel_MousePos
			// 
			this.toolStripLabel_MousePos.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
			this.toolStripLabel_MousePos.Name = "toolStripLabel_MousePos";
			this.toolStripLabel_MousePos.Size = new System.Drawing.Size(88, 22);
			this.toolStripLabel_MousePos.Text = "MousePos mm";
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// timer_progress
			// 
			this.timer_progress.Interval = 50;
			this.timer_progress.Tick += new System.EventHandler(this.timer_progress_Tick);
			// 
			// imageList1
			// 
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "flag.png");
			// 
			// contextMenu_GridRBtn
			// 
			this.contextMenu_GridRBtn.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_Find});
			this.contextMenu_GridRBtn.Name = "contextMenu_GridRBtn";
			this.contextMenu_GridRBtn.Size = new System.Drawing.Size(144, 26);
			this.contextMenu_GridRBtn.Text = "그리드뷰 우클릭";
			// 
			// ToolStripMenuItem_Find
			// 
			this.ToolStripMenuItem_Find.Name = "ToolStripMenuItem_Find";
			this.ToolStripMenuItem_Find.Size = new System.Drawing.Size(143, 22);
			this.ToolStripMenuItem_Find.Text = "찾기 [Ctrl+F]";
			this.ToolStripMenuItem_Find.Click += new System.EventHandler(this.ToolStripMenuItem_Find_Click);
			// 
			// contextMenuStrip_OnDraw
			// 
			this.contextMenuStrip_OnDraw.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_Flag,
            this.거리측정ToolStripMenuItem});
			this.contextMenuStrip_OnDraw.Name = "contextMenuStrip_OnDraw";
			this.contextMenuStrip_OnDraw.Size = new System.Drawing.Size(125, 48);
			// 
			// ToolStripMenuItem_Flag
			// 
			this.ToolStripMenuItem_Flag.Name = "ToolStripMenuItem_Flag";
			this.ToolStripMenuItem_Flag.Size = new System.Drawing.Size(124, 22);
			this.ToolStripMenuItem_Flag.Text = "Flag 지정";
			this.ToolStripMenuItem_Flag.Click += new System.EventHandler(this.ToolStripMenuItem_Flag_Click);
			// 
			// 거리측정ToolStripMenuItem
			// 
			this.거리측정ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.시작ToolStripMenuItem,
            this.종료ToolStripMenuItem,
            this.전체삭제ToolStripMenuItem});
			this.거리측정ToolStripMenuItem.Name = "거리측정ToolStripMenuItem";
			this.거리측정ToolStripMenuItem.Size = new System.Drawing.Size(124, 22);
			this.거리측정ToolStripMenuItem.Text = "거리측정";
			// 
			// 시작ToolStripMenuItem
			// 
			this.시작ToolStripMenuItem.Name = "시작ToolStripMenuItem";
			this.시작ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
			this.시작ToolStripMenuItem.Text = "시작";
			this.시작ToolStripMenuItem.Click += new System.EventHandler(this.시작ToolStripMenuItem_Click);
			// 
			// 종료ToolStripMenuItem
			// 
			this.종료ToolStripMenuItem.Name = "종료ToolStripMenuItem";
			this.종료ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
			this.종료ToolStripMenuItem.Text = "종료";
			this.종료ToolStripMenuItem.Click += new System.EventHandler(this.종료ToolStripMenuItem_Click);
			// 
			// 전체삭제ToolStripMenuItem
			// 
			this.전체삭제ToolStripMenuItem.Name = "전체삭제ToolStripMenuItem";
			this.전체삭제ToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
			this.전체삭제ToolStripMenuItem.Text = "전체 삭제";
			this.전체삭제ToolStripMenuItem.Click += new System.EventHandler(this.전체삭제ToolStripMenuItem_Click);
			// 
			// contextMenuStrip_Flag
			// 
			this.contextMenuStrip_Flag.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_FlagSave,
            this.ToolStripMenuItem_FlagLoad});
			this.contextMenuStrip_Flag.Name = "contextMenuStrip_Flag";
			this.contextMenuStrip_Flag.Size = new System.Drawing.Size(165, 48);
			// 
			// ToolStripMenuItem_FlagSave
			// 
			this.ToolStripMenuItem_FlagSave.Name = "ToolStripMenuItem_FlagSave";
			this.ToolStripMenuItem_FlagSave.Size = new System.Drawing.Size(164, 22);
			this.ToolStripMenuItem_FlagSave.Text = "Flag 파일로 저장";
			this.ToolStripMenuItem_FlagSave.Click += new System.EventHandler(this.ToolStripMenuItem_FlagSave_Click);
			// 
			// ToolStripMenuItem_FlagLoad
			// 
			this.ToolStripMenuItem_FlagLoad.Name = "ToolStripMenuItem_FlagLoad";
			this.ToolStripMenuItem_FlagLoad.Size = new System.Drawing.Size(164, 22);
			this.ToolStripMenuItem_FlagLoad.Text = "Flag 불러오기";
			this.ToolStripMenuItem_FlagLoad.Click += new System.EventHandler(this.ToolStripMenuItem_FlagLoad_Click);
			// 
			// imageList2
			// 
			this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
			this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList2.Images.SetKeyName(0, "Expand_Left.png");
			this.imageList2.Images.SetKeyName(1, "Expand_Right.png");
			this.imageList2.Images.SetKeyName(2, "Expand_up.png");
			this.imageList2.Images.SetKeyName(3, "Expand_down.png");
			// 
			// imageList3
			// 
			this.imageList3.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList3.ImageStream")));
			this.imageList3.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList3.Images.SetKeyName(0, "node_on.png");
			this.imageList3.Images.SetKeyName(1, "node_off.png");
			this.imageList3.Images.SetKeyName(2, "nodeDetail_on.png");
			this.imageList3.Images.SetKeyName(3, "nodeDetail_off.png");
			this.imageList3.Images.SetKeyName(4, "sid_on.png");
			this.imageList3.Images.SetKeyName(5, "sid_off.png");
			this.imageList3.Images.SetKeyName(6, "Speed_on.png");
			this.imageList3.Images.SetKeyName(7, "Speed_off.png");
			this.imageList3.Images.SetKeyName(8, "station_on.png");
			this.imageList3.Images.SetKeyName(9, "station_off.png");
			// 
			// UMapView
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Desktop;
			this.ClientSize = new System.Drawing.Size(1008, 696);
			this.Controls.Add(this.splitContainer1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "UMapView";
			this.Text = "Map View";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UMapView_FormClosing);
			this.Load += new System.EventHandler(this.Form1_Load);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			this.splitContainer1.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel1.PerformLayout();
			this.splitContainer2.Panel2.ResumeLayout(false);
			this.splitContainer2.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.splitContainer3.Panel1.ResumeLayout(false);
			this.splitContainer3.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
			this.splitContainer3.ResumeLayout(false);
			this.tabControl_Select_and_Flag.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabPage2.ResumeLayout(false);
			this.toolStrip3.ResumeLayout(false);
			this.toolStrip3.PerformLayout();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.tabControl_Object.ResumeLayout(false);
			this.tabPage_node.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_node)).EndInit();
			this.tabPage_station.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Station)).EndInit();
			this.tabPage_CID.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_CID)).EndInit();
			this.tabPage_Collision.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Collision)).EndInit();
			this.tabPage_link.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Link)).EndInit();
			this.tabPage_HID.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_HID)).EndInit();
			this.tabPage_FlowControl.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_FlowControl)).EndInit();
			this.tabPage_Teaching.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Teaching)).EndInit();
			this.tabPage_PathData.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_PathData)).EndInit();
			this.toolStrip2.ResumeLayout(false);
			this.toolStrip2.PerformLayout();
			this.contextMenu_GridRBtn.ResumeLayout(false);
			this.contextMenuStrip_OnDraw.ResumeLayout(false);
			this.contextMenuStrip_Flag.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStrip toolStrip2;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_find;
        private System.Windows.Forms.ToolStripButton toolStripBtn_MinMaxListWnd;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.ListView listView_CurSelInfo;
        private System.Windows.Forms.ToolStripButton toolStripBtn_VisibleZAxis;
        private System.Windows.Forms.ToolStripButton toolStripBtn_DrawCollisionInfo;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Timer timer_progress;
        private System.Windows.Forms.ToolStripButton toolStripBtn_HideStation;
        private System.Windows.Forms.ToolStripButton toolStripBtn_HideNode;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.TabControl tabControl_Object;
        private System.Windows.Forms.TabPage tabPage_node;
        private System.Windows.Forms.TabPage tabPage_station;
        private System.Windows.Forms.DataGridView dataGridView_node;
        private System.Windows.Forms.DataGridView dataGridView_Station;
        private System.Windows.Forms.TabPage tabPage_CID;
        private System.Windows.Forms.DataGridView dataGridView_CID;
        private System.Windows.Forms.TabPage tabPage_Collision;
        private System.Windows.Forms.DataGridView dataGridView_Collision;
        private System.Windows.Forms.TabPage tabPage_link;
        private System.Windows.Forms.DataGridView dataGridView_Link;
        private System.Windows.Forms.TabPage tabPage_HID;
        private System.Windows.Forms.DataGridView dataGridView_HID;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBox_FindItem;
        private System.Windows.Forms.ToolStripButton toolStripBtn_FindData;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_CurTabName;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_SelectedCellCnt;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.TabControl tabControl_Select_and_Flag;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.ListView listView_Selected;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_scale;
        private System.Windows.Forms.ToolStripButton toolStripButton_ShowStationID;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_RedrawTime;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_MousePos;
        private System.Windows.Forms.ContextMenuStrip contextMenu_GridRBtn;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Find;
        private System.Windows.Forms.ToolStripLabel toolStripLabel_TotalSize;
        private System.Windows.Forms.ListView listView_Flags;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_OnDraw;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Flag;
        private System.Windows.Forms.TabPage tabPage_FlowControl;
        private System.Windows.Forms.DataGridView dataGridView_FlowControl;
        private System.Windows.Forms.TabPage tabPage_Teaching;
        private System.Windows.Forms.DataGridView dataGridView_Teaching;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_Flag;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FlagSave;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FlagLoad;
        private System.Windows.Forms.Button button_wanring;
        private System.Windows.Forms.Button button_hideErr;
        private System.Windows.Forms.TextBox textBox_Err;
        private System.Windows.Forms.ListView listView_MapInfo;
        private System.Windows.Forms.ImageList imageList2;
        private System.Windows.Forms.ToolStripButton btnShowNodeDetail;
        private System.Windows.Forms.ToolStrip toolStrip3;
        private System.Windows.Forms.ToolStripButton toolStripBtn_MinMaxStatusWnd;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripBtn_FitWnd;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton1;
        private System.Windows.Forms.ToolStripMenuItem 분기노드추출ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem lookDown패턴검증ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 맵증속ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 증속수행ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 증속결과View에표시ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem 전체결과저장ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 선택된노드만저장ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 선택된노드원복ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 선택된노드증속값직접입력ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem 증속된값으로OcsLayout추출ToolStripMenuItem;
        private System.Windows.Forms.TabPage tabPage_PathData;
        private System.Windows.Forms.DataGridView dataGridView_PathData;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ImageList imageList3;
		private System.Windows.Forms.ToolStripMenuItem 거리측정ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem 시작ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem 종료ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem 전체삭제ToolStripMenuItem;
	}
}

