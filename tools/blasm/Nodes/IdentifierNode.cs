// File author is Ítalo Lima Marconato Matias
//
// Created on October 28 of 2018, at 00:48 BRT
// Last edited on October 28 of 2018, at 00:58 BRT

namespace Bliss.Assembler.Nodes
{
    public class IdentifierNode : Node
    {
        public string Value { get; private set; }

        public IdentifierNode(string filename, int line, string value)
        {
            this.filename = filename;
            this.line = line;
            Value = value;
        }
    }
}
