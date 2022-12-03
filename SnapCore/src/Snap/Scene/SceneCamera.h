#pragma once
#include <Snap/Scene/Camera.h>

namespace SnapEngine
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthoGraphic(float Size, float NearPlane, float FarPlane);
		void SetViewPortSize(uint32_t Width, uint32_t Height);

		float GetOrthoGraphicSize() const { return m_OrthoGraphicSize; }
		float GetOrthoGraphicNear() const { return m_OrthoGraphicNear; }
		float GetOrthoGraphicFar() const { return m_OrthoGraphicFar; }

		void SetOrthoGraphicSize(float size) { m_OrthoGraphicSize = size; CalculateProjection(); }

	private:
		void CalculateProjection();
	private:
		float m_OrthoGraphicSize = 10.0f;
		float m_OrthoGraphicNear = -1.0f, m_OrthoGraphicFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}