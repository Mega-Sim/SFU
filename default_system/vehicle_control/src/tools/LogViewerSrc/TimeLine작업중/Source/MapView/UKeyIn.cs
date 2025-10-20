using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MapView
{
    public partial class UKeyIn : Form
    {
        public string strVal = "";
        public UKeyIn()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            strVal = textBox1.Text;
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
