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
			auto& transform = this->GetComponent<TransformComponent>().m_Transform;

			/*
			if(Input::IsKeyPressed(Key::W))
				transform.m_Position.y += Speed * Time;
			if (Input::IsKeyPressed(Key::S))
				transform.m_Position.y -= Speed * Time;
			if (Input::IsKeyPressed(Key::D))
				transform.m_Position.x += Speed * Time;
			if (Input::IsKeyPressed(Key::A))
				transform.m_Position.x -= Speed * Time;
			*/

			if (Input::IsKeyPressed(Key::W))
				transform[3][1] += Speed * Time;
			if (Input::IsKeyPressed(Key::S))
				transform[3][1] -= Speed * Time;
			if (Input::IsKeyPressed(Key::D))
				transform[3][0] += Speed * Time;
			if (Input::IsKeyPressed(Key::A))
				transform[3][0] -= Speed * Time;
		}
	};
}