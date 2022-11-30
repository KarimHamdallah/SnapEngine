#pragma once
#include <Snap/Core/key.h>

namespace SnapEngine
{
	class Input
	{
	public:
		inline static bool IsKeyPressed(Key KeyCode) { return s_Instance->IsKeyPressedImpl((int)KeyCode); }
		inline static bool IsKeyReleased(Key KeyCode) { return s_Instance->IsKeyReleasedImpl((int)KeyCode); }
		inline static bool IsKeyDown(Key KeyCode) { return s_Instance->IsKeyDownImpl((int)KeyCode); }

		inline static bool IsMouseButtonPressed(MouseButton Button) { return s_Instance->IsMouseButtonPressedImpl((int)Button); }
		inline static bool IsMouseButtonReleased(MouseButton Button) { return s_Instance->IsMouseButtonReleasedImpl((int)Button); }

		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static std::pair<float, float> GetMousePos() { return std::pair<float, float>(1.0f, 1.0f); }

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
	};
}