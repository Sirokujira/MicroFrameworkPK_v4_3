//#define CQ_FRK_FM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
//#define KS_MB9BF568
//#define GR_SAKURA
#define GR_PEACH

using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using CQ.NETMF.SD;
using System.IO;

namespace SampleSD
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
        // CS: P8_4
        static SPI.SPI_module mod = SPI.SPI_module.SPI3;    // SPI ch2 = SPI3
        static Cpu.Pin pinSDCS = (Cpu.Pin)0x84;             // SPI CS = P8_4
#endif
        public static void Main()
        {
            StorageDevice.MountSD("SD", (SPI.SPI_module)mod, pinSDCS);
            //StorageDevice.MountSD("SD", (SPI.SPI_module)mod, Cpu.Pin.GPIO_NONE);
            string[] directories = System.IO.Directory.GetDirectories(@"\");
            Debug.Print("directory count: " + directories.Length.ToString());
            for (int i = 0; i < directories.Length; i++)
            {
                Debug.Print("directory: " + directories[i]);
            }
            string[] files = System.IO.Directory.GetFiles(directories[0]);
            Debug.Print("file count: " + files.Length.ToString());
            for (int i = 0; i < files.Length; i++)
            {
                Debug.Print("filename: " + files[i]);
            }
        }
    }
}
