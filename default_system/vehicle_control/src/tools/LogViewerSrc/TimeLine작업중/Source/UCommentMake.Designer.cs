namespace LogViewer
{
    partial class UCommentMake
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UCommentMake));
            this.textBox_Comment = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_LogFile = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_SelectedRange = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_Author = new System.Windows.Forms.TextBox();
            this.textBox_AmcRecvCols = new System.Windows.Forms.TextBox();
            this.textBox_AmcRecvTime = new System.Windows.Forms.TextBox();
            this.checkBox_UseLogPos = new System.Windows.Forms.CheckBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_AttachedFile = new System.Windows.Forms.TextBox();
            this.btn_FindAttached = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox_Comment
            // 
            this.textBox_Comment.Location = new System.Drawing.Point(83, 35);
            this.textBox_Comment.Multiline = true;
            this.textBox_Comment.Name = "textBox_Comment";
            this.textBox_Comment.Size = new System.Drawing.Size(366, 77);
            this.textBox_Comment.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "코멘트";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 211);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(54, 17);
            this.label2.TabIndex = 4;
            this.label2.Text = "Log File";
            // 
            // textBox_LogFile
            // 
            this.textBox_LogFile.Location = new System.Drawing.Point(83, 211);
            this.textBox_LogFile.Name = "textBox_LogFile";
            this.textBox_LogFile.ReadOnly = true;
            this.textBox_LogFile.Size = new System.Drawing.Size(157, 25);
            this.textBox_LogFile.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(240, 214);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(46, 17);
            this.label3.TabIndex = 6;
            this.label3.Text = "Range";
            // 
            // textBox_SelectedRange
            // 
            this.textBox_SelectedRange.Location = new System.Drawing.Point(292, 211);
            this.textBox_SelectedRange.Name = "textBox_SelectedRange";
            this.textBox_SelectedRange.ReadOnly = true;
            this.textBox_SelectedRange.Size = new System.Drawing.Size(157, 25);
            this.textBox_SelectedRange.TabIndex = 3;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 118);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(47, 17);
            this.label4.TabIndex = 8;
            this.label4.Text = "작성자";
            // 
            // textBox_Author
            // 
            this.textBox_Author.Location = new System.Drawing.Point(83, 118);
            this.textBox_Author.Name = "textBox_Author";
            this.textBox_Author.Size = new System.Drawing.Size(157, 25);
            this.textBox_Author.TabIndex = 1;
            // 
            // textBox_AmcRecvCols
            // 
            this.textBox_AmcRecvCols.Location = new System.Drawing.Point(277, 127);
            this.textBox_AmcRecvCols.Name = "textBox_AmcRecvCols";
            this.textBox_AmcRecvCols.ReadOnly = true;
            this.textBox_AmcRecvCols.Size = new System.Drawing.Size(163, 25);
            this.textBox_AmcRecvCols.TabIndex = 4;
            this.textBox_AmcRecvCols.Text = "AMC_RecvCols 숨김";
            // 
            // textBox_AmcRecvTime
            // 
            this.textBox_AmcRecvTime.Location = new System.Drawing.Point(277, 158);
            this.textBox_AmcRecvTime.Name = "textBox_AmcRecvTime";
            this.textBox_AmcRecvTime.ReadOnly = true;
            this.textBox_AmcRecvTime.Size = new System.Drawing.Size(163, 25);
            this.textBox_AmcRecvTime.TabIndex = 24;
            this.textBox_AmcRecvTime.Text = "AMC_Recv Time 숨김";
            // 
            // checkBox_UseLogPos
            // 
            this.checkBox_UseLogPos.AutoSize = true;
            this.checkBox_UseLogPos.Checked = true;
            this.checkBox_UseLogPos.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_UseLogPos.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.checkBox_UseLogPos.Location = new System.Drawing.Point(83, 184);
            this.checkBox_UseLogPos.Name = "checkBox_UseLogPos";
            this.checkBox_UseLogPos.Size = new System.Drawing.Size(106, 21);
            this.checkBox_UseLogPos.TabIndex = 25;
            this.checkBox_UseLogPos.Text = "Log Position";
            this.checkBox_UseLogPos.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 242);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(60, 17);
            this.label5.TabIndex = 27;
            this.label5.Text = "첨부파일";
            // 
            // textBox_AttachedFile
            // 
            this.textBox_AttachedFile.Location = new System.Drawing.Point(83, 242);
            this.textBox_AttachedFile.Name = "textBox_AttachedFile";
            this.textBox_AttachedFile.ReadOnly = true;
            this.textBox_AttachedFile.Size = new System.Drawing.Size(302, 25);
            this.textBox_AttachedFile.TabIndex = 26;
            // 
            // btn_FindAttached
            // 
            this.btn_FindAttached.BackColor = System.Drawing.Color.White;
            this.btn_FindAttached.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btn_FindAttached.Location = new System.Drawing.Point(391, 242);
            this.btn_FindAttached.Name = "btn_FindAttached";
            this.btn_FindAttached.Size = new System.Drawing.Size(57, 25);
            this.btn_FindAttached.TabIndex = 28;
            this.btn_FindAttached.Text = "...";
            this.btn_FindAttached.UseVisualStyleBackColor = false;
            this.btn_FindAttached.Click += new System.EventHandler(this.btn_FindAttached_Click);
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.Color.White;
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button1.Image = ((System.Drawing.Image)(resources.GetObject("button1.Image")));
            this.button1.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button1.Location = new System.Drawing.Point(243, 284);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(94, 41);
            this.button1.TabIndex = 23;
            this.button1.Text = "Discard";
            this.button1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button1.UseVisualStyleBackColor = false;
            this.button1.Click += new System.EventHandler(this.btn_Discard_Click);
            // 
            // button2
            // 
            this.button2.BackColor = System.Drawing.Color.White;
            this.button2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button2.Image = ((System.Drawing.Image)(resources.GetObject("button2.Image")));
            this.button2.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button2.Location = new System.Drawing.Point(137, 284);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(94, 41);
            this.button2.TabIndex = 22;
            this.button2.Text = "Apply";
            this.button2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button2.UseVisualStyleBackColor = false;
            this.button2.Click += new System.EventHandler(this.btn_Apply_Click);
            // 
            // UCommentMake
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(464, 335);
            this.Controls.Add(this.btn_FindAttached);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textBox_AttachedFile);
            this.Controls.Add(this.checkBox_UseLogPos);
            this.Controls.Add(this.textBox_AmcRecvTime);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.textBox_AmcRecvCols);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_Author);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_SelectedRange);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_LogFile);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox_Comment);
            this.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UCommentMake";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Make Comment";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UCommentMake_FormClosing);
            this.Load += new System.EventHandler(this.UCommentMake_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_Comment;
        public System.Windows.Forms.TextBox textBox_LogFile;
        public System.Windows.Forms.TextBox textBox_SelectedRange;
        public System.Windows.Forms.TextBox textBox_Author;
        public System.Windows.Forms.TextBox textBox_AmcRecvCols;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        public System.Windows.Forms.TextBox textBox_AmcRecvTime;
        public System.Windows.Forms.CheckBox checkBox_UseLogPos;
        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.TextBox textBox_AttachedFile;
        private System.Windows.Forms.Button btn_FindAttached;
    }
}