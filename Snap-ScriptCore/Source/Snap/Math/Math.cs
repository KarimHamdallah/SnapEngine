using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SnapEngine
{
    public class SnapMath
    {
        public static float Lerpf(float a, float b, float t)
        {
            return a * (1.0f - t) + (b * t);
        }

        public static float Clampf(float value, float min, float max)
        {
            return value < min ? min : value > max ? max : value;
        }
    }
}