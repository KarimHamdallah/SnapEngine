using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace SnapEngine
{
    public class Entity
    {
        protected Entity() { m_ID = 0; }
        internal Entity(ulong UUID) { m_ID = UUID; Console.WriteLine("Entity Constructor Called!"); }

        public readonly ulong m_ID;




        
        public vec3 Position
        {
            get
            {
                InternalCalls.Transform_GetPositionVec3(m_ID, out vec3 val);
                return val;
            }
            set { InternalCalls.Transform_SetPositionVec3(m_ID, ref value); }
        }






        public bool HasComponent<T>() where T : Component, new()
        {
            Type ComponentType = typeof(T);
            return InternalCalls.Entity_HasComponent(m_ID, ComponentType);
        }
        public T CreatAndGetComponent<T>() where T : Component, new()
        {
            if(!HasComponent<T>())
                return null;
            
            T Comp = new T() { m_Entity = this };
            return Comp;
        }
    }
}