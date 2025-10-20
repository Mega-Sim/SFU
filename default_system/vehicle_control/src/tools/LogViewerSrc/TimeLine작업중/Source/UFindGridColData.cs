using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;

namespace LogViewer
{
    public partial class UFindGridColData : Form
    {
        private DataGridView parentGrid = null;
        private int nCurCol = -1;
        private int nCurRow = -1;
        public UFindGridColData()
        {
            InitializeComponent();
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.UFindGridColData_FormClosing);
            textBox_FindText.Focus();
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
        }

        public void SetCurrentGrid(DataGridView grid)
        {
            try
            {

                this.parentGrid = grid;
                if (this.parentGrid.SelectedCells.Count == 0)
                {
                    this.label_CurCol.Text = "";

                }
                nCurCol = this.parentGrid.SelectedCells[0].ColumnIndex;
                nCurRow = this.parentGrid.SelectedCells[0].RowIndex;
                this.label_CurCol.Text = this.parentGrid.Columns[nCurCol].Name;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                //throw;
            }
        }
        private void button_Find_Click(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(textBox_FindText.Text))
                return;
            if (parentGrid == null)
                return;
            //if (this.parentGrid.SelectedCells.Count == 0)
            //{
            //    this.label_CurCol.Text = "";
            //    return;

            //}
            //nCurCol = this.parentGrid.SelectedCells[0].ColumnIndex;
            //nCurRow = this.parentGrid.SelectedCells[0].RowIndex;
            bool bContain = checkBox_Contain.Checked;
            int nAddIdx = nCurRow;
            parentGrid.ClearSelection();

            bool bFound = false;
            int nRowIdx = 0;
            //parentGrid.Sort(parentGrid.Columns[0], ListSortDirection.Ascending);
            for (int i = 0; i < parentGrid.Rows.Count; i++)
            {
                nRowIdx = i + nAddIdx + 1;
                nRowIdx %= parentGrid.Rows.Count;
                DataGridViewRow row = parentGrid.Rows[nRowIdx];
                string str = String.Format("{0}", row.Cells[nCurCol].Value);
                bFound = bContain ? str.ToUpper().Contains(textBox_FindText.Text.ToUpper()) : str.ToUpper() == textBox_FindText.Text.ToUpper();
                if (bFound)
                {
                    nCurRow = nRowIdx;
                    row.Cells[nCurCol].Selected = true;
                    parentGrid.CurrentCell = row.Cells[nCurCol];
                    parentGrid.FirstDisplayedScrollingRowIndex = nRowIdx;
                    break;
                }
            }
            
            if (!bFound)
            {
                this.BringToFront();
                MessageBox.Show(textBox_FindText.Text + "를 찾을 수 없습니다.", "Fail", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void textBox_FindText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button_Find_Click(this, e);
        }

        private void UFindGridColData_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }
    }
}
