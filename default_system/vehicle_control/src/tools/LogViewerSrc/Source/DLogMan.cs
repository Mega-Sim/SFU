using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace LogViewer
{


    // 전체 DDataSet의 로그 객체들을 관리하는 클래스. 로그 관리의 가장 최상위 static 클래스다
    // 현재 까지 Open 한 로그 폴더들을 가진 Container
    static public class DLogMan
    {
        // dataset 리스트
        // dataset은 한개의 로그폴더를 담는 Container
        static public List<DDataSet> DataSetList = new List<DDataSet>();

        // DataSet을 하나 생성 후 리스트에 추가한다
        static public DDataSet CreateDataSet(string folderPath)
        {
            // 이미 로딩된 로그 폴더일 경우 계속할 수 없음
            // if (Find(folderPath) != null)
            //     return null;

            DDataSet ds = new DDataSet();
            ds.folderPath = folderPath;
            DataSetList.Add(ds);

            return ds;
        }
        static public void DestroyLogFrame(int nIdx)
        {
            DDataSet ds = DataSetList[nIdx];
            ds.DestroyFrame();
            ds.ClearAll();

            DataSetList.RemoveAt(nIdx);
        }
        static public void DestroyLogFrame(string path)
        {
            for (int i = 0; i < DataSetList.Count; i++)
            {
                if (DataSetList[i].folderPath == path)
                {
                    DataSetList[i].DestroyFrame();
                    DataSetList[i].ClearAll();
                    DataSetList.RemoveAt(i);
                    break;
                }
            }
        }
        // 특정 폴더가 열렸는지 판단한다
        // 열리지 않았으면 return null
        static public DDataSet Find(string folderPath)
        {
            foreach (DDataSet data in DataSetList)
            {
                if (data.folderPath.ToUpper() == folderPath.ToUpper())
                {
                    return data;
                }
            }
            return null;
        }

        // 현재 보여지는 TabControl의 dataset 리턴
        static public DDataSet GetCurrentVisible()
        {
            foreach (DDataSet data in DataSetList)
            {
                if (data.IsVisibleTabControl())
                    return data;
            }
            return null;
        }
        static public int GetCurrentVisibleIdx()
        {
            for (int i = 0; i < DataSetList.Count; i++)
            {
                DDataSet data = DataSetList[i];
                if (data.IsVisibleTabControl())
                    return i;
            }
            return -1;
        }

 
        //// 메모리에서 특정 로그 폴더를 삭제한다
        //// 해당 로그관련해서 열려있는 창들도 다 close 된다
        //static public bool UnloadLog(string folderPath, bool bCloseForms = false)
        //{
        //    for (int i=0; i<DataSetList.Count(); i++)
        //    {
        //        DDataSet set = DataSetList.ElementAt(i);
        //        if (set.folderPath.ToUpper() == folderPath.ToUpper())
        //        {
        //            // 창닫기 옵션
        //            if(bCloseForms)
        //            {
        //                foreach(Form form in set.LogFormList)
        //                    form.Close();
        //                set.LogFormList.RemoveAll(o => o != null);
        //            }
        //            DataSetList.RemoveAt(i);
        //            return true;
        //        }
        //    }

        //    return false;
        //}

        //// 메모리에서 특정 로그 폴더를 삭제한다
        //// 해당 로그관련해서 열려있는 창들도 다 close 된다
        //static public bool UnloadLog(DDataSet data, bool bCloseForms = false)
        //{
        //    for (int i = 0; i < DataSetList.Count(); i++)
        //    {
        //        DDataSet set = DataSetList.ElementAt(i);

        //        if (set == data)
        //        {
        //            // 창닫기 옵션
        //            if (bCloseForms)
        //            {
        //                foreach (Form form in set.LogFormList)
        //                    form.Close();
        //                set.LogFormList.RemoveAll(o => o != null);
        //            }

        //            DataSetList.RemoveAt(i);
        //            return true;
        //        }
        //    }

        //    return false;
        //}

    }

}
