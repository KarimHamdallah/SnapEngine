using SnapEngine;
using System;

namespace SnapEngine
{
    public abstract class Component
    { 
        public Entity m_Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public vec3 Position
        {
            
            get
            {
                InternalCalls.Transform_GetPositionVec3(m_Entity.m_ID, out vec3 val);
                return val;
            }
            set{ InternalCalls.Transform_SetPositionVec3(m_Entity.m_ID, ref value); }
        }
    }

    public class RigidBody2DComponent : Component
    {
        public void ApplyLinearImpulseToCenter(vec2 Impulse, bool wake)
        {
            InternalCalls.RigidBody_ApplyLinearImpulseToCenter(m_Entity.m_ID, ref Impulse, wake);
        }
    }
}