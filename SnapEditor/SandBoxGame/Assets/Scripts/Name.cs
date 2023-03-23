using SnapEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SnapEngine;


namespace SandBox
{
    internal class NameScript : Entity
    {
        public TextRendererComponent textRenderer;
        public float Time = 0.0f;
        void OnCreat()
        {
            textRenderer = this.CreatAndGetComponent<TextRendererComponent>();
        }
        void OnUpdate(float TimeStep)
        {
            Time += TimeStep;
            textRenderer.Text = "Time = " + Time.ToString("N1");
        }
    }
}
