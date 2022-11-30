#pragma once
#include <Snap/Renderer/OrthoGraphicCamera.h>

#include <Snap/Events/WindowEvent.h>
#include <Snap/Events/MouseEvent.h>

#include <Snap/Core/TimeStep.h>

namespace SnapEngine
{
	class OrthoGraphicCameraController
	{
	public:
		OrthoGraphicCameraController(float AspectRatio, bool rotation = false);
		OrthoGraphicCameraController(float Width, float Height, bool rotation = false);

		void Update(TimeStep time);
		void ProcessEvent(IEvent& e);
		inline OrthoGraphicsCamera& GetCamera() { return m_Camera; }
		inline const OrthoGraphicsCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrollEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoGraphicsCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = glm::vec3(0.0f);
		float m_CameraRotaion = 0.0f;
		float m_CamSpeed = 1.0f;
		float m_CamRotationSpeed = 20.0f;
	};
}