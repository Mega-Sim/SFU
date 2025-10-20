using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Diagnostics;
namespace LogViewer
{
    public enum eBuho
    {
        Big, BigSame, Same, Small, SmallSame, IsNot, Contain       //contain 은 문자열 값의 경우에 해당 문자열을 포함하고 있으면
                                                                   // contain의 기호는 @ 골뱅이
    }

    public class DCondition
    {
        public eBuho buho;
        public dynamic val;

        public bool Check(dynamic theVal)
        {
            bool bRet = false;
            try
            {
                string strVal = theVal.ToString();
                //int intVal = 0;
                double dblVal = 0;
                Type theType;

                double dTmpVal = 0;
                //string strTmpVal = "";

                // theVal의 타입 케스팅 하는 부분 **********
                if (Double.TryParse(strVal, out dTmpVal))
                {
                    theVal = dTmpVal;
                    if (val.GetType() == Type.GetType("System.Double"))
                        dblVal = val;
                    else
                        dblVal = Convert.ToDouble(val);

                    theType = theVal.GetType();
                }
                else
                {
                    theVal = strVal;
                    if (val.GetType() == Type.GetType("System.String"))
                        strVal = val;
                    else
                        strVal = Convert.ToString(val);

                    theType = theVal.GetType();
                }

                // 비교할 값은 val 이다.


                //Type theType = theVal.GetType();
                //bool boolVal = false;
                //int intVal = 0;
                //double dblVal = 0;
                //string strHoge = theVal;
                //if()
                //else if()
                //else if (Double.TryParse(strHoge, out dblVal))
                //{
                //    theVal = dblVal;
                //    theType = theVal.GetType();
                //}
                //else
                //{

                //}
                //string strVal = "";
                //if (theType.GetType() != strHoge.GetType())
                //{
                //    if (theType == Type.GetType("System.Int32") || theType == Type.GetType("System.Double") || theType == Type.GetType("System.Boolean"))
                //    {
                //        boolVal = val == 0 ? false : true;
                //        intVal = (int)val;
                //        dblVal = (double)val;
                //    }
                //    if(theType == Type.GetType("System.String"))
                //        strVal = String.Format("{0}", val);
                //}


                int n = (int)buho;
                switch (n)
                {

                    case (int)eBuho.Big:

                        //if (theType == Type.GetType("System.Int32")) bRet = theVal < intVal;
                        if (theType == Type.GetType("System.Double")) bRet = theVal < dblVal;
                        //else if (theType == Type.GetType("System.String")) bRet = false;
                        //else bRet = theVal < intVal;

                        break;
                    case (int)eBuho.BigSame:

                        //if (theType == Type.GetType("System.Int32")) bRet = theVal == boolVal;
                        //else 
                        if (theType == Type.GetType("System.Double"))
                            bRet = theVal <= dblVal;
                        else if (theType == Type.GetType("System.String"))
                            bRet = theVal == strVal;
                        //else bRet = theVal <= intVal;

                        break;
                    case (int)eBuho.Same:

                        //if (theType == Type.GetType("System.Boolean")) bRet = theVal == boolVal;
                        //else 
                        if (theType == Type.GetType("System.Double"))
                            bRet = theVal == dblVal;
                        else if (theType == Type.GetType("System.String"))
                            bRet = theVal == strVal;
                        //else bRet = theVal == intVal;

                        break;
                    case (int)eBuho.Small:

                        //if (theType == Type.GetType("System.Boolean")) bRet = false;
                        //else 
                        if (theType == Type.GetType("System.Double"))
                            bRet = theVal > dblVal;
                        //else if (theType == Type.GetType("System.String")) bRet = false;
                        //else bRet = theVal > intVal;

                        break;
                    case (int)eBuho.SmallSame:

                        //if (theType == Type.GetType("System.Boolean")) bRet = theVal == boolVal;
                        //else 
                        if (theType == Type.GetType("System.Double"))
                            bRet = theVal >= dblVal;
                        else if (theType == Type.GetType("System.String"))
                            bRet = theVal == strVal;
                        //else bRet = theVal >= intVal;

                        break;
                    case (int)eBuho.IsNot:

                        //if (theType == Type.GetType("System.Boolean")) bRet = theVal != boolVal;
                        //else 
                        if (theType == Type.GetType("System.Double"))
                            bRet = theVal != dblVal;
                        else if (theType == Type.GetType("System.String"))
                            bRet = theVal != strVal;
                        //else bRet = theVal != intVal;

                        break;
                    case (int)eBuho.Contain:    // 해당 문자열을 포함하면
                        {
                            string strTheVal = String.Format("{0}", theVal);
                            bRet = strVal.Contains(strTheVal);
                            break;
                        }
                }
            }
            catch (Exception expt)
            {
                Debug.Assert(false, expt.Message);
                throw;
            }
            return bRet;
        }
    }
    // 그리드 셀의 색표시할 때 조건문으로 여러가지 색으로 보이게 할 수 있음
    // 그에대한 클래스
    public class DViewColor
    {
    

        // 사용자가 설정한 조건의 개수만큼 조건과 색이 들어가 있다
        public List<DCondition> conditionList = new List<DCondition>();
        public Color []colorList = new Color[COLOR_CNT];
        public string strCondition = "";

        public static int COLOR_CNT = 5;
        public DViewColor()
        {
            for (int i = 0; i < colorList.Length; i++)
            {
                colorList[i] = new Color();
                colorList[i] = Color.White;
            }
        }
        public void ClearCondition()
        {
            conditionList.Clear();
            strCondition = "";
            for (int i = 0; i < colorList.Length; i++)
                colorList[i] = Color.White;
        }
        public void Copy(DViewColor obj)
        {
            ClearCondition();

            for (int i = 0; i < obj.conditionList.Count; i++)
                this.conditionList.Add(obj.conditionList[i]);

            for (int i = 0; i < obj.colorList.Length; i++)
                this.colorList[i] = obj.colorList[i];
        }
        // UI에서 사용자가 문자열로 조건을 입력하면 그걸 데이터화 시킴
        // 콤마로 구분해서 여러 조건을 입력 할 수 있음
        public bool AddCondition(string cond, out string strErrMsg)
        {
            cond.Replace(" ", "");    // 공백 삭제
            cond.Replace("\t", "");    // 공백 삭제

            // 그냥 공백이면 아무것도 아닌것
            if (String.IsNullOrWhiteSpace(cond))
            {
                strErrMsg = "";
                return true;
            }

            string[] arrCond = cond.Split(',');
            for(int i=0; i<arrCond.Length; i++)
            {
                string strSentence = arrCond[i];
                if (strSentence.Length < 2)
                    continue;

                strSentence = strSentence.Replace(" ", "");    // 공백 삭제
                strSentence = strSentence.Replace("\t", "");    // 공백 삭제

                string strBuho = "";
                string strVal = "";
                if (strSentence.ElementAt(1) == '=' || strSentence.ElementAt(1) == '!' || strSentence.ElementAt(1) == '<' || strSentence.ElementAt(1) == '>')
                {
                    strBuho = strSentence.Substring(0, 2);
                    strVal = strSentence.Substring(2, strSentence.Length - 2);
                }
                else
                {
                    strBuho = strSentence.Substring(0, 1);
                    strVal = strSentence.Substring(1, strSentence.Length - 1);
                }

                if(String.IsNullOrEmpty(strVal))
                {
                    strErrMsg = "부호만 있고 값이 없습니다.";
                    return false;
                }

                if(strVal.Contains('>') || strVal.Contains('<') || strVal.Contains('='))
                {
                    strErrMsg = "값이 있어야 할 쪽에 부호가 있습니다.(<,>,=) [" + strVal + "]";
                    return false;
                }

                DCondition obj = new DCondition();
                

                if (strBuho == "<")
                    obj.buho = eBuho.Big;
                else if (strBuho == "<=" || strBuho == "=<")
                    obj.buho = eBuho.BigSame;
                else if (strBuho == "=" || strBuho == "==")
                    obj.buho = eBuho.Same;
                else if (strBuho == ">")
                    obj.buho = eBuho.Small;
                else if (strBuho == ">=" || strBuho == "=>")
                    obj.buho = eBuho.SmallSame;
                else if (strBuho == "!=" || strBuho == "=!")
                    obj.buho = eBuho.IsNot;
                else if (strBuho == "@")        // contain의 기호는 골뱅이@
                    obj.buho = eBuho.Contain;
                else
                {
                    strErrMsg = "부호를 파싱할 수 없습니다. [" + strBuho + "].";
                    return false;
                }

                int nVal = 0;
                double dVal = 0.0;
                
                if(Int32.TryParse(strVal, out nVal))
                {
                    obj.val = nVal;
                }
                else
                {
                    if(Double.TryParse(strVal, out dVal))
                        obj.val = dVal;
                    else
                        obj.val = strVal;
                }

                conditionList.Add(obj);
            }

            if (conditionList.Count == 0)
            {
                strErrMsg = "파싱 된 수식이 한개도 없습니다.";
                return false;
            }
            strCondition = cond;
            strErrMsg = "";
            return true;
        }

        public Color ContionCheck(dynamic val)
        {
            int i = 0;
            try
            {
                foreach (DCondition cond in conditionList)
                {
                    if (cond.Check(val))
                        return colorList[i];
                    i++;
                }
            }
            catch { }
            return Color.White;
        }

    }
}
