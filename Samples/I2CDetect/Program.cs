using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
//using CQ.NETMF.CQ_FRK_FM3.Hardware;


// CH1
// 0xa0(0x50): FRAM 
// CH5
// 0x7c(0x3e): AQM0802 LCD
// 0x90(0x48): ADT7410 temperature
// 0x92(0x49): PCF8591
// 0xa0(0x50): EEPROM
// 0xb8(0x5c): MLPS331 atmosphier

// SMBus Quick Command
// ===================
//
// This sends a single bit to the device, at the place of the Rd/Wr bit.
//
// A Addr Rd/Wr [A] P
// 
// Functionality flag: I2C_FUNC_SMBUS_QUICK
// 
namespace I2CDetect
{
    public class Program
    {
        static int I2CSmbusReadByte(ushort addr)
        {
            byte[] data = new byte[1];
            //FM3_I2CDevice i2c = new FM3_I2CDevice(i2c_ch, new FM3_I2CDevice.Configuration(addr, 100));
            I2CDevice i2c = new I2CDevice(new I2CDevice.Configuration(addr, 10));
            I2CDevice.I2CTransaction[] transaction = new I2CDevice.I2CTransaction[] { I2CDevice.CreateReadTransaction(data) };
            int result = i2c.Execute(transaction, 1000);
            if (result > 0)
                return result;
            else
                return -1;
        }

        static int I2CSmbusWriteQuick(ushort addr)
        {
            return I2CSmbusReadByte(addr);

        }

        static void ScanI2CBus(uint first, uint last)
        {
            uint i;
            uint j;
            int res;
            string line = "";

            Debug.Print("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
            for (i = 0; i < 128; i += 16)
            {
                line = i.ToString("x2") + ": ";
                for (j = 0; j < 16; j++)
                {
                    if (i + j < first || i + j > last)
                    {
                        line += "   ";
                        continue;
                    }
                    if ((i + j >= 0x30 && i + j <= 0x37) || (i + j >= 0x50 && i + j <= 0x5F))
                        res = I2CSmbusReadByte((ushort)(i+j));
                    else
                        res = I2CSmbusWriteQuick((ushort)(i + j));
                    if (res < 0)
                        line += "-- ";
                    else
                        line += (i + j).ToString("x2") + " ";
                }
                Debug.Print(line);
            }
        }

        public static void Main()
        {
            ScanI2CBus(32, 127);
            //ScanI2CBus(0x48, 0x49);
            //ScanI2CBus(0x50, 0x5b);
        }

    }
}
