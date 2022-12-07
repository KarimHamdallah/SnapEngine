#include "SnapPCH.h"
#include "SceneCamera.h"

#include <glm/gtx/transform.hpp>

namespace SnapEngine
{
	SceneCamera::SceneCamera()
		: m_Type(ProjectionType::OrthoGraphic)
	{
		CalculateProjection();
	}

	void SceneCamera::SetOrthoGraphic(float Size, float NearPlane, float FarPlane)
	{
		m_Type = ProjectionType::OrthoGraphic;
		m_OrthoGraphicSize = Size;
		m_OrthoGraphicNear = NearPlane;
		m_OrthoGraphicFar = FarPlane;

		CalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float NearPlane, float FarPlane)
	{
		m_Type = ProjectionType::Perspective;
		m_PerspetiveFov = fov;
		m_PerspectiveNear = NearPlane;
		m_PerspectiveFar = FarPlane;

		CalculateProjection();
	}

	void SceneCamera::SetViewPortSize(uint32_t Width, uint32_t Height)
	{
		m_AspectRatio = (float)Width / (float)Height;
		CalculateProjection();
	}

	void SceneCamera::CalculateProjection()
	{
		if (m_Type == ProjectionType::OrthoGraphic)
		{
			float left = -m_OrthoGraphicSize * m_AspectRatio * 0.5f;
			float right = m_OrthoGraphicSize * m_AspectRatio * 0.5f;
			float top = m_OrthoGraphicSize * 0.5f;
			float bottom = -m_OrthoGraphicSize * 0.5f;

			m_Projection = glm::ortho(left, right, bottom, top, m_OrthoGraphicNear, m_OrthoGraphicFar);
		}
		else
		{
			m_Projection = glm::perspective(m_PerspetiveFov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
	}
}