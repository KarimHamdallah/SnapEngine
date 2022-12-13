#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <Snap/Scene/SceneCamera.h>
#include <Snap/Scene/Scripts/CppScript.h>

namespace SnapEngine
{
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
			glm::mat4 Transform = glm::mat4(1.0f);
			Transform = glm::translate(Transform, m_Position);
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			Transform = glm::rotate(Transform, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			Transform = glm::scale(Transform, m_Scale);

			return Transform;
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
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		operator const glm::vec4& () { return m_Color; }


		glm::vec4 m_Color;
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
}