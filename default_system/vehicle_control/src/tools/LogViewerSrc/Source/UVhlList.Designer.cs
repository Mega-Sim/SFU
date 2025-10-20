namespace LogViewer
{
    partial class UVhlList
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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
            "",
            "P1_1L",
            "V30001",
            "192.168.100.100",
            "10.10.10.150",
            "OK: 125ms",
            "OBS감지로 건너뛰고감"}, -1);
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UVhlList));
            this.listView1 = new System.Windows.Forms.ListView();
            this.col_Idx = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.col_Memo = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btn_Refresh = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.col_LineName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.col_OhtName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.col_IP = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.col_Version = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btn_EraseMemoAll = new System.Windows.Forms.Button();
            this.btn_ShowSelectedItem = new System.Windows.Forms.Button();
            this.btn_ShowAllItem = new System.Windows.Forms.Button();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.btn_SelectHogi = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.col_Ping = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.CheckBoxes = true;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_Idx});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            listViewItem1.StateImageIndex = 0;
            this.listView1.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1});
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(651, 572);
            this.listView1.SmallImageList = this.imageList1;
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // col_Idx
            // 
            this.col_Idx.Text = "No.";
            this.col_Idx.Width = 58;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.label4);
            this.splitContainer1.Panel1.Controls.Add(this.label3);
            this.splitContainer1.Panel1.Controls.Add(this.label2);
            this.splitContainer1.Panel1.Controls.Add(this.btn_SelectHogi);
            this.splitContainer1.Panel1.Controls.Add(this.textBox2);
            this.splitContainer1.Panel1.Controls.Add(this.btn_ShowAllItem);
            this.splitContainer1.Panel1.Controls.Add(this.btn_ShowSelectedItem);
            this.splitContainer1.Panel1.Controls.Add(this.btn_EraseMemoAll);
            this.splitContainer1.Panel1.Controls.Add(this.progressBar1);
            this.splitContainer1.Panel1.Controls.Add(this.textBox1);
            this.splitContainer1.Panel1.Controls.Add(this.label1);
            this.splitContainer1.Panel1.Controls.Add(this.btn_Refresh);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listView1);
            this.splitContainer1.Size = new System.Drawing.Size(651, 678);
            this.splitContainer1.SplitterDistance = 101;
            this.splitContainer1.SplitterWidth = 5;
            this.splitContainer1.TabIndex = 0;
            this.splitContainer1.TabStop = false;
            // 
            // col_Memo
            // 
            this.col_Memo.Text = "Memo";
            this.col_Memo.Width = 175;
            // 
            // btn_Refresh
            // 
            this.btn_Refresh.BackColor = System.Drawing.Color.White;
            this.btn_Refresh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Refresh.Image = ((System.Drawing.Image)(resources.GetObject("btn_Refresh.Image")));
            this.btn_Refresh.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btn_Refresh.Location = new System.Drawing.Point(362, 20);
            this.btn_Refresh.Name = "btn_Refresh";
            this.btn_Refresh.Size = new System.Drawing.Size(87, 45);
            this.btn_Refresh.TabIndex = 0;
            this.btn_Refresh.Text = "Refresh";
            this.btn_Refresh.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btn_Refresh.UseVisualStyleBackColor = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.MistyRose;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(12, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(113, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "선택된 항목: 50개";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(105, 42);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(55, 23);
            this.textBox1.TabIndex = 2;
            this.textBox1.Text = "1";
            // 
            // progressBar1
            // 
            this.progressBar1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.progressBar1.Location = new System.Drawing.Point(0, 91);
            this.progressBar1.Margin = new System.Windows.Forms.Padding(2);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(651, 10);
            this.progressBar1.Step = 1;
            this.progressBar1.TabIndex = 3;
            // 
            // col_LineName
            // 
            this.col_LineName.Text = "Line";
            this.col_LineName.Width = 62;
            // 
            // col_OhtName
            // 
            this.col_OhtName.Text = "VHL";
            // 
            // col_IP
            // 
            this.col_IP.Text = "IP";
            this.col_IP.Width = 114;
            // 
            // col_Version
            // 
            this.col_Version.Text = "OHT Ver";
            this.col_Version.Width = 86;
            // 
            // btn_EraseMemoAll
            // 
            this.btn_EraseMemoAll.BackColor = System.Drawing.Color.White;
            this.btn_EraseMemoAll.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_EraseMemoAll.Location = new System.Drawing.Point(497, 63);
            this.btn_EraseMemoAll.Name = "btn_EraseMemoAll";
            this.btn_EraseMemoAll.Size = new System.Drawing.Size(127, 25);
            this.btn_EraseMemoAll.TabIndex = 4;
            this.btn_EraseMemoAll.Text = "전체 메모 삭제";
            this.btn_EraseMemoAll.UseVisualStyleBackColor = false;
            // 
            // btn_ShowSelectedItem
            // 
            this.btn_ShowSelectedItem.BackColor = System.Drawing.Color.White;
            this.btn_ShowSelectedItem.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ShowSelectedItem.Location = new System.Drawing.Point(497, 8);
            this.btn_ShowSelectedItem.Name = "btn_ShowSelectedItem";
            this.btn_ShowSelectedItem.Size = new System.Drawing.Size(127, 25);
            this.btn_ShowSelectedItem.TabIndex = 5;
            this.btn_ShowSelectedItem.Text = "선택된 항목 보기";
            this.btn_ShowSelectedItem.UseVisualStyleBackColor = false;
            // 
            // btn_ShowAllItem
            // 
            this.btn_ShowAllItem.BackColor = System.Drawing.Color.White;
            this.btn_ShowAllItem.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ShowAllItem.Location = new System.Drawing.Point(497, 35);
            this.btn_ShowAllItem.Name = "btn_ShowAllItem";
            this.btn_ShowAllItem.Size = new System.Drawing.Size(127, 25);
            this.btn_ShowAllItem.TabIndex = 5;
            this.btn_ShowAllItem.Text = "모든 항목 보기";
            this.btn_ShowAllItem.UseVisualStyleBackColor = false;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(186, 42);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(55, 23);
            this.textBox2.TabIndex = 6;
            this.textBox2.Text = "30";
            // 
            // btn_SelectHogi
            // 
            this.btn_SelectHogi.BackColor = System.Drawing.Color.White;
            this.btn_SelectHogi.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_SelectHogi.Image = ((System.Drawing.Image)(resources.GetObject("btn_SelectHogi.Image")));
            this.btn_SelectHogi.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btn_SelectHogi.Location = new System.Drawing.Point(247, 35);
            this.btn_SelectHogi.Name = "btn_SelectHogi";
            this.btn_SelectHogi.Size = new System.Drawing.Size(71, 35);
            this.btn_SelectHogi.TabIndex = 7;
            this.btn_SelectHogi.Text = "Select";
            this.btn_SelectHogi.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btn_SelectHogi.UseVisualStyleBackColor = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 47);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(87, 15);
            this.label2.TabIndex = 8;
            this.label2.Text = "호기 선택 범위";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(165, 48);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(15, 15);
            this.label3.TabIndex = 9;
            this.label3.Text = "~";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(359, 68);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(104, 15);
            this.label4.TabIndex = 10;
            this.label4.Text = "Refresh 10초 경과";
            // 
            // col_Ping
            // 
            this.col_Ping.Text = "Ping";
            this.col_Ping.Width = 88;
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "lightbulb_Off20x20.png");
            this.imageList1.Images.SetKeyName(1, "lightbulb_On20x20.png");
            // 
            // UVhlList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(651, 678);
            this.Controls.Add(this.splitContainer1);
            this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Name = "UVhlList";
            this.Text = "UVhlList";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader col_Idx;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btn_SelectHogi;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button btn_ShowAllItem;
        private System.Windows.Forms.Button btn_ShowSelectedItem;
        private System.Windows.Forms.Button btn_EraseMemoAll;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_Refresh;
        private System.Windows.Forms.ColumnHeader col_Memo;
        private System.Windows.Forms.ColumnHeader col_LineName;
        private System.Windows.Forms.ColumnHeader col_OhtName;
        private System.Windows.Forms.ColumnHeader col_IP;
        private System.Windows.Forms.ColumnHeader col_Version;
        private System.Windows.Forms.ColumnHeader col_Ping;
    }
}