using SnapEngine;
using System;
using System.Collections.Generic;

namespace SandBox
{
    internal class Player : Entity
    {
        TransformComponent m_Transform;
        RigidBody2DComponent m_RigidBody;
        bool once = true;

        public float Speed = 10.0f;
        public void OnCreat()
        {
            Console.WriteLine("EntityClass....OnCreat");
            m_Transform = CreatAndGetComponent<TransformComponent>();
            m_RigidBody = CreatAndGetComponent<RigidBody2DComponent>();
        }

        public void OnUpdate(float TimeStep)
        {
            if (InternalCalls.IsKeyPressed(Key.Space) && once)
            {
                Console.WriteLine("EntityClass....OnUpdate... SpaceKeyPressed!");
                if(m_RigidBody != null)
                    m_RigidBody.ApplyLinearImpulseToCenter(new vec2(0.0f, 3.0f), true);
                once = false;
            }

            if (InternalCalls.IsKeyReleased(Key.Space))
                once = true;

            
            vec3 Pos = m_Transform.Position;
            if (InternalCalls.IsKeyPressed(Key.W))
                Pos.y += Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.S))
                Pos.y -= Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.A))
                Pos.x -= Speed * TimeStep;
            if (InternalCalls.IsKeyPressed(Key.D))
                Pos.x += Speed * TimeStep;
            m_Transform.Position = Pos;
        }
    }
}