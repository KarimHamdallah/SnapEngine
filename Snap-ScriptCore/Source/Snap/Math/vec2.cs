using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SnapEngine
{
    public struct vec2
    {
        public float x, y;
        public vec2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public float Length()
        {
            return (float)Math.Sqrt((double)(x * x + y * y));
        }

        public float LengthSqrd()
        {
            return x * x + y * y;
        }
    }
}