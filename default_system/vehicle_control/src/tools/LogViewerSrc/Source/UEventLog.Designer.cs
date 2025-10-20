namespace LogViewer
{
    partial class UEventLog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UEventLog));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBox_StadardTime = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBox_From = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabel4 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBox_To = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabel3 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripButton_TableShow = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.dataGridView_event = new System.Windows.Forms.DataGridView();
            this.dataGridView_user = new System.Windows.Forms.DataGridView();
            this.context_EventLog = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.틀고정ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_FixCol = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_FixColDismiss = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Table = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_TableConnect = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_TableDisconnect = new System.Windows.Forms.ToolStripMenuItem();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.progressBar1 = new System.Windows.Forms.ToolStripProgressBar();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_event)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_user)).BeginInit();
            this.context_EventLog.SuspendLayout();
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
            this.splitContainer1.Panel1.Controls.Add(this.tableLayoutPanel1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(1200, 485);
            this.splitContainer1.SplitterDistance = 27;
            this.splitContainer1.TabIndex = 0;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 73.56052F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 26.43948F));
            this.tableLayoutPanel1.Controls.Add(this.toolStrip1, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(1200, 27);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.toolStripTextBox_StadardTime,
            this.toolStripSeparator1,
            this.toolStripLabel2,
            this.toolStripTextBox_From,
            this.toolStripLabel4,
            this.toolStripTextBox_To,
            this.toolStripLabel3,
            this.toolStripButton_TableShow,
            this.toolStripSeparator3,
            this.progressBar1});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(882, 27);
            this.toolStrip1.TabIndex = 21;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(66, 24);
            this.toolStripLabel1.Text = "기준 시간: ";
            this.toolStripLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // toolStripTextBox_StadardTime
            // 
            this.toolStripTextBox_StadardTime.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.toolStripTextBox_StadardTime.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.toolStripTextBox_StadardTime.Name = "toolStripTextBox_StadardTime";
            this.toolStripTextBox_StadardTime.Size = new System.Drawing.Size(150, 27);
            this.toolStripTextBox_StadardTime.Text = " ";
            this.toolStripTextBox_StadardTime.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 27);
            // 
            // toolStripLabel2
            // 
            this.toolStripLabel2.Name = "toolStripLabel2";
            this.toolStripLabel2.Size = new System.Drawing.Size(0, 24);
            // 
            // toolStripTextBox_From
            // 
            this.toolStripTextBox_From.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.toolStripTextBox_From.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.toolStripTextBox_From.Name = "toolStripTextBox_From";
            this.toolStripTextBox_From.Size = new System.Drawing.Size(50, 27);
            this.toolStripTextBox_From.Text = " ";
            this.toolStripTextBox_From.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // toolStripLabel4
            // 
            this.toolStripLabel4.Name = "toolStripLabel4";
            this.toolStripLabel4.Size = new System.Drawing.Size(23, 24);
            this.toolStripLabel4.Text = " ~ ";
            // 
            // toolStripTextBox_To
            // 
            this.toolStripTextBox_To.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.toolStripTextBox_To.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.toolStripTextBox_To.Name = "toolStripTextBox_To";
            this.toolStripTextBox_To.Size = new System.Drawing.Size(50, 27);
            this.toolStripTextBox_To.Text = " ";
            this.toolStripTextBox_To.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // toolStripLabel3
            // 
            this.toolStripLabel3.Margin = new System.Windows.Forms.Padding(0, 1, 5, 2);
            this.toolStripLabel3.Name = "toolStripLabel3";
            this.toolStripLabel3.Size = new System.Drawing.Size(19, 24);
            this.toolStripLabel3.Text = "초";
            // 
            // toolStripButton_TableShow
            // 
            this.toolStripButton_TableShow.BackColor = System.Drawing.SystemColors.Control;
            this.toolStripButton_TableShow.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.toolStripButton_TableShow.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_TableShow.Image")));
            this.toolStripButton_TableShow.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripButton_TableShow.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_TableShow.Margin = new System.Windows.Forms.Padding(4, 1, 4, 2);
            this.toolStripButton_TableShow.Name = "toolStripButton_TableShow";
            this.toolStripButton_TableShow.Size = new System.Drawing.Size(170, 24);
            this.toolStripButton_TableShow.Text = "Amc Recv 테이블 열기";
            this.toolStripButton_TableShow.Click += new System.EventHandler(this.button_show_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 27);
            // 
            // splitContainer2
            // 
            this.splitContainer2.BackColor = System.Drawing.Color.Gray;
            this.splitContainer2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.dataGridView_event);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.dataGridView_user);
            this.splitContainer2.Size = new System.Drawing.Size(1200, 454);
            this.splitContainer2.SplitterDistance = 486;
            this.splitContainer2.SplitterIncrement = 2;
            this.splitContainer2.SplitterWidth = 8;
            this.splitContainer2.TabIndex = 0;
            this.splitContainer2.MouseUp += new System.Windows.Forms.MouseEventHandler(this.splitContainer2_MouseUp);
            // 
            // dataGridView_event
            // 
            this.dataGridView_event.AllowUserToAddRows = false;
            this.dataGridView_event.AllowUserToDeleteRows = false;
            this.dataGridView_event.AllowUserToResizeRows = false;
            this.dataGridView_event.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_event.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_event.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dataGridView_event.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_event.Name = "dataGridView_event";
            this.dataGridView_event.ReadOnly = true;
            this.dataGridView_event.RowTemplate.Height = 23;
            this.dataGridView_event.ShowCellErrors = false;
            this.dataGridView_event.ShowCellToolTips = false;
            this.dataGridView_event.ShowEditingIcon = false;
            this.dataGridView_event.ShowRowErrors = false;
            this.dataGridView_event.Size = new System.Drawing.Size(484, 452);
            this.dataGridView_event.TabIndex = 13;
            this.dataGridView_event.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridView_CellMouseDblClick);
            this.dataGridView_event.CurrentCellChanged += new System.EventHandler(this.dataGridView1_CurrentCellChanged);
            this.dataGridView_event.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView_event_KeyDown);
            this.dataGridView_event.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView_event_MouseDown);
            // 
            // dataGridView_user
            // 
            this.dataGridView_user.AllowUserToAddRows = false;
            this.dataGridView_user.AllowUserToDeleteRows = false;
            this.dataGridView_user.AllowUserToResizeRows = false;
            this.dataGridView_user.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_user.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_user.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dataGridView_user.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_user.Name = "dataGridView_user";
            this.dataGridView_user.ReadOnly = true;
            this.dataGridView_user.RowTemplate.Height = 23;
            this.dataGridView_user.ShowCellErrors = false;
            this.dataGridView_user.ShowCellToolTips = false;
            this.dataGridView_user.ShowEditingIcon = false;
            this.dataGridView_user.ShowRowErrors = false;
            this.dataGridView_user.Size = new System.Drawing.Size(704, 452);
            this.dataGridView_user.TabIndex = 13;
            this.dataGridView_user.CurrentCellChanged += new System.EventHandler(this.dataGridView1_CurrentCellChanged);
            this.dataGridView_user.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView_user_KeyDown);
            this.dataGridView_user.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView_user_MouseDown);
            // 
            // context_EventLog
            // 
            this.context_EventLog.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.틀고정ToolStripMenuItem,
            this.ToolStripMenuItem_Table});
            this.context_EventLog.Name = "contextMenuStrip1";
            this.context_EventLog.Size = new System.Drawing.Size(167, 48);
            // 
            // 틀고정ToolStripMenuItem
            // 
            this.틀고정ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_FixCol,
            this.ToolStripMenuItem_FixColDismiss});
            this.틀고정ToolStripMenuItem.Name = "틀고정ToolStripMenuItem";
            this.틀고정ToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
            this.틀고정ToolStripMenuItem.Text = "틀고정";
            // 
            // ToolStripMenuItem_FixCol
            // 
            this.ToolStripMenuItem_FixCol.Name = "ToolStripMenuItem_FixCol";
            this.ToolStripMenuItem_FixCol.Size = new System.Drawing.Size(142, 22);
            this.ToolStripMenuItem_FixCol.Text = "틀 고정";
            this.ToolStripMenuItem_FixCol.Click += new System.EventHandler(this.ToolStripMenuItem_FixCol_Click);
            // 
            // ToolStripMenuItem_FixColDismiss
            // 
            this.ToolStripMenuItem_FixColDismiss.Name = "ToolStripMenuItem_FixColDismiss";
            this.ToolStripMenuItem_FixColDismiss.Size = new System.Drawing.Size(142, 22);
            this.ToolStripMenuItem_FixColDismiss.Text = "틀 고정 해제";
            this.ToolStripMenuItem_FixColDismiss.Click += new System.EventHandler(this.ToolStripMenuItem_FixColDismiss_Click);
            // 
            // ToolStripMenuItem_Table
            // 
            this.ToolStripMenuItem_Table.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_TableConnect,
            this.ToolStripMenuItem_TableDisconnect});
            this.ToolStripMenuItem_Table.Name = "ToolStripMenuItem_Table";
            this.ToolStripMenuItem_Table.Size = new System.Drawing.Size(166, 22);
            this.ToolStripMenuItem_Table.Text = "우측 테이블 연결";
            // 
            // ToolStripMenuItem_TableConnect
            // 
            this.ToolStripMenuItem_TableConnect.Name = "ToolStripMenuItem_TableConnect";
            this.ToolStripMenuItem_TableConnect.Size = new System.Drawing.Size(98, 22);
            this.ToolStripMenuItem_TableConnect.Text = "연결";
            this.ToolStripMenuItem_TableConnect.Click += new System.EventHandler(this.ToolStripMenuItem_TableConnect_Click);
            // 
            // ToolStripMenuItem_TableDisconnect
            // 
            this.ToolStripMenuItem_TableDisconnect.Name = "ToolStripMenuItem_TableDisconnect";
            this.ToolStripMenuItem_TableDisconnect.Size = new System.Drawing.Size(98, 22);
            this.ToolStripMenuItem_TableDisconnect.Text = "해제";
            this.ToolStripMenuItem_TableDisconnect.Click += new System.EventHandler(this.ToolStripMenuItem_TableDisconnect_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // progressBar1
            // 
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(200, 24);
            this.progressBar1.Step = 1;
            // 
            // UEventLog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1200, 485);
            this.Controls.Add(this.splitContainer1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "UEventLog";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ULogEvent_FormClosing);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_event)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_user)).EndInit();
            this.context_EventLog.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        public System.Windows.Forms.DataGridView dataGridView_event;
        public System.Windows.Forms.DataGridView dataGridView_user;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_StadardTime;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_From;
        private System.Windows.Forms.ToolStripLabel toolStripLabel4;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_To;
        private System.Windows.Forms.ToolStripLabel toolStripLabel3;
        private System.Windows.Forms.ToolStripButton toolStripButton_TableShow;
        private System.Windows.Forms.ContextMenuStrip context_EventLog;
        private System.Windows.Forms.ToolStripMenuItem 틀고정ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FixCol;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_FixColDismiss;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Table;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_TableConnect;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_TableDisconnect;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ToolStripProgressBar progressBar1;
    }
}