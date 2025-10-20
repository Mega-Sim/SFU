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
    public enum eSimpleLogType
    {
        DrivingCtrl
     , DrivingError
     , EQPIOError
     , ExtraJobThread
     , TaskControl
     , PathSearch
     , Trans
     , BCRRawData
     , CID_MONITORING
     , CID
     , DiagManager
     , COUNTOF
    }

    public enum eSimpleLogColIdx
    {
        Time_Date
       , Time
       , Data
        , COUNTOF
    }

    public class DSimpleLogRowItem
    {
        public dynamic []subItems  = new dynamic[(int)eSimpleLogColIdx.COUNTOF];
    }

    public class DSimpleLog
    {
        public string logName = "";
        public string filePath = "";
        public DataTable table = null;
        public bool bLoaded = false;

        public DSimpleLog()
        {
            table = CreateTable();
        }
        public void Clear()
        {
            logName = "";
            filePath = "";
            if (table != null)
                table.Rows.Clear();
            bLoaded = false;
        }
        public bool LoadFile(string path)
        {
            if (Path.GetFileName(path).ToUpper().IndexOf(logName.ToUpper()) < 0)
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
                DSimpleLogRowItem row = new DSimpleLogRowItem();

                if (!ParseLine(path, allLines[i], row))
                    continue;

                table.Rows.Add(row.subItems);
            }
            bLoaded = true;
            return true;
        }
        public static bool ParseLine(string filePath, string line, DSimpleLogRowItem row)
        {
            // string line 으로 들어오는 값
            // ex : [21:00:50.203] data~~~

            if (line.Length < 15) // 시간표시만 있어도 14글자임 [21:00:50.203]
                return false;

            SmallDateTime tm = new SmallDateTime();
            if (GMethod.GetTimeFromLine(filePath, line, ref tm) == false)
                return false;

            row.subItems[(int)eSimpleLogColIdx.Time_Date] = tm.GetDateTime();
            row.subItems[(int)eSimpleLogColIdx.Time] = tm.GetTimeString();
            row.subItems[(int)eSimpleLogColIdx.Data] = line.Substring(14);
            return true;
        }


        public static string GetHeaderName(eSimpleLogColIdx idx)
        {
            if (idx == eSimpleLogColIdx.Time_Date)
                return "Date";
            else if (idx == eSimpleLogColIdx.Time)
                return "Time";
            else if (idx == eSimpleLogColIdx.Data)
                return "Data";

            return "";
        }

        public static DataTable CreateTable()
        {
            DataTable theTable = new DataTable();

            string strColName = GetHeaderName(eSimpleLogColIdx.Time_Date);
            theTable.Columns.Add(strColName, Type.GetType("System.DateTime"));

            strColName = GetHeaderName(eSimpleLogColIdx.Time);
            theTable.Columns.Add(strColName, Type.GetType("System.String"));

            strColName = GetHeaderName(eSimpleLogColIdx.Data);
            theTable.Columns.Add(strColName, Type.GetType("System.String"));
            
            return theTable;
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
                DateTime t = (DateTime)row[(int)eSimpleLogColIdx.Time_Date];
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