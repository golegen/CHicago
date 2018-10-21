// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 23:19 BRT
// Last edited on October 20 of 2018, at 23:44 BRT

using System;
using Bliss.Assembler.Nodes;

namespace Bliss.Assembler
{
    public class CodeGenerator
    {
        readonly Node ast;

        public CodeGenerator(Node ast)
        {
            this.ast = ast;                                                                                                         // Set our root node
        }

        public Module Generate()
        {
            Module module = new Module();                                                                                           // Create a new module
            Visit(module, ast);                                                                                                     // Let's fill it
            return module;                                                                                                          // And return!
        }

        void Visit(Module module, Node node)
        {
            if (node is CodeBlock)                                                                                                  // Block of code?
                VisitSubnodes(module, node);                                                                                        // Yes, visit the subnodes
            else if (node is InstructionNode instr)                                                                                 // Instruction?
            {
                switch (instr.Opcode.ToLower())                                                                                     // Let's validate the opcode
                {
                    case "add":
                    case "sub":
                    case "mul":
                    case "div":
                    case "mod":
                    case "shl":
                    case "shr":
                    case "and":
                    case "or":
                    case "xor":
                    case "print":                                                                                                   // Instructions without any arguments
                        {
                            if (instr.Children.Count != 0)                                                                          // Valid amount of arguments (0)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 0 arguments");                              // No...

                            module.Body.Add(new Instruction((Opcode)Enum.Parse(typeof(Opcode), instr.Opcode.ToLower())));           // Add it to the module
                            break;
                        }
                    case "lds":                                                                                                     // Load signed integer
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 arguments");                              // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsUnsigned && num.UnsignedValue > int.MaxValue)                                                 // Too large unsigned value?
                                Utils.Warning($"{instr.Filename}:{instr.Line}", "integer constant is too large for its type");      // Yes, just warning the user

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.lds, num.SignedValue));                                      // Yes :)
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.lds, (int)num.UnsignedValue));                               // Yes, convert to signed value
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.lds, (int)num.FloatValue));                                  // Yes, convert to signed integer value

                            break;
                        }
                    case "ldu":                                                                                                     // Load unsigned integer
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 arguments");                              // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.ldu, (uint)num.SignedValue));                                // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.ldu, num.UnsignedValue));                                    // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.ldu, (uint)num.FloatValue));                                 // Yes, convert to unsigned integer value

                            break;
                        }
                    case "ldf":                                                                                                     // Load float
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 arguments");                              // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsUnsigned && num.UnsignedValue > float.MaxValue)                                               // Too large unsigned value?
                                Utils.Warning($"{instr.Filename}:{instr.Line}", "integer constant is too large for its type");      // Yes, just warning the user

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.ldf, (float)num.SignedValue));                               // Yes, convert to float value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.ldf, (float)num.UnsignedValue));                             // Yes, convert to float value
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.ldf, num.FloatValue));                                       // Yes :)

                            break;
                        }
                    default:
                        {
                            Utils.Error($"{instr.Filename}:{instr.Line}", $"unknown instruction '{instr.Opcode}'");                 // Unknown instruction...
                            break;
                        }
                }
            }
        }

        void VisitSubnodes(Module module, Node node)
        {
            foreach (Node sub in node.Children)                                                                                     // Visit the subnodes
                Visit(module, sub);
        }
    }
}
