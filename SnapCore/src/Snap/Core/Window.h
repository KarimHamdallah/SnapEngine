#pragma once

#include "SnapPCH.h"
#include <Snap/Events/Event.h>

namespace SnapEngine
{
	struct WindowProps
	{
		unsigned int Width;
		unsigned int Height;
		std::string Title;
		bool IsVsync = false;

		WindowProps(const std::string& Title = "SnapEngine Application",
			unsigned int Width = 800, unsigned int Height = 600)
			: Width(Width), Height(Height), Title(Title)
		{}
	};

	class IWindow
	{
	public:

		using ProcessEventFuncPtr = std::function<void(IEvent&)>;

		virtual ~IWindow() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void SetProcessEventFuncPtr(const ProcessEventFuncPtr& callback) = 0;
		virtual void SetVsync(bool enabled) = 0;
		virtual bool IsVsync() const = 0;

		static IWindow* Creat(const WindowProps& props = WindowProps());
	};
}