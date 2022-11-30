#include "SnapPCH.h"
#include "OrthoGraphicCameraController.h"

#include <Snap/Core/Input.h>

static float dt = 0.0f;

namespace SnapEngine
{
	OrthoGraphicCameraController::OrthoGraphicCameraController(float AspectRatio, bool rotation)
		: m_AspectRatio(AspectRatio), m_Camera(AspectRatio* m_ZoomLevel, -AspectRatio * m_ZoomLevel, m_ZoomLevel, -m_ZoomLevel, -1.0f, 1.0f),
		  m_Rotation(rotation)
	{}

	OrthoGraphicCameraController::OrthoGraphicCameraController(float Width, float Height, bool rotation)
		: m_AspectRatio(Width / Height), m_Camera(m_AspectRatio* m_ZoomLevel, -m_AspectRatio * m_ZoomLevel, m_ZoomLevel, -m_ZoomLevel, -1.0f, 1.0f),
		m_Rotation(rotation)
	{}

	void OrthoGraphicCameraController::Update(TimeStep time)
	{
		dt = time;

		if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::D))
			m_CameraPosition.x += m_CamSpeed * dt;
		else if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::A))
			m_CameraPosition.x -= m_CamSpeed * dt;
		if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::W))
			m_CameraPosition.y += m_CamSpeed * dt;
		else if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::S))
			m_CameraPosition.y -= m_CamSpeed * dt;
		
		m_Camera.SetPosition(m_CameraPosition);

		if (m_Rotation)
		{
			if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::E))
				m_CameraRotaion -= m_CamRotationSpeed * dt;
			else if (SnapEngine::Input::IsKeyPressed(SnapEngine::Key::Q))
				m_CameraRotaion += m_CamRotationSpeed * dt;
			
			m_Camera.SetRotation(m_CameraRotaion);
		}

		m_Camera.Update();
	}

	void OrthoGraphicCameraController::ProcessEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<MouseScrollEvent>(SNAP_BIND_FUNCTION(OrthoGraphicCameraController::OnMouseScrolled));
		dispatcher.DispatchEvent<WindowResizeEvent>(SNAP_BIND_FUNCTION(OrthoGraphicCameraController::OnWindowResized));
	}

	bool OrthoGraphicCameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		m_ZoomLevel -= e.GetOffsetY() * 0.25f;
		
		m_ZoomLevel = std::max(m_ZoomLevel, 0.1f);

		m_Camera.SetProjection(m_AspectRatio * m_ZoomLevel, -m_AspectRatio * m_ZoomLevel, m_ZoomLevel, -m_ZoomLevel, -1.0f, 1.0f);
		return false;
	}

	bool OrthoGraphicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(m_AspectRatio * m_ZoomLevel, -m_AspectRatio * m_ZoomLevel, m_ZoomLevel, -m_ZoomLevel, -1.0f, 1.0f);
		return false;
	}
}