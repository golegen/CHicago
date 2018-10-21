// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 17:36 BRT
// Last edited on October 20 of 2018, at 17:52 BRT

namespace Bliss.Assembler.Nodes
{
    public class StatementNode : Node
    {
        public static Node Parse(Parser parser)
        {
            if (!parser.AcceptToken(TokenType.EOL) && !parser.AcceptToken(TokenType.Semicolon))         // End of line/Semicolon?
                return InstructionNode.Parse(parser);                                                   // No, just redirect to the InstructionNode.Parse function (for now)
            return null;                                                                                // Yes, just return null
        }
    }
}
