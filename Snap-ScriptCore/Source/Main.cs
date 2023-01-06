using System;
using System.Collections.Generic;

namespace SnapEngine
{
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main Class Constructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World From C#");
        }

        public void Printvalue(float value)
        {
            Console.WriteLine($"C# Saying : {value}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# Saying : {message}");
        }
    }
}