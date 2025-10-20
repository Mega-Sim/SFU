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
    public partial class UFlag : Form
    {
        public string strMemo = "";
        public UFlag()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
        }

        private void button_apply_Click(object sender, EventArgs e)
        {
            strMemo = textBox_flagMemo.Text;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void textBox_flagMemo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button_apply_Click(this, e);
        }
    }
}
