namespace VibrationManager
{
    partial class UAnalysisInput
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UAnalysisInput));
            this.CSV_VIEW = new System.Windows.Forms.ListView();
            this.label2 = new System.Windows.Forms.Label();
            this.LimitVib = new System.Windows.Forms.TextBox();
            this.btn_Analysys = new System.Windows.Forms.Button();
            this.btn_Clear = new System.Windows.Forms.Button();
            this.CSV_Label = new System.Windows.Forms.Label();
            this.Check_All = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // CSV_VIEW
            // 
            this.CSV_VIEW.AllowDrop = true;
            this.CSV_VIEW.CheckBoxes = true;
            this.CSV_VIEW.Location = new System.Drawing.Point(12, 41);
            this.CSV_VIEW.Name = "CSV_VIEW";
            this.CSV_VIEW.Size = new System.Drawing.Size(317, 221);
            this.CSV_VIEW.TabIndex = 0;
            this.CSV_VIEW.UseCompatibleStateImageBehavior = false;
            this.CSV_VIEW.View = System.Windows.Forms.View.List;
            this.CSV_VIEW.DragDrop += new System.Windows.Forms.DragEventHandler(this.CSV_VIEW_DragDrop);
            this.CSV_VIEW.DragEnter += new System.Windows.Forms.DragEventHandler(this.CSV_VIEW_DragEnter);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("굴림", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(344, 61);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(114, 19);
            this.label2.TabIndex = 2;
            this.label2.Text = "Vibration(G)";
            // 
            // LimitVib
            // 
            this.LimitVib.Location = new System.Drawing.Point(483, 61);
            this.LimitVib.Name = "LimitVib";
            this.LimitVib.Size = new System.Drawing.Size(97, 21);
            this.LimitVib.TabIndex = 4;
            this.LimitVib.Text = "0.6";
            // 
            // btn_Analysys
            // 
            this.btn_Analysys.Font = new System.Drawing.Font("굴림", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btn_Analysys.Location = new System.Drawing.Point(347, 145);
            this.btn_Analysys.Name = "btn_Analysys";
            this.btn_Analysys.Size = new System.Drawing.Size(231, 50);
            this.btn_Analysys.TabIndex = 6;
            this.btn_Analysys.Text = "Analysis";
            this.btn_Analysys.UseVisualStyleBackColor = true;
            this.btn_Analysys.Click += new System.EventHandler(this.Analysys_Button);
            // 
            // btn_Clear
            // 
            this.btn_Clear.Font = new System.Drawing.Font("굴림", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btn_Clear.Location = new System.Drawing.Point(347, 212);
            this.btn_Clear.Name = "btn_Clear";
            this.btn_Clear.Size = new System.Drawing.Size(231, 50);
            this.btn_Clear.TabIndex = 7;
            this.btn_Clear.Text = "Clear";
            this.btn_Clear.UseVisualStyleBackColor = true;
            this.btn_Clear.Click += new System.EventHandler(this.Clear_Button);
            // 
            // CSV_Label
            // 
            this.CSV_Label.AutoSize = true;
            this.CSV_Label.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CSV_Label.Location = new System.Drawing.Point(12, 22);
            this.CSV_Label.Name = "CSV_Label";
            this.CSV_Label.Size = new System.Drawing.Size(56, 16);
            this.CSV_Label.TabIndex = 11;
            this.CSV_Label.Text = "*.CSV";
            // 
            // Check_All
            // 
            this.Check_All.AutoSize = true;
            this.Check_All.Location = new System.Drawing.Point(502, 98);
            this.Check_All.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Check_All.Name = "Check_All";
            this.Check_All.Size = new System.Drawing.Size(78, 16);
            this.Check_All.TabIndex = 18;
            this.Check_All.Text = "All Check";
            this.Check_All.UseVisualStyleBackColor = true;
            this.Check_All.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("굴림체", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.label1.Location = new System.Drawing.Point(477, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(101, 24);
            this.label1.TabIndex = 1;
            this.label1.Text = "Setting";
            // 
            // UAnalysisInput
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(590, 274);
            this.Controls.Add(this.Check_All);
            this.Controls.Add(this.CSV_Label);
            this.Controls.Add(this.btn_Clear);
            this.Controls.Add(this.btn_Analysys);
            this.Controls.Add(this.LimitVib);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CSV_VIEW);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UAnalysisInput";
            this.Text = "Vibration Manager";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView CSV_VIEW;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox LimitVib;
        private System.Windows.Forms.Button btn_Analysys;
        private System.Windows.Forms.Button btn_Clear;
        private System.Windows.Forms.Label CSV_Label;
        private System.Windows.Forms.CheckBox Check_All;
        private System.Windows.Forms.Label label1;
    }
}