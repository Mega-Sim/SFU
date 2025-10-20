using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Threading;
using Msg;
using System.Net;
using System.Net.Sockets;


namespace LogViewer
{
    public partial class UMapViewMan : Form
    {
        public enum MapViewStat
        {
            None = 0                // 아직 신호 받지 못함
            , Standby               // 맴 잘열려있고 대기상태
            , EmptyMap          // 맵 아직 안연상태
            , Openning          // 여는중
            , Working           // 뭔가 다른 작업중
        }
        // Inner 클래스 *****************************************
        public class DMapViewStatus
        {
            public MapViewStat status = MapViewStat.None;
            public string prj = "";
            public string line = "";
            public string version = "";
            public string date = "";
            public string path = "";
        };
        // *****************************************************************

        List<DMapViewStatus> mapViewStatList = new List<DMapViewStatus>();
        OpenFileDialog m_dlgMapOpen = new OpenFileDialog();



        public class UdpState
        {
            public IPEndPoint e;
            public UdpClient u;
        }

        private delegate void DelegateUpdateList(string msg);
        private DelegateUpdateList delegateUpdate;
        UdpState udpState = new UdpState();
        UdpClient listenSock = null;
        int m_nMyPortNum = 0;
        public UMapViewMan()
        {
            InitializeComponent();
            comboBox_Elem.SelectedIndex = 0;

            delegateUpdate = new DelegateUpdateList(UpdateListBox);


            // UDP 초기화
            /// MapView와 통신하기 위한 소켓 UDP
            // OHT Easy View : 10270 ~ 30개
            // MapView : 10370 ~ 30개

            // 해당 포트가 사용중이면 exception 발생해서 다음 포트를 검색한다
            for (int i = 0; i < 30; i++)
            {
                try
                {
                    listenSock = new UdpClient(10270 + i);
                    udpState.e = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 10270 + i);
                    m_nMyPortNum = 10270 + i;
                    break;
                }
                catch (Exception)
                {
                    continue;
                }
            }

            if (listenSock != null)
            {
                udpState.u = listenSock;
                listenSock.BeginReceive(new AsyncCallback(ReceiveCallback), udpState);  // 콜백 등록
            }
            else
            {
                if (MessageBox.Show("LogViewer 프로세스가 할당된 포트번호 개수 보다 더 많이 실행중입니다. 이 경우 MapView와 통신이 불가능 합니다. 안쓰는 OHTEasyView는 종료 후 다시 실행시켜 주세요.", "Warning", MessageBoxButtons.AbortRetryIgnore, MessageBoxIcon.Warning) != DialogResult.Ignore)
                    Application.Exit();
            }
        }

        public static UMapViewMan GetThis()
        {
            return UMain.GetThis().m_dlgMapMan;
        }
        public void SetNodeID(string val = "", bool bIsStation = false)
        {
            if (String.IsNullOrEmpty(val) == false)
                textBox_ID.Text = val;

            textBox_ID.Focus();
            comboBox_Elem.SelectedIndex = bIsStation ? 1 : 0;
            this.Show();
            this.BringToFront();
            
        }
        public void SetNodeID(int val = 0, bool bIsStation = false)
        {
            SetNodeID(String.Format("{0}", val), bIsStation);
        }

        // UDP Recv Callback
        public static void ReceiveCallback(IAsyncResult arg)
        {
            UdpClient u = (UdpClient)((UdpState)(arg.AsyncState)).u;
            IPEndPoint e = (IPEndPoint)((UdpState)(arg.AsyncState)).e;

            Byte[] receiveBytes = u.EndReceive(arg, ref e);
            string receiveString = Encoding.Default.GetString(receiveBytes);

            UMapViewMan form = Program.MainForm.m_dlgMapMan;
            if (receiveString.Length > 5)
            {
                
                // 받은 데이터로 리스트박스 갱신
                form.Invoke(form.delegateUpdate, receiveString);
                
            }
            
            form.listenSock.BeginReceive(new AsyncCallback(ReceiveCallback), form.udpState);      // 콜백 다시 ??등록
        }

        /// ******************************************************************************************
        /// <summary>   
        /// UDP 소켓으로 MapView에 데이터 전달하는 함수 
        /// </summary>
        public void SendData_toMapView(string data, int nPortNo = -1)   // -1은 broadcast 의미 10270 ~ 10290 까지 LogViewer
        {
            byte[] send_buffer = Encoding.Default.GetBytes(data);

            if (nPortNo == -1)       // send to all
            {
                for (int i = 0; i < 30; i++)
                {
                    try
                    {
                        IPEndPoint ad = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 10370+i);
                        Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                        s.SendTo(send_buffer, ad);       // UDP Send To
                    }
                    catch
                    {
                        continue;
                    }
                }
                return;
            }

            // 특정 client한테만 send
            try
            {
                IPEndPoint addr = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPortNo);  // 포트번호는 의미있게 내생일 10월 27일
                Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                sock.SendTo(send_buffer, addr);       // UDP Send To
            }
            catch (Exception e)
            {
                MessageBox.Show("MapView에 데이터 전송 실패.\n\n", e.Message);
            }

        }

        private bool m_bMapViewRequsted = false;   // 현재 MapView에 요청한 상태인지
        private int m_nTickRequest = 0;
        
        // Refresh 버튼 처리. 맵뷰 프로세스를 탐색한다
        private void button_Refresh_Click(object sender, EventArgs e)
        {
            if (m_bMapViewRequsted)
                return;

            listView_MapList.Items.Clear();
            m_bMapViewRequsted = true; // 요청 한 상태
            m_nTickRequest = Environment.TickCount;
            timer1.Start();

            string str = String.Format("{0}/{1}", Cmd.LOG_REQ_STATUS, m_nMyPortNum);
            SendData_toMapView(str);    // 데이터 보낸다
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            // 일정 시간동안 들어온 값들만 인정하기 위함
            if(Environment.TickCount - m_nTickRequest > 4000)
            {
                timer1.Stop();
                m_bMapViewRequsted = false;;
            }
        }

        // 메시지 받는 UMain의 Proc 함수에서 호출받는 함수
        // 기대값 : REP_STATUS / 1 / P-PRJ / P1L_7F / 0.1577 / 2018-04-27 오전 10:15 / D:\....
        public void UpdateListBox(string msg)
        {
            try
            {
                if (m_bMapViewRequsted == false)    // 요청하지 않았을 때 오면 무시
                    return;

                string[] arr = msg.Split('/');
                if (msg.Length < 5)
                    return;
                
                if (arr[0] == Cmd.MAP_REP_STATUS)
                {
                    if (arr.Length < 7)
                        return;

                    // 기대값 : REP_STATUS / P-PRJ / P1L_7F / 0.1577 / 2018-04-27 오전 10:15 / D:\.... / MapView포트번호
                   int nStat = 0;
                    Int32.TryParse(arr[1], out nStat);
                    MapViewStat curStat = (MapViewStat)nStat;

                    // 리스트 아이템 생성 후 데이터 꾸미기
                    string[] arrTmp = new string[listView_MapList.Columns.Count];
                    ListViewItem item = listView_MapList.Items.Add(new ListViewItem(arrTmp));

                    item.SubItems[0].Text = Convert.ToString(listView_MapList.Items.Count);
                    item.SubItems[1].Text = arr[2];
                    item.SubItems[2].Text = arr[3];
                    item.SubItems[3].Text = arr[4];
                    item.SubItems[4].Text = arr[5];
                    item.SubItems[5].Text = curStat.ToString();
                    item.SubItems[6].Text = arr[6];

                    int nPortNum = -1;
                    Int32.TryParse(arr[7], out nPortNum);
                    item.Tag = nPortNum;  //tag에 해당 port 번호를 넣는다

                    item.SubItems[5].ForeColor = curStat == MapViewStat.Standby ? Color.LightGreen : Color.LightPink;
                }

            }
            catch (Exception){            }
            
        }
        // 리스트에서 더블클릭하면 해당 맵 표시한다
        private void listView_MapList_DoubleClick(object sender, EventArgs e)
        {
            if (listView_MapList.SelectedItems.Count < 1)
                return;
            try
            {
                ListViewItem lvi = listView_MapList.SelectedItems[0];
                
                if (lvi.Tag == null)
                    return;
                int nPort = (int)lvi.Tag;
                if (nPort < 10370 || nPort >= 10400)
                    return;

                string str = String.Format("{0}/1", Cmd.LOG_REQ_SHOW);
                SendData_toMapView(str, nPort);        // 맵뷰 창 띄워달라고 요청
            }
            catch (Exception exp)
            {
                MessageBox.Show("맵 열기 실패\n\n" + exp.Message);
                throw;
            }
        }

        static string lastPath = "";
        private void button_MapOpen_Click(object sender, EventArgs e)
        {

            if(lastPath != "")
                m_dlgMapOpen.InitialDirectory = lastPath;
            m_dlgMapOpen.Filter = "Map File (*^*^*^*.zip)|*^*^*^*.zip";
            m_dlgMapOpen.Multiselect = true;

            if (m_dlgMapOpen.ShowDialog() != DialogResult.OK)
                return;

            lastPath = Path.GetDirectoryName(m_dlgMapOpen.FileName);

            foreach(string path in m_dlgMapOpen.FileNames)
            {
                // map extract 프로세스 시작
                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = Application.StartupPath + @"\MapView.exe";
                //startInfo.WindowStyle = ProcessWindowStyle.Hidden;  // 백그라운드로 실행
                startInfo.Arguments = @"""" + path + @"""" + " /MINIZE";
                Process pro = Process.Start(startInfo);
            } 
        }

        // 노드나, 스테이션 찾기 버튼 클릭
        private void button_FindNode_Click(object sender, EventArgs e)
        {
            
            if (textBox_CurMap.Tag == null)
            {
                MessageBox.Show("선택된 맵이 없습니다.");
                return;
            }
            try
            {
                //ListViewItem lvi = listView_MapList.SelectedItems[0];
                ////Process pro = (Process)lvi.Tag;
                ////if (pro == null)
                ////    return;
                //if(lvi.Tag == null)
                //{
                //    MessageBox.Show("해당 MapView를 찾을 수 없습니다.");
                //    return;
                //}

                int nPort = (int)textBox_CurMap.Tag;
                if(nPort < 10370 || nPort >= 10400)
                {
                    MessageBox.Show("해당 MapView를 찾을 수 없습니다.");
                    return;
                }
                
                string str = String.Format("{0}/{1}/{2}", Cmd.LOG_REQ_FIND, comboBox_Elem.SelectedIndex, textBox_ID.Text);
                SendData_toMapView(str, nPort);  // 맵뷰 에서 해당 노드or스테이션 찾아달라고 요청
            }
            catch (Exception exp)
            {
                MessageBox.Show("맵 열기 실패\n\n" + exp.Message);
                throw;
            }
        }

        // 모든 맵뷰 창 숨기기
        private void button_HideAll_Click(object sender, EventArgs e)
        {
            try
            {
                foreach(ListViewItem lvi in listView_MapList.Items)
                {
                    if (lvi.Tag == null)
                        continue;

                    int nPortNo = (int)lvi.Tag;
                    if (nPortNo < 10370 || nPortNo >= 10400)
                        continue;

                    string str = String.Format("{0}/0", Cmd.LOG_REQ_SHOW);
                    SendData_toMapView(str, nPortNo);   // 맵뷰 창 Hide 하라고 요청
                }
            }
            catch (Exception exp)
            {
                MessageBox.Show("통신 실패\n\n" + exp.Message);
            }
        }

        private void UMapViewMan_FormClosing(object sender, FormClosingEventArgs e)
        {
            Hide();
            e.Cancel = true;            
        }

        private void button_Close_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void listView_MapList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listView_MapList.SelectedItems.Count < 1)
            {
                textBox_CurMap.Text = "";
                textBox_CurMap.Tag = null;
            }
            try
            {
                ListViewItem lvi = listView_MapList.SelectedItems[0];

                if (lvi.Tag == null)
                {
                    textBox_CurMap.Text = "";
                    textBox_CurMap.Tag = null;
                }
                int nPort = (int)lvi.Tag;
                textBox_CurMap.Text = String.Format("[{0}] {1}  {2}  Ver: {3}", lvi.SubItems[0].Text, lvi.SubItems[1].Text, lvi.SubItems[2].Text, lvi.SubItems[3].Text); ;
                textBox_CurMap.Tag = nPort;
            }
            catch { }
                
        }

        private void textBox_ID_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                button_FindNode_Click(this, e);
        }
    }

    
}
