//#define CQ_FRK_FM3
//#define CQ_FRK_RX62N
//#define CQ_FRK_NXP_ARM
#define GR_PEACH

using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace SampleGPIOInterruptPort
{
    public class Program
    {
#if CQ_FRK_FM3
        static Cpu.Pin pinButton = (Cpu.Pin)0xF6;   // FM3 NMI pin (PF6)
#endif
#if CQ_FRK_RX62N
        static Cpu.Pin pinButton = (Cpu.Pin)101;    // LED: PC5 pin (CN2-36) = 12*8+5 = 101
#endif
#if CQ_FRK_NXP_ARM
        static Cpu.Pin pinButton = (Cpu.Pin)82;     // LED: P1[28] pin = 2*32+18 = 82 
#endif
#if GR_PEACH
        // Button 0: P6_0
        // Button 1: P6_1
        static Cpu.Pin pinButton = (Cpu.Pin)0x60;   // Button0: P6_0 pin = 0x60 
#endif

        static void GPIO_Interrupt_OnInterrupt(uint data1, uint data2, DateTime time)
        {
            Debug.Print("The button is pressed");
        }
        
        public static void Main()
        {
            InterruptPort GPIO_Interrupt = new InterruptPort(pinButton, false, Port.ResistorMode.PullUp, Port.InterruptMode.InterruptEdgeLow);
            GPIO_Interrupt.OnInterrupt += new NativeEventHandler(GPIO_Interrupt_OnInterrupt);
            Int32 i = 0;
            while (true)
            {
                Thread.Sleep(1000);
                i++;
            }
        }
    }
}

