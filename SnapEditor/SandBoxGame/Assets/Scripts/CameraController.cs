using SnapEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SandBox
{
    internal class CameraController : Entity
    {
        public float DistanceFromPlayer = 10.0f;
        public float Speed = 10.0f;
        void OnCreat()
        {
            Console.WriteLine("CameraController OnCreat");
        }

        void OnUpdate(float TimeStep)
        {
            if (InternalCalls.IsKeyPressed(Key.E))
                DistanceFromPlayer += Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.Q))
                DistanceFromPlayer -= Speed * TimeStep;

            vec3 Pos;
            Entity PlayerEntity = FindEntityByName("Sprite");
            if (PlayerEntity != null)
            {
                Pos = new vec3(PlayerEntity.Position.x, PlayerEntity.Position.y, DistanceFromPlayer);
            }
            else
            {
                Pos = new vec3(this.Position.x, this.Position.y, DistanceFromPlayer);
            }
            this.Position = Pos;
        }
    }
}