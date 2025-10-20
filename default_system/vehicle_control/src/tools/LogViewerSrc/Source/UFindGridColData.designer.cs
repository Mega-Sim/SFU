namespace LogViewer
{
    partial class UFindGridColData
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
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_FindText = new System.Windows.Forms.TextBox();
            this.button_Find = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.checkBox_Contain = new System.Windows.Forms.CheckBox();
            this.label_CurCol = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(19, 84);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(68, 17);
            this.label1.TabIndex = 0;
            this.label1.Text = "찾을 내용:";
            // 
            // textBox_FindText
            // 
            this.textBox_FindText.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.textBox_FindText.Location = new System.Drawing.Point(91, 81);
            this.textBox_FindText.Name = "textBox_FindText";
            this.textBox_FindText.Size = new System.Drawing.Size(89, 25);
            this.textBox_FindText.TabIndex = 1;
            this.textBox_FindText.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_FindText_KeyDown);
            // 
            // button_Find
            // 
            this.button_Find.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button_Find.Location = new System.Drawing.Point(186, 73);
            this.button_Find.Name = "button_Find";
            this.button_Find.Size = new System.Drawing.Size(75, 39);
            this.button_Find.TabIndex = 2;
            this.button_Find.Text = "다음 찾기";
            this.button_Find.UseVisualStyleBackColor = true;
            this.button_Find.Click += new System.EventHandler(this.button_Find_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(12, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(153, 17);
            this.label2.TabIndex = 0;
            this.label2.Text = "현재 선택된 열에서 찾기";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.Location = new System.Drawing.Point(12, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(73, 17);
            this.label3.TabIndex = 0;
            this.label3.Text = "선택된 열: ";
            // 
            // checkBox_Contain
            // 
            this.checkBox_Contain.AutoSize = true;
            this.checkBox_Contain.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.checkBox_Contain.Location = new System.Drawing.Point(267, 87);
            this.checkBox_Contain.Name = "checkBox_Contain";
            this.checkBox_Contain.Size = new System.Drawing.Size(130, 19);
            this.checkBox_Contain.TabIndex = 3;
            this.checkBox_Contain.Text = "일부 포함되도 인정";
            this.checkBox_Contain.UseVisualStyleBackColor = true;
            // 
            // label_CurCol
            // 
            this.label_CurCol.AutoSize = true;
            this.label_CurCol.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label_CurCol.Location = new System.Drawing.Point(92, 49);
            this.label_CurCol.Name = "label_CurCol";
            this.label_CurCol.Size = new System.Drawing.Size(53, 17);
            this.label_CurCol.TabIndex = 0;
            this.label_CurCol.Text = "Cur Col";
            // 
            // UFindGridColData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(409, 118);
            this.Controls.Add(this.checkBox_Contain);
            this.Controls.Add(this.button_Find);
            this.Controls.Add(this.textBox_FindText);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label_CurCol);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "UFindGridColData";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Find Column Data";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_FindText;
        private System.Windows.Forms.Button button_Find;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox checkBox_Contain;
        private System.Windows.Forms.Label label_CurCol;
    }
}