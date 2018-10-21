// File author is Ítalo Lima Marconato Matias
//
// Created on October 04 of 2018, at 18:30 BRT
// Last edited on October 05 of 2018, at 20:55 BRT

using System;
using System.IO;

namespace Bliss.Assembler
{
    public static class Utils
    {
        public static string GetProgramName()
        {
            return Path.GetFileNameWithoutExtension(Environment.GetCommandLineArgs()[0]);       // Get C/C++ argv using Environment.GetCommandLineArgs() and extract just the filename without the extension
        }

        public static void Warning(string pre, string msg)
        {
            Console.Error.Write($"{pre}: ");                                                    // Write the "pre" msg
            Console.ForegroundColor = ConsoleColor.Yellow;                                      // Change the color to yellow
            Console.Error.Write("warning: ");                                                   // Indicate that this is an warning
            Console.ResetColor();                                                               // Reset back the color to the default one
            Console.Error.WriteLine(msg);                                                       // Write the msg
        }

        public static void Error(string pre, string msg, string white = null)
        {
            Console.Error.Write($"{pre}: ");                                                    // Write the "pre" msg
            Console.ForegroundColor = ConsoleColor.Red;                                         // Change the color to red
            Console.Error.Write("error: ");                                                     // Indicate that this is an error
            Console.ResetColor();                                                               // Reset back the color to the default one
            Console.Error.Write(msg);                                                           // Write the msg

            if (white != null)                                                                  // We need to put something in the white color?
            {
                Console.ForegroundColor = ConsoleColor.White;                                   // Yup! Change the color to white
                Console.Error.WriteLine($" ´{white}´");                                         // Write the "white" msg
                Console.ResetColor();                                                           // And reset back the color to the default one
            }
            else
                Console.Error.WriteLine();                                                      // Nope...

            Console.WriteLine("compilation terminated.");
            Environment.Exit(1);                                                                // Exit
        }
    }
}
