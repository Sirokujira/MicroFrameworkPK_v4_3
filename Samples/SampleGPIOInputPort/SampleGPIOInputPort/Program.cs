//#define CQ_FRK_FM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
//#define GR_SAKURA
#define GR_PEACH

using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace SampleGPIOInputPort
{
    public class Program
    {

#if CQ_FRK_FM3
        static Cpu.Pin pinButton = (Cpu.Pin)0xF6;   // FM3 NMI pin
#endif
#if CQ_FRK_RX62N
        static Cpu.Pin pinButton = (Cpu.Pin)101;    // Button: PC5 pin (CN2-36) = 12*8+5 = 101
#endif
#if CQ_FRK_NXP_ARM
        static Cpu.Pin pinButton = (Cpu.Pin)82;     // Button: P1[28] pin = 2*32+18 = 82 
#endif
#if GR_PEACH
        // Button 0: P6_0
        // Button 1: P6_1
        static Cpu.Pin pinButton = (Cpu.Pin)0x60;   // Button0: P6_0 pin = 0x60 
#endif
        public static void Main()
        {
            InputPort GPIO_In = new InputPort(pinButton, false, Port.ResistorMode.PullUp);
            Int32 i = 0;
            while (true)
            {
                if (GPIO_In.Read())
                    Debug.Print("Botton is High");
                else
                    Debug.Print("Botton is Low");
                Thread.Sleep(1000);
                i++;
            }
        }
    }
}
