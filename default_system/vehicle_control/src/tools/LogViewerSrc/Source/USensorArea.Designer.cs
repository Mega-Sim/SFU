namespace LogViewer
{
	partial class USensorArea
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
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer3 = new System.Windows.Forms.SplitContainer();
			this.listView_TimeList = new System.Windows.Forms.ListView();
			this.col_idx = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_time = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_PatternNo = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.splitContainer4 = new System.Windows.Forms.SplitContainer();
			this.listView_PatternInfo = new System.Windows.Forms.ListView();
			this.col_Item = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_x = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_y = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.comboBox_Pattern = new System.Windows.Forms.ComboBox();
			this.btn_FitWnd = new System.Windows.Forms.Button();
			this.textBox_LogPath = new System.Windows.Forms.TextBox();
			this.btnOpenLog = new System.Windows.Forms.Button();
			this.textBox_AraPath = new System.Windows.Forms.TextBox();
			this.btn_OpenAraFile = new System.Windows.Forms.Button();
			this.pictureBox_Draw = new System.Windows.Forms.PictureBox();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.label1 = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
			this.splitContainer3.Panel1.SuspendLayout();
			this.splitContainer3.Panel2.SuspendLayout();
			this.splitContainer3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).BeginInit();
			this.splitContainer4.Panel1.SuspendLayout();
			this.splitContainer4.Panel2.SuspendLayout();
			this.splitContainer4.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox_Draw)).BeginInit();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer3);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
			this.splitContainer1.Size = new System.Drawing.Size(894, 691);
			this.splitContainer1.SplitterDistance = 223;
			this.splitContainer1.TabIndex = 0;
			this.splitContainer1.TabStop = false;
			// 
			// splitContainer3
			// 
			this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer3.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer3.Location = new System.Drawing.Point(0, 0);
			this.splitContainer3.Name = "splitContainer3";
			this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this.splitContainer3.Panel1.Controls.Add(this.listView_TimeList);
			// 
			// splitContainer3.Panel2
			// 
			this.splitContainer3.Panel2.Controls.Add(this.splitContainer4);
			this.splitContainer3.Size = new System.Drawing.Size(223, 691);
			this.splitContainer3.SplitterDistance = 382;
			this.splitContainer3.TabIndex = 1;
			this.splitContainer3.TabStop = false;
			// 
			// listView_TimeList
			// 
			this.listView_TimeList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_idx,
            this.col_time,
            this.col_PatternNo});
			this.listView_TimeList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_TimeList.Location = new System.Drawing.Point(0, 0);
			this.listView_TimeList.Name = "listView_TimeList";
			this.listView_TimeList.Size = new System.Drawing.Size(223, 382);
			this.listView_TimeList.TabIndex = 0;
			this.listView_TimeList.UseCompatibleStateImageBehavior = false;
			this.listView_TimeList.View = System.Windows.Forms.View.Details;
			// 
			// col_idx
			// 
			this.col_idx.Text = "idx";
			this.col_idx.Width = 35;
			// 
			// col_time
			// 
			this.col_time.Text = "Time";
			this.col_time.Width = 134;
			// 
			// col_PatternNo
			// 
			this.col_PatternNo.Text = "Pat.";
			this.col_PatternNo.Width = 42;
			// 
			// splitContainer4
			// 
			this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer4.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer4.IsSplitterFixed = true;
			this.splitContainer4.Location = new System.Drawing.Point(0, 0);
			this.splitContainer4.Name = "splitContainer4";
			this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer4.Panel1
			// 
			this.splitContainer4.Panel1.Controls.Add(this.label1);
			this.splitContainer4.Panel1.Controls.Add(this.comboBox_Pattern);
			// 
			// splitContainer4.Panel2
			// 
			this.splitContainer4.Panel2.Controls.Add(this.listView_PatternInfo);
			this.splitContainer4.Size = new System.Drawing.Size(223, 305);
			this.splitContainer4.SplitterDistance = 29;
			this.splitContainer4.TabIndex = 1;
			this.splitContainer4.TabStop = false;
			// 
			// listView_PatternInfo
			// 
			this.listView_PatternInfo.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_Item,
            this.col_x,
            this.col_y});
			this.listView_PatternInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_PatternInfo.FullRowSelect = true;
			this.listView_PatternInfo.GridLines = true;
			this.listView_PatternInfo.Location = new System.Drawing.Point(0, 0);
			this.listView_PatternInfo.MultiSelect = false;
			this.listView_PatternInfo.Name = "listView_PatternInfo";
			this.listView_PatternInfo.Size = new System.Drawing.Size(223, 272);
			this.listView_PatternInfo.TabIndex = 0;
			this.listView_PatternInfo.UseCompatibleStateImageBehavior = false;
			this.listView_PatternInfo.View = System.Windows.Forms.View.Details;
			// 
			// col_Item
			// 
			this.col_Item.Text = "Item";
			this.col_Item.Width = 91;
			// 
			// col_x
			// 
			this.col_x.Text = "X";
			this.col_x.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.col_x.Width = 70;
			// 
			// col_y
			// 
			this.col_y.Text = "Y";
			this.col_y.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.col_y.Width = 41;
			// 
			// splitContainer2
			// 
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.btn_FitWnd);
			this.splitContainer2.Panel1.Controls.Add(this.textBox_LogPath);
			this.splitContainer2.Panel1.Controls.Add(this.btnOpenLog);
			this.splitContainer2.Panel1.Controls.Add(this.textBox_AraPath);
			this.splitContainer2.Panel1.Controls.Add(this.btn_OpenAraFile);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.pictureBox_Draw);
			this.splitContainer2.Size = new System.Drawing.Size(667, 691);
			this.splitContainer2.SplitterDistance = 68;
			this.splitContainer2.TabIndex = 0;
			this.splitContainer2.TabStop = false;
			// 
			// comboBox_Pattern
			// 
			this.comboBox_Pattern.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.comboBox_Pattern.FormattingEnabled = true;
			this.comboBox_Pattern.Location = new System.Drawing.Point(118, 3);
			this.comboBox_Pattern.Name = "comboBox_Pattern";
			this.comboBox_Pattern.Size = new System.Drawing.Size(92, 23);
			this.comboBox_Pattern.TabIndex = 6;
			this.comboBox_Pattern.SelectedIndexChanged += new System.EventHandler(this.comboBox_Pattern_SelectedIndexChanged);
			// 
			// btn_FitWnd
			// 
			this.btn_FitWnd.Location = new System.Drawing.Point(572, 25);
			this.btn_FitWnd.Name = "btn_FitWnd";
			this.btn_FitWnd.Size = new System.Drawing.Size(62, 41);
			this.btn_FitWnd.TabIndex = 5;
			this.btn_FitWnd.Text = "Fit Wind";
			this.btn_FitWnd.UseVisualStyleBackColor = true;
			this.btn_FitWnd.Click += new System.EventHandler(this.btn_FitWnd_Click);
			// 
			// textBox_LogPath
			// 
			this.textBox_LogPath.Location = new System.Drawing.Point(156, 43);
			this.textBox_LogPath.Name = "textBox_LogPath";
			this.textBox_LogPath.ReadOnly = true;
			this.textBox_LogPath.Size = new System.Drawing.Size(400, 23);
			this.textBox_LogPath.TabIndex = 4;
			// 
			// btnOpenLog
			// 
			this.btnOpenLog.Location = new System.Drawing.Point(10, 41);
			this.btnOpenLog.Name = "btnOpenLog";
			this.btnOpenLog.Size = new System.Drawing.Size(140, 25);
			this.btnOpenLog.TabIndex = 3;
			this.btnOpenLog.Text = "로그파일 열기";
			this.btnOpenLog.UseVisualStyleBackColor = true;
			// 
			// textBox_AraPath
			// 
			this.textBox_AraPath.Location = new System.Drawing.Point(156, 14);
			this.textBox_AraPath.Name = "textBox_AraPath";
			this.textBox_AraPath.ReadOnly = true;
			this.textBox_AraPath.Size = new System.Drawing.Size(400, 23);
			this.textBox_AraPath.TabIndex = 2;
			// 
			// btn_OpenAraFile
			// 
			this.btn_OpenAraFile.Location = new System.Drawing.Point(10, 12);
			this.btn_OpenAraFile.Name = "btn_OpenAraFile";
			this.btn_OpenAraFile.Size = new System.Drawing.Size(140, 25);
			this.btn_OpenAraFile.TabIndex = 0;
			this.btn_OpenAraFile.Text = "기준 ara 파일 찾기";
			this.btn_OpenAraFile.UseVisualStyleBackColor = true;
			this.btn_OpenAraFile.Click += new System.EventHandler(this.btn_OpenAraFile_Click);
			// 
			// pictureBox_Draw
			// 
			this.pictureBox_Draw.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox_Draw.Location = new System.Drawing.Point(0, 0);
			this.pictureBox_Draw.Name = "pictureBox_Draw";
			this.pictureBox_Draw.Size = new System.Drawing.Size(667, 619);
			this.pictureBox_Draw.TabIndex = 0;
			this.pictureBox_Draw.TabStop = false;
			this.pictureBox_Draw.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox_Draw_Paint);
			this.pictureBox_Draw.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox_Draw_MouseDown);
			this.pictureBox_Draw.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox_Draw_MouseMove);
			this.pictureBox_Draw.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox_Draw_MouseUp);
			this.pictureBox_Draw.Resize += new System.EventHandler(this.pictureBox_Draw_Resize);
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 6);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(92, 15);
			this.label1.TabIndex = 7;
			this.label1.Text = "Pattern File Info";
			// 
			// USensorArea
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(894, 691);
			this.Controls.Add(this.splitContainer1);
			this.Name = "USensorArea";
			this.Text = "USensorArea";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.USensorArea_FormClosing);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer3.Panel1.ResumeLayout(false);
			this.splitContainer3.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
			this.splitContainer3.ResumeLayout(false);
			this.splitContainer4.Panel1.ResumeLayout(false);
			this.splitContainer4.Panel1.PerformLayout();
			this.splitContainer4.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).EndInit();
			this.splitContainer4.ResumeLayout(false);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel1.PerformLayout();
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox_Draw)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitContainer1;
		private System.Windows.Forms.ListView listView_TimeList;
		private System.Windows.Forms.SplitContainer splitContainer2;
		private System.Windows.Forms.TextBox textBox_LogPath;
		private System.Windows.Forms.Button btnOpenLog;
		private System.Windows.Forms.TextBox textBox_AraPath;
		private System.Windows.Forms.Button btn_OpenAraFile;
		private System.Windows.Forms.ColumnHeader col_idx;
		private System.Windows.Forms.ColumnHeader col_time;
		private System.Windows.Forms.ColumnHeader col_PatternNo;
		private System.Windows.Forms.PictureBox pictureBox_Draw;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.Button btn_FitWnd;
		private System.Windows.Forms.ComboBox comboBox_Pattern;
		private System.Windows.Forms.SplitContainer splitContainer3;
		private System.Windows.Forms.ListView listView_PatternInfo;
		private System.Windows.Forms.ColumnHeader col_Item;
		private System.Windows.Forms.ColumnHeader col_x;
		private System.Windows.Forms.ColumnHeader col_y;
		private System.Windows.Forms.SplitContainer splitContainer4;
		private System.Windows.Forms.Label label1;
	}
}