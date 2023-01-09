using SnapEngine;
using System;
using System.Collections.Generic;

namespace SandBox
{
    internal class Player : Entity
    {
        public void OnCreat()
        {
            Console.WriteLine("EntityClass....OnCreat");
        }

        public void OnUpdate(float TimeStep)
        {
            Console.WriteLine("EntityClass....OnUpdate");
        }
    }
}