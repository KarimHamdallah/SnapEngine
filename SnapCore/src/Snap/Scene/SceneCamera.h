#pragma once
#include <Snap/Scene/Camera.h>

namespace SnapEngine
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, OrthoGraphic = 1 };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetViewPortSize(uint32_t Width, uint32_t Height);
		
		/////////////// OrthoGraphic ///////////////////
		
		void SetOrthoGraphic(float Size, float NearPlane, float FarPlane);

		float GetOrthoGraphicSize() const { return m_OrthoGraphicSize; }
		float GetOrthoGraphicNear() const { return m_OrthoGraphicNear; }
		float GetOrthoGraphicFar() const { return m_OrthoGraphicFar; }

		void SetOrthoGraphicSize(float size) { m_OrthoGraphicSize = size; CalculateProjection(); }
		void SetOrthoGraphicNear(float nearplane) { m_OrthoGraphicNear = nearplane; CalculateProjection(); }
		void SetOrthoGraphicFar(float farplane) { m_OrthoGraphicFar = farplane; CalculateProjection(); }

		/////////////// Perspective ////////////////////
		
		void SetPerspective(float fov, float NearPlane, float FarPlane);

		float GetPerspectiveFov() const { return m_PerspetiveFov; }
		float GetPerspectiveNear() const { return m_PerspectiveNear; }
		float GetPerspectiveFar() const { return m_PerspectiveFar; }

		void SetPerspectiveFov(float fov) { m_PerspetiveFov = fov; CalculateProjection(); }
		void SetPerspectiveNear(float nearplane) { m_PerspectiveNear = nearplane; CalculateProjection(); }
		void SetPerspectiveFar(float farplane) { m_PerspectiveFar = farplane; CalculateProjection(); }



		ProjectionType GetProjectionType() const { return m_Type; }
		void SetProjectionType(ProjectionType Type) { m_Type = Type; }

	protected:
		void CalculateProjection();
	protected:
		float m_OrthoGraphicSize = 10.0f;
		float m_OrthoGraphicNear = -1.0f, m_OrthoGraphicFar = 1.0f;

		float m_PerspetiveFov = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;

		float m_AspectRatio = 0.0f;
		ProjectionType m_Type;
	};
}