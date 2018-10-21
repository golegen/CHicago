// File author is Ítalo Lima Marconato Matias
//
// Created on October 05 of 2018, at 20:34 BRT
// Last edited on October 20 of 2018, at 17:25 BRT

using System;
using System.Collections.Generic;
using System.Globalization;

namespace Bliss.Assembler
{
    public enum TokenType
    {
        Identifier,
        String,
        Number,
        Float,
        Parentheses,
        Braces,
        Semicolon,
        Colon,
        Comma,
        Dot,
        EOL
    }

    public class Token
    {
        public TokenType Type { get; private set; }
        public string Filename { get; private set; }
        public object Value { get; private set; }
        public int Line { get; private set; }

        public Token(TokenType type, string filename, object val, int line)
        {
            Filename = filename;                                                                                    // Filename...
            Type = type;                                                                                            // Type...
            Value = val;                                                                                            // This way we don't need to int.Parse, float.Parse or any other later!
            Line = line;                                                                                            // Line...
        }

        public override string ToString()
        {
            string ret = "Line " + Line + ": " + Type + (Value == null ? "" : $": ");                               // Let's build this string!

            if (Value != null)
            {
                if (Value is float)                                                                                 // Float?
                    ret += ((float)Value).ToString(new CultureInfo("en-US").NumberFormat);
                else if (Value is double)                                                                           // Double?
                    ret += ((double)Value).ToString(new CultureInfo("en-US").NumberFormat);
                else
                    ret += Value;                                                                                   // No special handling!
            }

            return ret;
        }
    }

    public class Lexer
    {
        readonly string filename;
        readonly string code;
        int position;
        int line;

        public Lexer(string filename, string code)
        {
            this.filename = filename;
            this.code = code;
            position = 0;
            line = 1;
        }

        public List<Token> Lex()
        {
            List<Token> ret = new List<Token>();

            position = 0;                                                                                           // Reset position and current line
            line = 1;

            while (GetChar() != -1)                                                                                 // Let's lex!
            {
                char curr = (char)GetChar();                                                                        // Let's get the current char!
                bool rchar = true;

                while (char.IsWhiteSpace(curr) && curr != '\n')
                {
                    ReadChar();
                    curr = (char)GetChar();
                }

                if (GetChar() == -1)
                    break;

                if (char.IsLetter(curr) || curr == '_' || curr == '@')                                              // Identifier?
                    ret.Add(ReadIdentifier(out rchar));
                else if (IsDecimal(curr) || curr == '-' || curr == '.')                                             // Number?
                    ret.Add(ReadNumber(out rchar));
                else if (curr == '(' || curr == ')')                                                                // Parentheses?
                    ret.Add(new Token(TokenType.Parentheses, filename, curr.ToString(), line));
                else if (curr == '{' || curr == '}')                                                                // Braces?
                    ret.Add(new Token(TokenType.Braces, filename, curr.ToString(), line));
                else if (curr == ';')                                                                               // Semicolon?
                    ret.Add(new Token(TokenType.Semicolon, filename, null, line));
                else if (curr == ':')                                                                               // Colon?
                    ret.Add(new Token(TokenType.Colon, filename, null, line));
                else if (curr == ',')                                                                               // Comma?
                    ret.Add(new Token(TokenType.Comma, filename, null, line));
                else if (curr == '.')                                                                               // Dot?
                    ret.Add(new Token(TokenType.Dot, filename, null, line));
                else if (curr == '\n')                                                                              // End of Line (EOL)?
                    ret.Add(new Token(TokenType.EOL, filename, null, line));
                else
                    Utils.Error($"{filename}:{line}", "unexpected symbol", curr.ToString());                        // Invalid symbol/character...

                if (rchar)
                    ReadChar();
            }

            return ret;
        }

        bool IsIdentifier(char ch)
        {
            return char.IsLetterOrDigit(ch) || ch == '_' || ch == '@';                                              // Valid identifier: Letter or digit, underline or at sign (@)
        }

        bool IsBinary(char ch)
        {
            return ch == '0' || ch == '1';                                                                          // Binary (base 2, 0 and 1)
        }

        bool IsOctal(char ch)
        {
            return ch >= '0' && ch <= '7';                                                                          // Octal (base 8, 0 to 7)
        }

        bool IsHexdecimal(char ch)
        {
            return IsDecimal(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');                           // Hexadecimal (base 16, 0 to 9 and A to F)
        }

        bool IsDecimal(char ch)
        {
            return ch >= '0' && ch <= '9';                                                                          // Decimal (base 10, 0 to 9)
        }

        Token ReadIdentifier(out bool rchar)
        {
            string str = "";
            int cline = line;

            rchar = false;

            while (char.IsLetterOrDigit((char)GetChar()) || GetChar() == '_' || GetChar() == '@')                   // Identifier valid character?
                str += (char)ReadChar();                                                                            // Yes!

            return new Token(TokenType.Identifier, filename, str, cline);                                           // Return the token!
        }

        Token ReadNumber(out bool rchar)
        {
            string str = "";
            int cline = line;
            NumberFormatInfo fmt = new CultureInfo("en-US").NumberFormat;                                           // Default number format

            rchar = false;

            if (GetChar() == '0' && GetChar(1) == 'b')                                                              // Binary?
            {
                ReadChar(); ReadChar();                                                                             // Yes, only allow unsigned numbers

                while (IsBinary((char)GetChar()))
                    str += (char)ReadChar();

                try
                {
                    return new Token(TokenType.Number, filename, Convert.ToUInt32(str, 2), cline);                  // Try to convert!
                }
                catch
                {
                    Utils.Warning($"{filename}:{cline}", "integer constant is too large for its type");             // Too large (probably)...
                    return new Token(TokenType.Number, filename, uint.MaxValue, cline);
                }
            }
            if (GetChar() == '0' && GetChar(1) == 'o')                                                              // Octal?
            {
                ReadChar(); ReadChar();                                                                             // Yes, only allow unsigned numbers

                while (IsOctal((char)GetChar()))
                    str += (char)ReadChar();

                try
                {
                    return new Token(TokenType.Number, filename, Convert.ToUInt32(str, 8), cline);                  // Try to convert!
                }
                catch
                {
                    Utils.Warning($"{filename}:{cline}", "integer constant is too large for its type");             // Too large (probably)...
                    return new Token(TokenType.Number, filename, uint.MaxValue, cline);
                }
            }
            if (GetChar() == '0' && GetChar(1) == 'x')                                                              // Hexadecimal
            {
                ReadChar(); ReadChar();                                                                             // Yes, only allow unsigned numbers

                while (IsHexdecimal((char)GetChar()))
                    str += (char)ReadChar();

                try
                {
                    return new Token(TokenType.Number, filename, Convert.ToUInt32(str, 16), cline);                 // Try to convert
                }
                catch
                {
                    Utils.Warning($"{filename}:{cline}", "integer constant is too large for its type");             // Too large (probably)...
                    return new Token(TokenType.Number, filename, uint.MaxValue, cline);
                }
            }

            bool fp = false;
            char force = ' ';

            if (GetChar() == '-')                                                                                   // Negative?
                str += (char)ReadChar();                                                                            // Yes!

            if (GetChar() == '.')                                                                                   // 0.?
            {
                str += "0.";                                                                                        // Yes, so we already know that it's a floating point number!
                ReadChar();
                fp = true;
            }

            while (IsDecimal((char)GetChar()) || (!fp && GetChar() == '.'))                                         // Let's consume the characters!
            {
                fp = fp ? fp : GetChar() == '.';
                str += (char)ReadChar();
            }

            if (GetChar() == 'f' || GetChar() == 'd')                                                               // Force to float/double?
                force = (char)ReadChar();                                                                           // Yes

            try
            {
                if ((fp && force != 'd') || (force == 'f'))                                                         // Try float?
                    return new Token(TokenType.Float, filename, float.Parse(str, fmt), cline);                      // Yes
                if (force == 'd')                                                                                   // Try double?
                    return new Token(TokenType.Float, filename, double.Parse(str, fmt), cline);                     // Yes
                return new Token(TokenType.Number, filename, int.Parse(str), cline);                                // Try int
            }
            catch
            {
                if (fp && force == ' ')                                                                             // No float/double force?
                {
                    try
                    {
                        return new Token(TokenType.Float, filename, double.Parse(str, fmt), cline);                 // So let's try double!
                    }
                    catch
                    {
                        if (str[0] == '-')                                                                          // Too small?
                        {
                            Utils.Warning($"{filename}:{cline}", "float constant is too small for its type");       // Yes...
                            return new Token(TokenType.Float, filename, float.MinValue, cline);
                        }

                        Utils.Warning($"{filename}:{cline}", "float constant is too large for its type");           // Too large
                        return new Token(TokenType.Float, filename, float.MaxValue, cline);
                    }
                }
                if (fp && force == 'f')                                                                             // Force float?
                {
                    if (str[0] == '-')                                                                              // Yes... too small?
                    {
                        Utils.Warning($"{filename}:{cline}", "float constant is too small for its type");           // Yes
                        return new Token(TokenType.Float, filename, float.MinValue, cline);
                    }

                    Utils.Warning($"{filename}:{cline}", "float constant is too large for its type");               // Too large
                    return new Token(TokenType.Float, filename, float.MaxValue, cline);
                }
                if (fp && force == 'd')                                                                             // Force double?
                {
                    if (str[0] == '-')                                                                              // Yes... too small?
                    {
                        Utils.Warning($"{filename}:{cline}", "float constant is too small for its type");           // Yes
                        return new Token(TokenType.Float, filename, double.MinValue, cline);
                    }

                    Utils.Warning($"{filename}:{cline}", "float constant is too large for its type");               // Too large
                    return new Token(TokenType.Float, filename, double.MaxValue, cline);
                }
                if (str[0] == '-')                                                                                  // Too small?
                {
                    Utils.Warning($"{filename}:{cline}", "integer constant is too small for its type");             // Yes...
                    return new Token(TokenType.Number, filename, int.MinValue, cline);
                }

                try
                {
                    return new Token(TokenType.Number, filename, uint.Parse(str), cline);                           // We can try uint!
                }
                catch
                {
                    Utils.Warning($"{filename}:{cline}", "integer constant is too large for its type");             // ... Too large
                    return new Token(TokenType.Number, filename, uint.MaxValue, cline);
                }
            }
        }

        int GetChar(int pos = 0)
        {
            if (position + pos >= code.Length)                                                                      // Sanity check
                return -1;                                                                                          // Failed, return -1
            return code[position + pos];
        }

        int ReadChar()
        {
            if (position >= code.Length)                                                                            // End of file?
                return -1;                                                                                          // Yes, return -1

            if (code[position] == '\n')                                                                             // Line feed?
                line++;                                                                                             // Yes, increase the line variable

            return code[position++];                                                                                // Increase the position variable and return the current character
        }
    }
}
