using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MapView
{
    static class Program
    {
        public static UMapView dlgMapView = null;
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            dlgMapView = new UMapView();
            Application.Run(dlgMapView);
            dlgMapView.BringToFront();
        }
        
    }
}
