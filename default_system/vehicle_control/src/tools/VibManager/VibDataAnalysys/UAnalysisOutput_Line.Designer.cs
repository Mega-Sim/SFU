namespace VibrationManager
{
    partial class UAnalysisOutput_Line
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
            this.Summary = new System.Windows.Forms.DataGridView();
            this.label14 = new System.Windows.Forms.Label();
            this.Detail = new System.Windows.Forms.DataGridView();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.Summary)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Detail)).BeginInit();
            this.SuspendLayout();
            // 
            // Summary
            // 
            this.Summary.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Summary.Location = new System.Drawing.Point(17, 61);
            this.Summary.Name = "Summary";
            this.Summary.RowTemplate.Height = 23;
            this.Summary.Size = new System.Drawing.Size(373, 505);
            this.Summary.TabIndex = 35;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label14.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label14.Location = new System.Drawing.Point(12, 21);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(135, 27);
            this.label14.TabIndex = 34;
            this.label14.Text = "Summary";
            // 
            // Detail
            // 
            this.Detail.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Detail.Location = new System.Drawing.Point(407, 61);
            this.Detail.Name = "Detail";
            this.Detail.RowTemplate.Height = 23;
            this.Detail.Size = new System.Drawing.Size(384, 505);
            this.Detail.TabIndex = 36;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label1.Location = new System.Drawing.Point(402, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(88, 27);
            this.label1.TabIndex = 37;
            this.label1.Text = "Detail";
            // 
            // UAnalysisOutput_Line
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(803, 591);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Detail);
            this.Controls.Add(this.Summary);
            this.Controls.Add(this.label14);
            this.Name = "UAnalysisOutput_Line";
            this.Text = "UAnalysisOutput_Line";
            ((System.ComponentModel.ISupportInitialize)(this.Summary)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Detail)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView Summary;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.DataGridView Detail;
        private System.Windows.Forms.Label label1;
    }
}