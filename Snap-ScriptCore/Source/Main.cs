using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace SnapEngine
{
    public struct Vector3
    {
        public float x, y, z;
        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    public static class InternalCalls
    {
        // C++ Internal Calls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CppFunc();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float Vector3_Dot(ref Vector3 v1, ref Vector3 v2);
    }

    public class Main
    {

        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main Class Constructor");
            InternalCalls.CppFunc();
            Vector3 v1 = new Vector3(1.0f, 5.0f, 3.0f);
            Vector3 v2 = new Vector3(2.0f, 2.0f, 1.0f);

            Console.WriteLine($"Dot Product Calculated From C++ And Debgged From C# Console.WriteLine = {InternalCalls.Vector3_Dot(ref v1, ref v2)}");
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