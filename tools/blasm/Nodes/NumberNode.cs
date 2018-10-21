// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 17:40 BRT
// Last edited on October 20 of 2018, at 18:28 BRT

namespace Bliss.Assembler.Nodes
{
    public class NumberNode : Node
    {
        public int SignedValue { get; private set; }
        public uint UnsignedValue { get; private set; }
        public float FloatValue { get; private set; }
        public bool IsSigned { get; private set; }
        public bool IsUnsigned { get; private set; }
        public bool IsFloat { get; private set; }

        public NumberNode(string filename, int line, int value)
        {
            this.filename = filename;
            this.line = line;
            SignedValue = value;
            IsSigned = true;
        }

        public NumberNode(string filename, int line, uint value)
        {
            this.filename = filename;
            this.line = line;
            UnsignedValue = value;
            IsUnsigned = true;
        }

        public NumberNode(string filename, int line, float value)
        {
            this.filename = filename;
            this.line = line;
            FloatValue = value;
            IsFloat = true;
        }
    }
}
