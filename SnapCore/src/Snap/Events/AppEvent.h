#pragma once
#include "Event.h"

namespace SnapEngine
{
	class AppStartEvent : public IEvent
	{
	public:
		AppStartEvent() {}

		EVENT_CLASS_TYPE(AppStart)
			EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	class AppUpdateEvent : public IEvent
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	class AppRenderEvent : public IEvent
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};
}