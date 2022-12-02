#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <Snap/Scene/Camera.h>

namespace SnapEngine
{
	struct TransformComponent
	{
	public:
		TransformComponent(const glm::vec3& Position = glm::vec3(0.0f), const glm::vec3& Scale = glm::vec3(1.0f), float Roation = 0.0f)
		{
			m_Transform = glm::translate(glm::mat4(1.0f), Position);
			m_Transform = glm::rotate(glm::mat4(1.0f), glm::radians(Roation), glm::vec3(0.0f, 0.0f, 1.0f));
			m_Transform = glm::scale(glm::mat4(1.0f), Scale);
		}

		TransformComponent(const TransformComponent& other) { m_Transform = other.m_Transform; }
		TransformComponent(const glm::mat4& Transform) : m_Transform(Transform) {}

		operator const glm::mat4& () const { return m_Transform; }


		glm::mat4 m_Transform = glm::mat4(1.0f);
	};

	struct TagComponent
	{
	public:
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : m_Tag(tag) {}

		std::string m_Tag;
	};



	struct SpriteRendererComponent
	{
	public:
		SpriteRendererComponent(const glm::vec4& Color = glm::vec4(1.0f)) : m_Color(Color) {}
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		operator const glm::vec4& () { return m_Color; }


		glm::vec4 m_Color;
	};


	struct CameraComponent
	{
	public:
		CameraComponent(const glm::mat4& Projection) : m_Camera(Projection) {}
		CameraComponent(const CameraComponent&) = default;

		Camera m_Camera;
		bool m_IsMain = false;
	};
}