// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 17:45 BRT
// Last edited on October 20 of 2018, at 18:28 BRT

namespace Bliss.Assembler.Nodes
{
    public class InstructionNode : Node
    {
        public string Opcode { get; private set; }

        public InstructionNode(string filename, int line, string opcode)
        {
            this.filename = filename;
            this.line = line;
            Opcode = opcode;
        }

        public static Node Parse(Parser parser)
        {
            Token opc = parser.ExpectToken(TokenType.Identifier);                                                       // Read the opcode name
            Node instr = new InstructionNode(opc.Filename, opc.Line, (string)opc.Value);                                // Create the node

            if (parser.AcceptToken(TokenType.EOL) || parser.AcceptToken(TokenType.Semicolon))                           // End of line/Semicolon?
                return instr;                                                                                           // Yes, return!

            while (true)
            {
                if (parser.MatchToken(TokenType.Float))                                                                 // Float?
                {
                    Token num = parser.ExpectToken(TokenType.Float);                                                    // Yes

                    instr.Children.Add(new NumberNode(num.Filename, num.Line, (float)opc.Value));
                }
                else
                {
                    Token num = parser.ExpectToken(TokenType.Number);                                                   // No, so it's a number

                    if (num.Value is int)                                                                               // Signed?
                        instr.Children.Add(new NumberNode(num.Filename, num.Line, (int)num.Value));                     // Yes
                    else
                        instr.Children.Add(new NumberNode(num.Filename, num.Line, (uint)num.Value));                    // No, it's unsigned
                }

                if (!parser.AcceptToken(TokenType.Comma))                                                               // Comma?
                    break;                                                                                              // Yes, probably there is more arguments!
            }

            if (parser.AcceptToken(TokenType.Semicolon) || parser.EndOfStream)                                          // Semicolon/End of file?
                return instr;                                                                                           // Yes, return!

            parser.ExpectToken(TokenType.EOL);                                                                          // No, so let's hope to have an EOL here!

            return instr;
        }
    }
}
