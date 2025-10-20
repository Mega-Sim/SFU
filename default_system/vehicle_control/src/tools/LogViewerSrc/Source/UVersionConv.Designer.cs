namespace LogViewer
{
	partial class UVersionConv
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UVersionConv));
			this.listBox_FileList = new System.Windows.Forms.ListBox();
			this.btnConvert = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.btn_FileDlg = new System.Windows.Forms.Button();
			this.btnClear = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.label3 = new System.Windows.Forms.Label();
			this.btnClose = new System.Windows.Forms.Button();
			this.panel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.SuspendLayout();
			// 
			// listBox_FileList
			// 
			this.listBox_FileList.AllowDrop = true;
			this.listBox_FileList.Font = new System.Drawing.Font("맑은 고딕", 8F);
			this.listBox_FileList.FormattingEnabled = true;
			this.listBox_FileList.Location = new System.Drawing.Point(21, 102);
			this.listBox_FileList.Name = "listBox_FileList";
			this.listBox_FileList.Size = new System.Drawing.Size(565, 212);
			this.listBox_FileList.TabIndex = 0;
			// 
			// btnConvert
			// 
			this.btnConvert.BackColor = System.Drawing.Color.LightGreen;
			this.btnConvert.FlatAppearance.BorderSize = 0;
			this.btnConvert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btnConvert.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btnConvert.Location = new System.Drawing.Point(124, 320);
			this.btnConvert.Name = "btnConvert";
			this.btnConvert.Size = new System.Drawing.Size(97, 36);
			this.btnConvert.TabIndex = 1;
			this.btnConvert.Text = "Convert";
			this.btnConvert.UseVisualStyleBackColor = false;
			this.btnConvert.Click += new System.EventHandler(this.btnConvert_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label1.Location = new System.Drawing.Point(62, 23);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(163, 21);
			this.label1.TabIndex = 2;
			this.label1.Text = "Log Version Convert";
			// 
			// btn_FileDlg
			// 
			this.btn_FileDlg.BackColor = System.Drawing.Color.LightGreen;
			this.btn_FileDlg.FlatAppearance.BorderSize = 0;
			this.btn_FileDlg.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_FileDlg.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_FileDlg.Location = new System.Drawing.Point(21, 320);
			this.btn_FileDlg.Name = "btn_FileDlg";
			this.btn_FileDlg.Size = new System.Drawing.Size(97, 36);
			this.btn_FileDlg.TabIndex = 3;
			this.btn_FileDlg.Text = "File Add";
			this.btn_FileDlg.UseVisualStyleBackColor = false;
			this.btn_FileDlg.Click += new System.EventHandler(this.btn_FileDlg_Click);
			// 
			// btnClear
			// 
			this.btnClear.BackColor = System.Drawing.Color.LightGreen;
			this.btnClear.FlatAppearance.BorderSize = 0;
			this.btnClear.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btnClear.Location = new System.Drawing.Point(489, 320);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(97, 36);
			this.btnClear.TabIndex = 4;
			this.btnClear.Text = "Clear";
			this.btnClear.UseVisualStyleBackColor = false;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label2.Location = new System.Drawing.Point(227, 330);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(140, 17);
			this.label2.TabIndex = 5;
			this.label2.Text = "파일 끌어다 놓기 가능";
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.btnClose);
			this.panel1.Controls.Add(this.label3);
			this.panel1.Controls.Add(this.pictureBox1);
			this.panel1.Controls.Add(this.label2);
			this.panel1.Controls.Add(this.btnClear);
			this.panel1.Controls.Add(this.btn_FileDlg);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Controls.Add(this.btnConvert);
			this.panel1.Controls.Add(this.listBox_FileList);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(600, 365);
			this.panel1.TabIndex = 7;
			// 
			// pictureBox1
			// 
			this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
			this.pictureBox1.Location = new System.Drawing.Point(21, 12);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(35, 39);
			this.pictureBox1.TabIndex = 7;
			this.pictureBox1.TabStop = false;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(18, 70);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(269, 15);
			this.label3.TabIndex = 8;
			this.label3.Text = "LOG 버전을 확인 후 하위 버전으로 변환 합니다. ";
			// 
			// btnClose
			// 
			this.btnClose.BackColor = System.Drawing.Color.LightPink;
			this.btnClose.FlatAppearance.BorderSize = 0;
			this.btnClose.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btnClose.Location = new System.Drawing.Point(530, 17);
			this.btnClose.Name = "btnClose";
			this.btnClose.Size = new System.Drawing.Size(56, 36);
			this.btnClose.TabIndex = 9;
			this.btnClose.Text = "Close";
			this.btnClose.UseVisualStyleBackColor = false;
			this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
			// 
			// UVersionConv
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(600, 365);
			this.Controls.Add(this.panel1);
			this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.Name = "UVersionConv";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.Text = "Log Version Convert";
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListBox listBox_FileList;
		private System.Windows.Forms.Button btnConvert;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btn_FileDlg;
		private System.Windows.Forms.Button btnClear;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button btnClose;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.PictureBox pictureBox1;
	}
}