// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 23:12 BRT
// Last edited on November 10 of 2018, at 13:22 BRT

using System;
using System.Collections.Generic;

namespace Bliss.Assembler
{
    public class Module
    {
        public List<Instruction> Body { get; private set; }
        public Dictionary<string, int> Labels { get; private set; }

        public Module()
        {
            Body = new List<Instruction>();                                                                             // Init our body
			Labels = new Dictionary<string, int>();                                                                     // Init our label list
        }

        public void Merge(Module module, bool ok)
        {
            uint bbase = (uint)Body.Count;
            
            foreach (var pair in module.Labels) {                                                                       // Add all the labels
                if (Labels.ContainsKey(pair.Key))                                                                       // Already exists?
                    Utils.Error(Utils.GetProgramName(), "redefinition of", pair.Key);                                   // Yes...
                
                Labels.Add(pair.Key, pair.Value + Body.Count);                                                          // Add it!
            }
            
            foreach (Instruction instr in module.Body)
			{
                if (instr.Opcode == Opcode.br || instr.Opcode == Opcode.brc ||
                    instr.Opcode == Opcode.call || instr.Opcode == Opcode.callc) {                                      // Fix the argument of this instruction?
                    if (instr.Argument is string) {                                                                     // Yes, label?
                        if (Labels.ContainsKey((string)instr.Argument))                                                 // We can resolve it?
                            Body.Add(new Instruction(instr.Opcode, (uint)Labels[(string)instr.Argument]));              // Yes :)
                        else if (!ok)
							Utils.Error(Utils.GetProgramName(), "undefined reference to", (string)instr.Argument);      // Error out :(
						else
                            Body.Add(instr);                                                                            // No...
                    }
                    else
                        Body.Add(new Instruction(instr.Opcode, ((uint)instr.Argument) + bbase));                        // No, so just fix the argument
                }
                else
                    Body.Add(instr);                                                                                    // No
			}
        }

        public byte[] ToByteArray()
        {
            List<byte> ret = new List<byte>()
            {
                (byte)'B',                                                                                              // Our header "magic"
                (byte)'L',
                (byte)'I',
                (byte)'S'
            };

            byte[] num = BitConverter.GetBytes((uint)Body.Count);                                                       // Let's get the body size

            if (!BitConverter.IsLittleEndian)                                                                           // Convert to little endian
                Array.Reverse(num);

            ret.AddRange(num);                                                                                          // Add range

            foreach (Instruction instr in Body)                                                                         // Let's add all the body instructions to the ret
                ret.AddRange(instr.ToByteArray());

            return ret.ToArray();
        }
    }
}
