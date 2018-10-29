// File author is Ítalo Lima Marconato Matias
//
// Created on October 04 of 2018, at 18:27 BRT
// Last edited on October 28 of 2018, at 00:40 BRT

using System;
using System.Collections.Generic;
using System.IO;

namespace Bliss.Assembler
{
    public class Program
    {
        public static void Main(string[] args)
        {
            List<string> inputs = new List<string>();
            string output = null;

            for (int i = 0; i < args.Length; i++)                                                                                       // Let's check the arguments!
            {
                if (args[i] == "--help")                                                                                                // Help?
                {
                    Console.WriteLine("Usage: {0} [options] file...", Utils.GetProgramName());                                          // Yes :)
                    Console.WriteLine("Options:");
                    Console.WriteLine("  --help              Display this information");
                    Console.WriteLine("  --version           Display compiler version information");
                    Console.WriteLine("  -dumpversion        Display the version of the compiler");
                    Console.WriteLine("  -o <file>           Place the output into <file>");
                    Environment.Exit(0);
                }
                else if (args[i] == "--version")                                                                                        // Full version string?
                {
                    Console.WriteLine("blasm (BLCC) 1.0.0");                                                                            // Yes :)
                    Console.WriteLine("Copyright (C) 2018 CHicago Development Team.");
                    Console.WriteLine("This is free software; There is NO warranty; not even for MERCHANTABILITY");
                    Console.WriteLine("or FITNESS FOR A PARTICULAR PURPOSE.");
                    Environment.Exit(0);
                }
                else if (args[i] == "-dumpversion")                                                                                     // Version number?
                {
                    Console.WriteLine("1.0.0");                                                                                         // Yes :)
                    Environment.Exit(0);
                }
                else if (args[i].StartsWith("-o"))                                                                                      // Set the output file?
                {
                    if (args[i] == "-o")                                                                                                // Yes, it's after the '-o', or we have an space before?
                    {
                        if (i + 1 >= args.Length)                                                                                       // We have an space before
                            Utils.Error(Utils.GetProgramName(), "missing filename after", "-o");                                        // Unexpected end of the cmd line!
                        else
                            output = args[++i];
                    }
                    else
                        output = args[i].Substring(args[i].LastIndexOf("-o") + 2);                                                      // Ok, it's just after the '-o'!
                }
                else if (args[i].StartsWith("-"))                                                                                       // Unrecognized command line option?
                    Utils.Error(Utils.GetProgramName(), "unrecognized command line option", args[i]);                                   // Yes :(
                else
                    inputs.Add(args[i]);                                                                                                // IT'S A FILE!
            }

            if (inputs.Count == 0)                                                                                                      // We have input files?
                Utils.Error(Utils.GetProgramName(), "no input files");                                                                  // No :(

            Module tmodule = new Module();                                                                                              // This is our temp module :)

            foreach (string file in inputs)                                                                                             // Let's parse all the files!
            {
                if (Directory.Exists(file))                                                                                             // It's a directory?
                    Utils.Error(Utils.GetProgramName(), $"{file}: it's a directory");                                                   // Yes... 
                else if (!File.Exists(file))                                                                                            // It exists?
                    Utils.Error(Utils.GetProgramName(), $"{file}: no such file or directory");                                          // No...

                tmodule.Merge(new CodeGenerator(new Parser(new Lexer(file, File.ReadAllText(file)).Lex()).Parse()).Generate(), true);   // Parse this file!
            }

            Module module = new Module();

			module.Merge(tmodule, false);                                                                                               // Check/fix labels

            if (output == null)                                                                                                         // Output filename set?
                File.WriteAllBytes("a.bliss", module.ToByteArray());                                                                    // No, so let's output to 'a.bliss'
            else
                File.WriteAllBytes(output, module.ToByteArray());                                                                       // Yes!
        }
    }
}
