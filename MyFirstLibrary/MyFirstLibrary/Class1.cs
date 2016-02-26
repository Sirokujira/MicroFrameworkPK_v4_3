using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using System.Runtime.CompilerServices;

namespace MyFirstLibrary
{
    public class ImpulseSender 
    { 
        uint _pin; // The native code supports only basic values, so we can't (easily) pass neither OutputPort nor Cpu.Pin. 
                // Cpu.Pin is 4-byte enum, and as we will see later uint is what we need on the native side. 
        
        public ImpulseSender(OutputPort port) 
        { 
            _pin = (uint)port.Id; // the pin number is enough to identify the port on the native side, as we will also see later 
        } 

        // every method we plan to implement in native code must be declared extern and marked with the following attribute 
        [MethodImpl(MethodImplOptions.InternalCall)] 
        public extern void WritePulse(bool positive, int durationCycles); 
    }
}
