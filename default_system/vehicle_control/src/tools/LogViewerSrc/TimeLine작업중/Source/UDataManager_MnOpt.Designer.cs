namespace LogViewer_DataManager
{
    partial class UDataManager_MnOpt
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
			System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
            "",
            "UC",
            "5",
            "직선구간에서 UBG 대차 센서가 두단계이상 변할때",
            "UBG 대차센서 불량, 틀어짐 감지"}, -1);
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UDataManager_MnOpt));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.chk_opt = new System.Windows.Forms.CheckBox();
			this.btn_Log2MLog = new System.Windows.Forms.Button();
			this.btn_stop = new System.Windows.Forms.Button();
			this.btn_Analyze = new System.Windows.Forms.Button();
			this.btn_MonitorDownload = new System.Windows.Forms.Button();
			this.textBox_MonitorLogPath = new System.Windows.Forms.TextBox();
			this.button_selectFolder = new System.Windows.Forms.Button();
			this.radioButton_LogTime1_Range = new System.Windows.Forms.RadioButton();
			this.label5 = new System.Windows.Forms.Label();
			this.radioButton_LogTime2_AfterAll = new System.Windows.Forms.RadioButton();
			this.radioButton_LogTime3_All = new System.Windows.Forms.RadioButton();
			this.label12 = new System.Windows.Forms.Label();
			this.numeric_LTimeFrom_Year = new System.Windows.Forms.NumericUpDown();
			this.label14 = new System.Windows.Forms.Label();
			this.numeric_LTimeFrom_Month = new System.Windows.Forms.NumericUpDown();
			this.label16 = new System.Windows.Forms.Label();
			this.numeric_LTimeFrom_Day = new System.Windows.Forms.NumericUpDown();
			this.label17 = new System.Windows.Forms.Label();
			this.numeric_LTimeFrom_Hour = new System.Windows.Forms.NumericUpDown();
			this.textBox7 = new System.Windows.Forms.TextBox();
			this.textBox8 = new System.Windows.Forms.TextBox();
			this.numeric_LTimeTo_Hour = new System.Windows.Forms.NumericUpDown();
			this.label19 = new System.Windows.Forms.Label();
			this.numeric_LTimeTo_Day = new System.Windows.Forms.NumericUpDown();
			this.label18 = new System.Windows.Forms.Label();
			this.numeric_LTimeTo_Month = new System.Windows.Forms.NumericUpDown();
			this.numeric_LTimeTo_Year = new System.Windows.Forms.NumericUpDown();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.listView_MonitorItems = new System.Windows.Forms.ListView();
			this.colChk = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colErrCnt = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colCond = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.colDesc = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.textBox_Desc = new System.Windows.Forms.TextBox();
			this.btn_Add = new System.Windows.Forms.Button();
			this.btn_Edit = new System.Windows.Forms.Button();
			this.textBox_Item = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.btn_UpLevel = new System.Windows.Forms.Button();
			this.textBox_ErrCnt = new System.Windows.Forms.TextBox();
			this.btn_UpDown = new System.Windows.Forms.Button();
			this.btn_DoEdit = new System.Windows.Forms.Button();
			this.btn_Delete = new System.Windows.Forms.Button();
			this.textBox_Cond = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Year)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Month)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Day)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Hour)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Hour)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Day)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Month)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Year)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
			this.splitContainer1.IsSplitterFixed = true;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.BackColor = System.Drawing.Color.Transparent;
			this.splitContainer1.Panel1.Controls.Add(this.chk_opt);
			this.splitContainer1.Panel1.Controls.Add(this.btn_Log2MLog);
			this.splitContainer1.Panel1.Controls.Add(this.btn_stop);
			this.splitContainer1.Panel1.Controls.Add(this.btn_Analyze);
			this.splitContainer1.Panel1.Controls.Add(this.btn_MonitorDownload);
			this.splitContainer1.Panel1.Controls.Add(this.textBox_MonitorLogPath);
			this.splitContainer1.Panel1.Controls.Add(this.button_selectFolder);
			this.splitContainer1.Panel1.Controls.Add(this.radioButton_LogTime1_Range);
			this.splitContainer1.Panel1.Controls.Add(this.label5);
			this.splitContainer1.Panel1.Controls.Add(this.radioButton_LogTime2_AfterAll);
			this.splitContainer1.Panel1.Controls.Add(this.radioButton_LogTime3_All);
			this.splitContainer1.Panel1.Controls.Add(this.label12);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeFrom_Year);
			this.splitContainer1.Panel1.Controls.Add(this.label14);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeFrom_Month);
			this.splitContainer1.Panel1.Controls.Add(this.label16);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeFrom_Day);
			this.splitContainer1.Panel1.Controls.Add(this.label17);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeFrom_Hour);
			this.splitContainer1.Panel1.Controls.Add(this.textBox7);
			this.splitContainer1.Panel1.Controls.Add(this.textBox8);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeTo_Hour);
			this.splitContainer1.Panel1.Controls.Add(this.label19);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeTo_Day);
			this.splitContainer1.Panel1.Controls.Add(this.label18);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeTo_Month);
			this.splitContainer1.Panel1.Controls.Add(this.numeric_LTimeTo_Year);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
			this.splitContainer1.Size = new System.Drawing.Size(586, 577);
			this.splitContainer1.SplitterDistance = 225;
			this.splitContainer1.TabIndex = 0;
			this.splitContainer1.TabStop = false;
			// 
			// chk_opt
			// 
			this.chk_opt.AutoSize = true;
			this.chk_opt.Location = new System.Drawing.Point(23, 58);
			this.chk_opt.Name = "chk_opt";
			this.chk_opt.Size = new System.Drawing.Size(63, 19);
			this.chk_opt.TabIndex = 152;
			this.chk_opt.Text = "Option";
			this.chk_opt.UseVisualStyleBackColor = true;
			this.chk_opt.CheckedChanged += new System.EventHandler(this.chk_opt_CheckedChanged);
			// 
			// btn_Log2MLog
			// 
			this.btn_Log2MLog.Location = new System.Drawing.Point(92, 55);
			this.btn_Log2MLog.Name = "btn_Log2MLog";
			this.btn_Log2MLog.Size = new System.Drawing.Size(289, 23);
			this.btn_Log2MLog.TabIndex = 151;
			this.btn_Log2MLog.Text = "이미 다운받은 Monitoring 로그를 MLOG 파일로\r\n 변환";
			this.btn_Log2MLog.UseVisualStyleBackColor = true;
			this.btn_Log2MLog.Visible = false;
			this.btn_Log2MLog.Click += new System.EventHandler(this.btn_Log2MLog_Click);
			// 
			// btn_stop
			// 
			this.btn_stop.BackColor = System.Drawing.Color.LightPink;
			this.btn_stop.FlatAppearance.BorderSize = 0;
			this.btn_stop.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_stop.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_stop.Location = new System.Drawing.Point(291, 12);
			this.btn_stop.Name = "btn_stop";
			this.btn_stop.Size = new System.Drawing.Size(117, 37);
			this.btn_stop.TabIndex = 150;
			this.btn_stop.Text = "중단";
			this.btn_stop.UseVisualStyleBackColor = false;
			this.btn_stop.Click += new System.EventHandler(this.btn_stop_Click);
			// 
			// btn_Analyze
			// 
			this.btn_Analyze.BackColor = System.Drawing.Color.PaleGreen;
			this.btn_Analyze.FlatAppearance.BorderSize = 0;
			this.btn_Analyze.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Analyze.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_Analyze.Location = new System.Drawing.Point(162, 12);
			this.btn_Analyze.Name = "btn_Analyze";
			this.btn_Analyze.Size = new System.Drawing.Size(117, 37);
			this.btn_Analyze.TabIndex = 149;
			this.btn_Analyze.Text = "분석";
			this.btn_Analyze.UseVisualStyleBackColor = false;
			this.btn_Analyze.Click += new System.EventHandler(this.btn_Analyze_Click);
			// 
			// btn_MonitorDownload
			// 
			this.btn_MonitorDownload.BackColor = System.Drawing.Color.PaleGreen;
			this.btn_MonitorDownload.FlatAppearance.BorderSize = 0;
			this.btn_MonitorDownload.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_MonitorDownload.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_MonitorDownload.Location = new System.Drawing.Point(17, 12);
			this.btn_MonitorDownload.Name = "btn_MonitorDownload";
			this.btn_MonitorDownload.Size = new System.Drawing.Size(131, 37);
			this.btn_MonitorDownload.TabIndex = 148;
			this.btn_MonitorDownload.Text = "로그 다운로드";
			this.btn_MonitorDownload.UseVisualStyleBackColor = false;
			this.btn_MonitorDownload.Click += new System.EventHandler(this.btn_MonitorDownload_Click);
			// 
			// textBox_MonitorLogPath
			// 
			this.textBox_MonitorLogPath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.textBox_MonitorLogPath.Location = new System.Drawing.Point(144, 177);
			this.textBox_MonitorLogPath.Name = "textBox_MonitorLogPath";
			this.textBox_MonitorLogPath.ReadOnly = true;
			this.textBox_MonitorLogPath.Size = new System.Drawing.Size(430, 23);
			this.textBox_MonitorLogPath.TabIndex = 147;
			this.textBox_MonitorLogPath.Visible = false;
			// 
			// button_selectFolder
			// 
			this.button_selectFolder.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.button_selectFolder.Location = new System.Drawing.Point(42, 175);
			this.button_selectFolder.Name = "button_selectFolder";
			this.button_selectFolder.Size = new System.Drawing.Size(90, 25);
			this.button_selectFolder.TabIndex = 146;
			this.button_selectFolder.Text = "폴더 지정";
			this.button_selectFolder.UseVisualStyleBackColor = true;
			this.button_selectFolder.Visible = false;
			this.button_selectFolder.Click += new System.EventHandler(this.button_selectFolder_Click);
			// 
			// radioButton_LogTime1_Range
			// 
			this.radioButton_LogTime1_Range.AutoSize = true;
			this.radioButton_LogTime1_Range.Checked = true;
			this.radioButton_LogTime1_Range.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.radioButton_LogTime1_Range.Location = new System.Drawing.Point(18, 96);
			this.radioButton_LogTime1_Range.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.radioButton_LogTime1_Range.Name = "radioButton_LogTime1_Range";
			this.radioButton_LogTime1_Range.Size = new System.Drawing.Size(117, 19);
			this.radioButton_LogTime1_Range.TabIndex = 112;
			this.radioButton_LogTime1_Range.TabStop = true;
			this.radioButton_LogTime1_Range.Text = "특정 시간대 지정";
			this.radioButton_LogTime1_Range.UseVisualStyleBackColor = true;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.BackColor = System.Drawing.Color.Yellow;
			this.label5.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label5.Location = new System.Drawing.Point(8, 203);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(277, 15);
			this.label5.TabIndex = 143;
			this.label5.Text = "검사항목 지정 / 편집 - 검사 할 항목을 체크하세요";
			// 
			// radioButton_LogTime2_AfterAll
			// 
			this.radioButton_LogTime2_AfterAll.AutoSize = true;
			this.radioButton_LogTime2_AfterAll.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.radioButton_LogTime2_AfterAll.Location = new System.Drawing.Point(18, 123);
			this.radioButton_LogTime2_AfterAll.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.radioButton_LogTime2_AfterAll.Name = "radioButton_LogTime2_AfterAll";
			this.radioButton_LogTime2_AfterAll.Size = new System.Drawing.Size(145, 19);
			this.radioButton_LogTime2_AfterAll.TabIndex = 113;
			this.radioButton_LogTime2_AfterAll.Text = "특정 시간대 이후 모두";
			this.radioButton_LogTime2_AfterAll.UseVisualStyleBackColor = true;
			// 
			// radioButton_LogTime3_All
			// 
			this.radioButton_LogTime3_All.AutoSize = true;
			this.radioButton_LogTime3_All.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.radioButton_LogTime3_All.Location = new System.Drawing.Point(18, 148);
			this.radioButton_LogTime3_All.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.radioButton_LogTime3_All.Name = "radioButton_LogTime3_All";
			this.radioButton_LogTime3_All.Size = new System.Drawing.Size(89, 19);
			this.radioButton_LogTime3_All.TabIndex = 114;
			this.radioButton_LogTime3_All.Text = "전체 시간대";
			this.radioButton_LogTime3_All.UseVisualStyleBackColor = true;
			// 
			// label12
			// 
			this.label12.AutoSize = true;
			this.label12.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label12.Location = new System.Drawing.Point(180, 149);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(20, 15);
			this.label12.TabIndex = 130;
			this.label12.Text = "To";
			// 
			// numeric_LTimeFrom_Year
			// 
			this.numeric_LTimeFrom_Year.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeFrom_Year.Location = new System.Drawing.Point(291, 97);
			this.numeric_LTimeFrom_Year.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeFrom_Year.Maximum = new decimal(new int[] {
            9000,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Year.Minimum = new decimal(new int[] {
            1900,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Year.Name = "numeric_LTimeFrom_Year";
			this.numeric_LTimeFrom_Year.Size = new System.Drawing.Size(54, 23);
			this.numeric_LTimeFrom_Year.TabIndex = 115;
			this.numeric_LTimeFrom_Year.Value = new decimal(new int[] {
            2018,
            0,
            0,
            0});
			// 
			// label14
			// 
			this.label14.AutoSize = true;
			this.label14.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label14.Location = new System.Drawing.Point(180, 102);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(35, 15);
			this.label14.TabIndex = 129;
			this.label14.Text = "From";
			// 
			// numeric_LTimeFrom_Month
			// 
			this.numeric_LTimeFrom_Month.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeFrom_Month.Location = new System.Drawing.Point(350, 97);
			this.numeric_LTimeFrom_Month.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeFrom_Month.Maximum = new decimal(new int[] {
            12,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Month.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Month.Name = "numeric_LTimeFrom_Month";
			this.numeric_LTimeFrom_Month.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeFrom_Month.TabIndex = 116;
			this.numeric_LTimeFrom_Month.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// label16
			// 
			this.label16.AutoSize = true;
			this.label16.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label16.Location = new System.Drawing.Point(489, 148);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(19, 15);
			this.label16.TabIndex = 128;
			this.label16.Text = "시";
			// 
			// numeric_LTimeFrom_Day
			// 
			this.numeric_LTimeFrom_Day.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeFrom_Day.Location = new System.Drawing.Point(395, 97);
			this.numeric_LTimeFrom_Day.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeFrom_Day.Maximum = new decimal(new int[] {
            31,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Day.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Day.Name = "numeric_LTimeFrom_Day";
			this.numeric_LTimeFrom_Day.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeFrom_Day.TabIndex = 117;
			this.numeric_LTimeFrom_Day.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// label17
			// 
			this.label17.AutoSize = true;
			this.label17.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label17.Location = new System.Drawing.Point(232, 148);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(53, 15);
			this.label17.TabIndex = 127;
			this.label17.Text = "년/월/일";
			// 
			// numeric_LTimeFrom_Hour
			// 
			this.numeric_LTimeFrom_Hour.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeFrom_Hour.Location = new System.Drawing.Point(449, 97);
			this.numeric_LTimeFrom_Hour.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeFrom_Hour.Maximum = new decimal(new int[] {
            23,
            0,
            0,
            0});
			this.numeric_LTimeFrom_Hour.Name = "numeric_LTimeFrom_Hour";
			this.numeric_LTimeFrom_Hour.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeFrom_Hour.TabIndex = 118;
			// 
			// textBox7
			// 
			this.textBox7.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox7.Location = new System.Drawing.Point(511, 143);
			this.textBox7.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.textBox7.Name = "textBox7";
			this.textBox7.ReadOnly = true;
			this.textBox7.Size = new System.Drawing.Size(38, 23);
			this.textBox7.TabIndex = 126;
			this.textBox7.Text = "59분";
			// 
			// textBox8
			// 
			this.textBox8.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox8.Location = new System.Drawing.Point(511, 97);
			this.textBox8.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.textBox8.Name = "textBox8";
			this.textBox8.ReadOnly = true;
			this.textBox8.Size = new System.Drawing.Size(38, 23);
			this.textBox8.TabIndex = 119;
			this.textBox8.Text = "0분";
			// 
			// numeric_LTimeTo_Hour
			// 
			this.numeric_LTimeTo_Hour.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeTo_Hour.Location = new System.Drawing.Point(449, 143);
			this.numeric_LTimeTo_Hour.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeTo_Hour.Maximum = new decimal(new int[] {
            23,
            0,
            0,
            0});
			this.numeric_LTimeTo_Hour.Name = "numeric_LTimeTo_Hour";
			this.numeric_LTimeTo_Hour.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeTo_Hour.TabIndex = 125;
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label19.Location = new System.Drawing.Point(232, 102);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(53, 15);
			this.label19.TabIndex = 120;
			this.label19.Text = "년/월/일";
			// 
			// numeric_LTimeTo_Day
			// 
			this.numeric_LTimeTo_Day.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeTo_Day.Location = new System.Drawing.Point(395, 143);
			this.numeric_LTimeTo_Day.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeTo_Day.Maximum = new decimal(new int[] {
            31,
            0,
            0,
            0});
			this.numeric_LTimeTo_Day.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.numeric_LTimeTo_Day.Name = "numeric_LTimeTo_Day";
			this.numeric_LTimeTo_Day.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeTo_Day.TabIndex = 124;
			this.numeric_LTimeTo_Day.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// label18
			// 
			this.label18.AutoSize = true;
			this.label18.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label18.Location = new System.Drawing.Point(489, 102);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(19, 15);
			this.label18.TabIndex = 121;
			this.label18.Text = "시";
			// 
			// numeric_LTimeTo_Month
			// 
			this.numeric_LTimeTo_Month.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeTo_Month.Location = new System.Drawing.Point(350, 143);
			this.numeric_LTimeTo_Month.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeTo_Month.Maximum = new decimal(new int[] {
            12,
            0,
            0,
            0});
			this.numeric_LTimeTo_Month.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.numeric_LTimeTo_Month.Name = "numeric_LTimeTo_Month";
			this.numeric_LTimeTo_Month.Size = new System.Drawing.Size(40, 23);
			this.numeric_LTimeTo_Month.TabIndex = 123;
			this.numeric_LTimeTo_Month.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// numeric_LTimeTo_Year
			// 
			this.numeric_LTimeTo_Year.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.numeric_LTimeTo_Year.Location = new System.Drawing.Point(291, 143);
			this.numeric_LTimeTo_Year.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.numeric_LTimeTo_Year.Maximum = new decimal(new int[] {
            9000,
            0,
            0,
            0});
			this.numeric_LTimeTo_Year.Minimum = new decimal(new int[] {
            1900,
            0,
            0,
            0});
			this.numeric_LTimeTo_Year.Name = "numeric_LTimeTo_Year";
			this.numeric_LTimeTo_Year.Size = new System.Drawing.Size(54, 23);
			this.numeric_LTimeTo_Year.TabIndex = 122;
			this.numeric_LTimeTo_Year.Value = new decimal(new int[] {
            2018,
            0,
            0,
            0});
			// 
			// splitContainer2
			// 
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer2.IsSplitterFixed = true;
			this.splitContainer2.Location = new System.Drawing.Point(0, 0);
			this.splitContainer2.Name = "splitContainer2";
			this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.listView_MonitorItems);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.textBox_Desc);
			this.splitContainer2.Panel2.Controls.Add(this.btn_Add);
			this.splitContainer2.Panel2.Controls.Add(this.btn_Edit);
			this.splitContainer2.Panel2.Controls.Add(this.textBox_Item);
			this.splitContainer2.Panel2.Controls.Add(this.label7);
			this.splitContainer2.Panel2.Controls.Add(this.btn_UpLevel);
			this.splitContainer2.Panel2.Controls.Add(this.textBox_ErrCnt);
			this.splitContainer2.Panel2.Controls.Add(this.btn_UpDown);
			this.splitContainer2.Panel2.Controls.Add(this.btn_DoEdit);
			this.splitContainer2.Panel2.Controls.Add(this.btn_Delete);
			this.splitContainer2.Panel2.Controls.Add(this.textBox_Cond);
			this.splitContainer2.Panel2.Controls.Add(this.label1);
			this.splitContainer2.Panel2.Controls.Add(this.label3);
			this.splitContainer2.Panel2.Controls.Add(this.label2);
			this.splitContainer2.Size = new System.Drawing.Size(586, 348);
			this.splitContainer2.SplitterDistance = 255;
			this.splitContainer2.TabIndex = 148;
			this.splitContainer2.TabStop = false;
			// 
			// listView_MonitorItems
			// 
			this.listView_MonitorItems.CheckBoxes = true;
			this.listView_MonitorItems.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colChk,
            this.colName,
            this.colErrCnt,
            this.colCond,
            this.colDesc});
			this.listView_MonitorItems.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_MonitorItems.Font = new System.Drawing.Font("맑은 고딕", 8F);
			this.listView_MonitorItems.FullRowSelect = true;
			this.listView_MonitorItems.GridLines = true;
			this.listView_MonitorItems.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			listViewItem1.StateImageIndex = 0;
			this.listView_MonitorItems.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1});
			this.listView_MonitorItems.Location = new System.Drawing.Point(0, 0);
			this.listView_MonitorItems.MultiSelect = false;
			this.listView_MonitorItems.Name = "listView_MonitorItems";
			this.listView_MonitorItems.Size = new System.Drawing.Size(586, 255);
			this.listView_MonitorItems.TabIndex = 131;
			this.listView_MonitorItems.UseCompatibleStateImageBehavior = false;
			this.listView_MonitorItems.View = System.Windows.Forms.View.Details;
			this.listView_MonitorItems.SelectedIndexChanged += new System.EventHandler(this.listView_MonitorItems_SelectedIndexChanged);
			// 
			// colChk
			// 
			this.colChk.Text = "검사여부";
			this.colChk.Width = 65;
			// 
			// colName
			// 
			this.colName.Text = "Item";
			this.colName.Width = 57;
			// 
			// colErrCnt
			// 
			this.colErrCnt.Text = "에러기준값";
			this.colErrCnt.Width = 74;
			// 
			// colCond
			// 
			this.colCond.Text = "조건";
			this.colCond.Width = 302;
			// 
			// colDesc
			// 
			this.colDesc.Text = "설명";
			this.colDesc.Width = 600;
			// 
			// textBox_Desc
			// 
			this.textBox_Desc.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Desc.Location = new System.Drawing.Point(427, 58);
			this.textBox_Desc.Name = "textBox_Desc";
			this.textBox_Desc.Size = new System.Drawing.Size(140, 23);
			this.textBox_Desc.TabIndex = 140;
			this.textBox_Desc.TextChanged += new System.EventHandler(this.textBox_Desc_TextChanged);
			// 
			// btn_Add
			// 
			this.btn_Add.BackColor = System.Drawing.Color.Transparent;
			this.btn_Add.FlatAppearance.BorderSize = 0;
			this.btn_Add.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Add.Font = new System.Drawing.Font("맑은 고딕", 8F);
			this.btn_Add.Image = ((System.Drawing.Image)(resources.GetObject("btn_Add.Image")));
			this.btn_Add.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_Add.Location = new System.Drawing.Point(13, 31);
			this.btn_Add.Name = "btn_Add";
			this.btn_Add.Size = new System.Drawing.Size(57, 23);
			this.btn_Add.TabIndex = 132;
			this.btn_Add.Text = "Add";
			this.btn_Add.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_Add.UseVisualStyleBackColor = false;
			this.btn_Add.Click += new System.EventHandler(this.btn_Add_Click);
			// 
			// btn_Edit
			// 
			this.btn_Edit.BackColor = System.Drawing.Color.Transparent;
			this.btn_Edit.FlatAppearance.BorderSize = 0;
			this.btn_Edit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Edit.Font = new System.Drawing.Font("맑은 고딕", 8F);
			this.btn_Edit.Image = ((System.Drawing.Image)(resources.GetObject("btn_Edit.Image")));
			this.btn_Edit.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_Edit.Location = new System.Drawing.Point(80, 31);
			this.btn_Edit.Name = "btn_Edit";
			this.btn_Edit.Size = new System.Drawing.Size(68, 23);
			this.btn_Edit.TabIndex = 147;
			this.btn_Edit.Text = "Edit";
			this.btn_Edit.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_Edit.UseVisualStyleBackColor = false;
			this.btn_Edit.Click += new System.EventHandler(this.button_Edit_Click);
			// 
			// textBox_Item
			// 
			this.textBox_Item.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Item.Location = new System.Drawing.Point(41, 58);
			this.textBox_Item.Name = "textBox_Item";
			this.textBox_Item.Size = new System.Drawing.Size(41, 23);
			this.textBox_Item.TabIndex = 133;
			this.textBox_Item.TextChanged += new System.EventHandler(this.textBox_Item_TextChanged);
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label7.Location = new System.Drawing.Point(89, 61);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(67, 15);
			this.label7.TabIndex = 146;
			this.label7.Text = "에러기준값";
			this.label7.Click += new System.EventHandler(this.label7_Click);
			// 
			// btn_UpLevel
			// 
			this.btn_UpLevel.FlatAppearance.BorderSize = 0;
			this.btn_UpLevel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_UpLevel.Font = new System.Drawing.Font("맑은 고딕", 7F);
			this.btn_UpLevel.Image = ((System.Drawing.Image)(resources.GetObject("btn_UpLevel.Image")));
			this.btn_UpLevel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_UpLevel.Location = new System.Drawing.Point(78, 3);
			this.btn_UpLevel.Name = "btn_UpLevel";
			this.btn_UpLevel.Size = new System.Drawing.Size(42, 23);
			this.btn_UpLevel.TabIndex = 134;
			this.btn_UpLevel.Text = "UP";
			this.btn_UpLevel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_UpLevel.UseVisualStyleBackColor = true;
			this.btn_UpLevel.Click += new System.EventHandler(this.btn_UpLevel_Click);
			// 
			// textBox_ErrCnt
			// 
			this.textBox_ErrCnt.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_ErrCnt.Location = new System.Drawing.Point(162, 59);
			this.textBox_ErrCnt.Name = "textBox_ErrCnt";
			this.textBox_ErrCnt.Size = new System.Drawing.Size(57, 23);
			this.textBox_ErrCnt.TabIndex = 145;
			this.textBox_ErrCnt.TextChanged += new System.EventHandler(this.textBox_ErrCnt_TextChanged);
			// 
			// btn_UpDown
			// 
			this.btn_UpDown.FlatAppearance.BorderSize = 0;
			this.btn_UpDown.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_UpDown.Font = new System.Drawing.Font("맑은 고딕", 7F);
			this.btn_UpDown.Image = ((System.Drawing.Image)(resources.GetObject("btn_UpDown.Image")));
			this.btn_UpDown.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_UpDown.Location = new System.Drawing.Point(119, 2);
			this.btn_UpDown.Name = "btn_UpDown";
			this.btn_UpDown.Size = new System.Drawing.Size(53, 23);
			this.btn_UpDown.TabIndex = 135;
			this.btn_UpDown.Text = "Down";
			this.btn_UpDown.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_UpDown.UseVisualStyleBackColor = true;
			this.btn_UpDown.Click += new System.EventHandler(this.btn_UpDown_Click);
			// 
			// btn_DoEdit
			// 
			this.btn_DoEdit.BackColor = System.Drawing.Color.Transparent;
			this.btn_DoEdit.FlatAppearance.BorderSize = 0;
			this.btn_DoEdit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_DoEdit.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.btn_DoEdit.Location = new System.Drawing.Point(4, 3);
			this.btn_DoEdit.Name = "btn_DoEdit";
			this.btn_DoEdit.Size = new System.Drawing.Size(75, 26);
			this.btn_DoEdit.TabIndex = 144;
			this.btn_DoEdit.Text = "편집하기";
			this.btn_DoEdit.UseVisualStyleBackColor = false;
			this.btn_DoEdit.Click += new System.EventHandler(this.btn_DoEdit_Click);
			// 
			// btn_Delete
			// 
			this.btn_Delete.FlatAppearance.BorderSize = 0;
			this.btn_Delete.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btn_Delete.Font = new System.Drawing.Font("맑은 고딕", 7F);
			this.btn_Delete.Image = ((System.Drawing.Image)(resources.GetObject("btn_Delete.Image")));
			this.btn_Delete.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.btn_Delete.Location = new System.Drawing.Point(172, 2);
			this.btn_Delete.Name = "btn_Delete";
			this.btn_Delete.Size = new System.Drawing.Size(59, 23);
			this.btn_Delete.TabIndex = 136;
			this.btn_Delete.Text = "Delete";
			this.btn_Delete.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.btn_Delete.UseVisualStyleBackColor = true;
			this.btn_Delete.Click += new System.EventHandler(this.btn_Delete_Click);
			// 
			// textBox_Cond
			// 
			this.textBox_Cond.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.textBox_Cond.Location = new System.Drawing.Point(262, 58);
			this.textBox_Cond.Name = "textBox_Cond";
			this.textBox_Cond.Size = new System.Drawing.Size(127, 23);
			this.textBox_Cond.TabIndex = 137;
			this.textBox_Cond.TextChanged += new System.EventHandler(this.textBox_Cond_TextChanged);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label1.Location = new System.Drawing.Point(9, 62);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(31, 15);
			this.label1.TabIndex = 138;
			this.label1.Text = "Item";
			this.label1.Click += new System.EventHandler(this.label1_Click);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label3.Location = new System.Drawing.Point(390, 58);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(31, 15);
			this.label3.TabIndex = 141;
			this.label3.Text = "설명";
			this.label3.Click += new System.EventHandler(this.label3_Click);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label2.Location = new System.Drawing.Point(225, 58);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(31, 15);
			this.label2.TabIndex = 139;
			this.label2.Text = "조건";
			this.label2.Click += new System.EventHandler(this.label2_Click);
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// UDataManager_MnOpt
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(586, 577);
			this.Controls.Add(this.splitContainer1);
			this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.Name = "UDataManager_MnOpt";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.Text = "Option";
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel1.PerformLayout();
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Year)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Month)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Day)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeFrom_Hour)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Hour)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Day)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Month)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numeric_LTimeTo_Year)).EndInit();
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel2.ResumeLayout(false);
			this.splitContainer2.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button btn_DoEdit;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox_Desc;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_Cond;
        private System.Windows.Forms.Button btn_Delete;
        private System.Windows.Forms.Button btn_UpDown;
        private System.Windows.Forms.Button btn_UpLevel;
        private System.Windows.Forms.TextBox textBox_Item;
        private System.Windows.Forms.Button btn_Add;
        private System.Windows.Forms.ListView listView_MonitorItems;
        private System.Windows.Forms.ColumnHeader colChk;
        private System.Windows.Forms.ColumnHeader colName;
        private System.Windows.Forms.ColumnHeader colCond;
        private System.Windows.Forms.ColumnHeader colDesc;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.NumericUpDown numeric_LTimeTo_Hour;
        private System.Windows.Forms.NumericUpDown numeric_LTimeTo_Day;
        private System.Windows.Forms.NumericUpDown numeric_LTimeTo_Month;
        private System.Windows.Forms.NumericUpDown numeric_LTimeTo_Year;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox textBox8;
        private System.Windows.Forms.NumericUpDown numeric_LTimeFrom_Hour;
        private System.Windows.Forms.NumericUpDown numeric_LTimeFrom_Day;
        private System.Windows.Forms.NumericUpDown numeric_LTimeFrom_Month;
        private System.Windows.Forms.NumericUpDown numeric_LTimeFrom_Year;
        private System.Windows.Forms.RadioButton radioButton_LogTime3_All;
        private System.Windows.Forms.RadioButton radioButton_LogTime2_AfterAll;
        private System.Windows.Forms.RadioButton radioButton_LogTime1_Range;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBox_ErrCnt;
        private System.Windows.Forms.ColumnHeader colErrCnt;
        private System.Windows.Forms.Button btn_Edit;
		private System.Windows.Forms.SplitContainer splitContainer2;
		public System.Windows.Forms.TextBox textBox_MonitorLogPath;
		public System.Windows.Forms.Button btn_MonitorDownload;
		public System.Windows.Forms.Button btn_Analyze;
		public System.Windows.Forms.Button button_selectFolder;
		public System.Windows.Forms.Button btn_stop;
		private System.Windows.Forms.Button btn_Log2MLog;
		private System.Windows.Forms.CheckBox chk_opt;
	}
}