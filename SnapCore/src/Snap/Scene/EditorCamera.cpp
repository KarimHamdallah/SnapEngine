/*
namespace SnapEngine
{
	EditorCamera::EditorCamera()
		: Camera(glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip))
	{
		CalculateView();
	}
	EditorCamera::EditorCamera(float Fov, float AspectRatio, float NearClip, float FarClip)
		: m_Fov(Fov), m_AspectRatio(AspectRatio), m_NearClip(NearClip), m_FarClip(FarClip),
		Camera(glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip))
	{
		CalculateView();
	}

	void EditorCamera::CalculateProjection()
	{
		m_AspectRatio = m_ViewPortWidth / m_ViewPortHeight;
		m_Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void EditorCamera::CalculateView()
	{
		// m_Yaw = 0.0f; m_Pitch = 0.0f; // Lock camera's rotation 
		m_Position = CalculatePosition(); // Current Pos After Update
		glm::quat Orientation = GetOrientation(); // Current Orientation
		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(Orientation));
	}
	
	void EditorCamera::Update(TimeStep Time)
	{
		if (Input::IsKeyPressed(Key::LeftControl))
		{
			// Mouse delta
			const glm::vec2& mouse_pos = { Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 mouse_delta = (mouse_pos - m_PreviouseMousePos) * 0.003f;
			m_PreviouseMousePos = mouse_pos;

			if (Input::IsMouseButtonPressed(MouseButton::MouseButtonLeft))
				MouseRotate(mouse_delta);
			if (Input::IsMouseButtonPressed(MouseButton::MouseButtonRight))
				MouseZoom(mouse_delta.y * 10.0f);
			if (Input::IsMouseButtonPressed(MouseButton::MouseButtonMiddle))
				MousePan(mouse_delta);
		}

		CalculateView();
	}

	void EditorCamera::ProcessEvent(IEvent& e)
	{
		EventDispatcher dispatacher(e);
		dispatacher.DispatchEvent<MouseScrollEvent>(SNAP_BIND_FUNCTION(EditorCamera::OnMouseScrollEvent));
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewPortWidth / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewPortHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f); // min dist 0.0f
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed 100.0f
		return speed;
	}

	void EditorCamera::MousePan(const glm::vec2& mouse_delta)
	{
		auto& [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * mouse_delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * mouse_delta.y * ySpeed * m_Distance;

	}

	void EditorCamera::MouseRotate(const glm::vec2& mouse_delta)
	{
		float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += YawSign * mouse_delta.x * RotationSpeed();
		m_Pitch += mouse_delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float mouse_delta_y)
	{
		m_Distance -= mouse_delta_y * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	bool EditorCamera::OnMouseScrollEvent(MouseScrollEvent& e)
	{
		m_Zoom = -e.GetOffsetY() * m_ScrollSpeed;
		MouseZoom(m_Zoom);
		CalculateView();
		return false;
	}
}
*/
#include "SnapPCH.h"
#include "EditorCamera.h"

#include <Snap/Core/Input.h>

namespace SnapEngine
{

	static bool FirstMouse = true;

	void EditorCamera::UpdateCamera(TimeStep Time)
	{
		// mouse movement
		if (Input::IsKeyPressed(Key::LeftShift)) // TODO:: May Move This To Event Process Functon
		{
			m_IsActive = true;

			auto [mouseX, mouseY] = Input::GetMousePos();

			if (FirstMouse)
			{
				m_PreviouseMousePos = { mouseX, mouseY };
				FirstMouse = false;
			}

			glm::vec2 mouse_delta = { mouseX - m_PreviouseMousePos.x, mouseY - m_PreviouseMousePos.y };
			m_PreviouseMousePos = { mouseX, mouseY };

			float xoffset = mouse_delta.x * sensitivity * Time;
			float yoffset = -mouse_delta.y * sensitivity * Time;

			orientation.y += xoffset;
			orientation.x += yoffset;

			glm::clamp(orientation.x, -89.0f, 89.0f);


			// keyboard movement
			if (Input::IsKeyPressed(Key::W))
				position += forward * (movementspeed * Time);
			if (Input::IsKeyPressed(Key::S))
				position -= forward * (movementspeed * Time);
			if (Input::IsKeyPressed(Key::D))
				position += right * (movementspeed * Time);
			if (Input::IsKeyPressed(Key::A))
				position -= right * (movementspeed * Time);
			if (Input::IsKeyPressed(Key::E))
				position += up * (movementspeed * Time);
			if (Input::IsKeyPressed(Key::Q))
				position -= up * (movementspeed * Time);
		}
		else
		{
			FirstMouse = true;
			m_IsActive = false;
		}

		UpdateCameraVectors();
	}

	void EditorCamera::UpdateCameraVectors()
	{
		// calculate direction from orientation
		glm::vec3 direction;

		direction.x = cos(glm::radians(orientation.y)) * cos(glm::radians(orientation.x));
		direction.y = sin(glm::radians(orientation.x));
		direction.z = sin(glm::radians(orientation.y)) * cos(glm::radians(orientation.x));

		// calculate camera vectors
		forward = glm::normalize(direction);
		right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
		up = glm::cross(right, forward);
	}

	void EditorCamera::CalculateProjection()
	{
		m_AspectRatio = m_ViewPortWidth / m_ViewPortHeight;
		m_Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::ProcessEvents(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<MouseScrollEvent>(SNAP_BIND_FUNCTION(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrollEvent& e)
	{
		m_Zoom = e.GetOffsetY() * -0.1f;
		m_Fov += m_Zoom * m_ZoomSpeed;
		m_Zoom = 0.0f;

		m_Fov = std::clamp(m_Fov, 1.0f, 45.0f);

		CalculateProjection();
		
		return false;
	}
}