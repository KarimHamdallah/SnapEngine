#include "SnapPCH.h"
#include "EditorCamera.h"

#include <Snap/Core/Input.h>

namespace SnapEngine
{
	EditorCamera::EditorCamera()
		: Camera(glm::perspective(m_Fov, m_AspectRatio, m_NearClip, m_FarClip))
	{
		CalculateView();
	}
	EditorCamera::EditorCamera(float Fov, float AspectRatio, float NearClip, float FarClip)
		: m_Fov(Fov), m_AspectRatio(AspectRatio), m_NearClip(NearClip), m_FarClip(FarClip),
		Camera(glm::perspective(m_Fov, m_AspectRatio, m_NearClip, m_FarClip))
	{
		CalculateView();
	}

	void EditorCamera::CalculateProjection()
	{
		m_AspectRatio = m_ViewPortWidth / m_ViewPortHeight;
		m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
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
				MouseZoom(mouse_delta.y);
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
		m_Zoom = 0.0f;
		CalculateView();
		return false;
	}
}