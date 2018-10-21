// File author is Ítalo Lima Marconato Matias
//
// Created on October 20 of 2018, at 16:50 BRT
// Last edited on October 21 of 2018, at 12:22 BRT

using System.Collections.Generic;
using Bliss.Assembler.Nodes;

namespace Bliss.Assembler
{
    public class Parser
    {
        public bool EndOfStream { get { return tokens.Count <= position; } }

        readonly List<Token> tokens = new List<Token>();
        int position = 0;

        public Parser(List<Token> tokens)
        {
            this.tokens = tokens;                                                                                                                   // Init our token list
        }

        public Node Parse()
        {
            position = 0;                                                                                                                           // Set the position back to 0

            CodeBlock block = new CodeBlock();                                                                                                      // Create the root node

            while (!EndOfStream)                                                                                                                    // Parse!
            {
                Node node = StatementNode.Parse(this);

                if (node != null)                                                                                                                   // We can add this node?
                    block.Children.Add(node); 						                                                                                // Yes!
            }

            return block;
        }

        public bool MatchToken(TokenType type)
        {
            return PeekToken() != null && PeekToken().Type == type;                                                                                 // Token type match?
        }

        public bool MatchToken(TokenType type, object value)
        {
            return PeekToken() != null && PeekToken().Type == type && PeekToken().Value == value;                                                   // Token type and value match?
        }

        public bool AcceptToken(TokenType type)
        {
            if (MatchToken(type))                                                                                                                   // Token type match?
            {
                ReadToken();                                                                                                                        // Yes!
                return true;
            }

            return false;                                                                                                                           // No...
        }

        public bool AcceptToken(TokenType type, ref Token token)
        {
            if (MatchToken(type))                                                                                                                   // Token type match?
            {
                token = ReadToken();                                                                                                                // Yes!
                return true;
            }

            return false;                                                                                                                           // No...
        }

        public bool AcceptToken(TokenType type, object value)
        {
            if (MatchToken(type, value))                                                                                                            // Token type and value match?
            {
                ReadToken();                                                                                                                        // Yes!
                return true;
            }

            return false;                                                                                                                           // No...
        }

        public bool AcceptToken(TokenType type, object value, ref Token token)
        {
            if (MatchToken(type, value))                                                                                                            // Token type and value match?
            {
                token = ReadToken();                                                                                                                // Yes!
                return true;
            }

            return false;                                                                                                                           // No...
        }

        public Token ExpectToken(TokenType type)
        {
            Token ret = null;

            if (AcceptToken(type, ref ret))                                                                                                         // Token type should match
                return ret;                                                                                                                         // Ok, match!

            ret = ReadToken();                                                                                                                      // ... let's get the value

            if (ret != null)                                                                                                                        // End of file?
                Utils.Error($"{ret.Filename}:{ret.Line}", $"expected {type}, got {ret.Type}");                                                      // No

            ret = PeekToken(-1);

            Utils.Error($"{ret.Filename}:{ret.Line}", $"expected {type}, got end of file");                                                         // Yes

            return null;
        }

        public Token ExpectToken(TokenType type, object value)
        {
            Token ret = null;

            if (AcceptToken(type, value, ref ret))                                                                                                  // Token type and value should match
                return ret;                                                                                                                         // Ok, match!

            ret = ReadToken();                                                                                                                      // ... let's get the value

            if (ret != null)                                                                                                                        // End of file?
                Utils.Error($"{ret.Filename}:{ret.Line}", $"expected {type} with value '{value}', got {ret.Type} with value '{ret.Value}");         // No

            ret = PeekToken(-1);

            Utils.Error($"{ret.Filename}:{ret.Line}", $"expected {type} with value '{value}', got end of file");                                    // Yes

            return null;
        }

        Token PeekToken(int n = 0)
        {
            if (position + n < tokens.Count)                                                                                                        // Sanity check
                return tokens[position + n];                                                                                                        // Ok!

            return null;                                                                                                                            // Failed, return null
        }

        Token ReadToken()
        {
            if (position < tokens.Count)                                                                                                            // End of the file?
                return tokens[position++];                                                                                                          // No!

            return null;                                                                                                                            // Yes, return null
        }
    }
}
