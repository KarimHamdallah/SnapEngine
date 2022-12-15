#pragma once

#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Snap/Core/TimeStep.h>
#include <Snap/Events/Event.h>
#include <Snap/Events/MouseEvent.h>

namespace SnapEngine
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera();
		EditorCamera(float Fov, float AspectRatio, float NearClip, float FarClip);
		virtual ~EditorCamera() {}

		void Update(TimeStep Time);
		void ProcessEvent(IEvent& e);

		float GetDistance() const { return m_Distance; }
		void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewPortSize(float Width, float Height) { m_ViewPortWidth = Width; m_ViewPortHeight = Height; CalculateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;

		inline const glm::vec3& GetPosition() const { return m_Position; }

		glm::vec3 CalculatePosition() const;
		glm::quat GetOrientation() const;

		inline float GetPitch() const { return m_Pitch; }
		inline float GetYaw() const { return m_Yaw; }

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		void MousePan(const glm::vec2& mouse_delta);
		void MouseRotate(const glm::vec2& mouse_delta);
		void MouseZoom(float mouse_delta_y);

	private:
		bool OnMouseScrollEvent(MouseScrollEvent& e);

	private:
		void CalculateProjection();
		void CalculateView();

	private:
		// ViewPort
		float m_ViewPortWidth = 1280.0f;
		float m_ViewPortHeight = 720.0f;
		// Matrices
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		// Distance
		float m_Distance = -10.0f;
		// Zoom
		float m_Zoom = 1.0f;
		float m_ScrollSpeed = 1.0f;

		// Movement
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;

		float m_Speed = 1.0f;
		glm::vec2 m_PreviouseMousePos = { 0.0f, 0.0f };

		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		// Camera
		float m_Fov = 45.0f;
		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
	};
}