using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MyTools;
using System.Diagnostics;
using System.IO;
namespace LogViewer
{
    public partial class UGridOption : Form
    {

        public DataGridView grid = null;
        float nOrgFontSize;
        public int m_nShowCntBefore, m_nShowCntAfter;
        public UEventLog refParent = null;
        
        public UGridOption()
        {
            InitializeComponent();
        }

        private void UGridSize_Load(object sender, EventArgs e)
        {
            if (grid == null)
            {
                MessageBox.Show("선택된 Table이 없습니다.");
                //Debug.Assert(false, "Grid 세팅 안함");
                return;
            }

            // 이건 EventLog 전용 파라미터이기 때문에, 다른데서 호출됐으면 안보이게 한다
            if (refParent == null)
            {
                label_sp.Visible = false;
                label_sp2.Visible = false;
                label_sp3.Visible = false;
                textBox_ShowCntBefore.Visible = false;
                textBox_ShowCntAfter.Visible = false;
            }

            Font font = grid.DefaultCellStyle.Font;
            nOrgFontSize = font.Size;
            numericUpDown_FontSize.Value = (decimal)nOrgFontSize;
            
            try
            {   textBox_ShowCntBefore.Text = Convert.ToString(m_nShowCntBefore);
                textBox_ShowCntAfter.Text = Convert.ToString(m_nShowCntAfter);
            }
            catch (Exception) { }
        }

        private void button_Apply_Click(object sender, EventArgs e)
        {
            try
            {
                if (refParent != null)
                {
                    m_nShowCntAfter = Convert.ToInt32(textBox_ShowCntAfter.Text);
                    m_nShowCntBefore = Convert.ToInt32(textBox_ShowCntBefore.Text);
                    if (m_nShowCntBefore < 10 || m_nShowCntBefore < 10)
                    {
                        MessageBox.Show("데이터 개수가 10보다 작을 수 없습니다.");
                        return;
                    }
                }

            }
            catch (Exception)
            {
                MessageBox.Show("데이터 개수 항목에 정수를 입력해 주세요.");
                return;
            }

            if (refParent != null)
            {
                refParent.m_nGridShowCnt_Before = m_nShowCntBefore;
                refParent.m_nGridShowCnt_After = m_nShowCntAfter;
            }
            // 적용하는데 시간이 어느정도 걸려서, Enable 상태로 적용중이라는걸 알려주자
            button_Apply.Enabled = false;
            button_Apply.Text = "Applying...";
            this.Invalidate();

            if ((int)numericUpDown_FontSize.Value != nOrgFontSize)
            {
                Font font = grid.DefaultCellStyle.Font;
                float nNewSize = (float)numericUpDown_FontSize.Value;
                Font newFont = new Font(font.Name, nNewSize, font.Style);
                grid.DefaultCellStyle.Font = newFont;
            }
            if ((int)numericUpDown_CellHeight.Value != 0)
            {
                DoHeight();
            }
            if (numericUpDown_CellWidth.Value != 0)
            {
                DoWidth();
            }
            if(checkBox_SaveINI.Checked)
            {
                SaveGridInfo(grid);
            }
            
            button_Apply.Text = "Apply";
            button_Apply.Enabled = true;
        }

        // 그리드의 폰트정보, 셀사이즈 등을 INI에 저장함
        // static 으로 모든 그리드가 쉽게 접근하기 위함
        public static void SaveGridInfo(DataGridView refGrid)
        {
            try
            {
                IniMan ini = new IniMan();
                ini.Path = UMain.INI_PATH;

                Font font = refGrid.DefaultCellStyle.Font;
                float fontSize = font.Size;
                int fontStype = (int)font.Style;

                ini.Write(refGrid.Name, "FontSize", Convert.ToString(fontSize));
                ini.Write(refGrid.Name, "FontStyle", Convert.ToString(fontStype));
                ini.Write(refGrid.Name, "RowTemplate.Height", Convert.ToString(refGrid.RowTemplate.Height));
                for (int i = 0; i < refGrid.Columns.Count; i++)
                {
                    ini.Write(refGrid.Name, String.Format("Col_Width_{0}", i), Convert.ToString(refGrid.Columns[i].Width));
                }
            }
            catch (Exception )
            {
                //exp.Message
                //throw;
            }
        }

        // 그리드의 폰트정보, 셀사이즈 등을 INI에 로딩
        // static 으로 모든 그리드가 쉽게 접근하기 위함
        public static void LoadGridInfo(DataGridView refGrid)
        {
            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;
            try
            {

                Font font = refGrid.DefaultCellStyle.Font;
                float OrgfontSize = font.Size;
                int OrgfontStype = (int)font.Style;

                double dFontSize = Convert.ToDouble(ini.Read(refGrid.Name, "FontSize", Convert.ToString(OrgfontSize)));
                int nFontStyle = Convert.ToInt32(ini.Read(refGrid.Name, "FontStyle", Convert.ToString(OrgfontStype)));
                Font newFont = new Font("맑은 고딕", (float)dFontSize, (FontStyle)nFontStyle);
                refGrid.DefaultCellStyle.Font = newFont;

                int Height = Convert.ToInt32(ini.Read(refGrid.Name, "RowTemplate.Height", Convert.ToString(refGrid.RowTemplate.Height)));
                for (int i = 0; i < refGrid.Columns.Count; i++)
                {
                    int nWidth = Convert.ToInt32(ini.Read(refGrid.Name, String.Format("Col_Width_{0}", i), Convert.ToString(refGrid.Columns[i].Width)));
                    refGrid.Columns[i].Width = nWidth;
                }
            }
            catch (Exception )
            {
                //Debug.Assert(false, exp.Message);
            }
        }
        private void DoWidth()
        {
            if (checkBox_Width_SelectedOnly.Checked)
            {
				List<int> selected = GridHelper.GetSelectedColumns(grid);
				foreach(int i in selected)
				{
					grid.Columns[i].Width += (int)numericUpDown_CellWidth.Value;
				}
            }

            for (int i = 0; i < grid.Columns.Count; i++)
            {
                grid.Columns[i].Width += (int)numericUpDown_CellWidth.Value;
            }

        }
        private void DoHeight()
        {
             grid.RowTemplate.Height += (int)numericUpDown_CellHeight.Value;

            foreach (DataGridViewRow row in grid.Rows)
            {
                row.Height = grid.RowTemplate.Height;
            }
            if((string)grid.Tag == "AMCRECV")
            {
                grid.Rows[0].Height = 50;     // 첫째행을 헤더처럼 쓴다
            
            }

        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            Close();
        }
		
		private void timer1_Tick(object sender, EventArgs e)
        {
            numericUpDown_FontSize.BackColor = (int)numericUpDown_FontSize.Value != nOrgFontSize ? Color.LightPink : Color.White;   
            numericUpDown_CellHeight.BackColor = (int)numericUpDown_CellHeight.Value != 0 ? Color.LightPink : Color.White;
            numericUpDown_CellWidth.BackColor = (int)numericUpDown_CellWidth.Value != 0 ? Color.LightPink : Color.White;

            int tmp1 = 0, tmp2 = 0;
            Int32.TryParse(textBox_ShowCntAfter.Text, out tmp1);
            Int32.TryParse(textBox_ShowCntBefore.Text, out tmp2);

            textBox_ShowCntAfter.BackColor = m_nShowCntAfter != tmp1 ? Color.LightPink : Color.White;
            textBox_ShowCntBefore.BackColor = m_nShowCntBefore != tmp2 ? Color.LightPink : Color.White;
 
        }
    }
}
