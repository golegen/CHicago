// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 22:32 BRT
// Last edited on October 20 of 2018, at 23:03 BRT

using System;
using System.Collections.Generic;

namespace Bliss.Assembler
{
    public enum Opcode
    {
        add,
        sub,
        mul,
        div,
        mod,
        shl,
        shr,
        and,
        or,
        xor,
        lds,
        ldu,
        ldf,
        print
    }

    public class Instruction
    {
        public Opcode Opcode { get; private set; }
        public object Argument { get; private set; }

        public Instruction(Opcode opcode, object arg = null)
        {
            Opcode = opcode;                                                            // Set the opcode
            Argument = arg;                                                             // Set the argument (optional)
        }

        public byte[] ToByteArray()
        {
            List<byte> ret = new List<byte>
            {
                (byte)Opcode                                                            // Init the ret list with the opcode
            };

            if (Argument != null && Argument is int)                                    // Signed int as argument?
            {
                byte[] bytes = BitConverter.GetBytes((int)Argument);                    // Yes, convert it to bytes

                if (!BitConverter.IsLittleEndian)                                       // Convert to little endian
                    Array.Reverse(bytes);

                ret.AddRange(bytes);                                                    // Add range!
            }
            else if (Argument != null && Argument is uint)                              // Unsigned int as argument?
            {
                byte[] bytes = BitConverter.GetBytes((uint)Argument);                   // Yes, convert it to bytes

                if (!BitConverter.IsLittleEndian)                                       // Convert to little endian
                    Array.Reverse(bytes);

                ret.AddRange(bytes);                                                    // Add range
            }
            else if (Argument != null && Argument is float)                             // Float as argument?
            {
                byte[] bytes = BitConverter.GetBytes((float)Argument);                  // Yes, convert it to bytes

                if (!BitConverter.IsLittleEndian)                                       // Convert to little endian
                    Array.Reverse(bytes);

                ret.AddRange(bytes);                                                    // Add range
            }

            return ret.ToArray();
        }
    }
}
