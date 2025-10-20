using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Reflection;

namespace VibrationManager
{
    partial class UAnalysisOutput
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
            this.NodeList = new System.Windows.Forms.DataGridView();
            this.label1 = new System.Windows.Forms.Label();
            this.AllList = new System.Windows.Forms.DataGridView();
            this.EventList = new System.Windows.Forms.DataGridView();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.DetailLink = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.DetailAvg = new System.Windows.Forms.TextBox();
            this.DetailMin = new System.Windows.Forms.TextBox();
            this.DetailMax = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.DetailOffset = new System.Windows.Forms.TextBox();
            this.DetailVel = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.MaxCal = new System.Windows.Forms.TextBox();
            this.MinCal = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.AllCal = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label100 = new System.Windows.Forms.Label();
            this.Summary = new System.Windows.Forms.DataGridView();
            this.DetailLinkType = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.DetailDetectLevel = new System.Windows.Forms.TextBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            ((System.ComponentModel.ISupportInitialize)(this.NodeList)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AllList)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EventList)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Summary)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // NodeList
            // 
            this.NodeList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.NodeList.Location = new System.Drawing.Point(14, 55);
            this.NodeList.Name = "NodeList";
            this.NodeList.RowTemplate.Height = 23;
            this.NodeList.Size = new System.Drawing.Size(697, 313);
            this.NodeList.TabIndex = 3;
            this.NodeList.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.NodeList_CellDoubleClick);
            this.NodeList.SortCompare += new System.Windows.Forms.DataGridViewSortCompareEventHandler(this.NodeList_SortCompare);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label1.Location = new System.Drawing.Point(14, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(152, 27);
            this.label1.TabIndex = 4;
            this.label1.Text = "Spec Over";
            // 
            // AllList
            // 
            this.AllList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.AllList.Location = new System.Drawing.Point(741, 484);
            this.AllList.Name = "AllList";
            this.AllList.RowTemplate.Height = 23;
            this.AllList.Size = new System.Drawing.Size(697, 313);
            this.AllList.TabIndex = 6;
            this.AllList.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.AllList_CellDoubleClick);
            // 
            // EventList
            // 
            this.EventList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.EventList.Location = new System.Drawing.Point(14, 484);
            this.EventList.Name = "EventList";
            this.EventList.RowTemplate.Height = 23;
            this.EventList.Size = new System.Drawing.Size(697, 313);
            this.EventList.TabIndex = 7;
            this.EventList.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.EventList_CellDoubleClick);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label2.Location = new System.Drawing.Point(736, 394);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(115, 27);
            this.label2.TabIndex = 8;
            this.label2.Text = "All Data";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label3.Location = new System.Drawing.Point(14, 394);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(88, 27);
            this.label3.TabIndex = 9;
            this.label3.Text = "Detail";
            // 
            // DetailLink
            // 
            this.DetailLink.Font = new System.Drawing.Font("굴림", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.DetailLink.Location = new System.Drawing.Point(174, 400);
            this.DetailLink.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailLink.Name = "DetailLink";
            this.DetailLink.Size = new System.Drawing.Size(67, 20);
            this.DetailLink.TabIndex = 10;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label4.Location = new System.Drawing.Point(263, 444);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(33, 12);
            this.label4.TabIndex = 11;
            this.label4.Text = "Max";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label5.Location = new System.Drawing.Point(415, 444);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(29, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "Min";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label6.Location = new System.Drawing.Point(124, 402);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 12);
            this.label6.TabIndex = 13;
            this.label6.Text = "Link";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label7.Location = new System.Drawing.Point(124, 444);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(29, 12);
            this.label7.TabIndex = 14;
            this.label7.Text = "Avg";
            // 
            // DetailAvg
            // 
            this.DetailAvg.Location = new System.Drawing.Point(174, 440);
            this.DetailAvg.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailAvg.Name = "DetailAvg";
            this.DetailAvg.Size = new System.Drawing.Size(67, 21);
            this.DetailAvg.TabIndex = 15;
            // 
            // DetailMin
            // 
            this.DetailMin.Location = new System.Drawing.Point(474, 440);
            this.DetailMin.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailMin.Name = "DetailMin";
            this.DetailMin.Size = new System.Drawing.Size(67, 21);
            this.DetailMin.TabIndex = 16;
            // 
            // DetailMax
            // 
            this.DetailMax.Location = new System.Drawing.Point(311, 440);
            this.DetailMax.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailMax.Name = "DetailMax";
            this.DetailMax.Size = new System.Drawing.Size(67, 21);
            this.DetailMax.TabIndex = 17;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label8.Location = new System.Drawing.Point(414, 402);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(43, 12);
            this.label8.TabIndex = 18;
            this.label8.Text = "Offset";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label9.Location = new System.Drawing.Point(567, 402);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(26, 12);
            this.label9.TabIndex = 19;
            this.label9.Text = "Vel";
            // 
            // DetailOffset
            // 
            this.DetailOffset.Location = new System.Drawing.Point(474, 399);
            this.DetailOffset.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailOffset.Name = "DetailOffset";
            this.DetailOffset.Size = new System.Drawing.Size(67, 21);
            this.DetailOffset.TabIndex = 20;
            // 
            // DetailVel
            // 
            this.DetailVel.Location = new System.Drawing.Point(631, 399);
            this.DetailVel.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailVel.Name = "DetailVel";
            this.DetailVel.Size = new System.Drawing.Size(67, 21);
            this.DetailVel.TabIndex = 21;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label10.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label10.Location = new System.Drawing.Point(1149, 404);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(97, 12);
            this.label10.TabIndex = 22;
            this.label10.Text = "모수계산(Max)";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label11.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label11.Location = new System.Drawing.Point(1153, 445);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(93, 12);
            this.label11.TabIndex = 23;
            this.label11.Text = "모수계산(Min)";
            // 
            // MaxCal
            // 
            this.MaxCal.Location = new System.Drawing.Point(1278, 399);
            this.MaxCal.Name = "MaxCal";
            this.MaxCal.Size = new System.Drawing.Size(133, 21);
            this.MaxCal.TabIndex = 24;
            // 
            // MinCal
            // 
            this.MinCal.Location = new System.Drawing.Point(1278, 440);
            this.MinCal.Name = "MinCal";
            this.MinCal.Size = new System.Drawing.Size(133, 21);
            this.MinCal.TabIndex = 25;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label12.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label12.Location = new System.Drawing.Point(862, 404);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(86, 12);
            this.label12.TabIndex = 26;
            this.label12.Text = "모수계산(All)";
            // 
            // AllCal
            // 
            this.AllCal.Location = new System.Drawing.Point(979, 399);
            this.AllCal.Name = "AllCal";
            this.AllCal.Size = new System.Drawing.Size(133, 21);
            this.AllCal.TabIndex = 27;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label14.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label14.Location = new System.Drawing.Point(736, 15);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(135, 27);
            this.label14.TabIndex = 30;
            this.label14.Text = "Summary";
            // 
            // label100
            // 
            this.label100.AutoSize = true;
            this.label100.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label100.Location = new System.Drawing.Point(258, 402);
            this.label100.Name = "label100";
            this.label100.Size = new System.Drawing.Size(38, 12);
            this.label100.TabIndex = 32;
            this.label100.Text = "Type";
            // 
            // Summary
            // 
            this.Summary.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Summary.Location = new System.Drawing.Point(741, 55);
            this.Summary.Name = "Summary";
            this.Summary.RowTemplate.Height = 23;
            this.Summary.Size = new System.Drawing.Size(697, 313);
            this.Summary.TabIndex = 33;
            // 
            // DetailLinkType
            // 
            this.DetailLinkType.Location = new System.Drawing.Point(311, 400);
            this.DetailLinkType.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailLinkType.Name = "DetailLinkType";
            this.DetailLinkType.Size = new System.Drawing.Size(67, 21);
            this.DetailLinkType.TabIndex = 34;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label16.Location = new System.Drawing.Point(567, 443);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(46, 12);
            this.label16.TabIndex = 35;
            this.label16.Text = "Detect";
            // 
            // DetailDetectLevel
            // 
            this.DetailDetectLevel.Location = new System.Drawing.Point(631, 440);
            this.DetailDetectLevel.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailDetectLevel.Name = "DetailDetectLevel";
            this.DetailDetectLevel.Size = new System.Drawing.Size(67, 21);
            this.DetailDetectLevel.TabIndex = 36;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1});
            this.toolStrip1.Location = new System.Drawing.Point(598, 10);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(117, 42);
            this.toolStrip1.TabIndex = 37;
            this.toolStrip1.Text = "MapView";
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.toolStripButton1.Image = global::VibrationManager.Properties.Resources.MapView_35x351;
            this.toolStripButton1.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(105, 39);
            this.toolStripButton1.Text = "MapView";
            this.toolStripButton1.Click += new System.EventHandler(this.toolStripButton1_Click_1);
            // 
            // UAnalysisOutput
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(1453, 813);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.DetailDetectLevel);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.DetailLinkType);
            this.Controls.Add(this.Summary);
            this.Controls.Add(this.label100);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.AllCal);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.MinCal);
            this.Controls.Add(this.MaxCal);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.DetailVel);
            this.Controls.Add(this.DetailOffset);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.DetailMax);
            this.Controls.Add(this.DetailMin);
            this.Controls.Add(this.DetailAvg);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.DetailLink);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.EventList);
            this.Controls.Add(this.AllList);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.NodeList);
            this.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Name = "UAnalysisOutput";
            ((System.ComponentModel.ISupportInitialize)(this.NodeList)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AllList)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EventList)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Summary)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DataGridView NodeList;
        private Label label1;
        private DataGridView AllList;
        private DataGridView EventList;
        private Label label2;
        private Label label3;
        private TextBox DetailLink;
        private Label label4;
        private Label label5;
        private Label label6;
        private Label label7;
        private TextBox DetailAvg;
        private TextBox DetailMin;
        private TextBox DetailMax;
        private Label label8;
        private Label label9;
        private TextBox DetailOffset;
        private TextBox DetailVel;
        private Label label10;
        private Label label11;
        private TextBox MaxCal;
        private TextBox MinCal;
        private Label label12;
        private TextBox AllCal;
        private Label label14;
        private Label label100;
        private DataGridView Summary;
        private TextBox DetailLinkType;
        private Label label16;
        private TextBox DetailDetectLevel;
        private ToolStrip toolStrip1;
        private ToolStripButton toolStripButton1;
    }
}