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
        public float DistanceFromPlayer;
        public void OnCreat()
        {

        }

        public void OnUpdate(float TimeStep)
        {
            Entity PlayerEntity = FindEntityByName("Sprite");
            if (PlayerEntity != null)
            {
                vec3 pos = new vec3(PlayerEntity.Position.x, PlayerEntity.Position.y, DistanceFromPlayer);
                this.Position = pos;
            }
        }
    }
}