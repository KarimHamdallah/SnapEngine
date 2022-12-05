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
		CameraComponent(float Size, float NearClipPlane, float FarClipPlane)
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