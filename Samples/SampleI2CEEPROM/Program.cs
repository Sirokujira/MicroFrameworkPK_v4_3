using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace SampleI2CEEPROM
{
    public class Program
    {
        static I2CDevice _24LC16 = new I2CDevice(new I2CDevice.Configuration(0x50, 100));

        public static int I2CEEPROM_ReadBytes(UInt16 Addr, byte[] data)
        {
            I2CDevice.I2CTransaction[] trans = new I2CDevice.I2CTransaction[] {
                I2CDevice.CreateWriteTransaction(new byte[] {(byte)(Addr >> 8) , (byte)(Addr & 0xff)}),
                I2CDevice.CreateReadTransaction(data)};
            int bytesTransfered = _24LC16.Execute(trans, 1000);
            return bytesTransfered;
        }

        public static int I2CEEPROM_WriteByte(UInt16 Addr, byte data)
        {
            I2CDevice.I2CTransaction[] trans = new I2CDevice.I2CTransaction[] {
                I2CDevice.CreateWriteTransaction(new byte[] {(byte)(Addr >> 8) , (byte)(Addr & 0xff), data})};
            int bytesTransfered = _24LC16.Execute(trans, 1000);
            System.Threading.Thread.Sleep(5);
            return bytesTransfered;
        }

        public static void Main()
        {
            int bytesTransfered;
            byte[] data = new byte[0x10];
            for (int i = 0; i < data.Length; i++) {
                data[i] = (byte)(i+5);
                bytesTransfered = I2CEEPROM_WriteByte((UInt16)i, data[i]);
                data[i] = (byte)0;
            }
            bytesTransfered = I2CEEPROM_ReadBytes(0, data);
            Debug.Print("bytesTransfered " + bytesTransfered.ToString());
            for (int i = 0; i < data.Length; i++)
                Debug.Print("data[" +i.ToString() + "] = " + data[i].ToString("X2"));
        }

    }
}
