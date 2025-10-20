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
    public partial class UCommentMake : Form
    {
        public DataGridView m_ParentGrid = null;
        public string m_folderPath = "";
        public UCommentMake()
        {
            InitializeComponent();
        }

        private void UCommentMake_Load(object sender, EventArgs e)
        {

        }

        public void Init()
        {
            textBox_Comment.Text = "";
            textBox_LogFile.Text = "";
            textBox_SelectedRange.Text = "";
            textBox_AmcRecvCols.Text = "";
            textBox_AmcRecvTime.Text = "";
            textBox_AttachedFile.Text = "";
        }

        // Apply 버튼
        private void btn_Apply_Click(object sender, EventArgs e)
        {
            if (textBox_Comment.Text.Length == 0 || textBox_Author.Text.Length == 0)
            {
                MessageBox.Show("내용을 입력하세요.");
                return;
            }

            // 한 행을 추가하고 해당 행 내용을 Dialog와 같이 입력한다
            int nRow = m_ParentGrid.Rows.Add();
            m_ParentGrid.Rows[nRow].Cells["col_comment"].Value = textBox_Comment.Text;
            m_ParentGrid.Rows[nRow].Cells["col_CommentLogFile"].Value = textBox_LogFile.Text;
            m_ParentGrid.Rows[nRow].Cells["col_CommentRange"].Value = textBox_SelectedRange.Text;
            m_ParentGrid.Rows[nRow].Cells["col_CommentAuthor"].Value = textBox_Author.Text;
            m_ParentGrid.Rows[nRow].Cells["col_AmcRcvShowCols"].Value = textBox_AmcRecvCols.Text;
            m_ParentGrid.Rows[nRow].Cells["col_AmcRcvOpenRange"].Value = textBox_AmcRecvTime.Text;
            m_ParentGrid.Rows[nRow].Cells["col_AttachedFile"].Value = textBox_AttachedFile.Text;

            textBox_AttachedFile.Text = "";
            for (int i = 0; i < m_ParentGrid.Rows.Count; i++)
                m_ParentGrid.AutoResizeRow(i);

            this.Hide();
            this.Visible = false;
        }

        // 취소버튼
        private void btn_Discard_Click(object sender, EventArgs e)
        {
            Init();
            this.Hide();
            this.Visible = false;
        }

        // 창 종료 금지시킴
        private void UCommentMake_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
        }

        private void btn_FindAttached_Click(object sender, EventArgs e)
        {
            UCommentAttach dlg = new UCommentAttach();
            dlg.textBox_Path.Text = m_folderPath;
            dlg.ShowDialog();

            if (dlg.DialogResult != DialogResult.OK)
                return;

            textBox_AttachedFile.Text = dlg.m_SelectedFileName;
        }
    }
}
