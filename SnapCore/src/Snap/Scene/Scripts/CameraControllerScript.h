#pragma once
#include <Snap/Scene/Scripts/CppScript.h>
#include <Snap/Scene/Comps/Components.h>

#include <Snap/Core/Input.h>


namespace SnapEngine
{
	class CameraControllerScript : public CppScript
	{
	public:

		float Speed = 10.0f;

		virtual void Start() override
		{

		}

		virtual void Destroy() override
		{

		}

		virtual void Update(TimeStep Time) override
		{
			auto& Transform = this->GetComponent<TransformComponent>().m_Transform;

			if(Input::IsKeyPressed(Key::W))
				Transform[3][1] += Speed * Time;
			if (Input::IsKeyPressed(Key::S))
				Transform[3][1] -= Speed * Time;
			if (Input::IsKeyPressed(Key::D))
				Transform[3][0] += Speed * Time;
			if (Input::IsKeyPressed(Key::A))
				Transform[3][0] -= Speed * Time;

		}
	};
}