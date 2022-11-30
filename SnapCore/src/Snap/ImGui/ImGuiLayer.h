#pragma once
#include <Snap/Core/Layer.h>

namespace SnapEngine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void Start() override;
		virtual void Destroy() override;
		//virtual void ImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}