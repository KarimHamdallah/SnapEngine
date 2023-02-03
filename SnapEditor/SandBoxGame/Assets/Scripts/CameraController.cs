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
        public float DistanceFromPlayer = 30.0f;

        private Entity Player;
        private RigidBody2DComponent Player_Rb2d;
        private float Time;
        void OnCreat()
        {
            Console.WriteLine("CameraController OnCreat");
            Player = FindEntityByName("Player");
            if (Player != null)
                Player_Rb2d = Player.CreatAndGetComponent<RigidBody2DComponent>();
            if (Player_Rb2d != null)
                Player_Rb2d.Type = RigidBody2DComponent.RigidBody2DType.STATIC;
        }

        void OnUpdate(float TimeStep)
        {

            if (Player != null && Player_Rb2d != null)
            {
                Time += TimeStep;

                if (Time > 5.0f)
                    Player_Rb2d.Type = RigidBody2DComponent.RigidBody2DType.DYNAMIC;

               float distanceFromPlayer = 0.0f; // ex: 0.0f
               vec2 LinearVelocity = Player_Rb2d.LinearVelocity;
               float Target = DistanceFromPlayer + LinearVelocity.Length(); // ex: 30.0f
               distanceFromPlayer = SnapMath.Lerpf(distanceFromPlayer, Target, 0.5f); // ex: 0.0f ~ 30
               
                
               vec3 CameraPosition = this.Position;
               CameraPosition.x = Player.Position.x;
               CameraPosition.y = Player.Position.y;
               CameraPosition.z = distanceFromPlayer;
               this.Position = CameraPosition;
            }
        }
    }
}