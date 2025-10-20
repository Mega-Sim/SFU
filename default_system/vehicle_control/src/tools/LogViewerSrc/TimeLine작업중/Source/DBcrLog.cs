using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.IO;
using MyTools;
using System.Windows.Forms;
using System.Diagnostics;

namespace LogViewer
{

    // ex : [21:00:50.203]/684005/1/ 16.0,      0
        public enum eBcrLogIdx
    {
        Time_Date = 0
       , Time
       , Bcr_Node
       , Bcr_NCnt
       , Offset
       , Bcr_Station

       , COUNTOF
    }
    public class DBcrLogRowItem
    {
        public dynamic[] subItems = new dynamic[(int)eBcrLogIdx.COUNTOF];      // dynamic 데이터 타입으로 double, bool, int 모두 런타임에 결정한다
        public DBcrLogRowItem() { }
    }

    public class DBcrLog
    {
        public string filePath = "";
        public static char szSpliter = '/';
        public bool bLoaded = false;
        public DataTable table = null;
        static public DViewColor[] CellColor = new DViewColor[(int)eBcrLogIdx.COUNTOF];

        public DBcrLog()
        {
            table = CreateTable();
             for(int i=0; i< (int)eBcrLogIdx.COUNTOF; i++)
            {
                CellColor[i] = new DViewColor();
            }
        }

        public void Clear()
        {
            filePath = "";
            if (table != null)
                table.Rows.Clear();
            bLoaded = false;
        }


        public bool LoadFile(string path)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf("BCR2") < 0)
                return false;
            
            // 한개 파일의 모든 라인 한번에 읽기
            string[] allLines = File.ReadAllLines(path, Encoding.Default/*Encoding.UTF8*/);   // UTF8로 읽어야하나..?

            if (allLines.Length == 0)
                return false;

            this.filePath = path;

            int nLineNo = 1;
            // 한줄 한줄 파싱하기
            for (int i = 0; i < allLines.Length; i++, nLineNo++)// (string line in allLines)
            {
                DBcrLogRowItem row = new DBcrLogRowItem();

                if (!ParseLine(path, allLines[i], row))
                    continue;

                table.Rows.Add(row.subItems);
            }
            bLoaded = true;
            return true;
        }
        public static bool ParseLine(string filePath, string line, DBcrLogRowItem row)
        {
            // string line 으로 들어오는 값
            // ex : [21:00:50.203]/684005/1/ 16.0,      0
            string[] arrSplit = line.Split(szSpliter);

            if (arrSplit.Length != 4)
                return false;
            string[] arrTmp = arrSplit[3].Split(',');

            if (arrTmp.Length != 2)
                return false;

            line = line.Replace(" ", "");       // 공백삭제
            line = line.Replace("\t", "");

            int nNode = -9999;
            int nNodeCnt = -9999;
            double dOffset = -9999;
            int nStationBCR = -9999;

            SmallDateTime tm = new SmallDateTime();
            if (GMethod.GetTimeFromLine(filePath, line, ref tm) == false)
                return false;
            
            Int32.TryParse(arrSplit[1], out nNode);
            Int32.TryParse(arrSplit[2], out nNodeCnt);
            Double.TryParse(arrTmp[0], out dOffset);
            Int32.TryParse(arrTmp[1], out nStationBCR);

            row.subItems[(int)eBcrLogIdx.Time_Date] = tm.GetDateTime();
            row.subItems[(int)eBcrLogIdx.Time] = tm.GetTimeString();
            row.subItems[(int)eBcrLogIdx.Bcr_Node] = nNode;
            row.subItems[(int)eBcrLogIdx.Bcr_NCnt] = nNodeCnt;
            row.subItems[(int)eBcrLogIdx.Offset] = dOffset;
            row.subItems[(int)eBcrLogIdx.Bcr_Station] = nStationBCR;

            return true;
        }

        public static DataTable CreateTable()
        {
            DataTable theTable = new DataTable();

            string strColName = GetHeaderName(eBcrLogIdx.Time_Date, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.DateTime"));

            strColName = GetHeaderName(eBcrLogIdx.Time, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.String"));

            strColName = GetHeaderName(eBcrLogIdx.Bcr_Node, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.Int32"));

            strColName = GetHeaderName(eBcrLogIdx.Bcr_NCnt, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.Int32"));

            strColName = GetHeaderName(eBcrLogIdx.Offset, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.Double"));

            strColName = GetHeaderName(eBcrLogIdx.Bcr_Station, asMultiLine: false);
            theTable.Columns.Add(strColName, Type.GetType("System.Int32"));

            return theTable;
        }
        public static string GetHeaderName(eBcrLogIdx idx, bool asMultiLine)
        {
            string strEnumName = idx.ToString();
            string[] arr = strEnumName.Split('_');
            char szSpliter = '.';

            if (asMultiLine)
                szSpliter = '\n';

            string ret = "";
            // 3개 이하일때는 그냥 replace하면 됨
            if (arr.Length <= 3)
            {
                ret = strEnumName.Replace('_', szSpliter);
            }
            else
            {
                // 3개가 넘어가면 앞에서부터 3개까지만 replace하고 그뒤는 그냥 둬야함
                int nPos = 0;
                for (int nCnt = 0; nCnt < 2; nCnt++)
                    nPos = strEnumName.IndexOf('_', nPos + 1);

                string strLeft = strEnumName.Substring(0, nPos + 1);
                string strRight = strEnumName.Substring(nPos + 1, (strEnumName.Length) - (nPos + 1));
                strLeft = strLeft.Replace('_', szSpliter);

                ret = strLeft + strRight;
            }
            return ret;
        }
        /// <summary>
        ///  특정 시간에서 전, 후 몇초간의 데이터를 table로 리턴한다
        /// </summary>
        public DataTable GetTable_asTime(DateTime tm, int nBeforeSec, int nAfterSec)
        {
            DateTime tmFrom = tm.AddSeconds(nBeforeSec * (-1));
            DateTime tmTo = tm.AddSeconds(nAfterSec);

            List<DataRow> rows = new List<DataRow>();
            DataTable newTable = CreateTable();

            foreach (DataRow row in table.Rows)
            {
                DateTime t = (DateTime)row[(int)eBcrLogIdx.Time_Date];
                if (t > tmFrom && t <= tmTo)    // from ~ to 시간대 이내의 데이터들을 newTable로 Clone 시킨다
                {
                    var desRow = newTable.NewRow();
                    var sourceRow = row;
                    desRow.ItemArray = sourceRow.ItemArray.Clone() as object[];
                    newTable.Rows.Add(desRow);

                }
            }
            return newTable;
        }
    }

}