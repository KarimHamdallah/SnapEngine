using SnapEngine;
using System;
using System.Collections.Generic;

namespace SandBox
{
    internal class Player : Entity
    {
        public float Speed = 10.0f;
        public float Time = 0.0f;

        public void OnCreat()
        {
            Console.WriteLine("EntityClass....OnCreat");
        }

        public void OnUpdate(float TimeStep)
        {
            Time += TimeStep;

            Entity Cam = FindEntityByName("Camera");
            if (Cam != null)
            {
                Console.WriteLine("Camera Entity is not null");
                // Get Camera Controller Script On Cam Entity
                CameraController Controller = Cam.As<CameraController>();
                if (Controller != null)
                {
                    if (InternalCalls.IsKeyPressed(Key.E))
                        Controller.DistanceFromPlayer += Speed * TimeStep;
                    if (InternalCalls.IsKeyPressed(Key.Q))
                        Controller.DistanceFromPlayer -= Speed * TimeStep;
                }
            }

            
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