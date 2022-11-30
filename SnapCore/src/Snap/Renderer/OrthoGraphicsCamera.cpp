#include "SnapPCH.h"
#include "OrthoGraphicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SnapEngine
{
	OrthoGraphicsCamera::OrthoGraphicsCamera(float right, float left, float top, float bottom, float nearplane, float farplane)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearplane, farplane);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoGraphicsCamera::SetProjection(float right, float left, float top, float bottom, float nearplane, float farplane)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearplane, farplane);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthoGraphicsCamera::UpdateCameraViewMatrix()
	{
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewMatrix = glm::inverse(Transform);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}