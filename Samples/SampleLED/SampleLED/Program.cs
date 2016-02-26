//#define CQ_FRK_FM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
//#define KS_MB9BF506
//#define KS_MB9BF568
//#define GR_SAKURA
#define GR_PEACH

using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace SampleLED
{
    public class Program
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
#if KS_MB9BF506
        static Cpu.Pin pinLED = (Cpu.Pin)0x40;      // LED: P40 pin 
        //static Cpu.Pin pinLED = (Cpu.Pin)0x62;      // LED: P62 pin 
#endif
#if KS_MB9BF568
        static Cpu.Pin pinLED = (Cpu.Pin)0x35;      // LED: P35 pin 
#endif
#if GR_SAKURA
        // D1: PA0 -> 0x50
        // D2: PA1 -> 0x51
        // D3: PA2 -> 0x52
        // D4: PA6 -> 0x56
        static Cpu.Pin pinLED = (Cpu.Pin)0x56;      // LED: PA6 pin
#endif
#if GR_PEACH
        // LED1 = P4_4,
        // LED2 = P3_2,
        // LED3 = P4_6,
        // LED4 = P4_7,
        static Cpu.Pin pinLED = (Cpu.Pin)0x44;      // LED1: P4_4 pin
#endif
        public static void Main()
        {
            OutputPort GPIO_Out = new OutputPort(pinLED, true);
            Int32 i = 0;
            while (true)
            {
                Debug.Print("Hello, World! " + i.ToString() + " times");
                GPIO_Out.Write(false);
                Thread.Sleep(500);
                GPIO_Out.Write(true);
                Thread.Sleep(500);
                i++;
            }
        }
    }
}
