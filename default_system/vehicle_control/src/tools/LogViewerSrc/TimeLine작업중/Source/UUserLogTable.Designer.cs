namespace LogViewer
{
    partial class UUserLogTable
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.panel1 = new System.Windows.Forms.Panel();
            this.dataGridView_defUser = new System.Windows.Forms.DataGridView();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.label_selectedItemCnt = new System.Windows.Forms.ToolStripStatusLabel();
            this.label_selectedSum = new System.Windows.Forms.ToolStripStatusLabel();
            this.label_spanTime = new System.Windows.Forms.ToolStripStatusLabel();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.찾기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.열선택ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.timer_ScrollTimer = new System.Windows.Forms.Timer(this.components);
            this.선택된노드맵뷰에서찾기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_defUser)).BeginInit();
            this.statusStrip1.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.dataGridView_defUser);
            this.panel1.Controls.Add(this.statusStrip1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(790, 564);
            this.panel1.TabIndex = 0;
            // 
            // dataGridView_defUser
            // 
            this.dataGridView_defUser.AllowUserToAddRows = false;
            this.dataGridView_defUser.AllowUserToDeleteRows = false;
            this.dataGridView_defUser.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_defUser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_defUser.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.dataGridView_defUser.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_defUser.Name = "dataGridView_defUser";
            this.dataGridView_defUser.RowHeadersWidth = 60;
            this.dataGridView_defUser.RowTemplate.Height = 23;
            this.dataGridView_defUser.ShowCellErrors = false;
            this.dataGridView_defUser.ShowCellToolTips = false;
            this.dataGridView_defUser.ShowEditingIcon = false;
            this.dataGridView_defUser.ShowRowErrors = false;
            this.dataGridView_defUser.Size = new System.Drawing.Size(790, 542);
            this.dataGridView_defUser.TabIndex = 1;
            this.dataGridView_defUser.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_defUser_CellDoubleClick);
            this.dataGridView_defUser.CurrentCellChanged += new System.EventHandler(this.dataGridView_CurrentCellChanged);
            this.dataGridView_defUser.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dataGridView_defUser_KeyDown);
            this.dataGridView_defUser.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dataGridView_defUser_MouseDown);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.label_selectedItemCnt,
            this.label_selectedSum,
            this.label_spanTime});
            this.statusStrip1.Location = new System.Drawing.Point(0, 542);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(790, 22);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // label_selectedItemCnt
            // 
            this.label_selectedItemCnt.BackColor = System.Drawing.Color.Gainsboro;
            this.label_selectedItemCnt.Margin = new System.Windows.Forms.Padding(0, 3, 6, 2);
            this.label_selectedItemCnt.Name = "label_selectedItemCnt";
            this.label_selectedItemCnt.Size = new System.Drawing.Size(94, 17);
            this.label_selectedItemCnt.Text = "Selected Item: 1";
            // 
            // label_selectedSum
            // 
            this.label_selectedSum.BackColor = System.Drawing.Color.Gainsboro;
            this.label_selectedSum.Margin = new System.Windows.Forms.Padding(0, 3, 6, 2);
            this.label_selectedSum.Name = "label_selectedSum";
            this.label_selectedSum.Size = new System.Drawing.Size(110, 17);
            this.label_selectedSum.Text = "Seleted Sum: 1000";
            // 
            // label_spanTime
            // 
            this.label_spanTime.BackColor = System.Drawing.Color.Gainsboro;
            this.label_spanTime.Margin = new System.Windows.Forms.Padding(0, 3, 6, 2);
            this.label_spanTime.Name = "label_spanTime";
            this.label_spanTime.Size = new System.Drawing.Size(113, 17);
            this.label_spanTime.Text = "Span Time: 100 sec";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.찾기ToolStripMenuItem,
            this.열선택ToolStripMenuItem,
            this.선택된노드맵뷰에서찾기ToolStripMenuItem,
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(243, 114);
            // 
            // 찾기ToolStripMenuItem
            // 
            this.찾기ToolStripMenuItem.Name = "찾기ToolStripMenuItem";
            this.찾기ToolStripMenuItem.Size = new System.Drawing.Size(242, 22);
            this.찾기ToolStripMenuItem.Text = "찾기";
            this.찾기ToolStripMenuItem.Click += new System.EventHandler(this.찾기ToolStripMenuItem_Click);
            // 
            // 열선택ToolStripMenuItem
            // 
            this.열선택ToolStripMenuItem.Name = "열선택ToolStripMenuItem";
            this.열선택ToolStripMenuItem.Size = new System.Drawing.Size(242, 22);
            this.열선택ToolStripMenuItem.Text = "열 선택";
            this.열선택ToolStripMenuItem.Click += new System.EventHandler(this.열선택ToolStripMenuItem_Click);
            // 
            // timer_ScrollTimer
            // 
            this.timer_ScrollTimer.Enabled = true;
            this.timer_ScrollTimer.Tick += new System.EventHandler(this.timer1_ScrollTimer);
            // 
            // 선택된노드맵뷰에서찾기ToolStripMenuItem
            // 
            this.선택된노드맵뷰에서찾기ToolStripMenuItem.Name = "선택된노드맵뷰에서찾기ToolStripMenuItem";
            this.선택된노드맵뷰에서찾기ToolStripMenuItem.Size = new System.Drawing.Size(242, 22);
            this.선택된노드맵뷰에서찾기ToolStripMenuItem.Text = "선택된 노드 맵뷰에서 찾기";
            this.선택된노드맵뷰에서찾기ToolStripMenuItem.Click += new System.EventHandler(this.선택된노드맵뷰에서찾기ToolStripMenuItem_Click);
            // 
            // 선택된스테이션맵뷰에서찾기ToolStripMenuItem
            // 
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem.Name = "선택된스테이션맵뷰에서찾기ToolStripMenuItem";
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem.Size = new System.Drawing.Size(242, 22);
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem.Text = "선택된 스테이션 맵뷰에서 찾기";
            this.선택된스테이션맵뷰에서찾기ToolStripMenuItem.Click += new System.EventHandler(this.선택된스테이션맵뷰에서찾기ToolStripMenuItem_Click);
            // 
            // UUserLogTable
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(790, 564);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "UUserLogTable";
            this.Text = "UUserLogTable";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_defUser)).EndInit();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 찾기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel label_selectedItemCnt;
        private System.Windows.Forms.ToolStripStatusLabel label_selectedSum;
        private System.Windows.Forms.ToolStripStatusLabel label_spanTime;
        private System.Windows.Forms.ToolStripMenuItem 열선택ToolStripMenuItem;
        private System.Windows.Forms.Timer timer_ScrollTimer;
        public System.Windows.Forms.DataGridView dataGridView_defUser;
        private System.Windows.Forms.ToolStripMenuItem 선택된노드맵뷰에서찾기ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 선택된스테이션맵뷰에서찾기ToolStripMenuItem;
    }
}