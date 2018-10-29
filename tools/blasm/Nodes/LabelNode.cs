// File author is Ítalo Lima Marconato Matias
//
// Created on October 28 of 2018, at 01:06 BRT
// Last edited on October 28 of 2018, at 01:13 BRT

namespace Bliss.Assembler.Nodes
{
    public class LabelNode : Node
    {
        public string Name { get; private set; }

        public LabelNode(string filename, int line, string name)
        {
            this.filename = filename;
            this.line = line;
            Name = name;
        }

        public static Node Parse(Parser parser)
        {
            Token name = parser.ExpectToken(TokenType.Identifier);                                                      // Read the name name
            Node label = new LabelNode(name.Filename, name.Line, (string)name.Value);                                   // Create the node

            if (parser.AcceptToken(TokenType.Semicolon) || parser.EndOfStream)                                          // Semicolon/End of file?
                return label;                                                                                           // Yes, return!

            parser.ExpectToken(TokenType.EOL);                                                                          // No, so let's hope to have an EOL here!

            return label;
        }
    }
}
