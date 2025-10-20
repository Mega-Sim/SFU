namespace LogViewer
{
    partial class ULogFrame
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ULogFrame));
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.btn_Expand = new System.Windows.Forms.ToolStripButton();
			this.textBox_CurTime = new System.Windows.Forms.ToolStripTextBox();
			this.textBox_Stat = new System.Windows.Forms.ToolStripTextBox();
			this.textBox_Speed = new System.Windows.Forms.ToolStripTextBox();
			this.textBox_Node = new System.Windows.Forms.ToolStripTextBox();
			this.tabControl_Info = new System.Windows.Forms.TabControl();
			this.tab_event = new System.Windows.Forms.TabPage();
			this.dataGridView_UserEvent = new System.Windows.Forms.DataGridView();
			this.tab_fileList = new System.Windows.Forms.TabPage();
			this.listView_file = new System.Windows.Forms.ListView();
			this.col_name = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_size = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tab_Comment = new System.Windows.Forms.TabPage();
			this.dataGridView_Comment = new System.Windows.Forms.DataGridView();
			this.col_comment = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_CommentLogFile = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_CommentRange = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_CommentAuthor = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_AmcRcvShowCols = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_AmcRcvOpenRange = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.col_AttachedFile = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.toolStrip2 = new System.Windows.Forms.ToolStrip();
			this.btn_CommentSave = new System.Windows.Forms.ToolStripButton();
			this.btn_Reload = new System.Windows.Forms.ToolStripButton();
			this.btn_CommentClear = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.btn_Add = new System.Windows.Forms.ToolStripButton();
			this.btn_Remove = new System.Windows.Forms.ToolStripButton();
			this.tabControl_Log = new System.Windows.Forms.TabControl();
			this.contextMenuStrip_TabCtrl = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
			this.contextMenuStrip_Files = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ToolStripMenuItem_OpenCurTime = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.탐색기에서열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.메모장에서열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.contextMenu_Event = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ToolStripMenuItem_FindInMapView_Station = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItem_FindInMapView_Node = new System.Windows.Forms.ToolStripMenuItem();
			this.imageList_Expand = new System.Windows.Forms.ImageList(this.components);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.tabControl_Info.SuspendLayout();
			this.tab_event.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_UserEvent)).BeginInit();
			this.tab_fileList.SuspendLayout();
			this.tab_Comment.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Comment)).BeginInit();
			this.toolStrip2.SuspendLayout();
			this.contextMenuStrip_TabCtrl.SuspendLayout();
			this.contextMenuStrip_Files.SuspendLayout();
			this.contextMenu_Event.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.BackColor = System.Drawing.Color.Transparent;
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.BackColor = System.Drawing.Color.Transparent;
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.BackColor = System.Drawing.SystemColors.Control;
			this.splitContainer1.Panel2.Controls.Add(this.tabControl_Log);
			this.splitContainer1.Size = new System.Drawing.Size(945, 547);
			this.splitContainer1.SplitterDistance = 460;
			this.splitContainer1.TabIndex = 0;
			// 
			// splitContainer2
			// 
			this.splitContainer2.BackColor = System.Drawing.Color.Transparent;
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.toolStrip1);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.tabControl_Info);
			this.splitContainer2.Size = new System.Drawing.Size(460, 547);
			this.splitContainer2.SplitterDistance = 28;
			this.splitContainer2.TabIndex = 0;
			// 
			// toolStrip1
			// 
			this.toolStrip1.BackColor = System.Drawing.Color.Gainsboro;
			this.toolStrip1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btn_Expand,
            this.textBox_CurTime,
            this.textBox_Stat,
            this.textBox_Speed,
            this.textBox_Node});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(460, 25);
			this.toolStrip1.TabIndex = 0;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// btn_Expand
			// 
			this.btn_Expand.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
			this.btn_Expand.BackColor = System.Drawing.Color.Transparent;
			this.btn_Expand.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btn_Expand.Image = ((System.Drawing.Image)(resources.GetObject("btn_Expand.Image")));
			this.btn_Expand.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Expand.Name = "btn_Expand";
			this.btn_Expand.Size = new System.Drawing.Size(23, 22);
			this.btn_Expand.Text = "toolStripButton1";
			this.btn_Expand.Click += new System.EventHandler(this.btn_Expand_Click);
			// 
			// textBox_CurTime
			// 
			this.textBox_CurTime.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.textBox_CurTime.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_CurTime.Name = "textBox_CurTime";
			this.textBox_CurTime.ReadOnly = true;
			this.textBox_CurTime.Size = new System.Drawing.Size(80, 25);
			this.textBox_CurTime.Text = "11:22:33.444";
			this.textBox_CurTime.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// textBox_Stat
			// 
			this.textBox_Stat.BackColor = System.Drawing.SystemColors.ControlLight;
			this.textBox_Stat.Name = "textBox_Stat";
			this.textBox_Stat.ReadOnly = true;
			this.textBox_Stat.Size = new System.Drawing.Size(30, 25);
			this.textBox_Stat.Text = "AG";
			this.textBox_Stat.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// textBox_Speed
			// 
			this.textBox_Speed.BackColor = System.Drawing.SystemColors.ControlLight;
			this.textBox_Speed.Font = new System.Drawing.Font("맑은 고딕", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Speed.Name = "textBox_Speed";
			this.textBox_Speed.ReadOnly = true;
			this.textBox_Speed.Size = new System.Drawing.Size(80, 25);
			this.textBox_Speed.Text = "1000.00 mm/s";
			this.textBox_Speed.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			// 
			// textBox_Node
			// 
			this.textBox_Node.BackColor = System.Drawing.SystemColors.ControlLight;
			this.textBox_Node.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Node.ForeColor = System.Drawing.Color.Blue;
			this.textBox_Node.Name = "textBox_Node";
			this.textBox_Node.ReadOnly = true;
			this.textBox_Node.Size = new System.Drawing.Size(160, 25);
			this.textBox_Node.Text = "N:123456(1234) S:12345";
			this.textBox_Node.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.textBox_Node.DoubleClick += new System.EventHandler(this.textBox_Node_Click);
			// 
			// tabControl_Info
			// 
			this.tabControl_Info.Controls.Add(this.tab_event);
			this.tabControl_Info.Controls.Add(this.tab_fileList);
			this.tabControl_Info.Controls.Add(this.tab_Comment);
			this.tabControl_Info.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl_Info.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.tabControl_Info.Location = new System.Drawing.Point(0, 0);
			this.tabControl_Info.Name = "tabControl_Info";
			this.tabControl_Info.SelectedIndex = 0;
			this.tabControl_Info.Size = new System.Drawing.Size(460, 515);
			this.tabControl_Info.TabIndex = 0;
			// 
			// tab_event
			// 
			this.tab_event.Controls.Add(this.dataGridView_UserEvent);
			this.tab_event.Location = new System.Drawing.Point(4, 24);
			this.tab_event.Name = "tab_event";
			this.tab_event.Padding = new System.Windows.Forms.Padding(3);
			this.tab_event.Size = new System.Drawing.Size(452, 487);
			this.tab_event.TabIndex = 1;
			this.tab_event.Text = "Event";
			this.tab_event.UseVisualStyleBackColor = true;
			// 
			// dataGridView_UserEvent
			// 
			this.dataGridView_UserEvent.AllowUserToAddRows = false;
			this.dataGridView_UserEvent.AllowUserToDeleteRows = false;
			this.dataGridView_UserEvent.AllowUserToResizeRows = false;
			this.dataGridView_UserEvent.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_UserEvent.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_UserEvent.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_UserEvent.Location = new System.Drawing.Point(3, 3);
			this.dataGridView_UserEvent.Name = "dataGridView_UserEvent";
			this.dataGridView_UserEvent.ReadOnly = true;
			this.dataGridView_UserEvent.RowHeadersWidth = 46;
			this.dataGridView_UserEvent.RowTemplate.Height = 23;
			this.dataGridView_UserEvent.ShowCellErrors = false;
			this.dataGridView_UserEvent.ShowCellToolTips = false;
			this.dataGridView_UserEvent.ShowEditingIcon = false;
			this.dataGridView_UserEvent.ShowRowErrors = false;
			this.dataGridView_UserEvent.Size = new System.Drawing.Size(446, 481);
			this.dataGridView_UserEvent.TabIndex = 14;
			this.dataGridView_UserEvent.CurrentCellChanged += new System.EventHandler(this.dataGridView_UserEvent_CurrentCellChanged);
			this.dataGridView_UserEvent.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView_UserEvent_KeyDown);
			this.dataGridView_UserEvent.Leave += new System.EventHandler(this.dataGridView_UserEvent_Leave);
			this.dataGridView_UserEvent.MouseClick += new System.Windows.Forms.MouseEventHandler(this.dataGridView_UserEvent_MouseClick);
			// 
			// tab_fileList
			// 
			this.tab_fileList.Controls.Add(this.listView_file);
			this.tab_fileList.Location = new System.Drawing.Point(4, 24);
			this.tab_fileList.Name = "tab_fileList";
			this.tab_fileList.Padding = new System.Windows.Forms.Padding(3);
			this.tab_fileList.Size = new System.Drawing.Size(404, 487);
			this.tab_fileList.TabIndex = 0;
			this.tab_fileList.Text = "File List";
			this.tab_fileList.UseVisualStyleBackColor = true;
			// 
			// listView_file
			// 
			this.listView_file.BackColor = System.Drawing.Color.Gainsboro;
			this.listView_file.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.listView_file.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_name,
            this.col_size});
			this.listView_file.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_file.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_file.FullRowSelect = true;
			this.listView_file.GridLines = true;
			this.listView_file.Location = new System.Drawing.Point(3, 3);
			this.listView_file.Name = "listView_file";
			this.listView_file.Size = new System.Drawing.Size(398, 481);
			this.listView_file.TabIndex = 1;
			this.listView_file.UseCompatibleStateImageBehavior = false;
			this.listView_file.View = System.Windows.Forms.View.Details;
			this.listView_file.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_file_KeyDown);
			this.listView_file.MouseClick += new System.Windows.Forms.MouseEventHandler(this.listView_file_MouseClick);
			this.listView_file.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView_file_MouseDoubleClick);
			// 
			// col_name
			// 
			this.col_name.Text = "FileName";
			this.col_name.Width = 271;
			// 
			// col_size
			// 
			this.col_size.Text = "Size [Byte]";
			this.col_size.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this.col_size.Width = 105;
			// 
			// tab_Comment
			// 
			this.tab_Comment.Controls.Add(this.dataGridView_Comment);
			this.tab_Comment.Controls.Add(this.toolStrip2);
			this.tab_Comment.Location = new System.Drawing.Point(4, 24);
			this.tab_Comment.Name = "tab_Comment";
			this.tab_Comment.Size = new System.Drawing.Size(404, 487);
			this.tab_Comment.TabIndex = 3;
			this.tab_Comment.Text = "Comment";
			this.tab_Comment.UseVisualStyleBackColor = true;
			// 
			// dataGridView_Comment
			// 
			this.dataGridView_Comment.AllowUserToAddRows = false;
			this.dataGridView_Comment.AllowUserToDeleteRows = false;
			this.dataGridView_Comment.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Comment.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.col_comment,
            this.col_CommentLogFile,
            this.col_CommentRange,
            this.col_CommentAuthor,
            this.col_AmcRcvShowCols,
            this.col_AmcRcvOpenRange,
            this.col_AttachedFile});
			this.dataGridView_Comment.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Comment.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView_Comment.Location = new System.Drawing.Point(0, 25);
			this.dataGridView_Comment.Name = "dataGridView_Comment";
			this.dataGridView_Comment.RowTemplate.Height = 23;
			this.dataGridView_Comment.Size = new System.Drawing.Size(404, 462);
			this.dataGridView_Comment.TabIndex = 2;
			this.dataGridView_Comment.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridView_Comment_CellMouseDoubleClick);
			// 
			// col_comment
			// 
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
			this.col_comment.DefaultCellStyle = dataGridViewCellStyle1;
			this.col_comment.HeaderText = "Comment";
			this.col_comment.MinimumWidth = 200;
			this.col_comment.Name = "col_comment";
			this.col_comment.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.col_comment.Width = 300;
			// 
			// col_CommentLogFile
			// 
			this.col_CommentLogFile.HeaderText = "File";
			this.col_CommentLogFile.Name = "col_CommentLogFile";
			this.col_CommentLogFile.ReadOnly = true;
			// 
			// col_CommentRange
			// 
			this.col_CommentRange.HeaderText = "Selected Range";
			this.col_CommentRange.Name = "col_CommentRange";
			this.col_CommentRange.ReadOnly = true;
			// 
			// col_CommentAuthor
			// 
			this.col_CommentAuthor.HeaderText = "Author";
			this.col_CommentAuthor.Name = "col_CommentAuthor";
			// 
			// col_AmcRcvShowCols
			// 
			this.col_AmcRcvShowCols.HeaderText = "AmcShowCols";
			this.col_AmcRcvShowCols.Name = "col_AmcRcvShowCols";
			this.col_AmcRcvShowCols.Visible = false;
			this.col_AmcRcvShowCols.Width = 5;
			// 
			// col_AmcRcvOpenRange
			// 
			this.col_AmcRcvOpenRange.HeaderText = "AmcRecvOpenRange";
			this.col_AmcRcvOpenRange.Name = "col_AmcRcvOpenRange";
			this.col_AmcRcvOpenRange.Visible = false;
			// 
			// col_AttachedFile
			// 
			this.col_AttachedFile.HeaderText = "Attached File";
			this.col_AttachedFile.Name = "col_AttachedFile";
			// 
			// toolStrip2
			// 
			this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btn_CommentSave,
            this.btn_Reload,
            this.btn_CommentClear,
            this.toolStripSeparator2,
            this.btn_Add,
            this.btn_Remove});
			this.toolStrip2.Location = new System.Drawing.Point(0, 0);
			this.toolStrip2.Name = "toolStrip2";
			this.toolStrip2.Size = new System.Drawing.Size(404, 25);
			this.toolStrip2.TabIndex = 1;
			this.toolStrip2.Text = "toolStrip2";
			// 
			// btn_CommentSave
			// 
			this.btn_CommentSave.Image = ((System.Drawing.Image)(resources.GetObject("btn_CommentSave.Image")));
			this.btn_CommentSave.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_CommentSave.Name = "btn_CommentSave";
			this.btn_CommentSave.Size = new System.Drawing.Size(52, 22);
			this.btn_CommentSave.Text = "Save";
			this.btn_CommentSave.ToolTipText = "Comment 저장";
			this.btn_CommentSave.Click += new System.EventHandler(this.btn_CommentSave_Click);
			// 
			// btn_Reload
			// 
			this.btn_Reload.Image = ((System.Drawing.Image)(resources.GetObject("btn_Reload.Image")));
			this.btn_Reload.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Reload.Name = "btn_Reload";
			this.btn_Reload.Size = new System.Drawing.Size(63, 22);
			this.btn_Reload.Text = "Reload";
			this.btn_Reload.Click += new System.EventHandler(this.btn_Reload_Click);
			// 
			// btn_CommentClear
			// 
			this.btn_CommentClear.Image = ((System.Drawing.Image)(resources.GetObject("btn_CommentClear.Image")));
			this.btn_CommentClear.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_CommentClear.Name = "btn_CommentClear";
			this.btn_CommentClear.Size = new System.Drawing.Size(54, 22);
			this.btn_CommentClear.Text = "Clear";
			this.btn_CommentClear.ToolTipText = "Comment 삭제";
			this.btn_CommentClear.Click += new System.EventHandler(this.btn_CommentClear_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			// 
			// btn_Add
			// 
			this.btn_Add.BackColor = System.Drawing.Color.Transparent;
			this.btn_Add.Image = ((System.Drawing.Image)(resources.GetObject("btn_Add.Image")));
			this.btn_Add.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Add.Name = "btn_Add";
			this.btn_Add.Size = new System.Drawing.Size(49, 22);
			this.btn_Add.Text = "Add";
			this.btn_Add.Click += new System.EventHandler(this.btn_Add_Click);
			// 
			// btn_Remove
			// 
			this.btn_Remove.BackColor = System.Drawing.Color.Transparent;
			this.btn_Remove.Image = ((System.Drawing.Image)(resources.GetObject("btn_Remove.Image")));
			this.btn_Remove.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Remove.Name = "btn_Remove";
			this.btn_Remove.Size = new System.Drawing.Size(70, 22);
			this.btn_Remove.Text = "Remove";
			this.btn_Remove.Click += new System.EventHandler(this.btn_Remove_Click);
			// 
			// tabControl_Log
			// 
			this.tabControl_Log.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl_Log.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.tabControl_Log.Location = new System.Drawing.Point(0, 0);
			this.tabControl_Log.Name = "tabControl_Log";
			this.tabControl_Log.SelectedIndex = 0;
			this.tabControl_Log.Size = new System.Drawing.Size(481, 547);
			this.tabControl_Log.TabIndex = 0;
			this.tabControl_Log.MouseClick += new System.Windows.Forms.MouseEventHandler(this.tabControl_Log_MouseClick);
			// 
			// contextMenuStrip_TabCtrl
			// 
			this.contextMenuStrip_TabCtrl.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1});
			this.contextMenuStrip_TabCtrl.Name = "contextMenuStrip1";
			this.contextMenuStrip_TabCtrl.Size = new System.Drawing.Size(115, 26);
			// 
			// toolStripMenuItem1
			// 
			this.toolStripMenuItem1.Name = "toolStripMenuItem1";
			this.toolStripMenuItem1.Size = new System.Drawing.Size(114, 22);
			this.toolStripMenuItem1.Text = "탭 닫기";
			this.toolStripMenuItem1.Click += new System.EventHandler(this.toolStripMenuItem1_Click);
			// 
			// contextMenuStrip_Files
			// 
			this.contextMenuStrip_Files.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_OpenCurTime,
            this.toolStripSeparator1,
            this.탐색기에서열기ToolStripMenuItem,
            this.메모장에서열기ToolStripMenuItem});
			this.contextMenuStrip_Files.Name = "contextMenuStrip_Files";
			this.contextMenuStrip_Files.Size = new System.Drawing.Size(191, 76);
			// 
			// ToolStripMenuItem_OpenCurTime
			// 
			this.ToolStripMenuItem_OpenCurTime.Name = "ToolStripMenuItem_OpenCurTime";
			this.ToolStripMenuItem_OpenCurTime.Size = new System.Drawing.Size(190, 22);
			this.ToolStripMenuItem_OpenCurTime.Text = "선택된 시간으로 열기";
			this.ToolStripMenuItem_OpenCurTime.Click += new System.EventHandler(this.ToolStripMenuItem_OpenCurTime_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(187, 6);
			// 
			// 탐색기에서열기ToolStripMenuItem
			// 
			this.탐색기에서열기ToolStripMenuItem.Name = "탐색기에서열기ToolStripMenuItem";
			this.탐색기에서열기ToolStripMenuItem.Size = new System.Drawing.Size(190, 22);
			this.탐색기에서열기ToolStripMenuItem.Text = "탐색기에서 열기";
			this.탐색기에서열기ToolStripMenuItem.Click += new System.EventHandler(this.탐색기에서열기ToolStripMenuItem_Click);
			// 
			// 메모장에서열기ToolStripMenuItem
			// 
			this.메모장에서열기ToolStripMenuItem.Name = "메모장에서열기ToolStripMenuItem";
			this.메모장에서열기ToolStripMenuItem.Size = new System.Drawing.Size(190, 22);
			this.메모장에서열기ToolStripMenuItem.Text = "메모장에서 열기";
			this.메모장에서열기ToolStripMenuItem.Click += new System.EventHandler(this.메모장에서열기ToolStripMenuItem_Click);
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// contextMenu_Event
			// 
			this.contextMenu_Event.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_FindInMapView_Station,
            this.ToolStripMenuItem_FindInMapView_Node});
			this.contextMenu_Event.Name = "contextMenu_Event";
			this.contextMenu_Event.Size = new System.Drawing.Size(221, 48);
			// 
			// ToolStripMenuItem_FindInMapView_Station
			// 
			this.ToolStripMenuItem_FindInMapView_Station.Name = "ToolStripMenuItem_FindInMapView_Station";
			this.ToolStripMenuItem_FindInMapView_Station.Size = new System.Drawing.Size(220, 22);
			this.ToolStripMenuItem_FindInMapView_Station.Text = "선택된 Station 맵에서 보기";
			this.ToolStripMenuItem_FindInMapView_Station.Click += new System.EventHandler(this.ToolStripMenuItem_FindInMapView_Station_Click);
			// 
			// ToolStripMenuItem_FindInMapView_Node
			// 
			this.ToolStripMenuItem_FindInMapView_Node.Name = "ToolStripMenuItem_FindInMapView_Node";
			this.ToolStripMenuItem_FindInMapView_Node.Size = new System.Drawing.Size(220, 22);
			this.ToolStripMenuItem_FindInMapView_Node.Text = "선택된 Node 맵에서 보기";
			this.ToolStripMenuItem_FindInMapView_Node.Click += new System.EventHandler(this.ToolStripMenuItem_FindInMapView_Node_Click);
			// 
			// imageList_Expand
			// 
			this.imageList_Expand.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList_Expand.ImageStream")));
			this.imageList_Expand.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList_Expand.Images.SetKeyName(0, "Expand_Left.png");
			this.imageList_Expand.Images.SetKeyName(1, "Expand_Right.png");
			// 
			// ULogFrame
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(945, 547);
			this.Controls.Add(this.splitContainer1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "ULogFrame";
			this.Text = "ULogFrame";
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel1.PerformLayout();
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.tabControl_Info.ResumeLayout(false);
			this.tab_event.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_UserEvent)).EndInit();
			this.tab_fileList.ResumeLayout(false);
			this.tab_Comment.ResumeLayout(false);
			this.tab_Comment.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Comment)).EndInit();
			this.toolStrip2.ResumeLayout(false);
			this.toolStrip2.PerformLayout();
			this.contextMenuStrip_TabCtrl.ResumeLayout(false);
			this.contextMenuStrip_Files.ResumeLayout(false);
			this.contextMenu_Event.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btn_Expand;
        private System.Windows.Forms.TabPage tab_fileList;
        private System.Windows.Forms.ListView listView_file;
        private System.Windows.Forms.ColumnHeader col_size;
        private System.Windows.Forms.ColumnHeader col_name;
        private System.Windows.Forms.TabPage tab_event;
        public System.Windows.Forms.TabControl tabControl_Log;
        public System.Windows.Forms.DataGridView dataGridView_UserEvent;
        private System.Windows.Forms.ToolStripTextBox textBox_CurTime;
        private System.Windows.Forms.ToolStripTextBox textBox_Stat;
        private System.Windows.Forms.ToolStripTextBox textBox_Speed;
        private System.Windows.Forms.ToolStripTextBox textBox_Node;
        private System.Windows.Forms.TabPage tab_Comment;
        private System.Windows.Forms.DataGridView dataGridView_Comment;
        private System.Windows.Forms.ToolStrip toolStrip2;
        private System.Windows.Forms.ToolStripButton btn_CommentSave;
        private System.Windows.Forms.ToolStripButton btn_CommentClear;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_TabCtrl;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_Files;
        private System.Windows.Forms.ToolStripMenuItem 탐색기에서열기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 메모장에서열기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_OpenCurTime;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btn_Add;
        private System.Windows.Forms.ToolStripButton btn_Remove;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ToolStripButton btn_Reload;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_comment;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_CommentLogFile;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_CommentRange;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_CommentAuthor;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_AmcRcvShowCols;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_AmcRcvOpenRange;
        private System.Windows.Forms.DataGridViewTextBoxColumn col_AttachedFile;
        private System.Windows.Forms.ContextMenuStrip contextMenu_Event;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FindInMapView_Node;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FindInMapView_Station;
        private System.Windows.Forms.ImageList imageList_Expand;
		public System.Windows.Forms.TabControl tabControl_Info;
	}
}