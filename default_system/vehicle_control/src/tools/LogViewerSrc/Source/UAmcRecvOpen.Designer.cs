namespace LogViewer
{
    partial class UAmcRecvOpen
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UAmcRecvOpen));
            this.label4 = new System.Windows.Forms.Label();
            this.textBox_time = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox_before = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox_after = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btn_Discard = new System.Windows.Forms.Button();
            this.btn_Apply = new System.Windows.Forms.Button();
            this.CheckGearRatio = new System.Windows.Forms.CheckBox();
            this.Axis1 = new System.Windows.Forms.TextBox();
            this.Axis2 = new System.Windows.Forms.TextBox();
            //this.Axis3 = new System.Windows.Forms.TextBox();
            this.Axis4 = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            //this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(30, 60);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 15);
            this.label4.TabIndex = 12;
            this.label4.Text = "시간";
            // 
            // textBox_time
            // 
            this.textBox_time.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_time.Location = new System.Drawing.Point(99, 56);
            this.textBox_time.Name = "textBox_time";
            this.textBox_time.Size = new System.Drawing.Size(157, 25);
            this.textBox_time.TabIndex = 10;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(30, 98);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 15);
            this.label2.TabIndex = 14;
            this.label2.Text = "이전";
            // 
            // textBox_before
            // 
            this.textBox_before.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_before.Location = new System.Drawing.Point(99, 94);
            this.textBox_before.Name = "textBox_before";
            this.textBox_before.Size = new System.Drawing.Size(102, 25);
            this.textBox_before.TabIndex = 13;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(30, 127);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(31, 15);
            this.label3.TabIndex = 16;
            this.label3.Text = "이후";
            // 
            // textBox_after
            // 
            this.textBox_after.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_after.Location = new System.Drawing.Point(99, 123);
            this.textBox_after.Name = "textBox_after";
            this.textBox_after.Size = new System.Drawing.Size(102, 25);
            this.textBox_after.TabIndex = 15;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(207, 102);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(27, 15);
            this.label5.TabIndex = 17;
            this.label5.Text = "[초]";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(207, 131);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(27, 15);
            this.label6.TabIndex = 18;
            this.label6.Text = "[초]";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(30, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(168, 17);
            this.label1.TabIndex = 19;
            this.label1.Text = "AMC Recv Loading Option";
            // 
            // btn_Discard
            // 
            this.btn_Discard.BackColor = System.Drawing.Color.White;
            this.btn_Discard.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Discard.Image = ((System.Drawing.Image)(resources.GetObject("btn_Discard.Image")));
            this.btn_Discard.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btn_Discard.Location = new System.Drawing.Point(194, 255);
            this.btn_Discard.Name = "btn_Discard";
            this.btn_Discard.Size = new System.Drawing.Size(89, 41);
            this.btn_Discard.TabIndex = 21;
            this.btn_Discard.Text = "Discard";
            this.btn_Discard.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btn_Discard.UseVisualStyleBackColor = false;
            this.btn_Discard.Click += new System.EventHandler(this.btn_Discard_Click);
            // 
            // btn_Apply
            // 
            this.btn_Apply.BackColor = System.Drawing.Color.White;
            this.btn_Apply.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Apply.Image = ((System.Drawing.Image)(resources.GetObject("btn_Apply.Image")));
            this.btn_Apply.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btn_Apply.Location = new System.Drawing.Point(33, 255);
            this.btn_Apply.Name = "btn_Apply";
            this.btn_Apply.Size = new System.Drawing.Size(89, 41);
            this.btn_Apply.TabIndex = 20;
            this.btn_Apply.Text = "Apply";
            this.btn_Apply.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btn_Apply.UseVisualStyleBackColor = false;
            this.btn_Apply.Click += new System.EventHandler(this.btn_Apply_Click);
            // 
            // CheckGearRatio
            // 
            this.CheckGearRatio.AutoSize = true;
            this.CheckGearRatio.Location = new System.Drawing.Point(33, 160);
            this.CheckGearRatio.Name = "CheckGearRatio";
            this.CheckGearRatio.Size = new System.Drawing.Size(92, 19);
            this.CheckGearRatio.TabIndex = 22;
            this.CheckGearRatio.Text = "Gear Setting";
            this.CheckGearRatio.UseVisualStyleBackColor = true;
            // 
            // Axis1
            // 
            this.Axis1.Location = new System.Drawing.Point(33, 206);
            this.Axis1.Name = "Axis1";
            this.Axis1.Size = new System.Drawing.Size(72, 23);
            this.Axis1.TabIndex = 23;
            // 
            // Axis2
            // 
            this.Axis2.Location = new System.Drawing.Point(122, 206);
            this.Axis2.Name = "Axis2";
            this.Axis2.Size = new System.Drawing.Size(72, 23);
            this.Axis2.TabIndex = 24;
            // 
            // Axis3
            // 
            //this.Axis3.Location = new System.Drawing.Point(168, 206);
            //this.Axis3.Name = "Axis3";
            //this.Axis3.Size = new System.Drawing.Size(72, 23);
            //this.Axis3.TabIndex = 25;
            // 
            // Axis4
            // 
            this.Axis4.Location = new System.Drawing.Point(211, 206);  //new System.Drawing.Point(246, 206);
            this.Axis4.Name = "Axis4";
            this.Axis4.Size = new System.Drawing.Size(72, 23);
            this.Axis4.TabIndex = 26;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(56, 188);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(26, 15);
            this.label7.TabIndex = 27;
            this.label7.Text = "1축";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(146, 188);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(26, 15);
            this.label8.TabIndex = 28;
            this.label8.Text = "2축";
            // 
            // label9
            // 
            //this.label9.AutoSize = true;
            //this.label9.Location = new System.Drawing.Point(191, 188);
            //this.label9.Name = "label9";
            //this.label9.Size = new System.Drawing.Size(26, 15);
            //this.label9.TabIndex = 29;
            //this.label9.Text = "3축";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(235, 188);  //new System.Drawing.Point(271, 188);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(26, 15);
            this.label10.TabIndex = 30;
            this.label10.Text = "4축";
            // 
            // UAmcRecvOpen
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(317, 310);
            this.Controls.Add(this.label10);
            //this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.Axis4);
            //this.Controls.Add(this.Axis3);
            this.Controls.Add(this.Axis2);
            this.Controls.Add(this.Axis1);
            this.Controls.Add(this.CheckGearRatio);
            this.Controls.Add(this.btn_Discard);
            this.Controls.Add(this.btn_Apply);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBox_after);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_before);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox_time);
            this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UAmcRecvOpen";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "AMC Recv Open";
            this.Load += new System.EventHandler(this.UAmcRecvOpen_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.TextBox textBox_time;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.TextBox textBox_before;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.TextBox textBox_after;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_Discard;
        private System.Windows.Forms.Button btn_Apply;
        private System.Windows.Forms.CheckBox CheckGearRatio;
        private System.Windows.Forms.TextBox Axis1;
        private System.Windows.Forms.TextBox Axis2;
        //private System.Windows.Forms.TextBox Axis3;
        private System.Windows.Forms.TextBox Axis4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        //private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
    }
}