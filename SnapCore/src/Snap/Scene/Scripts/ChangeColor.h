#pragma once
#include <Snap/Scene/Scripts/CppScript.h>

#include <Snap/Scene/Comps/Components.h>
#include <Snap/Core/Input.h>

namespace SnapEngine
{
	class ChangeColorScript : public CppScript
	{
	public:
		virtual void Start() override
		{

		}

		virtual void Destroy() override
		{

		}

		virtual void Update(TimeStep Time) override
		{
			auto& Sprite = this->GetComponent<SpriteRendererComponent>();

			if (Input::IsKeyPressed(Key::Up))
				Sprite.m_Color.x += 0.01f;
			if (Input::IsKeyPressed(Key::Down))
				Sprite.m_Color.x -= 0.01f;
		}
	};
}