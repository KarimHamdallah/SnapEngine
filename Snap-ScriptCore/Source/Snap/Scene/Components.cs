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
        public enum RigidBody2DType { STATIC = 0, DYNAMIC, KINEMATIC };
        public vec2 LinearVelocity
        {
            get
            {
                InternalCalls.RigidBody2D_GetLinearVelocity(m_Entity.m_ID, out vec2 LinearVelocity);
                return LinearVelocity;
            }
        }

        public RigidBody2DType Type
        {
            get
            {
                InternalCalls.RigidBody2D_GetBodyType(m_Entity.m_ID, out RigidBody2DType BodyType);
                return BodyType;
            }
            set 
            {
                InternalCalls.RigidBody2D_SetBodyType(m_Entity.m_ID, ref value);
            }
        }

        public void ApplyLinearImpulseToCenter(vec2 Impulse, bool wake)
        {
            InternalCalls.RigidBody2D_ApplyLinearImpulseToCenter(m_Entity.m_ID, ref Impulse, wake);
        }
    }
}