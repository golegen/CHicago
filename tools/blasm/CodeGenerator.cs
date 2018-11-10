// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 23:19 BRT
// Last edited on November 10 of 2018, at 13:31 BRT

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
            else if (node is LabelNode label)                                                                                       // Label?
            {
                if (module.Labels.ContainsKey(label.Name))                                                                          // Yes, let's check if it isn't an redefinition
                    Utils.Error($"{label.Filename}:{label.Line}", "redefinition of", label.Name);

                module.Labels.Add(label.Name, module.Body.Count);                                                                   // And add it!
            }
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
                    case "pop":
                    case "ret":
                    case "fargs":
                    case "flocs":
                    case "cmp":                                                                                                     // Instructions without any arguments
                        {
                            if (instr.Children.Count != 0)                                                                          // Valid amount of arguments (0)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 0 argument");                               // No...

                            module.Body.Add(new Instruction((Opcode)Enum.Parse(typeof(Opcode), instr.Opcode.ToLower())));           // Add it to the module
                            break;
                        }
                    case "lds":                                                                                                     // Load signed integer
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

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
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

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
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

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
                    case "ldloc":                                                                                                   // Load local
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.ldloc, (uint)num.SignedValue));                              // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.ldloc, num.UnsignedValue));                                  // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.ldloc, (uint)num.FloatValue));                               // Yes, convert to unsigned integer value

                            break;
                        }
                    case "stloc":                                                                                                   // Store local
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.stloc, (uint)num.SignedValue));                              // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.stloc, num.UnsignedValue));                                  // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.stloc, (uint)num.FloatValue));                               // Yes, convert to unsigned integer value

                            break;
                        }
                    case "br":                                                                                                      // Jump
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (instr.Children[0] is NumberNode)                                                                    // Valid argument?
                            {
                                NumberNode num = (NumberNode)instr.Children[0];                                                     // Yes (number)

                                if (num.IsSigned)                                                                                   // Signed value?
                                    module.Body.Add(new Instruction(Opcode.br, (uint)num.SignedValue));                             // Yes, convert to unsigned value
                                else if (num.IsUnsigned)                                                                            // Unsigned value?
                                    module.Body.Add(new Instruction(Opcode.br, num.UnsignedValue));                                 // Yes :)
                                else if (num.IsFloat)                                                                               // Float value?
                                    module.Body.Add(new Instruction(Opcode.br, (uint)num.FloatValue));                              // Yes, convert to unsigned integer value
                            }
                            else if (instr.Children[0] is IdentifierNode)
                                module.Body.Add(new Instruction(Opcode.br, ((IdentifierNode)instr.Children[0]).Value));             // Yes (identifier)
                            else
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            break;
                        }
                    case "brc":                                                                                                     // Conditional jump
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (instr.Children[0] is NumberNode)                                                                    // Valid argument?
                            {
                                NumberNode num = (NumberNode)instr.Children[0];                                                     // Yes (number)

                                if (num.IsSigned)                                                                                   // Signed value?
                                    module.Body.Add(new Instruction(Opcode.brc, (uint)num.SignedValue));                            // Yes, convert to unsigned value
                                else if (num.IsUnsigned)                                                                            // Unsigned value?
                                    module.Body.Add(new Instruction(Opcode.brc, num.UnsignedValue));                                // Yes :)
                                else if (num.IsFloat)                                                                               // Float value?
                                    module.Body.Add(new Instruction(Opcode.brc, (uint)num.FloatValue));                             // Yes, convert to unsigned integer value
                            }
                            else if (instr.Children[0] is IdentifierNode)
                                module.Body.Add(new Instruction(Opcode.br, ((IdentifierNode)instr.Children[0]).Value));             // Yes (identifier)
                            else
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            break;
                        }
                    case "call":                                                                                                    // Call
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (instr.Children[0] is NumberNode)                                                                    // Valid argument?
                            {
                                NumberNode num = (NumberNode)instr.Children[0];                                                     // Yes (number)

                                if (num.IsSigned)                                                                                   // Signed value?
                                    module.Body.Add(new Instruction(Opcode.call, (uint)num.SignedValue));                           // Yes, convert to unsigned value
                                else if (num.IsUnsigned)                                                                            // Unsigned value?
                                    module.Body.Add(new Instruction(Opcode.call, num.UnsignedValue));                               // Yes :)
                                else if (num.IsFloat)                                                                               // Float value?
                                    module.Body.Add(new Instruction(Opcode.call, (uint)num.FloatValue));                            // Yes, convert to unsigned integer value
                            }
                            else if (instr.Children[0] is IdentifierNode)
                                module.Body.Add(new Instruction(Opcode.call, ((IdentifierNode)instr.Children[0]).Value));           // Yes (string)
                            else
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            break;
                        }
                    case "ncall":                                                                                                   // Native call
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.ncall, (uint)num.SignedValue));                              // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.ncall, num.UnsignedValue));                                  // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.ncall, (uint)num.FloatValue));                               // Yes, convert to unsigned integer value

                            break;
                        }
                    case "ncallc":                                                                                                  // Native conditional call
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.ncallc, (uint)num.SignedValue));                             // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.ncallc, num.UnsignedValue));                                 // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.ncallc, (uint)num.FloatValue));                              // Yes, convert to unsigned integer value

                            break;
                        }
                    case "aargs":                                                                                                   // Alloc args space
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.aargs, (uint)num.SignedValue));                              // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.aargs, num.UnsignedValue));                                  // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.aargs, (uint)num.FloatValue));                               // Yes, convert to unsigned integer value

                            break;
                        }
                    case "alocs":                                                                                                   // Alloc locs space
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is NumberNode))                                                                 // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            NumberNode num = (NumberNode)instr.Children[0];

                            if (num.IsSigned)                                                                                       // Signed value?
                                module.Body.Add(new Instruction(Opcode.alocs, (uint)num.SignedValue));                              // Yes, convert to unsigned value
                            else if (num.IsUnsigned)                                                                                // Unsigned value?
                                module.Body.Add(new Instruction(Opcode.alocs, num.UnsignedValue));                                  // Yes :)
                            else if (num.IsFloat)                                                                                   // Float value?
                                module.Body.Add(new Instruction(Opcode.alocs, (uint)num.FloatValue));                               // Yes, convert to unsigned integer value

                            break;
                        }
                    case "setc":                                                                                                    // Set condition
                        {
                            if (instr.Children.Count != 1)                                                                          // Valid amount of arguments (1)?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "expected 1 argument");                               // No...

                            if (!(instr.Children[0] is IdentifierNode))                                                             // Valid argument?
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid argument");                                  // No...

                            IdentifierNode ident = (IdentifierNode)instr.Children[0];

                            if (ident.Value.ToLower() == "eq")                                                                      // Convert the condition to the setc argument!
                                module.Body.Add(new Instruction(Opcode.setc, (uint)0));
                            else if (ident.Value.ToLower() == "neq")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)1));
                            else if (ident.Value.ToLower() == "lo")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)2));
                            else if (ident.Value.ToLower() == "nlo")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)3));
                            else if (ident.Value.ToLower() == "gr")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)4));
                            else if (ident.Value.ToLower() == "ngr")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)5));
                            else if (ident.Value.ToLower() == "le")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)6));
                            else if (ident.Value.ToLower() == "nle")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)7));
                            else if (ident.Value.ToLower() == "ge")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)8));
                            else if (ident.Value.ToLower() == "nge")
                                module.Body.Add(new Instruction(Opcode.setc, (uint)9));
                            else
                                Utils.Error($"{instr.Filename}:{instr.Line}", "invalid condition");                                 // Invalid condition...

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
