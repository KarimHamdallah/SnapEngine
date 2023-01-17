using SnapEngine;
using System;
using System.Collections.Generic;

namespace SandBox
{
    internal class Player : Entity
    {
        bool once = true;

        public float Speed = 10.0f;

        public void OnCreat()
        {
            Console.WriteLine("Player...EntityClass....OnCreat");
        }

        public void OnUpdate(float TimeStep)
        {
            if (InternalCalls.IsKeyPressed(Key.Space) && once)
            {
                Console.WriteLine("Player...EntityClass....OnUpdate... SpaceKeyPressed!");
                once = false;
            }

            if (InternalCalls.IsKeyReleased(Key.Space))
                once = true;


            vec3 Pos = this.Position;
            if (InternalCalls.IsKeyPressed(Key.W))
                Pos.y += Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.S))
                Pos.y -= Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.A))
                Pos.x -= Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.D))
                Pos.x += Speed * TimeStep;
            this.Position = Pos;
        }
    }
}