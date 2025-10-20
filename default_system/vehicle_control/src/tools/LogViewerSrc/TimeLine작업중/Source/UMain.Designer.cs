namespace LogViewer
{
    partial class UMain
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UMain));
			this.miniToolStrip = new System.Windows.Forms.ToolStrip();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.toolStripButton_Load = new System.Windows.Forms.ToolStripButton();
			this.toolStripButton_CellColor = new System.Windows.Forms.ToolStripButton();
			this.btn_MapView = new System.Windows.Forms.ToolStripButton();
			this.toolStripButton_Diagnose = new System.Windows.Forms.ToolStripButton();
			this.btn_DataManager = new System.Windows.Forms.ToolStripButton();
			this.btn_Diff = new System.Windows.Forms.ToolStripButton();
			this.btn_CustomTest1 = new System.Windows.Forms.ToolStripButton();
			this.panel1 = new System.Windows.Forms.Panel();
			this.timer_refresh = new System.Windows.Forms.Timer(this.components);
			this.toolStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// miniToolStrip
			// 
			this.miniToolStrip.AutoSize = false;
			this.miniToolStrip.CanOverflow = false;
			this.miniToolStrip.Dock = System.Windows.Forms.DockStyle.None;
			this.miniToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.miniToolStrip.Location = new System.Drawing.Point(9, 3);
			this.miniToolStrip.Name = "miniToolStrip";
			this.miniToolStrip.Size = new System.Drawing.Size(1184, 25);
			this.miniToolStrip.TabIndex = 0;
			// 
			// toolStrip1
			// 
			this.toolStrip1.CanOverflow = false;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_Load,
            this.toolStripButton_CellColor,
            this.btn_MapView,
            this.toolStripButton_Diagnose,
            this.btn_DataManager,
            this.btn_Diff,
            this.btn_CustomTest1});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(1286, 47);
			this.toolStrip1.TabIndex = 3;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// toolStripButton_Load
			// 
			this.toolStripButton_Load.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.toolStripButton_Load.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Load.Image")));
			this.toolStripButton_Load.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripButton_Load.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButton_Load.Name = "toolStripButton_Load";
			this.toolStripButton_Load.Size = new System.Drawing.Size(110, 44);
			this.toolStripButton_Load.Text = "Log Loading";
			this.toolStripButton_Load.ToolTipText = "로그 열기";
			this.toolStripButton_Load.Click += new System.EventHandler(this.ribbonButton_SetFolder_Click);
			this.toolStripButton_Load.MouseDown += new System.Windows.Forms.MouseEventHandler(this.toolStripButton_Load_MouseDown);
			// 
			// toolStripButton_CellColor
			// 
			this.toolStripButton_CellColor.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.toolStripButton_CellColor.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_CellColor.Image")));
			this.toolStripButton_CellColor.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripButton_CellColor.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButton_CellColor.Name = "toolStripButton_CellColor";
			this.toolStripButton_CellColor.Size = new System.Drawing.Size(98, 44);
			this.toolStripButton_CellColor.Text = "Cell Color";
			this.toolStripButton_CellColor.ToolTipText = "Cell 색 조건 설정";
			this.toolStripButton_CellColor.Click += new System.EventHandler(this.ribbonButton_GridColor_Click);
			// 
			// btn_MapView
			// 
			this.btn_MapView.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_MapView.Image = ((System.Drawing.Image)(resources.GetObject("btn_MapView.Image")));
			this.btn_MapView.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btn_MapView.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_MapView.Name = "btn_MapView";
			this.btn_MapView.Size = new System.Drawing.Size(101, 44);
			this.btn_MapView.Text = "Map View";
			this.btn_MapView.ToolTipText = "맵 관리";
			this.btn_MapView.Click += new System.EventHandler(this.btnMapView_Click);
			// 
			// toolStripButton_Diagnose
			// 
			this.toolStripButton_Diagnose.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.toolStripButton_Diagnose.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_Diagnose.Image")));
			this.toolStripButton_Diagnose.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.toolStripButton_Diagnose.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.toolStripButton_Diagnose.Name = "toolStripButton_Diagnose";
			this.toolStripButton_Diagnose.Size = new System.Drawing.Size(96, 44);
			this.toolStripButton_Diagnose.Text = "Diagnosis";
			this.toolStripButton_Diagnose.Click += new System.EventHandler(this.ribbonBtn_Test_Click);
			// 
			// btn_DataManager
			// 
			this.btn_DataManager.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_DataManager.Image = ((System.Drawing.Image)(resources.GetObject("btn_DataManager.Image")));
			this.btn_DataManager.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btn_DataManager.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_DataManager.Name = "btn_DataManager";
			this.btn_DataManager.Size = new System.Drawing.Size(117, 44);
			this.btn_DataManager.Text = "Data Manager";
			this.btn_DataManager.Click += new System.EventHandler(this.btn_DataManager_Click);
			// 
			// btn_Diff
			// 
			this.btn_Diff.BackColor = System.Drawing.SystemColors.Control;
			this.btn_Diff.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_Diff.Image = ((System.Drawing.Image)(resources.GetObject("btn_Diff.Image")));
			this.btn_Diff.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btn_Diff.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_Diff.Name = "btn_Diff";
			this.btn_Diff.Size = new System.Drawing.Size(87, 44);
			this.btn_Diff.Text = "Diff Files";
			this.btn_Diff.Click += new System.EventHandler(this.btn_Diff_Click);
			// 
			// btn_CustomTest1
			// 
			this.btn_CustomTest1.BackColor = System.Drawing.SystemColors.Control;
			this.btn_CustomTest1.Font = new System.Drawing.Font("Calibri", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.btn_CustomTest1.Image = ((System.Drawing.Image)(resources.GetObject("btn_CustomTest1.Image")));
			this.btn_CustomTest1.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btn_CustomTest1.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btn_CustomTest1.Name = "btn_CustomTest1";
			this.btn_CustomTest1.Size = new System.Drawing.Size(141, 44);
			this.btn_CustomTest1.Text = "UBG Pattern Chk";
			this.btn_CustomTest1.Click += new System.EventHandler(this.btn_CustomTest1_Click);
			// 
			// panel1
			// 
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 47);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(1286, 680);
			this.panel1.TabIndex = 4;
			// 
			// timer_refresh
			// 
			this.timer_refresh.Enabled = true;
			this.timer_refresh.Tick += new System.EventHandler(this.timer_refresh_Tick);
			// 
			// UMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1286, 727);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.toolStrip1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
			this.Name = "UMain";
			this.Text = "Log Viewer";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UMain_FormClosing);
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.UMain_FormClosed);
			this.Load += new System.EventHandler(this.Form_Main_Load);
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ToolStrip miniToolStrip;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButton_Load;
        private System.Windows.Forms.ToolStripButton toolStripButton_CellColor;
        private System.Windows.Forms.Timer timer_refresh;
        private System.Windows.Forms.ToolStripButton btn_MapView;
        public System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ToolStripButton btn_CustomTest1;
        private System.Windows.Forms.ToolStripButton toolStripButton_Diagnose;
		private System.Windows.Forms.ToolStripButton btn_DataManager;
		private System.Windows.Forms.ToolStripButton btn_Diff;
	}
}

