#pragma once

#include <Snap/Events/Event.h>
#include <Snap/Core/TimeStep.h>
namespace SnapEngine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_DebugName(name)
		{}

		virtual ~Layer() {}

		virtual void Start() {}
		virtual void Destroy() {}
		virtual void Update(TimeStep Time) {}
		virtual void ImGuiRender() {}

		virtual void ProcessEvent(IEvent& e) {}

		inline const std::string& GetName() const { return m_DebugName; }

	private:
		std::string m_DebugName;
	};
}