using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using Microsoft.SPOT.Net.NetworkInformation;

namespace WebserverHelloWorld
{
    public class Program
    {
        public static void Main()
        {
            //Microsoft.SPOT.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces()[0].EnableDhcp();
            Thread.Sleep(3000);
            NetworkInterface[] networkInterfaces = NetworkInterface.GetAllNetworkInterfaces();
            networkInterfaces[0].EnableStaticIP("192.168.1.150", "255.255.255.0", "192.168.1.1");
            Debug.Print("Web Server IP Address: " + Microsoft.SPOT.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces()[0].IPAddress);
            WebServer webServer = new WebServer();
        }
    }
}