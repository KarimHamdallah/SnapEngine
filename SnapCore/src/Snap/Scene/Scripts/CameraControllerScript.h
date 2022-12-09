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
			// Zoom
			if(HasComponent<CameraComponent>())
			{
				auto& Cam = this->GetComponent<CameraComponent>();
				float OrthoSize = m_Zoom + Cam.m_Camera.GetOrthoGraphicSize();
				OrthoSize = std::clamp(OrthoSize, 1.0f, 100.0f);
				m_Zoom = 0.0f;
				Cam.m_Camera.SetOrthoGraphicSize(OrthoSize);
			}
			
			// Movement
			if(HasComponent<TransformComponent>())
			{
				auto& transform = this->GetComponent<TransformComponent>();
				if (Input::IsKeyPressed(Key::W))
					transform.m_Position.y += Speed * Time;
				if (Input::IsKeyPressed(Key::S))
					transform.m_Position.y -= Speed * Time;
				if (Input::IsKeyPressed(Key::D))
					transform.m_Position.x += Speed * Time;
				if (Input::IsKeyPressed(Key::A))
					transform.m_Position.x -= Speed * Time;
				if (Input::IsKeyPressed(Key::Q))
					transform.m_Rotation.z -= Speed * Time * 5.0f;
				if (Input::IsKeyPressed(Key::E))
					transform.m_Rotation.z += Speed * Time * 5.0f;
			}
		}

		virtual void ProcessEvents(IEvent& e) override
		{
			EventDispatcher dispatcher(e);
			dispatcher.DispatchEvent<MouseScrollEvent>(SNAP_BIND_FUNCTION(CameraControllerScript::OnMouseScroll));
		}



	private:
		bool OnMouseScroll(MouseScrollEvent& e)
		{
			m_Zoom += e.GetOffsetY() * - 0.5f;
			return false;
		}

		float Clampf(float value, float min, float max)
		{
			if (value < min)
				return min;
			else if (value > max)
				return max;
			return value;
		}


	private:
		float m_Zoom = 1.0f;
	};
}