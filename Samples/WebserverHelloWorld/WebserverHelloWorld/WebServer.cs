// The original program is as follows
// http://forums.netduino.com/index.php?/topic/1500-netduino-plus-hello-world-web-server/

//#define CQ_FRK_FM3          // WKFM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
//#define GR_SAKURA
#define GR_PEACH

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace WebserverHelloWorld
{
    public class WebServer : IDisposable
    {
#if CQ_FRK_FM3
        static Cpu.Pin pinLED = (Cpu.Pin)0xF3;      // LED: PF3 pin
#endif
#if CQ_FRK_RX62N
        static Cpu.Pin pinLED = (Cpu.Pin)0x0D;      // LED: P15 pin (CN2-18) = 1*8+5 = 13
#endif
#if CQ_FRK_NXP_ARM
        static Cpu.Pin pinLED = (Cpu.Pin)50;        // LED: P1[18] pin = 1*32+18 = 50 
#endif
#if GR_SAKURA
        static Cpu.Pin pinLED = (Cpu.Pin)86;        // LED: PA6 pin = 10*8+6 = 86 
#endif
#if GR_PEACH
        // LED1 = P4_4,
        // LED2 = P3_2,
        // LED3 = P4_6,
        // LED4 = P4_7,
        static Cpu.Pin pinLED = (Cpu.Pin)0x44;      // LED1: P4_4 pin
#endif
        private Socket server = null;
        //open connection to onbaord led so we can blink it with every request
        private OutputPort LED = new OutputPort(pinLED, false);
        public WebServer()
        {
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, 80);
            server.Bind(localEndPoint);
            server.Listen(5);
            while (true)
            {
                try
                {
                    using (Socket clientSocket = server.Accept())
                    {
                        if (clientSocket == null)
                            continue;
                        //Get clients IP
                        //IPEndPoint clientEP = clientSocket.RemoteEndPoint as IPEndPoint;
                        //EndPoint clientEndPoint = clientSocket.RemoteEndPoint;
                        if (clientSocket.Poll(-1, SelectMode.SelectRead))
                        {
                            int bytesReceived = clientSocket.Available;
                            if (bytesReceived > 0)
                            {
                                //Get request
                                byte[] buffer = new byte[bytesReceived];
                                int byteCount = clientSocket.Receive(buffer, bytesReceived, SocketFlags.None);
                                string request = new string(Encoding.UTF8.GetChars(buffer));
                                Debug.Print(request);
                                //Compose a response
                                string response = "Hello World";
                                string header = "HTTP/1.0 200 OK\r\nContent-Type: text; charset=utf-8\r\nContent-Length: " + response.Length.ToString() + "\r\nConnection: close\r\n\r\n";
                                clientSocket.Send(Encoding.UTF8.GetBytes(header), header.Length, SocketFlags.None);
                                clientSocket.Send(Encoding.UTF8.GetBytes(response), response.Length, SocketFlags.None);
                                //Blink the onboard LED
                                LED.Write(true);
                                Thread.Sleep(150);
                                LED.Write(false);
                            }
                        }

                    }

                }
                catch (Exception e)
                {
                    Debug.Print(e.StackTrace);
                    break;
                }
            }
        }        
        #region IDisposable Members
        ~WebServer()
        {
            Dispose();
        }
        public void Dispose()
        {
            if (server != null)
                server.Close();
        }
        #endregion
    }
}



