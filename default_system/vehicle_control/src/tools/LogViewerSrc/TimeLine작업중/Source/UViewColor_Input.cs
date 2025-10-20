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
    public partial class UViewColor_Input : Form
    {
        public string strCondition = "";
        public UViewColor_Input()
        {
            InitializeComponent();
        }

        private void button_apply_Click(object sender, EventArgs e)
        {
            strCondition = textBox1.Text;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}
