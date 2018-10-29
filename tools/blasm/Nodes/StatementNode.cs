// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 17:36 BRT
// Last edited on October 26 of 2018, at 01:11 BRT

namespace Bliss.Assembler.Nodes
{
    public class StatementNode : Node
    {
        public static Node Parse(Parser parser)
        {
            if (parser.AcceptToken(TokenType.EOL) || parser.AcceptToken(TokenType.Semicolon))			// End of line/Semicolon?
                return null;                                                                            // Yes, return null
            else if (parser.AcceptToken(TokenType.Colon))                                               // Colon?
                return LabelNode.Parse(parser);                                                         // Yes, so it's a label!

            return InstructionNode.Parse(parser);                                                       // Just redirect to the InstructionNode.Parse function
        }
    }
}
