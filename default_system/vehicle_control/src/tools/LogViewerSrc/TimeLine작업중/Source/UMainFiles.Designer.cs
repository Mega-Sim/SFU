namespace LogViewer
{
    partial class UMainFiles
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UMainFiles));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.splitContainer3 = new System.Windows.Forms.SplitContainer();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.treeView_FileList = new System.Windows.Forms.TreeView();
			this.label2 = new System.Windows.Forms.Label();
			this.btn_Load = new System.Windows.Forms.Button();
			this.textBox_Path = new System.Windows.Forms.TextBox();
			this.listView_LoadedFolder = new System.Windows.Forms.ListView();
			this.col_LoadedNo = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.col_LoadedPath = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.label3 = new System.Windows.Forms.Label();
			this.btn_Unloading = new System.Windows.Forms.Button();
			this.pictureBox2 = new System.Windows.Forms.PictureBox();
			this.btn_SetRoot = new System.Windows.Forms.Button();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.label1 = new System.Windows.Forms.Label();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.contextMenuStrip_LoadedFolder = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.탐색기에서열기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
			this.splitContainer3.Panel1.SuspendLayout();
			this.splitContainer3.Panel2.SuspendLayout();
			this.splitContainer3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.contextMenuStrip_LoadedFolder.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.splitContainer3);
			this.splitContainer1.Panel1.Controls.Add(this.btn_SetRoot);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
			this.splitContainer1.Size = new System.Drawing.Size(1011, 566);
			this.splitContainer1.SplitterDistance = 359;
			this.splitContainer1.SplitterWidth = 8;
			this.splitContainer1.TabIndex = 0;
			this.splitContainer1.TabStop = false;
			// 
			// splitContainer3
			// 
			this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer3.Location = new System.Drawing.Point(0, 57);
			this.splitContainer3.Name = "splitContainer3";
			this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this.splitContainer3.Panel1.Controls.Add(this.progressBar1);
			this.splitContainer3.Panel1.Controls.Add(this.treeView_FileList);
			this.splitContainer3.Panel1.Controls.Add(this.label2);
			this.splitContainer3.Panel1.Controls.Add(this.btn_Load);
			this.splitContainer3.Panel1.Controls.Add(this.textBox_Path);
			// 
			// splitContainer3.Panel2
			// 
			this.splitContainer3.Panel2.Controls.Add(this.listView_LoadedFolder);
			this.splitContainer3.Panel2.Controls.Add(this.label3);
			this.splitContainer3.Panel2.Controls.Add(this.btn_Unloading);
			this.splitContainer3.Panel2.Controls.Add(this.pictureBox2);
			this.splitContainer3.Size = new System.Drawing.Size(359, 509);
			this.splitContainer3.SplitterDistance = 261;
			this.splitContainer3.SplitterWidth = 5;
			this.splitContainer3.TabIndex = 17;
			// 
			// progressBar1
			// 
			this.progressBar1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.progressBar1.Location = new System.Drawing.Point(0, 212);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(359, 10);
			this.progressBar1.Step = 1;
			this.progressBar1.TabIndex = 34;
			this.progressBar1.Visible = false;
			// 
			// treeView_FileList
			// 
			this.treeView_FileList.Dock = System.Windows.Forms.DockStyle.Fill;
			this.treeView_FileList.Location = new System.Drawing.Point(0, 46);
			this.treeView_FileList.Name = "treeView_FileList";
			this.treeView_FileList.Size = new System.Drawing.Size(359, 176);
			this.treeView_FileList.TabIndex = 31;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.label2.Dock = System.Windows.Forms.DockStyle.Top;
			this.label2.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label2.Location = new System.Drawing.Point(0, 25);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(134, 21);
			this.label2.TabIndex = 30;
			this.label2.Text = "Unloaded Folder";
			// 
			// btn_Load
			// 
			this.btn_Load.BackColor = System.Drawing.Color.White;
			this.btn_Load.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.btn_Load.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Load.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_Load.Location = new System.Drawing.Point(0, 222);
			this.btn_Load.Name = "btn_Load";
			this.btn_Load.Size = new System.Drawing.Size(359, 39);
			this.btn_Load.TabIndex = 29;
			this.btn_Load.Text = "폴더 로딩";
			this.btn_Load.UseVisualStyleBackColor = false;
			this.btn_Load.Click += new System.EventHandler(this.btn_Load_Click);
			// 
			// textBox_Path
			// 
			this.textBox_Path.BackColor = System.Drawing.Color.LightGreen;
			this.textBox_Path.Dock = System.Windows.Forms.DockStyle.Top;
			this.textBox_Path.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Path.Location = new System.Drawing.Point(0, 0);
			this.textBox_Path.Name = "textBox_Path";
			this.textBox_Path.ReadOnly = true;
			this.textBox_Path.Size = new System.Drawing.Size(359, 25);
			this.textBox_Path.TabIndex = 23;
			// 
			// listView_LoadedFolder
			// 
			this.listView_LoadedFolder.BackColor = System.Drawing.SystemColors.Control;
			this.listView_LoadedFolder.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_LoadedNo,
            this.col_LoadedPath});
			this.listView_LoadedFolder.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_LoadedFolder.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.listView_LoadedFolder.FullRowSelect = true;
			this.listView_LoadedFolder.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.listView_LoadedFolder.Location = new System.Drawing.Point(0, 24);
			this.listView_LoadedFolder.Name = "listView_LoadedFolder";
			this.listView_LoadedFolder.Size = new System.Drawing.Size(359, 180);
			this.listView_LoadedFolder.TabIndex = 33;
			this.listView_LoadedFolder.UseCompatibleStateImageBehavior = false;
			this.listView_LoadedFolder.View = System.Windows.Forms.View.Details;
			this.listView_LoadedFolder.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_LoadedFolder_KeyDown);
			this.listView_LoadedFolder.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView_LoadedFolder_MouseDoubleClick);
			this.listView_LoadedFolder.MouseDown += new System.Windows.Forms.MouseEventHandler(this.listView_LoadedFolder_MouseDown);
			// 
			// col_LoadedNo
			// 
			this.col_LoadedNo.Text = "No.";
			this.col_LoadedNo.Width = 35;
			// 
			// col_LoadedPath
			// 
			this.col_LoadedPath.Text = "Path";
			this.col_LoadedPath.Width = 316;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.label3.Dock = System.Windows.Forms.DockStyle.Top;
			this.label3.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label3.Location = new System.Drawing.Point(0, 3);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(118, 21);
			this.label3.TabIndex = 32;
			this.label3.Text = "Loaded Folder";
			// 
			// btn_Unloading
			// 
			this.btn_Unloading.BackColor = System.Drawing.Color.White;
			this.btn_Unloading.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.btn_Unloading.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Unloading.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_Unloading.Location = new System.Drawing.Point(0, 204);
			this.btn_Unloading.Name = "btn_Unloading";
			this.btn_Unloading.Size = new System.Drawing.Size(359, 39);
			this.btn_Unloading.TabIndex = 30;
			this.btn_Unloading.Text = "폴더 언로딩";
			this.btn_Unloading.UseVisualStyleBackColor = false;
			this.btn_Unloading.Click += new System.EventHandler(this.btn_Unloading_Click);
			// 
			// pictureBox2
			// 
			this.pictureBox2.BackColor = System.Drawing.Color.Transparent;
			this.pictureBox2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox2.Dock = System.Windows.Forms.DockStyle.Top;
			this.pictureBox2.Location = new System.Drawing.Point(0, 0);
			this.pictureBox2.Name = "pictureBox2";
			this.pictureBox2.Size = new System.Drawing.Size(359, 3);
			this.pictureBox2.TabIndex = 34;
			this.pictureBox2.TabStop = false;
			// 
			// btn_SetRoot
			// 
			this.btn_SetRoot.BackColor = System.Drawing.Color.White;
			this.btn_SetRoot.Dock = System.Windows.Forms.DockStyle.Top;
			this.btn_SetRoot.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_SetRoot.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_SetRoot.Image = ((System.Drawing.Image)(resources.GetObject("btn_SetRoot.Image")));
			this.btn_SetRoot.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_SetRoot.Location = new System.Drawing.Point(0, 0);
			this.btn_SetRoot.Name = "btn_SetRoot";
			this.btn_SetRoot.Size = new System.Drawing.Size(359, 57);
			this.btn_SetRoot.TabIndex = 16;
			this.btn_SetRoot.Text = "  SET ROOT";
			this.btn_SetRoot.UseVisualStyleBackColor = false;
			this.btn_SetRoot.Click += new System.EventHandler(this.btn_SetRoot_Click);
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
			this.splitContainer2.Panel1.BackColor = System.Drawing.Color.White;
			this.splitContainer2.Panel1.Controls.Add(this.pictureBox1);
			this.splitContainer2.Panel1.Controls.Add(this.label1);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.BackColor = System.Drawing.SystemColors.AppWorkspace;
			this.splitContainer2.Size = new System.Drawing.Size(644, 566);
			this.splitContainer2.SplitterDistance = 51;
			this.splitContainer2.TabIndex = 8;
			// 
			// pictureBox1
			// 
			this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
			this.pictureBox1.Location = new System.Drawing.Point(6, 10);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(29, 34);
			this.pictureBox1.TabIndex = 28;
			this.pictureBox1.TabStop = false;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label1.Location = new System.Drawing.Point(39, 22);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(67, 21);
			this.label1.TabIndex = 26;
			this.label1.Text = "Preview";
			// 
			// imageList1
			// 
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "Folder_opened_20x20.png");
			this.imageList1.Images.SetKeyName(1, "Folder_closed_20x20.png");
			this.imageList1.Images.SetKeyName(2, "document_25x30.png");
			this.imageList1.Images.SetKeyName(3, "Folder_opened_cmt_20x20.png");
			this.imageList1.Images.SetKeyName(4, "Folder_closed_cmt_20x20.png");
			// 
			// contextMenuStrip_LoadedFolder
			// 
			this.contextMenuStrip_LoadedFolder.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.탐색기에서열기ToolStripMenuItem});
			this.contextMenuStrip_LoadedFolder.Name = "contextMenuStrip_LoadedFolder";
			this.contextMenuStrip_LoadedFolder.Size = new System.Drawing.Size(163, 26);
			// 
			// 탐색기에서열기ToolStripMenuItem
			// 
			this.탐색기에서열기ToolStripMenuItem.Name = "탐색기에서열기ToolStripMenuItem";
			this.탐색기에서열기ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
			this.탐색기에서열기ToolStripMenuItem.Text = "탐색기에서 열기";
			this.탐색기에서열기ToolStripMenuItem.Click += new System.EventHandler(this.탐색기에서열기ToolStripMenuItem_Click);
			// 
			// UMainFiles
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1011, 566);
			this.Controls.Add(this.splitContainer1);
			this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.Name = "UMainFiles";
			this.Text = "UMainFiles";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UMainFiles_FormClosing);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.splitContainer3.Panel1.ResumeLayout(false);
			this.splitContainer3.Panel1.PerformLayout();
			this.splitContainer3.Panel2.ResumeLayout(false);
			this.splitContainer3.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
			this.splitContainer3.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.contextMenuStrip_LoadedFolder.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Button btn_SetRoot;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.TextBox textBox_Path;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TreeView treeView_FileList;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btn_Load;
        private System.Windows.Forms.ListView listView_LoadedFolder;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btn_Unloading;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_LoadedFolder;
        private System.Windows.Forms.ToolStripMenuItem 탐색기에서열기ToolStripMenuItem;
        private System.Windows.Forms.ColumnHeader col_LoadedNo;
        private System.Windows.Forms.ColumnHeader col_LoadedPath;
        private System.Windows.Forms.ProgressBar progressBar1;
    }
}