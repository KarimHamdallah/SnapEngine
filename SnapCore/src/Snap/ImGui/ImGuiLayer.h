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
		virtual void ProcessEvent(IEvent& e) override;

		void Begin();
		void End();

		inline void BlockEvents(bool block) { m_BlockEvents = block; }

	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true; // Block Any EventProcessing of any layer behind ImGui Layer ( that means break loop of process Layer events inside Apllication class)

	private:
		void SetDarkThemeColors();
	};
}