// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 17:35 BRT
// Last edited on October 20 of 2018, at 18:25 BRT

using System.Collections.Generic;

namespace Bliss.Assembler.Nodes
{
    public abstract class Node
    {
        public List<Node> Children { get { return children; } }
        public string Filename { get { return filename; } }
        public int Line { get { return line; }} 

        readonly List<Node> children = new List<Node>();
        protected string filename;
        protected int line;
    }
}
