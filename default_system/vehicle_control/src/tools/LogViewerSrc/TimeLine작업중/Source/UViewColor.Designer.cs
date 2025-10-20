namespace LogViewer
{
    partial class UViewColor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UViewColor));
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button_Edit = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonApply = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage_AmcRecv = new System.Windows.Forms.TabPage();
            this.dataGridView_ColorAmcRcv = new System.Windows.Forms.DataGridView();
            this.tabPage_User = new System.Windows.Forms.TabPage();
            this.label3 = new System.Windows.Forms.Label();
            this.tabPage_Bcr = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.context_Color = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripMenu_Edit = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage_AmcRecv.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_ColorAmcRcv)).BeginInit();
            this.tabPage_User.SuspendLayout();
            this.tabPage_Bcr.SuspendLayout();
            this.context_Color.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.White;
            this.splitContainer1.Panel1.Controls.Add(this.pictureBox1);
            this.splitContainer1.Panel1.Controls.Add(this.button_Edit);
            this.splitContainer1.Panel1.Controls.Add(this.label1);
            this.splitContainer1.Panel1.Controls.Add(this.buttonApply);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tabControl1);
            this.splitContainer1.Size = new System.Drawing.Size(543, 540);
            this.splitContainer1.SplitterDistance = 56;
            this.splitContainer1.TabIndex = 0;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.InitialImage = null;
            this.pictureBox1.Location = new System.Drawing.Point(9, 6);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(43, 43);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pictureBox1.TabIndex = 10;
            this.pictureBox1.TabStop = false;
            // 
            // button_Edit
            // 
            this.button_Edit.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.button_Edit.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_Edit.Location = new System.Drawing.Point(375, 19);
            this.button_Edit.Name = "button_Edit";
            this.button_Edit.Size = new System.Drawing.Size(75, 26);
            this.button_Edit.TabIndex = 9;
            this.button_Edit.Text = "Edit";
            this.button_Edit.UseVisualStyleBackColor = true;
            this.button_Edit.Click += new System.EventHandler(this.button_Edit_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(60, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(147, 20);
            this.label1.TabIndex = 8;
            this.label1.Text = "Cell Color 조건 정의";
            // 
            // buttonApply
            // 
            this.buttonApply.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.buttonApply.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.buttonApply.Location = new System.Drawing.Point(456, 19);
            this.buttonApply.Name = "buttonApply";
            this.buttonApply.Size = new System.Drawing.Size(75, 26);
            this.buttonApply.TabIndex = 7;
            this.buttonApply.Text = "Apply";
            this.buttonApply.UseVisualStyleBackColor = true;
            this.buttonApply.Click += new System.EventHandler(this.buttonApply_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage_AmcRecv);
            this.tabControl1.Controls.Add(this.tabPage_User);
            this.tabControl1.Controls.Add(this.tabPage_Bcr);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(543, 480);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage_AmcRecv
            // 
            this.tabPage_AmcRecv.Controls.Add(this.dataGridView_ColorAmcRcv);
            this.tabPage_AmcRecv.Location = new System.Drawing.Point(4, 24);
            this.tabPage_AmcRecv.Name = "tabPage_AmcRecv";
            this.tabPage_AmcRecv.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_AmcRecv.Size = new System.Drawing.Size(535, 452);
            this.tabPage_AmcRecv.TabIndex = 0;
            this.tabPage_AmcRecv.Text = "AMC Recv";
            this.tabPage_AmcRecv.UseVisualStyleBackColor = true;
            // 
            // dataGridView_ColorAmcRcv
            // 
            this.dataGridView_ColorAmcRcv.AllowUserToAddRows = false;
            this.dataGridView_ColorAmcRcv.AllowUserToDeleteRows = false;
            this.dataGridView_ColorAmcRcv.AllowUserToResizeRows = false;
            this.dataGridView_ColorAmcRcv.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_ColorAmcRcv.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_ColorAmcRcv.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dataGridView_ColorAmcRcv.Location = new System.Drawing.Point(3, 3);
            this.dataGridView_ColorAmcRcv.Name = "dataGridView_ColorAmcRcv";
            this.dataGridView_ColorAmcRcv.RowTemplate.Height = 23;
            this.dataGridView_ColorAmcRcv.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dataGridView_ColorAmcRcv.ShowCellErrors = false;
            this.dataGridView_ColorAmcRcv.ShowCellToolTips = false;
            this.dataGridView_ColorAmcRcv.ShowEditingIcon = false;
            this.dataGridView_ColorAmcRcv.ShowRowErrors = false;
            this.dataGridView_ColorAmcRcv.Size = new System.Drawing.Size(529, 446);
            this.dataGridView_ColorAmcRcv.TabIndex = 0;
            this.dataGridView_ColorAmcRcv.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView_ColorAmcRcv_MouseDown);
            // 
            // tabPage_User
            // 
            this.tabPage_User.Controls.Add(this.label3);
            this.tabPage_User.Location = new System.Drawing.Point(4, 24);
            this.tabPage_User.Name = "tabPage_User";
            this.tabPage_User.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_User.Size = new System.Drawing.Size(535, 452);
            this.tabPage_User.TabIndex = 1;
            this.tabPage_User.Text = "User";
            this.tabPage_User.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 13);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 15);
            this.label3.TabIndex = 1;
            this.label3.Text = "준비중입니다..";
            // 
            // tabPage_Bcr
            // 
            this.tabPage_Bcr.Controls.Add(this.label2);
            this.tabPage_Bcr.Location = new System.Drawing.Point(4, 24);
            this.tabPage_Bcr.Name = "tabPage_Bcr";
            this.tabPage_Bcr.Size = new System.Drawing.Size(535, 452);
            this.tabPage_Bcr.TabIndex = 2;
            this.tabPage_Bcr.Text = "BCR";
            this.tabPage_Bcr.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 13);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(85, 15);
            this.label2.TabIndex = 0;
            this.label2.Text = "준비중입니다..";
            // 
            // context_Color
            // 
            this.context_Color.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenu_Edit});
            this.context_Color.Name = "contextMenuStrip1";
            this.context_Color.Size = new System.Drawing.Size(95, 26);
            // 
            // toolStripMenu_Edit
            // 
            this.toolStripMenu_Edit.Name = "toolStripMenu_Edit";
            this.toolStripMenu_Edit.Size = new System.Drawing.Size(94, 22);
            this.toolStripMenu_Edit.Text = "Edit";
            this.toolStripMenu_Edit.Click += new System.EventHandler(this.toolStripMenu_Edit_Click);
            // 
            // UViewColor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(543, 540);
            this.Controls.Add(this.splitContainer1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UViewColor";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UViewColor_FormClosing);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPage_AmcRecv.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_ColorAmcRcv)).EndInit();
            this.tabPage_User.ResumeLayout(false);
            this.tabPage_User.PerformLayout();
            this.tabPage_Bcr.ResumeLayout(false);
            this.tabPage_Bcr.PerformLayout();
            this.context_Color.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button buttonApply;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage_AmcRecv;
        private System.Windows.Forms.DataGridView dataGridView_ColorAmcRcv;
        private System.Windows.Forms.TabPage tabPage_User;
        private System.Windows.Forms.TabPage tabPage_Bcr;
        private System.Windows.Forms.ContextMenuStrip context_Color;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenu_Edit;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button_Edit;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
    }
}