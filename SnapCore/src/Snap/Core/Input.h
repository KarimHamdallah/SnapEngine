#pragma once
#include <Snap/Core/key.h>

namespace SnapEngine
{
	class Input
	{
	public:
		static bool IsKeyPressed(Key KeyCode);
		static bool IsKeyReleased(Key KeyCode);
		static bool IsKeyDown(Key KeyCode);

		static bool IsMouseButtonPressed(MouseButton Button);
		static bool IsMouseButtonReleased(MouseButton Button);

		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePos();

		/*
	protected:
		virtual bool IsKeyPressedImpl(int KeyCode) = 0;
		virtual bool IsKeyReleasedImpl(int KeyCode) = 0;
		virtual bool IsKeyDownImpl(int KeyCode) = 0;

		virtual bool IsMouseButtonPressedImpl(int Button) = 0;
		virtual bool IsMouseButtonReleasedImpl(int Button) = 0;

		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
	private:
		static Input* s_Instance;
		*/
	};
}