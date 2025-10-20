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
using MyTools;
using System.Runtime.CompilerServices;
using System.Diagnostics;

namespace LogViewer
{
    public partial class UNotebook : Form 
    {
        public DDataSet m_parent = null;
        private string m_asUpperCase = "";     //rich textbox에 들어가는 텍스트 대문자로 저장해서 가지고 있음. 검색할때 유리
        public string m_filePath = "";
        private bool m_bFocusFind = false;
        public List<LineTime> listLineTime = new List<LineTime>();
        public string[] m_lines;
        int m_nScrollFoundLine = -1;
        double m_dFoundSpantime = 0;
        string m_strLineFoundMsg = "";
        int m_nFoundLineLen = 0;

        public UNotebook()
        {
            InitializeComponent();
            this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseWheel);
            this.label_path.Text = "";
            this.richTextBox1.Text = "";
        }

        private void panel1_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            bool bZoomIn = e.Delta > 0;
            
            try
            {
                if (bZoomIn)
                    richTextBox1.ZoomFactor *= (float)1.1;
                else
                    richTextBox1.ZoomFactor *= (float)0.9;
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
                return;
            }
        }

        public void FileOpen(string strPath)
        {
            try
            {
                richTextBox1.Clear();
                listLineTime.Clear();

                if (File.Exists(strPath) == false)
                {
                    MessageBox.Show("파일이 없습니다.\n\n" + strPath);
                    return;
                }
                richTextBox1.Text = File.ReadAllText(strPath, Encoding.Default);
                m_lines = File.ReadAllLines(strPath, Encoding.Default);
                m_filePath = strPath;

                m_asUpperCase = richTextBox1.Text.ToUpper();
                this.Text = Path.GetFileNameWithoutExtension(strPath);
                this.label_path.Text = strPath;
                this.toolStrip1.Visible = true;
            }
            catch(Exception e)
            {
                MessageBox.Show(e.Message);
                return;
            }
        }
        
        // 문자열 다음찾기. 한번찾고 다시 찾으면 그 다음위치로 검색
        public void FindWord_inLine(string word, bool bMatchCase = true, bool bWholeWord = false)       
        {
            if (String.IsNullOrEmpty(word) || String.IsNullOrEmpty(richTextBox1.Text))
            {
                MessageBox.Show("Invalid");
                return;
            }
            RichTextBoxFinds opt = bWholeWord ? RichTextBoxFinds.WholeWord : (bMatchCase ? RichTextBoxFinds.MatchCase : RichTextBoxFinds.None);

            if (bMatchCase == false)
                word = word.ToUpper();

            //if (strLastFindWord != word)        // 새로운 단어를 검색하면 이전에 찾은 위치 초기화
            //    nLastFindPos = 0;
            
            int nLen = word.Length;
            int nPos = richTextBox1.SelectionStart + richTextBox1.SelectionLength;
            int nFindPos = richTextBox1.Find(word, nPos, richTextBox1.Text.Length, opt);
            bool bFound = nFindPos >= 0;

            // 찾기 실패했을 경우, 앞쪽에 찾는 단어가 잇다면, 처음부터 검색할 기회를 준다
            if (bFound == false)
            {
                bool bExists = false;

                if (bMatchCase)
                    bExists = richTextBox1.Text.IndexOf(word, 0) >= 0; 
                else
                    bExists = m_asUpperCase.IndexOf(word, 0) >= 0;

                if (bExists)
                {
                    if(MessageBox.Show("끝까지 찾았습니다.\n처음부터 다시 검색할까요?", "Find", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        nFindPos = richTextBox1.Find(word, 0, richTextBox1.Text.Length, opt);
                        bFound = nFindPos >= 0;
                    }
                }
            }

            if(bFound == false)
            {
                MessageBox.Show("찾는 문자열이 없습니다.");
                return;
            }
            richTextBox1.Focus();
        }

        private void CheckItTheTime(int nTheIdx, int nIdx_p1, int nIdx_s1)
        {

        }
        // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
        // 파일이름으로 날짜 파싱까지는 하지 않고 그냥 today 날짜로 만듬. 시간만 의미가 있다
        public bool GetTimeFromLine_asToday(string __line, ref DateTime _out_theTime)
        {
            if (String.IsNullOrEmpty(__line)) return false;
            if (__line.Length < 8) return false;

            // [22:59:57.984]/1111/105/-..............   <<<==== 이런 포맷
            int __nHour = 0, __nMin = 0, _nSec = 0, __nMSec = 0;
            int __nStart = __line.IndexOf("[") + 1;
            int __nEnd = __line.IndexOf("]");
            if (__nStart < 0 || __nEnd < 0 || __nEnd - __nStart < 8 || __nEnd > 15) return false;
            string __strTime = __line.Substring(__nStart, __nEnd - __nStart);

            string[] __arr = __strTime.Split(':');
            if (__arr.Length == 3)
            {
                if (__arr[2].Length < 3) return false;
                string[] __arr_sec = __arr[2].Split('.');
                if (__arr_sec.Length != 2) return false;

                if (Int32.TryParse(__arr[0], out __nHour) == false || Int32.TryParse(__arr[1], out __nMin) == false) return false;
                if (MyRange.IsInRange(0, 25, __nHour) == false || MyRange.IsInRange(0, 60, __nMin) == false) return false;
                if (Int32.TryParse(__arr_sec[0], out _nSec) == false) return false;
                if (Int32.TryParse(__arr_sec[1], out __nMSec) == false) return false;
                if (MyRange.IsInRange(0, 60, _nSec) == false || MyRange.IsInRange(0, 1000, __nMSec) == false) return false;
            }
            else if (__arr.Length == 4)
            {
                if (Int32.TryParse(__arr[0], out __nHour) == false || Int32.TryParse(__arr[1], out __nMin) == false) return false;
                if (Int32.TryParse(__arr[2], out _nSec) == false || Int32.TryParse(__arr[3], out __nMSec) == false) return false;
                if (MyRange.IsInRange(0, 25, __nHour) == false || MyRange.IsInRange(0, 60, __nMin) == false) return false;
                if (MyRange.IsInRange(0, 60, _nSec) == false || MyRange.IsInRange(0, 1000, __nMSec) == false) return false;
            }
            else return false;
            
            _out_theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, __nHour, __nMin, _nSec, __nMSec);
            return true;
    

        }
        // 원하는 시간이 있는 라인으로 이동한다
        // 리턴값은, theTime과 이동하는 라인과의 차이값이다 second
        public double GotoTime(DateTime theTime)
        {
            int nLineCnt = m_lines.Length;
            if (nLineCnt < 3)
                return 0;
         
            
            int nTotalTick = Environment.TickCount;
            
            // 날짜만 today로 바꿈
            theTime = new DateTime(DateTime.Today.Year, DateTime.Today.Month, DateTime.Today.Day, theTime.Hour, theTime.Minute, theTime.Second, theTime.Millisecond);

            DateTime tmLine = new DateTime();
            DateTime tmpTime = new DateTime();

            int nFoundLine = -1;
            do
            {
                DateTime tmFirstLine = tmpTime;
                // 시작줄 확인                
                if (GetTimeFromLine_asToday(m_lines[0], ref tmFirstLine) == false)
                {
                    if (GetTimeFromLine_asToday(m_lines[1], ref tmFirstLine) == false)
                      break;
                }
                if (theTime < tmFirstLine)   // 첫줄은 예외로 검사해봄
                {
                    nFoundLine = 0;
                    break;
                }

                // 마지막줄 확인
                DateTime tmLastLine = tmpTime;
                int nTmp = nLineCnt - 1;
                
                // 첫줄라인, 마지막줄 라인은 시작하자마자 먼저 체크해본다
                if (GetTimeFromLine_asToday(m_lines[nTmp], ref tmLastLine) == false)
                {
                    nTmp--;

                    if (GetTimeFromLine_asToday(m_lines[nTmp], ref tmLastLine) == false)
                        break;
                }
                if (theTime > tmLastLine)   // 마지막줄 예외로 검사해봄
                {
                    nFoundLine = nTmp;
                    break;
                }

            } while (false);

            // ##############################################
            // 분할정복 알고리즘 사용 Divide And Conqueror.
            // 분할정복을 사용하면 10만줄의 라인이 있어도, 원하는 시간을 최악의 경우에도 20번 이내의 검색으로 찾을 수 있다
            //
            int nPrevDivIdx = nLineCnt;
            int nDivIdx = nLineCnt / 2;
            int nCnt = 0;
            
            while(nFoundLine < 0)
            {
                if (GetTimeFromLine_asToday(m_lines[nDivIdx], ref tmLine) == false)
                    break;
                
                // Divied 된 인덱스가 5일경우 6번과 비교해서 찾았는지 확인한다
                if (nDivIdx < nLineCnt - 1)
                {
                    DateTime tm_p1 = new DateTime();
                    
                    if (GetTimeFromLine_asToday(m_lines[nDivIdx + 1], ref tm_p1) == false)
                        break;
                    
                    bool bHubo = tm_p1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_p1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx + 1;
                        break;
                    }
                }

                // Divied 된 인덱스가 5일경우 4번과 비교해서 찾았는지 확인한다
                if (nDivIdx > 0)
                {
                    DateTime tm_s1 = new DateTime();
                    if (GetTimeFromLine_asToday(m_lines[nDivIdx - 1], ref tm_s1) == false)
                        break;
                    
                    bool bHubo = tm_s1 > theTime;
                    bool bHubo2 = tmLine > theTime;
                    if (bHubo != bHubo2)
                    {
                        // 거의 찾았다
                        TimeSpan span_p1 = tm_s1 - theTime;
                        TimeSpan span_cur = tmLine - theTime;

                        nFoundLine = Math.Abs(span_cur.TotalSeconds) < Math.Abs(span_p1.TotalSeconds) ? nDivIdx : nDivIdx - 1;
                        break;
                    }
                }

                // Divided 된 인덱스와 인접하는 값들이 아니라면 다시 Divide한다
                int nGap = Math.Abs(nDivIdx - nPrevDivIdx);
                if (nGap <= 1)
                    break;

                // 분할정복 알고리즘. 반으로 나눈다
                int nDiv = nGap / 2;
                nPrevDivIdx = nDivIdx;
                nDivIdx = theTime > tmLine ? (nDivIdx + nDiv) : (nDivIdx - nDiv);
                nCnt++;
            }

			m_strLineFoundMsg = "";

            if (nFoundLine < 0)
            {
                Trace.WriteLine("Fail to find line");
                return Double.MaxValue;
            }
            DateTime tmFound = new DateTime();
            if (GetTimeFromLine_asToday(m_lines[nFoundLine], ref tmFound) == false)
            {
                Trace.WriteLine("Fail to find line_2");
                return Double.MaxValue;
            }


            TimeSpan span = theTime - tmFound;
            m_nFoundLineLen = m_lines[nFoundLine].Length;
            m_strLineFoundMsg = String.Format("찾는 시간:  {0:00}:{1:00}:{2:00}.{3:#0.##0}\n찾은 시간:  {4:00}:{5:00}:{6:00}.{7:#0.##0}\n\n {8}초의 오차가 있습니다.", 
               theTime.Hour, theTime.Minute, theTime.Second, theTime.Millisecond, 
               tmFound.Hour, tmFound.Minute, tmFound.Second, tmFound.Millisecond, span.TotalSeconds );

            Trace.WriteLine(String.Format("Find in at {0} times in {1}. Elapse: {2}", nCnt , nLineCnt, Environment.TickCount - nTotalTick));

            m_dFoundSpantime = span.TotalSeconds;
            m_nScrollFoundLine = nFoundLine;
            

            return 0;
        }
        // 찾기 버튼
        private void toolStripBtnFind_Click(object sender, EventArgs e)
        {
            // 검색할때 대소구분, 전체단어 일치할때만 옵션이 있다
            FindWord_inLine(textBox_Find.Text, btn_FindMatchCase.Checked, btn_findWholeWord.Checked);
        }

        // 해당 경로 열기
        private void btn_Path_Click(object sender, EventArgs e)
        {
            string path = label_path.Text;

            if (File.Exists(path) == false)
            {
                MessageBox.Show("파일이 없습니다.\n\n" + path);
                return;
            }
            try
            {
                string arg = "/select, \"" + path;
                System.Diagnostics.Process.Start("explorer.exe", arg);
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
                return;
            }
        }
        
        public void SetCurrentLine(int nLineNo)
        {
            m_nScrollFoundLine = nLineNo;
        }

        public int GetCurrentLineNo()
        {
            int nCurLine = richTextBox1.GetLineFromCharIndex(richTextBox1.SelectionStart);
            return nCurLine;
        }

        // 현재 rich text 에서 선택된 부분을 리턴함
        public void GetCurrentSelection(out int nStart, out int nLength)
        {
            nStart = richTextBox1.SelectionStart;
            nLength = richTextBox1.SelectionLength;
        }

        // 해당위치를 선택한다. 노란색 음영 표시
        public void SetSelection(int nStart, int nLength)
        {
            if (richTextBox1.Text.Length < 1)
                return;
            try
            {

                richTextBox1.SelectAll();
                richTextBox1.SelectionBackColor = richTextBox1.BackColor;

                // 선택 범위가 0이면 해당 라인을 선택해준다
                if (nLength == 0)
                {
                    int nLine = richTextBox1.GetLineFromCharIndex(nStart);
                    nLength = richTextBox1.Lines[nLine].Length;
                    nStart = richTextBox1.GetFirstCharIndexFromLine(nLine);
                }

                richTextBox1.Select(nStart, nLength);
                richTextBox1.SelectionBackColor = Color.Yellow;
               
                VisibleScrollToCaret();
            }
            catch (Exception e)
            {
                MessageBox.Show("Exception\n\n" + e.Message);
            }
        }

        // 현재 선택한 곳으로 박스 제일 위로 스크롤을 맞춘다.
        // 다만 선택한 위치가 현재 보이는 위치에 있다면 스크롤을 맞추지 않는다. 더 혼란스럽기 때문
        // 보이지 않는 위치면 화면에 보이는 최상단에 스크롤을 맞춤
        void VisibleScrollToCaret()
        {
            // 현재 화면에 몇번째부터 몇번째 부터 보여지는 범위 인지 char index를 알아냄
            int nStart = richTextBox1.GetCharIndexFromPosition(new Point(0, 0));
            int nEnd = richTextBox1.GetCharIndexFromPosition(new Point(richTextBox1.ClientRectangle.Width, richTextBox1.ClientRectangle.Height));

            // 영역 내면 skip
            if (richTextBox1.SelectionStart > nStart && richTextBox1.SelectionStart < nEnd)
                return;
            
            // 영역 밖이면 scroll
            richTextBox1.ScrollToCaret();

        }
        // 타이머에서 하단 StatusBar 갱신한다
        int nPrevPos = -1;
        int nPrevPos2 = -1;
        private void timer1_Tick(object sender, EventArgs e)
        {
            this.toolStrip1.Visible = true;
            // 커서 변경 사항이 있을 때을 때만
            if(nPrevPos == richTextBox1.SelectionStart && nPrevPos2 == richTextBox1.SelectionLength)
                return;

            nPrevPos = richTextBox1.SelectionStart;
            nPrevPos2 = richTextBox1.SelectionLength;

            // 현재 몇번 라인인지 표시
            int nCurLine = richTextBox1.GetLineFromCharIndex(richTextBox1.SelectionStart);
            label_CurLineNo.Text = String.Format("Cur Line No.: {0}", nCurLine+1);

            // 전체 라인 표시
            int nTotalLineCnt = richTextBox1.GetLineFromCharIndex(richTextBox1.Text.Length);
            label_totalLineCnt.Text = String.Format("Total Line: {0}", nTotalLineCnt+1);

            // 선택된 라인이 몇개인지 표시
            int nEndIdx = richTextBox1.SelectionStart + richTextBox1.SelectionLength;
            int nSelectionEndLine = richTextBox1.GetLineFromCharIndex(nEndIdx);
            int nSelectedLineCnt = nSelectionEndLine - nCurLine;
            label_selectedLineCnt.Text = String.Format("Selected Line Cnt: {0}", nSelectedLineCnt+1);

            int nCurLineStartIdx = richTextBox1.GetFirstCharIndexFromLine(nCurLine);
            int nCurLineEndIdx = richTextBox1.GetFirstCharIndexFromLine(nSelectionEndLine);

            
            // 선택한 시간이 몇초인지 계산해서 하단 Label에 보여주는 부분
            TimeSpan span = new TimeSpan();
            
            bool bFailToCalTime = false;
            try
            {
                string strTimeInfo = richTextBox1.Text.Substring(nCurLineStartIdx, 14); // 시간이 14글자 정도 됨
                SmallDateTime startTime = new SmallDateTime();
                GMethod.GetTimeFromLine(m_filePath, strTimeInfo, ref startTime);

                strTimeInfo = richTextBox1.Text.Substring(nCurLineEndIdx, 14); // 시간이 14글자 정도 됨
                SmallDateTime endTime = new SmallDateTime();
                GMethod.GetTimeFromLine(m_filePath, strTimeInfo, ref endTime);
                span = endTime.GetDateTime() - startTime.GetDateTime();
                if (span.TotalSeconds < 0)
                    throw new Exception();

                DateTime tmCurrent = endTime.GetDateTime();
                if(m_parent != null)
                {
                    m_parent.Frame.SetCurrentTime(tmCurrent, m_filePath);
                }
            }
            catch
            {
                bFailToCalTime = true;
                label_timeSpan.Text = "";
            }

            if (bFailToCalTime == false)
            {
                label_timeSpan.Text = String.Format("Selected Time Span: {0:0.##0} sec", span.TotalSeconds);
            }
            label_modeStat.Text = "";
            label_Speed.Text = "";
        }

        private void richTextBox1_KeyDown(object sender, KeyEventArgs e)
        {
            bool bControl = (Control.ModifierKeys & Keys.Control) != 0;
            bool bSpace = (Control.ModifierKeys & Keys.Space) != 0;

            // 찾기
            if (e.KeyCode == Keys.F && bControl)    //찾기 기능
            {
                textBox_Find.Focus();
            }
            else if(e.KeyCode == Keys.C && bControl)
            {
                richTextBox1.Copy();
            }
            else if (e.KeyCode == Keys.Enter && m_bFocusFind)
                toolStripBtnFind_Click(this, e);
        }

        private void copyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBox1.Copy();
        }

        // 검색 문자열 입력 후 엔터 쳤을 때 바로 검색 버튼 누른것처럼 되게
        private void textBox_Find_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                m_bFocusFind = true;
                toolStripBtnFind_Click(this, e);
            }
        }

        private void richTextBox1_MouseDown(object sender, MouseEventArgs e)
        {
            m_bFocusFind = false;
            if (e.Button == MouseButtons.Right)
            {
                Point mousePos = MyTools.GMethod.GetMousePos_onControl(richTextBox1);
                contextMenuStrip1.Show(richTextBox1, mousePos);
            }
        }

        //// 자동 줄바꿈 옵션
        //private void btn_lineFeed_Click(object sender, EventArgs e)
        //{
        //    richTextBox1.WordWrap = btn_lineFeed.Checked;
        //}

        private void UNotebook_FormClosing(object sender, FormClosingEventArgs e)
        {
            DestroySelf();
        }
        public void DestroySelf()
        {
            this.m_parent = null;
            this.richTextBox1.Dispose();
            this.listLineTime.Clear();
            this.Dispose();
        }
        
        bool bAtOnce = true;

        // 스크롤 타이머
        private void timer_Scroll_Tick(object sender, EventArgs e)
        {
            if (bAtOnce == false)
                return;
            bAtOnce = false;

            if (m_nScrollFoundLine >= 0 )  
            {
                // 찾은 라인으로 스크롤해줌
                int nPos = richTextBox1.GetFirstCharIndexFromLine(m_nScrollFoundLine);
                int nLen = richTextBox1.Lines[m_nScrollFoundLine].Length;

                richTextBox1.SelectAll();
                richTextBox1.SelectionBackColor = richTextBox1.BackColor;

                richTextBox1.Select(nPos, Math.Max(nLen-1, 0));
                richTextBox1.SelectionBackColor = Color.Yellow;

                VisibleScrollToCaret();

                if (String.IsNullOrEmpty(m_strLineFoundMsg) == false)
                    MessageBox.Show(m_strLineFoundMsg);
            }

            m_strLineFoundMsg = "";
            m_nScrollFoundLine = -1;
            bAtOnce = true;
        }
    }
}
