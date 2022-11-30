#pragma once
#include <Snap/Core/Input.h>

namespace SnapEngine
{
	class WindowsInput : public Input
	{
	public:
		virtual bool IsKeyPressedImpl(int KeyCode) override;
		virtual bool IsKeyReleasedImpl(int KeyCode) override;
		virtual bool IsKeyDownImpl(int KeyCode) override;

		virtual bool IsMouseButtonPressedImpl(int Button) override;
		virtual bool IsMouseButtonReleasedImpl(int Button) override;

		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMousePosImpl() override;
	};
}