// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 23:12 BRT
// Last edited on October 20 of 2018, at 23:30 BRT

using System;
using System.Collections.Generic;

namespace Bliss.Assembler
{
    public class Module
    {
        public List<Instruction> Body { get; private set; }

        public Module()
        {
            Body = new List<Instruction>();                             // Init our body
        }

        public void Merge(Module module)
        {
            foreach (Instruction instr in module.Body)
                Body.Add(instr);
        }

        public byte[] ToByteArray()
        {
            List<byte> ret = new List<byte>()
            {
                (byte)'B',                                              // Our header "magic"
                (byte)'L',
                (byte)'I',
                (byte)'S'
            };

            byte[] num = BitConverter.GetBytes((uint)Body.Count);       // Let's get the body size

            if (!BitConverter.IsLittleEndian)                           // Convert to little endian
                Array.Reverse(num);

            ret.AddRange(num);                                          // Add range

            foreach (Instruction instr in Body)                         // Let's add all the body instructions to the ret
                ret.AddRange(instr.ToByteArray());

            return ret.ToArray();
        }
    }
}
