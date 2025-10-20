namespace LogViewer
{
    partial class UNotebook
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UNotebook));
			this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.copyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.panel1 = new System.Windows.Forms.Panel();
			this.richTextBox1 = new System.Windows.Forms.RichTextBox();
			this.statusStrip1 = new System.Windows.Forms.StatusStrip();
			this.label_totalLineCnt = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_CurLineNo = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_selectedLineCnt = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_timeSpan = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_modeStat = new System.Windows.Forms.ToolStripStatusLabel();
			this.label_Speed = new System.Windows.Forms.ToolStripStatusLabel();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
			this.textBox_Find = new System.Windows.Forms.ToolStripTextBox();
			this.btn_Find = new System.Windows.Forms.ToolStripButton();
			this.btn_FindMatchCase = new System.Windows.Forms.ToolStripButton();
			this.btn_findWholeWord = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.btn_Path = new System.Windows.Forms.ToolStripButton();
			this.label_path = new System.Windows.Forms.ToolStripLabel();
			this.timer_Scroll = new System.Windows.Forms.Timer(this.components);
			this.panel1.SuspendLayout();
			this.statusStrip1.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// contextMenuStrip1
			// 
			this.contextMenuStrip1.Name = "contextMenuStrip1";
			this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
			// 
			// copyToolStripMenuItem
			// 
			this.copyToolStripMenuItem.Name = "copyToolStripMenuItem";
			this.copyToolStripMenuItem.Size = new System.Drawing.Size(102, 22);
			this.copyToolStripMenuItem.Text = "Copy";
			this.copyToolStripMenuItem.Click += new System.EventHandler(this.copyToolStripMenuItem_Click);
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.richTextBox1);
			this.panel1.Controls.Add(this.statusStrip1);
			this.panel1.Controls.Add(this.toolStrip1);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(803, 442);
			this.panel1.TabIndex = 5;
			// 
			// richTextBox1
			// 
			this.richTextBox1.BackColor = System.Drawing.Color.White;
			this.richTextBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.richTextBox1.DetectUrls = false;
			this.richTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.richTextBox1.Location = new System.Drawing.Point(0, 25);
			this.richTextBox1.Name = "richTextBox1";
			this.richTextBox1.ReadOnly = true;
			this.richTextBox1.Size = new System.Drawing.Size(803, 395);
			this.richTextBox1.TabIndex = 5;
			this.richTextBox1.Text = "Rich text box ^^";
			this.richTextBox1.WordWrap = false;
			this.richTextBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.richTextBox1_KeyDown);
			this.richTextBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.richTextBox1_MouseDown);
			// 
			// statusStrip1
			// 
			this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.label_totalLineCnt,
            this.label_CurLineNo,
            this.label_selectedLineCnt,
            this.label_timeSpan,
            this.label_modeStat,
            this.label_Speed});
			this.statusStrip1.Location = new System.Drawing.Point(0, 420);
			this.statusStrip1.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
			this.statusStrip1.Name = "statusStrip1";
			this.statusStrip1.Size = new System.Drawing.Size(803, 22);
			this.statusStrip1.TabIndex = 4;
			this.statusStrip1.Text = "statusStrip1";
			// 
			// label_totalLineCnt
			// 
			this.label_totalLineCnt.Margin = new System.Windows.Forms.Padding(8, 3, 8, 2);
			this.label_totalLineCnt.Name = "label_totalLineCnt";
			this.label_totalLineCnt.Size = new System.Drawing.Size(61, 17);
			this.label_totalLineCnt.Text = "Total Line";
			// 
			// label_CurLineNo
			// 
			this.label_CurLineNo.Margin = new System.Windows.Forms.Padding(8, 3, 8, 2);
			this.label_CurLineNo.Name = "label_CurLineNo";
			this.label_CurLineNo.Size = new System.Drawing.Size(53, 17);
			this.label_CurLineNo.Text = "Cur Line";
			// 
			// label_selectedLineCnt
			// 
			this.label_selectedLineCnt.Margin = new System.Windows.Forms.Padding(8, 3, 8, 2);
			this.label_selectedLineCnt.Name = "label_selectedLineCnt";
			this.label_selectedLineCnt.Size = new System.Drawing.Size(105, 17);
			this.label_selectedLineCnt.Text = "Selected Line Cnt";
			// 
			// label_timeSpan
			// 
			this.label_timeSpan.Name = "label_timeSpan";
			this.label_timeSpan.Size = new System.Drawing.Size(120, 17);
			this.label_timeSpan.Text = "Selected Time Span";
			// 
			// label_modeStat
			// 
			this.label_modeStat.Margin = new System.Windows.Forms.Padding(8, 3, 8, 2);
			this.label_modeStat.Name = "label_modeStat";
			this.label_modeStat.Size = new System.Drawing.Size(63, 17);
			this.label_modeStat.Text = "mode_stat";
			// 
			// label_Speed
			// 
			this.label_Speed.Margin = new System.Windows.Forms.Padding(8, 3, 8, 2);
			this.label_Speed.Name = "label_Speed";
			this.label_Speed.Size = new System.Drawing.Size(40, 17);
			this.label_Speed.Text = "speed";
			// 
			// toolStrip1
			// 
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.textBox_Find,
            this.btn_Find,
            this.btn_FindMatchCase,
            this.btn_findWholeWord,
            this.toolStripSeparator2,
            this.btn_Path,
            this.label_path});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(803, 25);
			this.toolStrip1.TabIndex = 2;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// toolStripLabel1
			// 
			this.toolStripLabel1.Name = "toolStripLabel1";
			this.toolStripLabel1.Size = new System.Drawing.Size(38, 22);
			this.toolStripLabel1.Text = "Text :";
			// 
			// textBox_Find
			// 
			this.textBox_Find.Name = "textBox_Find";
			this.textBox_Find.Size = new System.Drawing.Size(150, 25);
			this.textBox_Find.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_Find_KeyDown);
			// 
			// btn_Find
			// 
			this.btn_Find.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.btn_Find.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.btn_Find.Image = ((System.Drawing.Image)(resources.GetObject("btn_Find.Image")));
			this.btn_Find.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Find.Name = "btn_Find";
			this.btn_Find.Size = new System.Drawing.Size(49, 22);
			this.btn_Find.Text = "  Find  ";
			this.btn_Find.ToolTipText = "  Find  ";
			this.btn_Find.Click += new System.EventHandler(this.toolStripBtnFind_Click);
			// 
			// btn_FindMatchCase
			// 
			this.btn_FindMatchCase.BackColor = System.Drawing.SystemColors.Control;
			this.btn_FindMatchCase.CheckOnClick = true;
			this.btn_FindMatchCase.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.btn_FindMatchCase.Image = ((System.Drawing.Image)(resources.GetObject("btn_FindMatchCase.Image")));
			this.btn_FindMatchCase.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_FindMatchCase.Name = "btn_FindMatchCase";
			this.btn_FindMatchCase.Size = new System.Drawing.Size(78, 22);
			this.btn_FindMatchCase.Text = "Match Case";
			this.btn_FindMatchCase.ToolTipText = "대소문자 구분";
			// 
			// btn_findWholeWord
			// 
			this.btn_findWholeWord.BackColor = System.Drawing.SystemColors.Control;
			this.btn_findWholeWord.CheckOnClick = true;
			this.btn_findWholeWord.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.btn_findWholeWord.Image = ((System.Drawing.Image)(resources.GetObject("btn_findWholeWord.Image")));
			this.btn_findWholeWord.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_findWholeWord.Name = "btn_findWholeWord";
			this.btn_findWholeWord.Size = new System.Drawing.Size(82, 22);
			this.btn_findWholeWord.Text = "Whole Match";
			this.btn_findWholeWord.ToolTipText = "단어 단위 검색";
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			// 
			// btn_Path
			// 
			this.btn_Path.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.btn_Path.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.btn_Path.Image = ((System.Drawing.Image)(resources.GetObject("btn_Path.Image")));
			this.btn_Path.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Path.Margin = new System.Windows.Forms.Padding(3, 1, 3, 2);
			this.btn_Path.Name = "btn_Path";
			this.btn_Path.Size = new System.Drawing.Size(34, 22);
			this.btn_Path.Text = "Path";
			this.btn_Path.Click += new System.EventHandler(this.btn_Path_Click);
			// 
			// label_path
			// 
			this.label_path.Name = "label_path";
			this.label_path.Size = new System.Drawing.Size(70, 22);
			this.label_path.Text = ".... Path ....";
			// 
			// timer_Scroll
			// 
			this.timer_Scroll.Enabled = true;
			this.timer_Scroll.Tick += new System.EventHandler(this.timer_Scroll_Tick);
			// 
			// UNotebook
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.ClientSize = new System.Drawing.Size(803, 442);
			this.Controls.Add(this.panel1);
			this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.Name = "UNotebook";
			this.Text = "Unote";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UNotebook_FormClosing);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.statusStrip1.ResumeLayout(false);
			this.statusStrip1.PerformLayout();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem copyToolStripMenuItem;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel label_totalLineCnt;
        private System.Windows.Forms.ToolStripStatusLabel label_CurLineNo;
        private System.Windows.Forms.ToolStripStatusLabel label_selectedLineCnt;
        private System.Windows.Forms.ToolStripStatusLabel label_timeSpan;
        private System.Windows.Forms.ToolStripStatusLabel label_modeStat;
        private System.Windows.Forms.ToolStripStatusLabel label_Speed;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripTextBox textBox_Find;
        private System.Windows.Forms.ToolStripButton btn_Find;
        private System.Windows.Forms.ToolStripButton btn_FindMatchCase;
        private System.Windows.Forms.ToolStripButton btn_findWholeWord;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btn_Path;
        private System.Windows.Forms.ToolStripLabel label_path;
        private System.Windows.Forms.Timer timer_Scroll;
    }
}