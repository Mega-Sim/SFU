namespace LogViewer
{
    partial class UAmcRecvTable
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            DestroySelf();
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UAmcRecvTable));
			this.panel1 = new System.Windows.Forms.Panel();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.dataGridView1 = new System.Windows.Forms.DataGridView();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.btn_HeaderVisible = new System.Windows.Forms.ToolStripButton();
			this.btn_TimeLine = new System.Windows.Forms.ToolStripButton();
			this.btn_ExportExcel = new System.Windows.Forms.ToolStripButton();
			this.btn_GridSetting = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.btn_Find = new System.Windows.Forms.ToolStripButton();
			this.btn_Expand = new System.Windows.Forms.ToolStripButton();
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this.label_OrgTime = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_SelectedTime = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_SpanTime = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_Sum = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_SelectedItemCnt = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_Path2 = new System.Windows.Forms.ToolStripStatusLabel();
			this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.틀고정ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItem_FixCol = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItem_UnfixCol = new System.Windows.Forms.ToolStripMenuItem();
			this.ToolStripMenuItem_HideCols = new System.Windows.Forms.ToolStripMenuItem();
			this.찾기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.색ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellRed = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellYellow = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellGreen = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellPurple = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellBlue = new System.Windows.Forms.ToolStripMenuItem();
			this.menu_cellInit = new System.Windows.Forms.ToolStripMenuItem();
			this.imageList_Expand = new System.Windows.Forms.ImageList(this.components);
			this.menu_cellWhite = new System.Windows.Forms.ToolStripMenuItem();
			this.panel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
			this.toolStrip1.SuspendLayout();
			this.statusStrip1.SuspendLayout();
			this.contextMenuStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.splitContainer1);
			this.panel1.Controls.Add(this.statusStrip1);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(1119, 585);
			this.panel1.TabIndex = 2;
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer1.IsSplitterFixed = true;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.dataGridView1);
			this.splitContainer1.Panel1.Controls.Add(this.toolStrip1);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Paint += new System.Windows.Forms.PaintEventHandler(this.splitContainer1_Panel2_Paint);
			this.splitContainer1.Size = new System.Drawing.Size(1119, 563);
			this.splitContainer1.SplitterDistance = 776;
			this.splitContainer1.TabIndex = 1;
			// 
			// dataGridView1
			// 
			this.dataGridView1.AllowUserToAddRows = false;
			this.dataGridView1.AllowUserToDeleteRows = false;
			this.dataGridView1.AllowUserToResizeColumns = false;
			this.dataGridView1.AllowUserToResizeRows = false;
			this.dataGridView1.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView1.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.dataGridView1.Location = new System.Drawing.Point(0, 25);
			this.dataGridView1.Name = "dataGridView1";
			this.dataGridView1.RowHeadersWidth = 60;
			this.dataGridView1.RowTemplate.Height = 23;
			this.dataGridView1.ShowCellErrors = false;
			this.dataGridView1.ShowCellToolTips = false;
			this.dataGridView1.ShowEditingIcon = false;
			this.dataGridView1.Size = new System.Drawing.Size(776, 538);
			this.dataGridView1.TabIndex = 4;
			this.dataGridView1.ColumnDividerWidthChanged += new System.Windows.Forms.DataGridViewColumnEventHandler(this.dataGridView1_ColumnDividerWidthChanged);
			this.dataGridView1.CurrentCellChanged += new System.EventHandler(this.dataGridView1_CurrentCellChanged);
			this.dataGridView1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView1_KeyDown);
			this.dataGridView1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView1_MouseDown);
			// 
			// toolStrip1
			// 
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btn_HeaderVisible,
            this.btn_TimeLine,
            this.btn_ExportExcel,
            this.btn_GridSetting,
            this.toolStripSeparator1,
            this.btn_Find,
            this.btn_Expand});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(776, 25);
			this.toolStrip1.TabIndex = 5;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// btn_HeaderVisible
			// 
			this.btn_HeaderVisible.Enabled = false;
			this.btn_HeaderVisible.Image = ((System.Drawing.Image)(resources.GetObject("btn_HeaderVisible.Image")));
			this.btn_HeaderVisible.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_HeaderVisible.Name = "btn_HeaderVisible";
			this.btn_HeaderVisible.Size = new System.Drawing.Size(104, 22);
			this.btn_HeaderVisible.Text = "Header Visible";
			this.btn_HeaderVisible.ToolTipText = "헤더 보이기/ 감추기";
			this.btn_HeaderVisible.Visible = false;
			this.btn_HeaderVisible.Click += new System.EventHandler(this.toolStripBtn_Header_Click);
			// 
			// btn_TimeLine
			// 
			this.btn_TimeLine.Image = ((System.Drawing.Image)(resources.GetObject("btn_TimeLine.Image")));
			this.btn_TimeLine.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_TimeLine.Name = "btn_TimeLine";
			this.btn_TimeLine.Size = new System.Drawing.Size(79, 22);
			this.btn_TimeLine.Text = "Time Line";
			this.btn_TimeLine.Click += new System.EventHandler(this.toolStripBtn_TimeLine_Click);
			// 
			// btn_ExportExcel
			// 
			this.btn_ExportExcel.Image = ((System.Drawing.Image)(resources.GetObject("btn_ExportExcel.Image")));
			this.btn_ExportExcel.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_ExportExcel.Name = "btn_ExportExcel";
			this.btn_ExportExcel.Size = new System.Drawing.Size(92, 22);
			this.btn_ExportExcel.Text = "Export Excel";
			this.btn_ExportExcel.Click += new System.EventHandler(this.toolStripBtn_ExportExcel_Click);
			// 
			// btn_GridSetting
			// 
			this.btn_GridSetting.Image = ((System.Drawing.Image)(resources.GetObject("btn_GridSetting.Image")));
			this.btn_GridSetting.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_GridSetting.Name = "btn_GridSetting";
			this.btn_GridSetting.Size = new System.Drawing.Size(91, 22);
			this.btn_GridSetting.Text = "Grid Setting";
			this.btn_GridSetting.Click += new System.EventHandler(this.toolStripBtn_GridSetting_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
			// 
			// btn_Find
			// 
			this.btn_Find.BackColor = System.Drawing.Color.Silver;
			this.btn_Find.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.btn_Find.Image = ((System.Drawing.Image)(resources.GetObject("btn_Find.Image")));
			this.btn_Find.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Find.Name = "btn_Find";
			this.btn_Find.Size = new System.Drawing.Size(34, 22);
			this.btn_Find.Tag = "";
			this.btn_Find.Text = "Find";
			this.btn_Find.Click += new System.EventHandler(this.btn_Find_Click);
			// 
			// btn_Expand
			// 
			this.btn_Expand.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
			this.btn_Expand.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btn_Expand.Image = ((System.Drawing.Image)(resources.GetObject("btn_Expand.Image")));
			this.btn_Expand.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Expand.Name = "btn_Expand";
			this.btn_Expand.Size = new System.Drawing.Size(23, 22);
			this.btn_Expand.Text = "toolStripButton1";
			this.btn_Expand.Click += new System.EventHandler(this.button_ShowHideCols_Click);
			// 
			// statusStrip1
			// 
			this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.label_OrgTime,
            this.label_SelectedTime,
            this.label_SpanTime,
            this.label_Sum,
            this.label_SelectedItemCnt,
            this.label_Path2});
			this.statusStrip1.Location = new System.Drawing.Point(0, 563);
			this.statusStrip1.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(1119, 22);
			this.statusStrip1.TabIndex = 0;
			this.statusStrip1.Text = "statusStrip1";
			// 
			// label_OrgTime
			// 
			this.label_OrgTime.BackColor = System.Drawing.Color.Gainsboro;
			this.label_OrgTime.Margin = new System.Windows.Forms.Padding(10, 3, 0, 2);
			this.label_OrgTime.Name = "label_OrgTime";
			this.label_OrgTime.Size = new System.Drawing.Size(57, 17);
			this.label_OrgTime.Text = "Org Time";
			// 
			// label_SelectedTime
			// 
			this.label_SelectedTime.BackColor = System.Drawing.Color.Gainsboro;
			this.label_SelectedTime.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label_SelectedTime.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.label_SelectedTime.Name = "label_SelectedTime";
			this.label_SelectedTime.Size = new System.Drawing.Size(98, 22);
			this.label_SelectedTime.Text = "Selected Time: ";
			// 
			// label_SpanTime
			// 
			this.label_SpanTime.BackColor = System.Drawing.Color.Gainsboro;
			this.label_SpanTime.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label_SpanTime.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.label_SpanTime.Name = "label_SpanTime";
			this.label_SpanTime.Size = new System.Drawing.Size(79, 22);
			this.label_SpanTime.Text = "Span Time: ";
			// 
			// label_Sum
			// 
			this.label_Sum.BackColor = System.Drawing.Color.Gainsboro;
			this.label_Sum.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label_Sum.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.label_Sum.Name = "label_Sum";
			this.label_Sum.Size = new System.Drawing.Size(42, 22);
			this.label_Sum.Text = "Sum: ";
			// 
			// label_SelectedItemCnt
			// 
			this.label_SelectedItemCnt.BackColor = System.Drawing.Color.Gainsboro;
			this.label_SelectedItemCnt.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label_SelectedItemCnt.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.label_SelectedItemCnt.Name = "label_SelectedItemCnt";
			this.label_SelectedItemCnt.Size = new System.Drawing.Size(121, 22);
			this.label_SelectedItemCnt.Text = "Selected Item Cnt: ";
			// 
			// label_Path2
			// 
			this.label_Path2.BackColor = System.Drawing.Color.Gainsboro;
			this.label_Path2.Margin = new System.Windows.Forms.Padding(10, 3, 0, 2);
			this.label_Path2.Name = "label_Path2";
			this.label_Path2.Size = new System.Drawing.Size(31, 17);
			this.label_Path2.Text = "Path";
			this.label_Path2.DoubleClick += new System.EventHandler(this.label_Path2_DoubleClick);
			// 
			// contextMenuStrip1
			// 
			this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.틀고정ToolStripMenuItem,
            this.ToolStripMenuItem_HideCols,
            this.찾기ToolStripMenuItem,
            this.색ToolStripMenuItem});
			this.contextMenuStrip1.Name = "contextMenuStrip1";
			this.contextMenuStrip1.Size = new System.Drawing.Size(167, 114);
			// 
			// 틀고정ToolStripMenuItem
			// 
			this.틀고정ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_FixCol,
            this.ToolStripMenuItem_UnfixCol});
			this.틀고정ToolStripMenuItem.Name = "틀고정ToolStripMenuItem";
			this.틀고정ToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
			this.틀고정ToolStripMenuItem.Text = "틀 고정";
			// 
			// ToolStripMenuItem_FixCol
			// 
			this.ToolStripMenuItem_FixCol.Name = "ToolStripMenuItem_FixCol";
			this.ToolStripMenuItem_FixCol.Size = new System.Drawing.Size(98, 22);
			this.ToolStripMenuItem_FixCol.Text = "설정";
			this.ToolStripMenuItem_FixCol.Click += new System.EventHandler(this.ToolStripMenuItem_FixCol_Click);
			// 
			// ToolStripMenuItem_UnfixCol
			// 
			this.ToolStripMenuItem_UnfixCol.Name = "ToolStripMenuItem_UnfixCol";
			this.ToolStripMenuItem_UnfixCol.Size = new System.Drawing.Size(98, 22);
			this.ToolStripMenuItem_UnfixCol.Text = "해제";
			this.ToolStripMenuItem_UnfixCol.Click += new System.EventHandler(this.ToolStripMenuItem_UnfixCol_Click);
			// 
			// ToolStripMenuItem_HideCols
			// 
			this.ToolStripMenuItem_HideCols.Name = "ToolStripMenuItem_HideCols";
			this.ToolStripMenuItem_HideCols.Size = new System.Drawing.Size(166, 22);
			this.ToolStripMenuItem_HideCols.Text = "선택된 열 숨기기";
			this.ToolStripMenuItem_HideCols.Click += new System.EventHandler(this.ToolStripMenuItem_HideCols_Click);
			// 
			// 찾기ToolStripMenuItem
			// 
			this.찾기ToolStripMenuItem.Name = "찾기ToolStripMenuItem";
			this.찾기ToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
			this.찾기ToolStripMenuItem.Text = "찾기";
			this.찾기ToolStripMenuItem.Click += new System.EventHandler(this.찾기ToolStripMenuItem_Click);
			// 
			// 색ToolStripMenuItem
			// 
			this.색ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_cellRed,
            this.menu_cellYellow,
            this.menu_cellGreen,
            this.menu_cellWhite,
            this.menu_cellPurple,
            this.menu_cellBlue,
            this.menu_cellInit});
			this.색ToolStripMenuItem.Name = "색ToolStripMenuItem";
			this.색ToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
			this.색ToolStripMenuItem.Text = "색 지정";
			// 
			// menu_cellRed
			// 
			this.menu_cellRed.BackColor = System.Drawing.Color.Red;
			this.menu_cellRed.Name = "menu_cellRed";
			this.menu_cellRed.Size = new System.Drawing.Size(152, 22);
			this.menu_cellRed.Text = "RED";
			this.menu_cellRed.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// menu_cellYellow
			// 
			this.menu_cellYellow.BackColor = System.Drawing.Color.Yellow;
			this.menu_cellYellow.Name = "menu_cellYellow";
			this.menu_cellYellow.Size = new System.Drawing.Size(152, 22);
			this.menu_cellYellow.Text = "YELLOW";
			this.menu_cellYellow.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// menu_cellGreen
			// 
			this.menu_cellGreen.BackColor = System.Drawing.Color.Lime;
			this.menu_cellGreen.Name = "menu_cellGreen";
			this.menu_cellGreen.Size = new System.Drawing.Size(152, 22);
			this.menu_cellGreen.Text = "GREEN";
			this.menu_cellGreen.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// menu_cellPurple
			// 
			this.menu_cellPurple.BackColor = System.Drawing.Color.Fuchsia;
			this.menu_cellPurple.Name = "menu_cellPurple";
			this.menu_cellPurple.Size = new System.Drawing.Size(152, 22);
			this.menu_cellPurple.Text = "PURPLE";
			this.menu_cellPurple.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// menu_cellBlue
			// 
			this.menu_cellBlue.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
			this.menu_cellBlue.Name = "menu_cellBlue";
			this.menu_cellBlue.Size = new System.Drawing.Size(152, 22);
			this.menu_cellBlue.Text = "BLUE";
			this.menu_cellBlue.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// menu_cellInit
			// 
			this.menu_cellInit.Name = "menu_cellInit";
			this.menu_cellInit.Size = new System.Drawing.Size(152, 22);
			this.menu_cellInit.Text = "전체 초기화";
			this.menu_cellInit.Click += new System.EventHandler(this.menu_cellInit_Click);
			// 
			// imageList_Expand
			// 
			this.imageList_Expand.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList_Expand.ImageStream")));
			this.imageList_Expand.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList_Expand.Images.SetKeyName(0, "Expand_Left.png");
			this.imageList_Expand.Images.SetKeyName(1, "Expand_Right.png");
			// 
			// menu_cellWhite
			// 
			this.menu_cellWhite.BackColor = System.Drawing.Color.White;
			this.menu_cellWhite.Name = "menu_cellWhite";
			this.menu_cellWhite.Size = new System.Drawing.Size(152, 22);
			this.menu_cellWhite.Text = "WHITE";
			this.menu_cellWhite.Click += new System.EventHandler(this.menu_cellColor_Click);
			// 
			// UAmcRecvTable
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1119, 585);
			this.Controls.Add(this.panel1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "UAmcRecvTable";
			this.Text = " ";
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.statusStrip1.ResumeLayout(false);
			this.statusStrip1.PerformLayout();
			this.contextMenuStrip1.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        public System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 틀고정ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FixCol;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_UnfixCol;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_HideCols;
        private System.Windows.Forms.ToolStripStatusLabel label_SelectedTime;
        private System.Windows.Forms.ToolStripStatusLabel label_SpanTime;
        private System.Windows.Forms.ToolStripStatusLabel label_Sum;
        private System.Windows.Forms.ToolStripStatusLabel label_SelectedItemCnt;
        private System.Windows.Forms.ToolStripStatusLabel label_OrgTime;
        private System.Windows.Forms.ToolStripStatusLabel label_Path2;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btn_HeaderVisible;
        private System.Windows.Forms.ToolStripButton btn_TimeLine;
        private System.Windows.Forms.ToolStripButton btn_ExportExcel;
        private System.Windows.Forms.ToolStripButton btn_GridSetting;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btn_Find;
        private System.Windows.Forms.ToolStripButton btn_Expand;
        private System.Windows.Forms.ToolStripMenuItem 찾기ToolStripMenuItem;
        private System.Windows.Forms.ImageList imageList_Expand;
		private System.Windows.Forms.ToolStripMenuItem 색ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem menu_cellRed;
		private System.Windows.Forms.ToolStripMenuItem menu_cellYellow;
		private System.Windows.Forms.ToolStripMenuItem menu_cellGreen;
		private System.Windows.Forms.ToolStripMenuItem menu_cellPurple;
		private System.Windows.Forms.ToolStripMenuItem menu_cellBlue;
		private System.Windows.Forms.ToolStripMenuItem menu_cellInit;
		private System.Windows.Forms.ToolStripMenuItem menu_cellWhite;
	}
}