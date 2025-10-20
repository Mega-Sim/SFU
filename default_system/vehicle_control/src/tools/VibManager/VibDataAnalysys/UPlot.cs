using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;


namespace VibrationManager
{

    public partial class UPlot : Form
    {

        public UPlot(NodeData[] Data, double MaxSpeed, double MinSpeed)
        {
            InitializeComponent();

            VerticalLineAnnotation VA; VerticalLineAnnotation VA2;
            RectangleAnnotation RA; 
            ChartArea CA; Series S1;
            ChartArea CA2; Series S2;

            CA = Graph.ChartAreas[0];
            S1 = Graph.Series[0];      
            CA2 = Graph2.ChartAreas[0];
            S2 = Graph.Series[0];

            VA = new VerticalLineAnnotation();
            VA.AxisX = CA.AxisX;
            VA.AllowMoving = true;
            VA.IsInfinitive = true;
            VA.ClipToChartArea = CA.Name;
            VA.Name = "myLine";
            VA.LineColor = Color.Red;
            VA.LineWidth = 2;         // use your numbers!

            RA = new RectangleAnnotation();
            RA.AxisX = CA.AxisX;
            RA.AllowMoving = true;
            RA.IsSizeAlwaysRelative = false;
            RA.Width = Data.Count()/17;         
            RA.LineColor = Color.Red;
            RA.BackColor = Color.Red;
            RA.AxisY = CA.AxisY2;
            RA.Height = MaxSpeed/17;

            VA2 = new VerticalLineAnnotation();
            VA2.AxisX = CA2.AxisX;
            VA2.AllowMoving = true;
            VA2.IsInfinitive = true;
            VA2.ClipToChartArea = CA2.Name;
            VA2.Name = "myLine";
            VA2.LineColor = Color.Red;
            VA2.LineWidth = 2;         // use your numbers!
            VA2.X = 1;


            Graph.Series["RMS"].Points.Clear();
            Graph.Series["Velocity"].Points.Clear();

            Graph2.Series["X"].Points.Clear();
            Graph2.Series["Y"].Points.Clear();
            Graph2.Series["Z"].Points.Clear();
            Graph2.Series["X integral"].Points.Clear();

            int MaxIdx = 0;
            double inteX = 0;            
            double MaxRms = 0;

            try
            {

                for (int j = 1; j <= Data.Count(); j++)
                {
                    double Rms = Data[j].VibMax;
                    double Vel = Data[j].Vel;
                    double x = Data[j].MaxX;
                    double y = Data[j].MaxY;
                    double z = Data[j].MaxZ;
                    double speed = Data[j].Vel;

                    inteX += x*(-1);

                    if (MaxRms < Rms)
                    {
                        MaxRms = Rms;
                        MaxIdx = j;
                    }

                    Graph.Series["RMS"].Points.AddXY(j, Rms);
                    Graph.Series["Velocity"].Points.AddXY(j, Vel);
                    Graph2.Series["X"].Points.AddXY(j, x);
                    Graph2.Series["Y"].Points.AddXY(j, y);
                    Graph2.Series["Z"].Points.AddXY(j, z);
                    Graph2.Series["X integral"].Points.AddXY(j, inteX);
                }

            }
            catch
            {
                
            }

            DetailMax.Text = String.Format("{0:0} mm/s", MaxSpeed);
            DetailMin.Text = String.Format("{0:0} mm/s", MinSpeed);
            DetailMinus.Text = String.Format("{0:0}", MaxSpeed-MinSpeed);

            VA.X = (int)(MaxIdx);
            VA.Y = 0;

            RA.X = VA.X - RA.Width / 2;
            RA.Y = -RA.Height;
            RA.ForeColor = Color.White;
            RA.Text = String.Format("{0:0.00}", MaxRms);

            VA2.X = (int)(MaxIdx);
            VA2.Y = 0;

            Graph.Annotations.Add(VA);
            Graph.Annotations.Add(RA);
            Graph2.Annotations.Add(VA2);

            Graph.ChartAreas["ChartArea1"].AxisX.MajorGrid.Enabled = false;
            Graph.ChartAreas["ChartArea1"].AxisY.MajorGrid.Enabled = false;
            Graph.ChartAreas["ChartArea1"].AxisY2.MajorGrid.Enabled = false;

            Graph2.ChartAreas["ChartArea1"].AxisX.MajorGrid.Enabled = false;
            Graph2.ChartAreas["ChartArea1"].AxisY.MajorGrid.Enabled = false;
            Graph2.ChartAreas["ChartArea1"].AxisY2.MajorGrid.Enabled = false;


            this.Show();
        }

        private void Graph_Click(object sender, EventArgs e)
        {

        }
    }
}
