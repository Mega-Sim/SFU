using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LogViewer
{
    public partial class UCommonTypeIn : Form
    {
        public string txt = "";
        public UCommonTypeIn()
        {
            InitializeComponent();
        }

        private void UCommonTypeIn_Load(object sender, EventArgs e)
        {
            textBox1.Text = txt;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            txt = textBox1.Text;
            DialogResult = DialogResult.OK;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

		private void textBox1_KeyDown(object sender, KeyEventArgs e)
		{
			if(e.KeyCode == Keys.Enter)
			{
				button1_Click(this, e);
			}
		}
	}
}
