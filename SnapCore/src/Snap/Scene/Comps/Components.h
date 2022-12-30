#pragma once
#include <glm/glm.hpp>
#include <Snap/Core/UUID.h>
#include <glm/gtx/transform.hpp>
#include <Snap/Scene/SceneCamera.h>
#include <Snap/Scene/Scripts/CppScript.h>
#include <Snap/Renderer/Texture.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace SnapEngine
{
	struct IDComponent
	{
		IDComponent() = default;
		IDComponent(UUID uuid) : ID(uuid) {}
		IDComponent(const IDComponent&) = default;

		UUID ID;
	};

	struct TransformComponent
	{
	public:
		TransformComponent(const glm::vec3& Position = glm::vec3(0.0f), const glm::vec3& Scale = glm::vec3(1.0f), const glm::vec3& Rotation = glm::vec3(0.0f))
			: m_Position(Position), m_Rotation(Rotation), m_Scale(Scale)
		{}

		TransformComponent(const TransformComponent& other) = default;

		operator const glm::mat4() const
		{ 
			return GetTransformMatrix();
		}

		glm::mat4 GetTransformMatrix() const
		{
			/*
			glm::mat4 Transform = glm::mat4(1.0f);
			Transform = glm::translate(Transform, m_Position);
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			Transform = glm::scale(Transform, m_Scale);
			*/
			return glm::translate(glm::mat4(1.0f), m_Position) *
				glm::toMat4(glm::quat(glm::vec3(glm::radians(m_Rotation.x), glm::radians(m_Rotation.y), glm::radians(m_Rotation.z)))) *
				glm::scale(glm::mat4(1.0f), m_Scale);
		}

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
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
		SpriteRendererComponent(const SnapPtr<Texture2D>& Tex, const glm::vec4& Color = glm::vec4(1.0f)) : m_Texture(Tex), m_Color(Color) {}
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		operator const glm::vec4& () { return m_Color; }


		glm::vec4 m_Color;
		SnapPtr<Texture2D> m_Texture = nullptr;
		float m_TilingFactor = 1.0f;
	};


	struct CameraComponent
	{
	public:
		CameraComponent(float Size = 10.0f, float NearClipPlane = -100.0f, float FarClipPlane = 100.0f)
		{
			m_Camera.SetOrthoGraphic(Size, NearClipPlane, FarClipPlane);
		}
		CameraComponent(const CameraComponent&) = default;

		SceneCamera m_Camera;
		bool m_IsMain = false;
		bool m_FixedAspectRatio = false;
	};

	struct CppScriptComponent
	{
		CppScript* m_Instance = nullptr;

		// Function Ptrs
		CppScript* (*InstantiateScriptFuncPtr)();
		void(*DestroyScript)(CppScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScriptFuncPtr = []() { return static_cast<CppScript*>(new T()); };
			DestroyScript = [](CppScriptComponent* cppSc) { delete cppSc->m_Instance; cppSc->m_Instance = nullptr; };
		}
	};

	struct RigidBody2DComponent
	{
		enum class RigidBodyType { STATIC = 0, DYNAMIC, KINEMATIC };
		
		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;

		RigidBodyType m_Type = RigidBodyType::STATIC;
		bool m_FixedRotation = false;

		void* RunTimeBody = nullptr;
	};

	struct BoxCollider2DComponent
	{
		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		glm::vec2 m_Size = { 0.5f, 0.5f };
		glm::vec2 m_Offset = { 0.0f, 0.0f };

		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.5f;
		float m_RestitutionThreshold = 0.5f;

		void* RunTimeFixture= nullptr;
	};
}