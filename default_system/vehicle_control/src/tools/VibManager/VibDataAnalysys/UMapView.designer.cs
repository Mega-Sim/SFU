namespace VibrationManager
{
    partial class UMapViewMan
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UMapViewMan));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button_Close = new System.Windows.Forms.Button();
            this.button_Refresh = new System.Windows.Forms.Button();
            this.button_MapOpen = new System.Windows.Forms.Button();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.listView_MapList = new System.Windows.Forms.ListView();
            this.columnHeader_No = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Project = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Line = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Version = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Date = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Status = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Path = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.textBox_CurMap = new System.Windows.Forms.TextBox();
            this.button_HideAll = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.comboBox_Elem = new System.Windows.Forms.ComboBox();
            this.button_FindNode = new System.Windows.Forms.Button();
            this.textBox_ID = new System.Windows.Forms.TextBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
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
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.White;
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer3);
            this.splitContainer1.Panel2.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.splitContainer1.Size = new System.Drawing.Size(675, 387);
            this.splitContainer1.SplitterDistance = 76;
            this.splitContainer1.TabIndex = 0;
            this.splitContainer1.TabStop = false;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer2.IsSplitterFixed = true;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.pictureBox1);
            this.splitContainer2.Panel1.Controls.Add(this.label1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.button_Close);
            this.splitContainer2.Panel2.Controls.Add(this.button_Refresh);
            this.splitContainer2.Panel2.Controls.Add(this.button_MapOpen);
            this.splitContainer2.Size = new System.Drawing.Size(675, 76);
            this.splitContainer2.SplitterDistance = 385;
            this.splitContainer2.TabIndex = 0;
            this.splitContainer2.TabStop = false;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(34, 21);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(39, 35);
            this.pictureBox1.TabIndex = 15;
            this.pictureBox1.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(78, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(177, 20);
            this.label1.TabIndex = 14;
            this.label1.Text = "Map View Management";
            // 
            // button_Close
            // 
            this.button_Close.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_Close.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_Close.Image = ((System.Drawing.Image)(resources.GetObject("button_Close.Image")));
            this.button_Close.Location = new System.Drawing.Point(188, 12);
            this.button_Close.Name = "button_Close";
            this.button_Close.Size = new System.Drawing.Size(80, 55);
            this.button_Close.TabIndex = 14;
            this.button_Close.Text = "Close";
            this.button_Close.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.button_Close.UseVisualStyleBackColor = true;
            this.button_Close.Click += new System.EventHandler(this.button_Close_Click);
            // 
            // button_Refresh
            // 
            this.button_Refresh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_Refresh.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_Refresh.Image = ((System.Drawing.Image)(resources.GetObject("button_Refresh.Image")));
            this.button_Refresh.Location = new System.Drawing.Point(100, 12);
            this.button_Refresh.Name = "button_Refresh";
            this.button_Refresh.Size = new System.Drawing.Size(80, 55);
            this.button_Refresh.TabIndex = 16;
            this.button_Refresh.Text = "Refresh";
            this.button_Refresh.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.button_Refresh.UseVisualStyleBackColor = true;
            this.button_Refresh.Click += new System.EventHandler(this.button_Refresh_Click);
            // 
            // button_MapOpen
            // 
            this.button_MapOpen.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.button_MapOpen.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_MapOpen.Image = ((System.Drawing.Image)(resources.GetObject("button_MapOpen.Image")));
            this.button_MapOpen.Location = new System.Drawing.Point(13, 12);
            this.button_MapOpen.Name = "button_MapOpen";
            this.button_MapOpen.Size = new System.Drawing.Size(80, 55);
            this.button_MapOpen.TabIndex = 15;
            this.button_MapOpen.Text = "Map Load";
            this.button_MapOpen.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.button_MapOpen.UseVisualStyleBackColor = true;
            this.button_MapOpen.Click += new System.EventHandler(this.button_MapOpen_Click);
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
            this.splitContainer3.Panel1.Controls.Add(this.listView_MapList);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.textBox_CurMap);
            this.splitContainer3.Panel2.Controls.Add(this.button_HideAll);
            this.splitContainer3.Panel2.Controls.Add(this.label3);
            this.splitContainer3.Panel2.Controls.Add(this.comboBox_Elem);
            this.splitContainer3.Panel2.Controls.Add(this.button_FindNode);
            this.splitContainer3.Panel2.Controls.Add(this.textBox_ID);
            this.splitContainer3.Size = new System.Drawing.Size(675, 307);
            this.splitContainer3.SplitterDistance = 200;
            this.splitContainer3.TabIndex = 0;
            this.splitContainer3.TabStop = false;
            // 
            // listView_MapList
            // 
            this.listView_MapList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_No,
            this.columnHeader_Project,
            this.columnHeader_Line,
            this.columnHeader_Version,
            this.columnHeader_Date,
            this.columnHeader_Status,
            this.columnHeader_Path});
            this.listView_MapList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView_MapList.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.listView_MapList.FullRowSelect = true;
            this.listView_MapList.GridLines = true;
            this.listView_MapList.Location = new System.Drawing.Point(0, 0);
            this.listView_MapList.MultiSelect = false;
            this.listView_MapList.Name = "listView_MapList";
            this.listView_MapList.Size = new System.Drawing.Size(675, 200);
            this.listView_MapList.TabIndex = 9;
            this.listView_MapList.UseCompatibleStateImageBehavior = false;
            this.listView_MapList.View = System.Windows.Forms.View.Details;
            this.listView_MapList.SelectedIndexChanged += new System.EventHandler(this.listView_MapList_SelectedIndexChanged);
            this.listView_MapList.DoubleClick += new System.EventHandler(this.listView_MapList_DoubleClick);
            // 
            // columnHeader_No
            // 
            this.columnHeader_No.Text = "No.";
            this.columnHeader_No.Width = 35;
            // 
            // columnHeader_Project
            // 
            this.columnHeader_Project.Text = "Prj";
            this.columnHeader_Project.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader_Project.Width = 79;
            // 
            // columnHeader_Line
            // 
            this.columnHeader_Line.Text = "Line";
            this.columnHeader_Line.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader_Line.Width = 86;
            // 
            // columnHeader_Version
            // 
            this.columnHeader_Version.Text = "Version";
            this.columnHeader_Version.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader_Version.Width = 73;
            // 
            // columnHeader_Date
            // 
            this.columnHeader_Date.Text = "Date";
            this.columnHeader_Date.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader_Date.Width = 153;
            // 
            // columnHeader_Status
            // 
            this.columnHeader_Status.Text = "Status";
            this.columnHeader_Status.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader_Status.Width = 90;
            // 
            // columnHeader_Path
            // 
            this.columnHeader_Path.Text = "Path";
            this.columnHeader_Path.Width = 600;
            // 
            // textBox_CurMap
            // 
            this.textBox_CurMap.BackColor = System.Drawing.Color.DarkGray;
            this.textBox_CurMap.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_CurMap.Location = new System.Drawing.Point(12, 18);
            this.textBox_CurMap.Name = "textBox_CurMap";
            this.textBox_CurMap.ReadOnly = true;
            this.textBox_CurMap.Size = new System.Drawing.Size(354, 25);
            this.textBox_CurMap.TabIndex = 20;
            // 
            // button_HideAll
            // 
            this.button_HideAll.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.button_HideAll.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_HideAll.Location = new System.Drawing.Point(372, 11);
            this.button_HideAll.Name = "button_HideAll";
            this.button_HideAll.Size = new System.Drawing.Size(69, 36);
            this.button_HideAll.TabIndex = 15;
            this.button_HideAll.Text = "Hide All";
            this.button_HideAll.UseVisualStyleBackColor = true;
            this.button_HideAll.Click += new System.EventHandler(this.button_HideAll_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.Location = new System.Drawing.Point(9, 63);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(122, 17);
            this.label3.TabIndex = 19;
            this.label3.Text = "선택된 맵에서 찾기";
            // 
            // comboBox_Elem
            // 
            this.comboBox_Elem.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Elem.FormattingEnabled = true;
            this.comboBox_Elem.Items.AddRange(new object[] {
            "Node",
            "Station"});
            this.comboBox_Elem.Location = new System.Drawing.Point(145, 60);
            this.comboBox_Elem.Name = "comboBox_Elem";
            this.comboBox_Elem.Size = new System.Drawing.Size(106, 25);
            this.comboBox_Elem.TabIndex = 18;
            // 
            // button_FindNode
            // 
            this.button_FindNode.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.button_FindNode.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_FindNode.Location = new System.Drawing.Point(372, 53);
            this.button_FindNode.Name = "button_FindNode";
            this.button_FindNode.Size = new System.Drawing.Size(69, 36);
            this.button_FindNode.TabIndex = 17;
            this.button_FindNode.Text = "Find";
            this.button_FindNode.UseVisualStyleBackColor = true;
            this.button_FindNode.Click += new System.EventHandler(this.button_FindNode_Click);
            // 
            // textBox_ID
            // 
            this.textBox_ID.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_ID.Location = new System.Drawing.Point(258, 60);
            this.textBox_ID.Name = "textBox_ID";
            this.textBox_ID.Size = new System.Drawing.Size(108, 25);
            this.textBox_ID.TabIndex = 16;
            this.textBox_ID.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_ID_KeyDown);
            // 
            // timer1
            // 
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // UMapViewMan
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(675, 387);
            this.Controls.Add(this.splitContainer1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(504, 296);
            this.Name = "UMapViewMan";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Map View Managent";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UMapViewMan_FormClosing);
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
            this.splitContainer3.Panel2.ResumeLayout(false);
            this.splitContainer3.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_Close;
        private System.Windows.Forms.Button button_Refresh;
        private System.Windows.Forms.Button button_MapOpen;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.ListView listView_MapList;
        private System.Windows.Forms.ColumnHeader columnHeader_No;
        private System.Windows.Forms.ColumnHeader columnHeader_Project;
        private System.Windows.Forms.ColumnHeader columnHeader_Line;
        private System.Windows.Forms.ColumnHeader columnHeader_Version;
        private System.Windows.Forms.ColumnHeader columnHeader_Date;
        private System.Windows.Forms.ColumnHeader columnHeader_Status;
        private System.Windows.Forms.ColumnHeader columnHeader_Path;
        private System.Windows.Forms.TextBox textBox_CurMap;
        private System.Windows.Forms.Button button_HideAll;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox comboBox_Elem;
        private System.Windows.Forms.Button button_FindNode;
        private System.Windows.Forms.TextBox textBox_ID;
        public System.Windows.Forms.Timer timer1;
    }
}