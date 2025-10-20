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
namespace LogViewer
{
    public partial class UAmcRecvOpen : Form
    {
        public int nBefore_sec;
        public int nAfter_sec;
        public bool bCheck;
        public double []WheelUnitPulse = { 1, 1, 1, 1 };
        public DateTime m_SelectedTime;
        
        private void UAmcRecvOpen_Load(object sender, EventArgs e)
        {
            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;

            string before = ini.Read("AMCRecvOpen", "BeforeSec", "40");
            string after = ini.Read("AMCRecvOpen", "AfterSec", "20");
            
            textBox_time.Text = String.Format("{0:00}:{1:00}:{2:00}", m_SelectedTime.Hour, m_SelectedTime.Minute, m_SelectedTime.Second);
            textBox_before.Text = String.Format("{0}", before);
            textBox_after.Text = String.Format("{0}", after);
        }

        private void btn_Discard_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        public UAmcRecvOpen()
        {
            InitializeComponent();
        }

        private void btn_Apply_Click(object sender, EventArgs e)
        {
            try
            {
                int Hour, Min, Sec;

                string[] arr = textBox_time.Text.Split(':');
                if(arr.Length != 3)
                    throw new Exception();
                if (!Int32.TryParse(arr[0], out Hour))
                    throw new Exception();
                if (!Int32.TryParse(arr[1], out Min))
                    throw new Exception();
                if (!Int32.TryParse(arr[2], out Sec))
                    throw new Exception();
                
                m_SelectedTime = new DateTime(m_SelectedTime.Year, m_SelectedTime.Month, m_SelectedTime.Day, Hour, Min, Sec);
            }
            catch (Exception)
            {
                MessageBox.Show("시간 파싱 실패");
                return;
            }
            
            // 예외값 체크
            if (!Int32.TryParse(textBox_before.Text, out nBefore_sec) || !Int32.TryParse(textBox_after.Text, out nAfter_sec))
            {
                MessageBox.Show("정수를 입력해야 합니다.");
                return;
            }

            if (nBefore_sec < 0 || nBefore_sec > 1800 || nAfter_sec < 0 || nAfter_sec > 1800)
            {
                MessageBox.Show("0~1800 초의 범위만 가능합니다.");
                return;
            }

            bCheck = CheckGearRatio.Checked ? true : false;

            if(bCheck)
            {
                if (!double.TryParse(Axis1.Text, out WheelUnitPulse[0]) || !double.TryParse(Axis2.Text, out WheelUnitPulse[1])||
                    !double.TryParse(Axis4.Text, out WheelUnitPulse[3])) //!double.TryParse(Axis3.Text, out WheelUnitPulse[2]) || 
                {
                    MessageBox.Show("정수를 입력해야 합니다.");
                    return;
                }

                foreach(double n in WheelUnitPulse)
                {
                    if (n <= 0)
                    {
                        MessageBox.Show("0보다 큰 수를 입력해야 합니다.");
                        return;
                    }
                }

            }            

            IniMan ini = new IniMan();
            ini.Path = UMain.INI_PATH;

            ini.Write("AMCRecvOpen", "BeforeSec", textBox_before.Text);
            ini.Read("AMCRecvOpen", "AfterSec", textBox_after.Text);
            
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}