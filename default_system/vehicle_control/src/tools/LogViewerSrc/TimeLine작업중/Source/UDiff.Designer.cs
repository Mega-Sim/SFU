namespace Diff
{
    partial class UDiff
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UDiff));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.label3 = new System.Windows.Forms.Label();
			this.checkBox_INIComp = new System.Windows.Forms.CheckBox();
			this.btn_CompWinmerge = new System.Windows.Forms.Button();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.btn_usage = new System.Windows.Forms.Button();
			this.btn_Reset = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.textBox_FileFilter = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.btn_run = new System.Windows.Forms.Button();
			this.btn_Gijun = new System.Windows.Forms.Button();
			this.btn_load = new System.Windows.Forms.Button();
			this.listView_fileList = new System.Windows.Forms.ListView();
			this.col_idx = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_diff = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_path = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.splitContainer3 = new System.Windows.Forms.SplitContainer();
			this.listView_org = new System.Windows.Forms.ListView();
			this.col_line = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_text = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.textBox_org = new System.Windows.Forms.TextBox();
			this.listView_target = new System.Windows.Forms.ListView();
			this.col_line2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_text2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.textBox_target = new System.Windows.Forms.TextBox();
			this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.기준지정ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.삭제ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.모두삭제ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.메모장에서열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.탐색기에서해당위치열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.panel1 = new System.Windows.Forms.Panel();
			this.panel2 = new System.Windows.Forms.Panel();
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
			this.contextMenuStrip1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.BackColor = System.Drawing.Color.Gray;
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.ForeColor = System.Drawing.SystemColors.ControlText;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.splitContainer3);
			this.splitContainer1.Size = new System.Drawing.Size(1076, 616);
			this.splitContainer1.SplitterDistance = 706;
			this.splitContainer1.SplitterWidth = 8;
			this.splitContainer1.TabIndex = 0;
			// 
			// splitContainer2
			// 
			this.splitContainer2.BackColor = System.Drawing.SystemColors.Control;
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer2.IsSplitterFixed = true;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.BackColor = System.Drawing.SystemColors.Control;
			this.splitContainer2.Panel1.Controls.Add(this.panel2);
			this.splitContainer2.Panel1.Controls.Add(this.label3);
			this.splitContainer2.Panel1.Controls.Add(this.checkBox_INIComp);
			this.splitContainer2.Panel1.Controls.Add(this.btn_CompWinmerge);
			this.splitContainer2.Panel1.Controls.Add(this.pictureBox1);
			this.splitContainer2.Panel1.Controls.Add(this.btn_usage);
			this.splitContainer2.Panel1.Controls.Add(this.btn_Reset);
			this.splitContainer2.Panel1.Controls.Add(this.label2);
			this.splitContainer2.Panel1.Controls.Add(this.textBox_FileFilter);
			this.splitContainer2.Panel1.Controls.Add(this.label1);
			this.splitContainer2.Panel1.Controls.Add(this.btn_run);
			this.splitContainer2.Panel1.Controls.Add(this.btn_Gijun);
			this.splitContainer2.Panel1.Controls.Add(this.btn_load);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.listView_fileList);
			this.splitContainer2.Size = new System.Drawing.Size(706, 616);
			this.splitContainer2.SplitterDistance = 143;
			this.splitContainer2.TabIndex = 0;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("맑은 고딕", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label3.Location = new System.Drawing.Point(61, 24);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(91, 25);
			this.label3.TabIndex = 11;
			this.label3.Text = "Diff Files";
			// 
			// checkBox_INIComp
			// 
			this.checkBox_INIComp.AutoSize = true;
			this.checkBox_INIComp.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.checkBox_INIComp.Location = new System.Drawing.Point(395, 106);
			this.checkBox_INIComp.Name = "checkBox_INIComp";
			this.checkBox_INIComp.Size = new System.Drawing.Size(107, 21);
			this.checkBox_INIComp.TabIndex = 10;
			this.checkBox_INIComp.Text = "INI 의미 비교";
			this.checkBox_INIComp.UseVisualStyleBackColor = true;
			// 
			// btn_CompWinmerge
			// 
			this.btn_CompWinmerge.BackColor = System.Drawing.Color.Gainsboro;
			this.btn_CompWinmerge.FlatAppearance.BorderSize = 0;
			this.btn_CompWinmerge.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_CompWinmerge.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_CompWinmerge.Image = ((System.Drawing.Image)(resources.GetObject("btn_CompWinmerge.Image")));
			this.btn_CompWinmerge.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_CompWinmerge.Location = new System.Drawing.Point(499, 61);
			this.btn_CompWinmerge.Name = "btn_CompWinmerge";
			this.btn_CompWinmerge.Size = new System.Drawing.Size(93, 37);
			this.btn_CompWinmerge.TabIndex = 9;
			this.btn_CompWinmerge.Text = "윈머지";
			this.btn_CompWinmerge.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_CompWinmerge.UseVisualStyleBackColor = false;
			this.btn_CompWinmerge.Click += new System.EventHandler(this.btn_CompWinmerge_Click);
			// 
			// pictureBox1
			// 
			this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
			this.pictureBox1.Location = new System.Drawing.Point(23, 18);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(43, 31);
			this.pictureBox1.TabIndex = 8;
			this.pictureBox1.TabStop = false;
			// 
			// btn_usage
			// 
			this.btn_usage.BackColor = System.Drawing.SystemColors.Control;
			this.btn_usage.FlatAppearance.BorderSize = 0;
			this.btn_usage.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btn_usage.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_usage.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_usage.Location = new System.Drawing.Point(212, 54);
			this.btn_usage.Name = "btn_usage";
			this.btn_usage.Size = new System.Drawing.Size(71, 24);
			this.btn_usage.TabIndex = 7;
			this.btn_usage.Text = "사용법";
			this.btn_usage.UseVisualStyleBackColor = false;
			this.btn_usage.Click += new System.EventHandler(this.btn_usage_Click);
			// 
			// btn_Reset
			// 
			this.btn_Reset.BackColor = System.Drawing.Color.Gainsboro;
			this.btn_Reset.FlatAppearance.BorderSize = 0;
			this.btn_Reset.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Reset.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_Reset.Image = ((System.Drawing.Image)(resources.GetObject("btn_Reset.Image")));
			this.btn_Reset.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_Reset.Location = new System.Drawing.Point(598, 18);
			this.btn_Reset.Name = "btn_Reset";
			this.btn_Reset.Size = new System.Drawing.Size(93, 37);
			this.btn_Reset.TabIndex = 5;
			this.btn_Reset.Text = "Reset ";
			this.btn_Reset.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_Reset.UseVisualStyleBackColor = false;
			this.btn_Reset.Click += new System.EventHandler(this.btn_Reset_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label2.Location = new System.Drawing.Point(17, 104);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(65, 17);
			this.label2.TabIndex = 4;
			this.label2.Text = "파일 필터";
			// 
			// textBox_FileFilter
			// 
			this.textBox_FileFilter.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_FileFilter.Location = new System.Drawing.Point(98, 102);
			this.textBox_FileFilter.Name = "textBox_FileFilter";
			this.textBox_FileFilter.Size = new System.Drawing.Size(138, 23);
			this.textBox_FileFilter.TabIndex = 3;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.BackColor = System.Drawing.Color.Yellow;
			this.label1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label1.Location = new System.Drawing.Point(63, 63);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(127, 15);
			this.label1.TabIndex = 2;
			this.label1.Text = "다중 파일 차이점 비교";
			// 
			// btn_run
			// 
			this.btn_run.BackColor = System.Drawing.Color.Gainsboro;
			this.btn_run.FlatAppearance.BorderSize = 0;
			this.btn_run.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_run.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_run.Image = ((System.Drawing.Image)(resources.GetObject("btn_run.Image")));
			this.btn_run.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_run.Location = new System.Drawing.Point(499, 18);
			this.btn_run.Name = "btn_run";
			this.btn_run.Size = new System.Drawing.Size(93, 37);
			this.btn_run.TabIndex = 1;
			this.btn_run.Text = "Run  ";
			this.btn_run.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_run.UseVisualStyleBackColor = false;
			this.btn_run.Click += new System.EventHandler(this.btn_run_Click);
			// 
			// btn_Gijun
			// 
			this.btn_Gijun.BackColor = System.Drawing.Color.Gainsboro;
			this.btn_Gijun.FlatAppearance.BorderSize = 0;
			this.btn_Gijun.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Gijun.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_Gijun.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_Gijun.Location = new System.Drawing.Point(395, 63);
			this.btn_Gijun.Name = "btn_Gijun";
			this.btn_Gijun.Size = new System.Drawing.Size(98, 37);
			this.btn_Gijun.TabIndex = 0;
			this.btn_Gijun.Text = "기준파일 지정";
			this.btn_Gijun.UseVisualStyleBackColor = false;
			this.btn_Gijun.Click += new System.EventHandler(this.btn_Gijun_Click);
			// 
			// btn_load
			// 
			this.btn_load.BackColor = System.Drawing.Color.Gainsboro;
			this.btn_load.FlatAppearance.BorderSize = 0;
			this.btn_load.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_load.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_load.Image = ((System.Drawing.Image)(resources.GetObject("btn_load.Image")));
			this.btn_load.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_load.Location = new System.Drawing.Point(395, 18);
			this.btn_load.Name = "btn_load";
			this.btn_load.Size = new System.Drawing.Size(98, 37);
			this.btn_load.TabIndex = 0;
			this.btn_load.Text = "Load  ";
			this.btn_load.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_load.UseVisualStyleBackColor = false;
			this.btn_load.Click += new System.EventHandler(this.btn_load_Click);
			// 
			// listView_fileList
			// 
			this.listView_fileList.AutoArrange = false;
			this.listView_fileList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_idx,
            this.col_diff,
            this.col_path});
			this.listView_fileList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_fileList.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_fileList.FullRowSelect = true;
			this.listView_fileList.GridLines = true;
			this.listView_fileList.Location = new System.Drawing.Point(0, 0);
			this.listView_fileList.Name = "listView_fileList";
			this.listView_fileList.Size = new System.Drawing.Size(706, 469);
			this.listView_fileList.TabIndex = 3;
			this.listView_fileList.UseCompatibleStateImageBehavior = false;
			this.listView_fileList.View = System.Windows.Forms.View.Details;
			this.listView_fileList.SelectedIndexChanged += new System.EventHandler(this.listView_fileList_SelectedIndexChanged);
			this.listView_fileList.DoubleClick += new System.EventHandler(this.listView_fileList_DoubleClick);
			this.listView_fileList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_fileList_KeyDown);
			this.listView_fileList.MouseDown += new System.Windows.Forms.MouseEventHandler(this.listView_fileList_MouseDown);
			// 
			// col_idx
			// 
			this.col_idx.Text = "No.";
			// 
			// col_diff
			// 
			this.col_diff.Text = "Diff";
			// 
			// col_path
			// 
			this.col_path.Text = "경로";
			this.col_path.Width = 572;
			// 
			// splitContainer3
			// 
			this.splitContainer3.BackColor = System.Drawing.SystemColors.Control;
			this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer3.Location = new System.Drawing.Point(0, 0);
			this.splitContainer3.Name = "splitContainer3";
			this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this.splitContainer3.Panel1.Controls.Add(this.listView_org);
			this.splitContainer3.Panel1.Controls.Add(this.textBox_org);
			// 
			// splitContainer3.Panel2
			// 
			this.splitContainer3.Panel2.Controls.Add(this.listView_target);
			this.splitContainer3.Panel2.Controls.Add(this.textBox_target);
			this.splitContainer3.Size = new System.Drawing.Size(362, 616);
			this.splitContainer3.SplitterDistance = 283;
			this.splitContainer3.TabIndex = 0;
			// 
			// listView_org
			// 
			this.listView_org.AutoArrange = false;
			this.listView_org.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_line,
            this.col_text});
			this.listView_org.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_org.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_org.FullRowSelect = true;
			this.listView_org.GridLines = true;
			this.listView_org.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.listView_org.Location = new System.Drawing.Point(0, 23);
			this.listView_org.Name = "listView_org";
			this.listView_org.Size = new System.Drawing.Size(362, 260);
			this.listView_org.TabIndex = 6;
			this.listView_org.UseCompatibleStateImageBehavior = false;
			this.listView_org.View = System.Windows.Forms.View.Details;
			this.listView_org.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_org_KeyDown);
			// 
			// col_line
			// 
			this.col_line.Text = "Line";
			this.col_line.Width = 58;
			// 
			// col_text
			// 
			this.col_text.Text = "Text";
			this.col_text.Width = 302;
			// 
			// textBox_org
			// 
			this.textBox_org.Dock = System.Windows.Forms.DockStyle.Top;
			this.textBox_org.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_org.Location = new System.Drawing.Point(0, 0);
			this.textBox_org.Name = "textBox_org";
			this.textBox_org.ReadOnly = true;
			this.textBox_org.Size = new System.Drawing.Size(362, 23);
			this.textBox_org.TabIndex = 5;
			// 
			// listView_target
			// 
			this.listView_target.AutoArrange = false;
			this.listView_target.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_line2,
            this.col_text2});
			this.listView_target.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_target.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_target.FullRowSelect = true;
			this.listView_target.GridLines = true;
			this.listView_target.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.listView_target.Location = new System.Drawing.Point(0, 23);
			this.listView_target.Name = "listView_target";
			this.listView_target.Size = new System.Drawing.Size(362, 306);
			this.listView_target.TabIndex = 7;
			this.listView_target.UseCompatibleStateImageBehavior = false;
			this.listView_target.View = System.Windows.Forms.View.Details;
			this.listView_target.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_org_KeyDown);
			// 
			// col_line2
			// 
			this.col_line2.Text = "Line";
			// 
			// col_text2
			// 
			this.col_text2.Text = "Text";
			this.col_text2.Width = 302;
			// 
			// textBox_target
			// 
			this.textBox_target.Dock = System.Windows.Forms.DockStyle.Top;
			this.textBox_target.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_target.Location = new System.Drawing.Point(0, 0);
			this.textBox_target.Name = "textBox_target";
			this.textBox_target.ReadOnly = true;
			this.textBox_target.Size = new System.Drawing.Size(362, 23);
			this.textBox_target.TabIndex = 6;
			// 
			// contextMenuStrip1
			// 
			this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.기준지정ToolStripMenuItem,
            this.삭제ToolStripMenuItem,
            this.모두삭제ToolStripMenuItem,
            this.메모장에서열기ToolStripMenuItem,
            this.탐색기에서해당위치열기ToolStripMenuItem});
			this.contextMenuStrip1.Name = "contextMenuStrip1";
			this.contextMenuStrip1.Size = new System.Drawing.Size(163, 114);
			// 
			// 기준지정ToolStripMenuItem
			// 
			this.기준지정ToolStripMenuItem.Name = "기준지정ToolStripMenuItem";
			this.기준지정ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.기준지정ToolStripMenuItem.Text = "기준 파일 지정";
			this.기준지정ToolStripMenuItem.Click += new System.EventHandler(this.기준지정ToolStripMenuItem_Click);
			// 
			// 삭제ToolStripMenuItem
			// 
			this.삭제ToolStripMenuItem.Name = "삭제ToolStripMenuItem";
			this.삭제ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.삭제ToolStripMenuItem.Text = "삭제";
			this.삭제ToolStripMenuItem.Click += new System.EventHandler(this.삭제ToolStripMenuItem_Click);
			// 
			// 모두삭제ToolStripMenuItem
			// 
			this.모두삭제ToolStripMenuItem.Name = "모두삭제ToolStripMenuItem";
			this.모두삭제ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.모두삭제ToolStripMenuItem.Text = "모두 삭제";
			this.모두삭제ToolStripMenuItem.Click += new System.EventHandler(this.모두삭제ToolStripMenuItem_Click);
			// 
			// 메모장에서열기ToolStripMenuItem
			// 
			this.메모장에서열기ToolStripMenuItem.Name = "메모장에서열기ToolStripMenuItem";
			this.메모장에서열기ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.메모장에서열기ToolStripMenuItem.Text = "메모장에서 열기";
			this.메모장에서열기ToolStripMenuItem.Click += new System.EventHandler(this.메모장에서열기ToolStripMenuItem_Click);
			// 
			// 탐색기에서해당위치열기ToolStripMenuItem
			// 
			this.탐색기에서해당위치열기ToolStripMenuItem.Name = "탐색기에서해당위치열기ToolStripMenuItem";
			this.탐색기에서해당위치열기ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.탐색기에서해당위치열기ToolStripMenuItem.Text = "탐색기에서 열기";
			this.탐색기에서해당위치열기ToolStripMenuItem.Click += new System.EventHandler(this.탐색기에서해당위치열기ToolStripMenuItem_Click);
			// 
			// progressBar1
			// 
			this.progressBar1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.progressBar1.Location = new System.Drawing.Point(0, 616);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(1076, 10);
			this.progressBar1.TabIndex = 5;
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.splitContainer1);
			this.panel1.Controls.Add(this.progressBar1);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(1076, 626);
			this.panel1.TabIndex = 6;
			// 
			// panel2
			// 
			this.panel2.BackColor = System.Drawing.Color.Gray;
			this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel2.Location = new System.Drawing.Point(0, 0);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(706, 10);
			this.panel2.TabIndex = 12;
			// 
			// UDiff
			// 
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1076, 626);
			this.Controls.Add(this.panel1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "UDiff";
			this.Text = "DIFF";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UMain_FormClosing);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel1.PerformLayout();
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.splitContainer3.Panel1.ResumeLayout(false);
			this.splitContainer3.Panel1.PerformLayout();
			this.splitContainer3.Panel2.ResumeLayout(false);
			this.splitContainer3.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
			this.splitContainer3.ResumeLayout(false);
			this.contextMenuStrip1.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_run;
        private System.Windows.Forms.Button btn_load;
        private System.Windows.Forms.ListView listView_fileList;
        private System.Windows.Forms.ColumnHeader col_idx;
        private System.Windows.Forms.ColumnHeader col_diff;
        private System.Windows.Forms.ColumnHeader col_path;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.ListView listView_org;
        private System.Windows.Forms.TextBox textBox_org;
        private System.Windows.Forms.ListView listView_target;
        private System.Windows.Forms.TextBox textBox_target;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 기준지정ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 삭제ToolStripMenuItem;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_FileFilter;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button btn_Reset;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ColumnHeader col_line;
        private System.Windows.Forms.ColumnHeader col_text;
        private System.Windows.Forms.ColumnHeader col_line2;
        private System.Windows.Forms.ColumnHeader col_text2;
        private System.Windows.Forms.Button btn_Gijun;
        private System.Windows.Forms.Button btn_usage;
        private System.Windows.Forms.ToolStripMenuItem 모두삭제ToolStripMenuItem;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStripMenuItem 메모장에서열기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 탐색기에서해당위치열기ToolStripMenuItem;
        private System.Windows.Forms.Button btn_CompWinmerge;
        private System.Windows.Forms.CheckBox checkBox_INIComp;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel2;
	}
}

