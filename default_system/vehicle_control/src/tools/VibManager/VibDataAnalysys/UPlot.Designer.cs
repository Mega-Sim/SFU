namespace VibrationManager
{
    partial class UPlot
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.Graph = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.Graph2 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.DetailMax = new System.Windows.Forms.TextBox();
            this.DetailMin = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.DetailMinus = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.Graph)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Graph2)).BeginInit();
            this.SuspendLayout();
            // 
            // Graph
            // 
            this.Graph.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.Graph.BorderlineColor = System.Drawing.Color.Transparent;
            this.Graph.BorderSkin.BackColor = System.Drawing.Color.Transparent;
            this.Graph.BorderSkin.PageColor = System.Drawing.Color.Transparent;
            this.Graph.BorderSkin.SkinStyle = System.Windows.Forms.DataVisualization.Charting.BorderSkinStyle.Emboss;
            chartArea1.BackColor = System.Drawing.Color.Transparent;
            chartArea1.BackSecondaryColor = System.Drawing.Color.White;
            chartArea1.BorderColor = System.Drawing.Color.Transparent;
            chartArea1.Name = "ChartArea1";
            this.Graph.ChartAreas.Add(chartArea1);
            this.Graph.Cursor = System.Windows.Forms.Cursors.Default;
            legend1.Name = "Legend1";
            this.Graph.Legends.Add(legend1);
            this.Graph.Location = new System.Drawing.Point(12, 49);
            this.Graph.Name = "Graph";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series1.Color = System.Drawing.Color.Blue;
            series1.Legend = "Legend1";
            series1.MarkerBorderColor = System.Drawing.Color.Transparent;
            series1.Name = "RMS";
            series1.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series2.ChartArea = "ChartArea1";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series2.Color = System.Drawing.Color.Fuchsia;
            series2.LabelForeColor = System.Drawing.Color.Cyan;
            series2.Legend = "Legend1";
            series2.MarkerBorderWidth = 0;
            series2.Name = "Velocity";
            series2.YAxisType = System.Windows.Forms.DataVisualization.Charting.AxisType.Secondary;
            this.Graph.Series.Add(series1);
            this.Graph.Series.Add(series2);
            this.Graph.Size = new System.Drawing.Size(848, 352);
            this.Graph.TabIndex = 1;
            this.Graph.Text = "VIbChart";
            this.Graph.Click += new System.EventHandler(this.Graph_Click);
            // 
            // Graph2
            // 
            this.Graph2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.Graph2.BorderlineColor = System.Drawing.Color.Transparent;
            this.Graph2.BorderSkin.BackColor = System.Drawing.Color.Transparent;
            this.Graph2.BorderSkin.PageColor = System.Drawing.Color.Transparent;
            this.Graph2.BorderSkin.SkinStyle = System.Windows.Forms.DataVisualization.Charting.BorderSkinStyle.Emboss;
            chartArea2.BackColor = System.Drawing.Color.Transparent;
            chartArea2.BackSecondaryColor = System.Drawing.Color.White;
            chartArea2.BorderColor = System.Drawing.Color.Transparent;
            chartArea2.Name = "ChartArea1";
            this.Graph2.ChartAreas.Add(chartArea2);
            this.Graph2.Cursor = System.Windows.Forms.Cursors.Default;
            legend2.Name = "Legend1";
            this.Graph2.Legends.Add(legend2);
            this.Graph2.Location = new System.Drawing.Point(12, 396);
            this.Graph2.Name = "Graph2";
            series3.ChartArea = "ChartArea1";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series3.Color = System.Drawing.Color.SpringGreen;
            series3.Legend = "Legend1";
            series3.Name = "X";
            series4.ChartArea = "ChartArea1";
            series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series4.Color = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
            series4.Legend = "Legend1";
            series4.Name = "Y";
            series5.ChartArea = "ChartArea1";
            series5.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series5.Color = System.Drawing.Color.DarkOrange;
            series5.Legend = "Legend1";
            series5.Name = "Z";
            series6.ChartArea = "ChartArea1";
            series6.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series6.Color = System.Drawing.Color.Fuchsia;
            series6.Legend = "Legend1";
            series6.Name = "X integral";
            series6.YAxisType = System.Windows.Forms.DataVisualization.Charting.AxisType.Secondary;
            this.Graph2.Series.Add(series3);
            this.Graph2.Series.Add(series4);
            this.Graph2.Series.Add(series5);
            this.Graph2.Series.Add(series6);
            this.Graph2.Size = new System.Drawing.Size(848, 352);
            this.Graph2.TabIndex = 2;
            this.Graph2.Text = "VIbChart";
            // 
            // DetailMax
            // 
            this.DetailMax.Location = new System.Drawing.Point(291, 17);
            this.DetailMax.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailMax.Name = "DetailMax";
            this.DetailMax.Size = new System.Drawing.Size(67, 21);
            this.DetailMax.TabIndex = 22;
            // 
            // DetailMin
            // 
            this.DetailMin.Location = new System.Drawing.Point(514, 17);
            this.DetailMin.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailMin.Name = "DetailMin";
            this.DetailMin.Size = new System.Drawing.Size(67, 21);
            this.DetailMin.TabIndex = 21;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label5.Location = new System.Drawing.Point(445, 20);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(63, 13);
            this.label5.TabIndex = 20;
            this.label5.Text = "최소속도";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label4.Location = new System.Drawing.Point(222, 20);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(63, 13);
            this.label4.TabIndex = 19;
            this.label4.Text = "최대속도";
            // 
            // DetailMinus
            // 
            this.DetailMinus.Location = new System.Drawing.Point(80, 17);
            this.DetailMinus.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DetailMinus.Name = "DetailMinus";
            this.DetailMinus.Size = new System.Drawing.Size(67, 21);
            this.DetailMinus.TabIndex = 24;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(25, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(49, 13);
            this.label1.TabIndex = 23;
            this.label1.Text = "속도차";
            // 
            // Vib_Plot
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(873, 785);
            this.Controls.Add(this.DetailMinus);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.DetailMax);
            this.Controls.Add(this.DetailMin);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.Graph2);
            this.Controls.Add(this.Graph);
            this.Name = "Vib_Plot";
            this.Text = "Vib_Plot";
            ((System.ComponentModel.ISupportInitialize)(this.Graph)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Graph2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart Graph;
        private System.Windows.Forms.DataVisualization.Charting.Chart Graph2;
        private System.Windows.Forms.TextBox DetailMax;
        private System.Windows.Forms.TextBox DetailMin;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox DetailMinus;
        private System.Windows.Forms.Label label1;
    }
}