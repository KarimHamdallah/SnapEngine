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

    public class TextRendererComponent : Component
    {
        public string Text
        {
            get
            {
                return InternalCalls.TextComponent_GetText(m_Entity.m_ID);
            }
            set { InternalCalls.TextComponent_SetText(m_Entity.m_ID, value); }
        }
        public vec4 Color
        {
            get
            {
                InternalCalls.TextComponent_GetColor(m_Entity.m_ID, out vec4 val);
                return val;
            }
            set { InternalCalls.TextComponent_SetColor(m_Entity.m_ID, ref value); }
        }

        public float KerningOffset
        {
            get
            {
                InternalCalls.TextComponent_GetKerningOffset(m_Entity.m_ID, out float val);
                return val;
            }
            set { InternalCalls.TextComponent_SetKerningOffset(m_Entity.m_ID, ref value); }
        }

        public float LineSpacing
        {
            get
            {
                InternalCalls.TextComponent_GetLineSpacing(m_Entity.m_ID, out float val);
                return val;
            }
            set { InternalCalls.TextComponent_SetLineSpacing(m_Entity.m_ID, ref value); }
        }
    }
}