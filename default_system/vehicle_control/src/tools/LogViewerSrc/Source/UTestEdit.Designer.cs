namespace LogViewer
{
    partial class UTestEdit
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
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem("Test1");
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem("Test3");
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem("test1");
            this.listView1 = new System.Windows.Forms.ListView();
            this.textBox_name = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button_addnew = new System.Windows.Forms.Button();
            this.checkBox_AutoMode = new System.Windows.Forms.CheckBox();
            this.textBox_query = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_iconName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.button_IconFileDialog = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_grouppingCnt = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_ignoreCnt = new System.Windows.Forms.TextBox();
            this.textBox_help = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.textBox_desc = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.button_adjust = new System.Windows.Forms.Button();
            this.button_delete = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            listViewItem4.StateImageIndex = 0;
            listViewItem5.StateImageIndex = 0;
            listViewItem6.StateImageIndex = 0;
            this.listView1.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem4,
            listViewItem5,
            listViewItem6});
            this.listView1.Location = new System.Drawing.Point(23, 85);
            this.listView1.MultiSelect = false;
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(301, 363);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            // 
            // textBox_name
            // 
            this.textBox_name.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_name.Location = new System.Drawing.Point(471, 83);
            this.textBox_name.Name = "textBox_name";
            this.textBox_name.Size = new System.Drawing.Size(192, 23);
            this.textBox_name.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(406, 86);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 15);
            this.label1.TabIndex = 3;
            this.label1.Text = "검사 이름";
            // 
            // button_addnew
            // 
            this.button_addnew.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_addnew.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_addnew.Location = new System.Drawing.Point(542, 412);
            this.button_addnew.Name = "button_addnew";
            this.button_addnew.Size = new System.Drawing.Size(121, 36);
            this.button_addnew.TabIndex = 4;
            this.button_addnew.Text = "새 항목으로 추가";
            this.button_addnew.UseVisualStyleBackColor = true;
            this.button_addnew.Click += new System.EventHandler(this.button_addnew_Click);
            // 
            // checkBox_AutoMode
            // 
            this.checkBox_AutoMode.AutoSize = true;
            this.checkBox_AutoMode.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.checkBox_AutoMode.Location = new System.Drawing.Point(471, 304);
            this.checkBox_AutoMode.Name = "checkBox_AutoMode";
            this.checkBox_AutoMode.Size = new System.Drawing.Size(116, 19);
            this.checkBox_AutoMode.TabIndex = 5;
            this.checkBox_AutoMode.Text = "Auto Mode Only";
            this.checkBox_AutoMode.UseVisualStyleBackColor = true;
            // 
            // textBox_query
            // 
            this.textBox_query.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_query.Location = new System.Drawing.Point(471, 112);
            this.textBox_query.Multiline = true;
            this.textBox_query.Name = "textBox_query";
            this.textBox_query.Size = new System.Drawing.Size(192, 60);
            this.textBox_query.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(366, 115);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(99, 15);
            this.label2.TabIndex = 3;
            this.label2.Text = "검사 조건(Query)";
            // 
            // textBox_iconName
            // 
            this.textBox_iconName.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_iconName.Location = new System.Drawing.Point(471, 205);
            this.textBox_iconName.Name = "textBox_iconName";
            this.textBox_iconName.Size = new System.Drawing.Size(162, 23);
            this.textBox_iconName.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.Location = new System.Drawing.Point(394, 208);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 15);
            this.label3.TabIndex = 3;
            this.label3.Text = "항목 아이콘";
            // 
            // button_IconFileDialog
            // 
            this.button_IconFileDialog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_IconFileDialog.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_IconFileDialog.Location = new System.Drawing.Point(639, 205);
            this.button_IconFileDialog.Name = "button_IconFileDialog";
            this.button_IconFileDialog.Size = new System.Drawing.Size(24, 23);
            this.button_IconFileDialog.TabIndex = 7;
            this.button_IconFileDialog.Text = "...";
            this.button_IconFileDialog.UseVisualStyleBackColor = true;
            this.button_IconFileDialog.Click += new System.EventHandler(this.button_IconFileDialog_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label4.Location = new System.Drawing.Point(330, 242);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(135, 15);
            this.label4.TabIndex = 9;
            this.label4.Text = "Grouping Tolerance Cnt";
            // 
            // textBox_grouppingCnt
            // 
            this.textBox_grouppingCnt.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_grouppingCnt.Location = new System.Drawing.Point(471, 234);
            this.textBox_grouppingCnt.Name = "textBox_grouppingCnt";
            this.textBox_grouppingCnt.Size = new System.Drawing.Size(72, 23);
            this.textBox_grouppingCnt.TabIndex = 8;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label5.Location = new System.Drawing.Point(394, 271);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(64, 15);
            this.label5.TabIndex = 11;
            this.label5.Text = "Ignore Cnt";
            // 
            // textBox_ignoreCnt
            // 
            this.textBox_ignoreCnt.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_ignoreCnt.Location = new System.Drawing.Point(471, 263);
            this.textBox_ignoreCnt.Name = "textBox_ignoreCnt";
            this.textBox_ignoreCnt.Size = new System.Drawing.Size(72, 23);
            this.textBox_ignoreCnt.TabIndex = 10;
            // 
            // textBox_help
            // 
            this.textBox_help.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_help.Location = new System.Drawing.Point(471, 338);
            this.textBox_help.Multiline = true;
            this.textBox_help.Name = "textBox_help";
            this.textBox_help.ReadOnly = true;
            this.textBox_help.Size = new System.Drawing.Size(192, 60);
            this.textBox_help.TabIndex = 12;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label6.Location = new System.Drawing.Point(427, 179);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(31, 15);
            this.label6.TabIndex = 14;
            this.label6.Text = "설명";
            // 
            // textBox_desc
            // 
            this.textBox_desc.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_desc.Location = new System.Drawing.Point(471, 176);
            this.textBox_desc.Name = "textBox_desc";
            this.textBox_desc.Size = new System.Drawing.Size(192, 23);
            this.textBox_desc.TabIndex = 13;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label7.Location = new System.Drawing.Point(408, 341);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(50, 15);
            this.label7.TabIndex = 14;
            this.label7.Text = "도움말 :";
            // 
            // button_adjust
            // 
            this.button_adjust.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_adjust.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_adjust.Location = new System.Drawing.Point(471, 412);
            this.button_adjust.Name = "button_adjust";
            this.button_adjust.Size = new System.Drawing.Size(65, 36);
            this.button_adjust.TabIndex = 4;
            this.button_adjust.Text = "수정";
            this.button_adjust.UseVisualStyleBackColor = true;
            this.button_adjust.Click += new System.EventHandler(this.button_modify_Click);
            // 
            // button_delete
            // 
            this.button_delete.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button_delete.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_delete.Location = new System.Drawing.Point(401, 412);
            this.button_delete.Name = "button_delete";
            this.button_delete.Size = new System.Drawing.Size(64, 36);
            this.button_delete.TabIndex = 4;
            this.button_delete.Text = "삭제";
            this.button_delete.UseVisualStyleBackColor = true;
            this.button_delete.Click += new System.EventHandler(this.button_delete_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label8.Location = new System.Drawing.Point(20, 25);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(320, 17);
            this.label8.TabIndex = 3;
            this.label8.Text = "검사 항목들을 편집하거나 추가/삭제 할 수 있습니다.";
            // 
            // UTestEdit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(686, 460);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.textBox_desc);
            this.Controls.Add(this.textBox_help);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textBox_ignoreCnt);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_grouppingCnt);
            this.Controls.Add(this.button_IconFileDialog);
            this.Controls.Add(this.checkBox_AutoMode);
            this.Controls.Add(this.button_delete);
            this.Controls.Add(this.button_adjust);
            this.Controls.Add(this.button_addnew);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox_query);
            this.Controls.Add(this.textBox_iconName);
            this.Controls.Add(this.textBox_name);
            this.Controls.Add(this.listView1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UTestEdit";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Test Edit";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.TextBox textBox_name;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_addnew;
        private System.Windows.Forms.CheckBox checkBox_AutoMode;
        private System.Windows.Forms.TextBox textBox_query;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox_iconName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button_IconFileDialog;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBox_grouppingCnt;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox_ignoreCnt;
        private System.Windows.Forms.TextBox textBox_help;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBox_desc;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button button_adjust;
        private System.Windows.Forms.Button button_delete;
        private System.Windows.Forms.Label label8;
    }
}