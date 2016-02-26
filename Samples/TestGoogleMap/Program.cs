// インターフェース2011年3月号より
using System;
using System.Net;
using System.IO;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Input;
using Microsoft.SPOT.Presentation;
using Microsoft.SPOT.Presentation.Controls;

namespace TestGoogleMap
{
    public class Program : Microsoft.SPOT.Application
    {
        public static void Main()
        {
            Program myApplication = new Program();
            Window mainWindow = myApplication.CreateWindow();

            // 地図画像設定
            string latitude = "35.7100402917006";
            string longitude = "139.81068611349178";
            string zoom = "8";
            string size = "240x220";
            string type = "mobile";
            string format = "gif";
            string key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

            // 地図アクセスURL
            // http://maps.google.com (74.125.235.131)
            string url = "http://74.125.235.64/maps/api/staticmap?key=" + key + "&format=" + format + "&center=" + latitude + "," + longitude + "&zoom=" + 
                "&size=" + size + "&maptype=" + type + "&markers=" + latitude + "," + longitude + ",small";
            // バッファ
            byte[] response = new byte[1024 * 28];
            // 表示ビットマップ
            Bitmap bmp = new Bitmap(mainWindow.Width, mainWindow.Height);
            Bitmap Respbmp = null;
            // 受信データ量
            int read = 0;
            int total = 0;
            HttpWebRequest req = WebRequest.Create(url) as HttpWebRequest;
            req.Method = "GET";
            req.KeepAlive = false;
            req.ReadWriteTimeout = 10000;

            using (WebResponse resp = req.GetResponse())
            {
                using (Stream respStream = resp.GetResponseStream())
                {
                    do
                    {
                        read = respStream.Read(response, total, 0x400);
                        total += read;
                    } while (read != 0);
                }
            }
            // 受信した地図画像
            try
            {
                Respbmp = new Bitmap(response, Bitmap.BitmapImageType.Gif);
                // 画面表示
                bmp.Clear();
                bmp.DrawImage(0, 0, Respbmp, 0, 0, bmp.Width, bmp.Height);
                bmp.Flush();
            }
            catch (Exception ex)
            {
                Debug.Print(ex.ToString());
            }
            while (true)
            {
                Thread.Sleep(10000);
            }
        }

        private Window mainWindow;

        public Window CreateWindow()
        {
            // Create a window object and set its size to the
            // size of the display.
            mainWindow = new Window();
            mainWindow.Height = SystemMetrics.ScreenHeight;
            mainWindow.Width = SystemMetrics.ScreenWidth;

            // Create a single text control.
            Text text = new Text();

            //text.Font = Resources.GetFont(Resources.FontResources.small);
            //text.TextContent = Resources.GetString(Resources.StringResources.String1);
            text.HorizontalAlignment = Microsoft.SPOT.Presentation.HorizontalAlignment.Center;
            text.VerticalAlignment = Microsoft.SPOT.Presentation.VerticalAlignment.Center;

            // Add the text control to the window.
            mainWindow.Child = text;

            // Connect the button handler to all of the buttons.
            mainWindow.AddHandler(Buttons.ButtonUpEvent, new RoutedEventHandler(OnButtonUp), false);

            // Set the window visibility to visible.
            mainWindow.Visibility = Visibility.Visible;

            // Attach the button focus to the window.
            Buttons.Focus(mainWindow);

            return mainWindow;
        }

        private void OnButtonUp(object sender, RoutedEventArgs evt)
        {
            ButtonEventArgs e = (ButtonEventArgs)evt;

            // Print the button code to the Visual Studio output window.
            Debug.Print(e.Button.ToString());
        }
    }
}
