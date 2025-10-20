using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace Diff
{
    public class IniElem
    {
        public string name = "";
        public string val = "";
        public bool bFlag = false;      // 비교 결과 저장
        public bool bSkipItem = false;      // 비교 하지 않는 항목인지
    }
    public class IniSection
    {
        public string name = "";
        public List<IniElem> elemList = new List<IniElem>();
    }

    public class DIni
    {
        public string filePath = "";
        public List<IniSection> sectionList = new List<IniSection>();
        public bool bLoaded = false;
        // 문자열에서 특정 값 추출
        // ex)  txt = "Tolerance_mm=130.01" ,  name= "Tolerance_mm=" 일 경우
        // val 값 리턴 130.01
        // 만약 찾는 값이 없을 경우 return false
        public static bool GetThisValue(string txt, string name, out string val)
        {
            int pos = txt.IndexOf(name);
            val = "";
            string strVal = "";

            for (int i = pos+name.Length+1 ;  i < txt.Length ;  i++)
            {
                bool bEndChar = txt[i] == ' ' || txt[i] == '#' || txt[i] == '/';
                if (bEndChar)
                    break;
                strVal += txt.ElementAt(i);
            }

            // 마지막 값이 .일 경우 제거
            // ex 120.  ==> 120
            if (strVal.Length >= 1)
            {
                if (strVal[strVal.Length - 1] == '.')
                {
                    strVal.Remove(strVal.Length - 1, 1);
                }
             }

            // 개수가 없을 경우 return false
            if (strVal.Length == 0)
                return false;

            val = strVal;
            return true;
        }

        public bool LoadINI(string filePath)
        {
            
            string[] lines = null;
            if (bLoaded)
                return true;
            try
            {
                if (File.Exists(filePath) == false)
                    return false;

                lines = File.ReadAllLines(filePath);
            }
            catch (Exception)
            {
                return false;
            }
            this.filePath = filePath;
            IniSection curSection = null;
            
            foreach (string str in lines)
            {
                string line = str;
                line.Trim();
                line = line.Replace(" ", "");
                line = line.Replace("	", "");
                line = line.Replace("\t", "");

                if (line.Length < 3)
                    continue;

                bool bIsAlphabet = (line[0] >= 'a' && line[0] <= 'z') || (line[0] >= 'A' && line[0] <= 'Z');
                bool bIsDigit = (line[0] >= '1' && line[0] <= '9') || line[0] == '0';
                bool bIsBrace = line[0] == '[';
                bool bIsSpecialChar = !bIsAlphabet && !bIsDigit && !bIsBrace;

                // 특수문자로 시작하면 해석 안함
                if (bIsSpecialChar)
                    continue;

                bool bBraceStarted = line[0] == '[';
                int nBraceEndPos = line.IndexOf(']');
                bool bBraceEnded = nBraceEndPos >= 2;

                // 새 섹션 시작됨 -- 괄호 시작과 끝이 있는 라인 [ ]
                if (bBraceStarted && bBraceEnded)
                {
                    if(curSection != null)
                    {
                        if (curSection.elemList.Count > 0)
                        {
                            sectionList.Add(curSection);
                            curSection = null;
                        }
                    }

                    string sectionName = line.Substring(1, nBraceEndPos - 1);
                    curSection = new IniSection();
                    curSection.name = sectionName;
                    continue;
                }

                int nValPos = line.IndexOf('=');
                if (nValPos >= 1)
                {
                    try
                    {
                        // element 값을 찾았으면 이름-값 을 섹션안에 저장한다
                        string strVal = "";
                        if (GetThisValue(line, line.Substring(0, nValPos), out strVal) == false)
                            continue;

                        IniElem elem = new IniElem();
                        elem.name = line.Substring(0, nValPos);
                        elem.val = strVal;
                        curSection.elemList.Add(elem);
                    }
                    catch(Exception exp)
                    {
                        MessageBox.Show(line + "\n파싱 도중 에러 발생\n\n" + exp);
                        continue;
                    }
                }
            }

            // 끝값 처리
            if (sectionList.Count > 0)
            {
                if (sectionList.Last() != curSection)
                    sectionList.Add(curSection);
            }
            bLoaded = true;
            return true;
        }

        // Element 플래그 초기화
        public void ElemFlagInit()
        {
            foreach(IniSection sec in sectionList)
            {
                foreach(IniElem elem in sec.elemList)
                    elem.bFlag = false;
            }
        }

        public void CompIni(string path)
        {
            DIni iniB = new DIni();
            if (iniB.LoadINI(path) == false)
                return;

            CompIni(iniB);

        }
        public void CompIni(DIni iniB)
        {
            this.ElemFlagInit();    // 비교 결과를 flag에 저장하기 위해서 초기화한다
            iniB.ElemFlagInit();

            foreach (IniSection secA in this.sectionList)
            {
                // 섹션 비교
                int nIdx = iniB.sectionList.FindIndex(o => o.name == secA.name);
                if (nIdx < 0)
                    continue;

                // Element 비교
                IniSection secB = iniB.sectionList[nIdx];
                foreach(IniElem elemA in secA.elemList)
                {
                    nIdx = secB.elemList.FindIndex(o => o.name == elemA.name);
                    if (nIdx < 0)
                    {
                        // Skip 해도 되는 item 이면 강제로 플래그 살리고 넘어간다
                        if (elemA.bSkipItem)
                            elemA.bFlag = true;
                            
                        continue;
                    }

                    IniElem elemB = secB.elemList[nIdx];
                    // Skip 해도 되는 item 이면 강제로 플래그 살리고 넘어간다
                    if (elemA.bSkipItem)
                    {
                        elemA.bFlag = true;
                        elemB.bFlag = true;
                        continue;
                    }

                    if (elemB.val != elemA.val)
                        continue;

                    // 비교가 완료 됐을 때만 flag를 살려준다
                    elemA.bFlag = true;
                    elemB.bFlag = true;
                }
            }
        }
        public IniElem GetElement(string section, string elemName)
        {
            IniElem elem = null;
            try
            {   elem = this.sectionList.Find(o => o.name == section).elemList.Find(j => j.name == elemName);
            }
            catch{}
            return elem;
        }
    }
    class DIniComp
    {
    }
    
}
