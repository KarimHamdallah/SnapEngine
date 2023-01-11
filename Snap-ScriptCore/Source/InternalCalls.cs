using System;
using System.Runtime.CompilerServices;

namespace SnapEngine
{
    public static class InternalCalls
    {
        // C++ Internal Calls
        // Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool IsKeyPressed(Key key);

        // Components
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_GetPositionVec3(ulong EntityID, out vec3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_SetPositionVec3(ulong EntityID, ref vec3 value);

        // Math
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float Snap_Sin(float value);

        // Physics
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_ApplyLinearImpulseToCenter(ulong EntityID, ref vec2 impulse, bool wake);

        // Entity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_HasComponent(ulong EntityID, Type ComponentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Entity_GetComponent(ulong EntityID, Type ComponentType);
    }
}