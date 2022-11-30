#pragma once
#include <glm/glm.hpp>

namespace SnapEngine
{
	class OrthoGraphicsCamera
	{
	public:
		OrthoGraphicsCamera(float right, float left, float top, float bottom, float nearplane, float farplane);

		void SetProjection(float right, float left, float top, float bottom, float nearplane, float farplane);

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& Position) { m_Position = Position; m_RequireUpdate = true; }

		inline float GetRotation() const { return m_Rotation; }
		inline void SetRotation(float Rotation) { m_Rotation = Rotation; m_RequireUpdate = true; }
		
		inline void Update() { if (m_RequireUpdate) { UpdateCameraViewMatrix(); m_RequireUpdate = false; } }

		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionViewMatrix() const { return m_ProjectionViewMatrix; }

	private:
		void UpdateCameraViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionViewMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Rotation = 0;

		bool m_RequireUpdate = false;
	};
}