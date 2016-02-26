// How to execute
// - Copy the follwing bitmap files on root directory of SD card.
//   MIF201201l.bmp
//   MIF201202l.bmp
//   MIF201203l.bmp
//   MIF201204l.bmp
//   MIF201205l.bmp
//   MIF201206l.bmp
//   MIF201207l.bmp

//#define CQ_FRK_FM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
//#define KS_MB9BF568
//#define GR_SAKURA
#define GR_PEACH

using System;
using System.IO;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
//using System.Runtime.InteropServices;
using CQ.NETMF.SD;
using CQ.NETMF.LCD;

namespace SampleSDBitmap
{
    public class Program
    {
#if CQ_FRK_FM3
        static SPI.SPI_module mod = SPI.SPI_module.SPI1;    // SPI ch0 = SPI1
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x30;             // SPI CS = P30
#endif
#if CQ_FRK_RX62N
        static SPI.SPI_module mod = SPI.SPI_module.SPI1;    // SPI ch0 = SPI1
        static Cpu.Pin pinSDCS = (Cpu.Pin)100;              // SPI CS = PC4 = 8*0xc + 4 = 100
#endif
#if CQ_FRK_NXP_ARM
        static SPI.SPI_module mod = SPI.SPI_module.SPI1;    // SPI ch0 = SPI1
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x06;             // SPI CS = P0[6]
#endif
#if KS_MB9BF568
        static SPI.SPI_module mod = SPI.SPI_module.SPI1;    // SPI ch0 = SPI1
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x26;             // SPI CS = P26
#endif
#if GR_SAKURA
        // CS: PC0 - 0x60
        static SPI.SPI_module mod = SPI.SPI_module.SPI1;    // SPI ch0 = SPI1
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x60;             // SPI CS = PC0 - 0x60
#endif
#if GR_PEACH
        // CS: PC0 - 0x60
        static SPI.SPI_module mod = SPI.SPI_module.SPI3;    // SPI ch2 = SPI3
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x84;             // SPI CS = P8_4
#endif
        static int headerSize = 0x36;
        static int bitmapDy = 1;

        public void BmpRead16(string inputFileName)
        {
            int ofs = 0;
            FileStream inputFile = new FileStream(inputFileName, FileMode.Open);
            // byte[] bitmapFileHeader = new byte[sizeof(BITMAPFILEHEADER)];
            // byte[] bitmapInfoHeader = new byte[sizeof(BITMAPINFOHEADER)];
            // ofs += inputFile.Read(bitmapFileHeader, ofs, sizeof(BITMAPFILEHEADER));
            // ofs += inputFile.Read(bitmapInfoHeader, ofs, sizeof(BITMAPFILEHEADER));
            byte[] bitmapHeader = new byte[headerSize];
            ofs += inputFile.Read(bitmapHeader, 0, (int)headerSize);
            int wx = (int)Utility.ExtractValueFromArray(bitmapHeader, 18, 4);
            int wy = (int)Utility.ExtractValueFromArray(bitmapHeader, 22, 4);
            int depth = (int)Utility.ExtractValueFromArray(bitmapHeader, 28, 2);
            int lineBytes = wx * depth / 8;
            int bufSize = lineBytes * bitmapDy;
            byte[] bitmapOneLine = new byte[bufSize];
            for (int y = wy - 1 - bitmapDy; y >= 0; y -= bitmapDy)
            {
                // Debug.Print("y = " + y.ToString());
                inputFile.Read(bitmapOneLine, 0, (int)bufSize);
                LCDDevice.BitBltEx(0, (uint)y, (uint)wx, (uint)bitmapDy, bitmapOneLine);
                Thread.Sleep(10);
                // Debug.GC(true);
                // Debug.EnableGCMessages(true);
                // Debug.Print(Debug.GC(true).ToString());
            }
            inputFile.Close();
        }

        public static void Main()
        {
            Debug.Print("Mounting SD...");
            StorageDevice.MountSD("SD", mod, pinSDCS);
            LCDDevice.Clear();
            Program p = new Program();
            while (true)
            {
                for (int i = 1; i <= 1; i++)
                {
                    p.BmpRead16(@"\SD\MIF20120" + i.ToString() + "l.bmp");
                    Thread.Sleep(2000);
                }
            }
        }

    }
}
