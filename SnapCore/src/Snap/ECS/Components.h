#pragma once
#include <Snap/ECS/Component.h>
#include <glm/glm.hpp>

struct TransformComponent : public Component
{
public:
	TransformComponent() = default;
	TransformComponent(const glm::vec3& Position, const glm::vec3& Rotation, const glm::vec3& Scale)
		: m_Position(Position), m_Rotation(Rotation), m_Scale(Scale)
	{}

	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale = { 0.0f, 0.0f, 0.0f };
};

struct SpriteRendererComponent : public Component
{
public:
	SpriteRendererComponent() = default;
	SpriteRendererComponent(glm::vec4 Color = glm::vec4(1.0f))
		: m_Color(Color)
	{}

	glm::vec4 m_Color;
};

struct TagComponent : public Component
{
	TagComponent() = default;
	TagComponent(const std::string& Tag)
		: m_Tag(Tag)
	{}

	std::string m_Tag = "";
};