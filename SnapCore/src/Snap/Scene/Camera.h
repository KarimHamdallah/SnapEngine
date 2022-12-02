#pragma once
#include <glm/glm.hpp>

namespace SnapEngine
{
	class Camera
	{
	public:
		Camera(const glm::mat4& Projection) : m_Projection(Projection) {}

		const glm::mat4& GetProjectionMatrix() const { return m_Projection; }

	private:
		glm::mat4 m_Projection;
	};
}