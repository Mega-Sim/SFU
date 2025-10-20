using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MathGeometry2D;

namespace MapView
{
    public partial class UVisibleZLevel : Form
    {

        public DRange range = new DRange();
        public UVisibleZLevel()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            textBox_min.Text = Convert.ToString(trackBar1.Value);
            
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            textBox_max.Text = Convert.ToString(trackBar2.Value);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                range.min = Convert.ToInt32(textBox_min.Text);
                range.max = Convert.ToInt32(textBox_max.Text);
                if(range.ValidationCheck(ShowMsgBox: true) == false)
                    return;

                this.DialogResult = DialogResult.OK;
                this.Close();
            }
            catch
            {
                MessageBox.Show("정수값을 입력하세요.");
            }
        }
    }
}
