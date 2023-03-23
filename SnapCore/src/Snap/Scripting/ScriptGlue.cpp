#include "SnapPCH.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <glm/glm.hpp>
#include <Snap/Core/Input.h>
#include <Snap/Scene/Comps/Components.h>
#include <box2d/b2_body.h>

#define SNAP_ADD_INTERNAL_CALL(FuncName) mono_add_internal_call("SnapEngine.InternalCalls::" #FuncName, FuncName)


namespace Utils
{
	static b2BodyType Rigidbody2DTypeToBox2DType(SnapEngine::RigidBody2DComponent::RigidBodyType Type)
	{
		switch (Type)
		{
		case SnapEngine::RigidBody2DComponent::RigidBodyType::STATIC: return b2_staticBody;
		case SnapEngine::RigidBody2DComponent::RigidBodyType::DYNAMIC: return b2_dynamicBody;
		case SnapEngine::RigidBody2DComponent::RigidBodyType::KINEMATIC: return b2_kinematicBody;

			SNAP_ASSERT_MSG(false, "Unkown Type!");
			return b2_staticBody;
		}
	}

	std::string MonoStringToString(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		return str;
	}
}

namespace Scripting
{
	std::unordered_map< MonoType*, std::function<bool(SnapEngine::Entity)>> s_EntityHasComponentFunc;

	static bool IsKeyPressed(SnapEngine::Key key)
	{
		return SnapEngine::Input::IsKeyPressed(key);
	}

	static bool IsKeyReleased(SnapEngine::Key key)
	{
		return SnapEngine::Input::IsKeyReleased(key);
	}

	template<typename... T>
	static void SaveComponentMonoTypeWithItsHasComponentRelatedFunction()
	{
		([&]()
			{
				std::string TypenName = typeid(T).name();
				size_t pos = TypenName.find_last_of(":");
				std::string ComponentTypenName = TypenName.substr(pos + 1);
				std::string ManagedName = fmt::format("SnapEngine.{}", ComponentTypenName.c_str());

				// Get SnapEngine.ComponentTypenName Mono Type
				MonoType* ManagedType = mono_reflection_type_from_name(ManagedName.data(), ScriptingEngine::GetCoreAssemblyImage());

				// Save HasComponent Functions With MonoType* Key
				s_EntityHasComponentFunc[ManagedType] = [](SnapEngine::Entity entity) { return entity.HasComponent<T>(); };
			}(), ...);
	}

	template<typename... Component>
	static void SaveComponentMonoTypeWithItsHasComponentRelatedFunction(SnapEngine::ComponentGroup<Component...>)
	{
		SaveComponentMonoTypeWithItsHasComponentRelatedFunction<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		SaveComponentMonoTypeWithItsHasComponentRelatedFunction(SnapEngine::AllComponents{});
	}




	static bool Entity_HasComponent(SnapEngine::UUID EntityID, MonoReflectionType* ComponentType)
	{
		SnapEngine::Entity Entity = ScriptingEngine::GetSceneContext()->GetEntityWithUUID(EntityID);
		MonoType* ManagedType = mono_reflection_type_get_type(ComponentType);

		return s_EntityHasComponentFunc.at(ManagedType)(Entity);
	}

	static void Transform_GetPositionVec3(SnapEngine::UUID EntityID, glm::vec3* pos)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		*pos = e.GetComponent<SnapEngine::TransformComponent>().m_Position;
	}

	static void Transform_SetPositionVec3(uint64_t EntityID, glm::vec3* pos)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		e.GetComponent<SnapEngine::TransformComponent>().m_Position = *pos;
	}

	static float Snap_Sin(float value)
	{
		return glm::sin(value);
	}

	static void RigidBody2D_ApplyLinearImpulseToCenter(SnapEngine::UUID EntityID, glm::vec2* impulse, bool wake)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::RigidBody2DComponent>());
		b2Body* Body = (b2Body*)e.GetComponent<SnapEngine::RigidBody2DComponent>().RunTimeBody;
		Body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static uint64_t Entity_FindEntityByName(MonoString* EntityName)
	{
		char* cStr = mono_string_to_utf8(EntityName);
		std::string Name(cStr);

		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SNAP_ASSERT(scene);
		SnapEngine::Entity e = scene->FindEntityByName(Name);
		if (!e)
			return 0;
		
		mono_free(cStr);

		return e.GetComponent<SnapEngine::IDComponent>().ID;
	}

	static MonoObject* GetScriptInsatnce(SnapEngine::UUID EntityID)
	{
		return ScriptingEngine::GetManagedObject(EntityID);
	}

	static void RigidBody2D_GetLinearVelocity(SnapEngine::UUID EntityID, glm::vec2* OutLinearVelocity)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::RigidBody2DComponent>());
		b2Body* Body = (b2Body*)e.GetComponent<SnapEngine::RigidBody2DComponent>().RunTimeBody;
		b2Vec2 Linearvelocity = Body->GetLinearVelocity();
		*OutLinearVelocity = { Linearvelocity.x, Linearvelocity.y };
	}

	static void RigidBody2D_GetBodyType(SnapEngine::UUID EntityID, SnapEngine::RigidBody2DComponent::RigidBodyType* OutBodyType)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::RigidBody2DComponent>());
		SnapEngine::RigidBody2DComponent::RigidBodyType Bodytype = e.GetComponent<SnapEngine::RigidBody2DComponent>().m_Type;
		*OutBodyType = Bodytype;
	}

	static void RigidBody2D_SetBodyType(SnapEngine::UUID EntityID, SnapEngine::RigidBody2DComponent::RigidBodyType* OutBodyType)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::RigidBody2DComponent>());
		e.GetComponent<SnapEngine::RigidBody2DComponent>().m_Type = *OutBodyType;
		b2Body* Body = (b2Body*)e.GetComponent<SnapEngine::RigidBody2DComponent>().RunTimeBody;
		Body->SetType(Utils::Rigidbody2DTypeToBox2DType(*OutBodyType));
	}


	static void TextComponent_SetText(SnapEngine::UUID EntityID, MonoString* textString)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		e.GetComponent<SnapEngine::TextRendererComponent>().m_TextString = Utils::MonoStringToString(textString);
	}

	static MonoString* TextComponent_GetText(SnapEngine::UUID EntityID)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());
		
		return mono_string_new(ScriptingEngine::GetAppDomain(), e.GetComponent<SnapEngine::TextRendererComponent>().m_TextString.c_str());
	}

	static void TextComponent_GetColor(SnapEngine::UUID EntityID, glm::vec4* Color)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		*Color = e.GetComponent<SnapEngine::TextRendererComponent>().m_Color;
	}

	static void TextComponent_SetColor(SnapEngine::UUID EntityID, glm::vec4* Color)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		e.GetComponent<SnapEngine::TextRendererComponent>().m_Color = *Color;
	}

	static void TextComponent_GetKerningOffset(SnapEngine::UUID EntityID, float* KerningOffset)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		*KerningOffset = e.GetComponent<SnapEngine::TextRendererComponent>().KerningOffset;
	}

	static void TextComponent_SetKerningOffset(SnapEngine::UUID EntityID, float* KerningOffset)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		e.GetComponent<SnapEngine::TextRendererComponent>().KerningOffset = *KerningOffset;
	}

	static void TextComponent_GetLineSpacing(SnapEngine::UUID EntityID, float* LineSpacing)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		*LineSpacing = e.GetComponent<SnapEngine::TextRendererComponent>().LineSpacing;
	}

	static void TextComponent_SetLineSpacing(SnapEngine::UUID EntityID, float* LineSpacing)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		SNAP_ASSERT(e);
		SNAP_ASSERT(e.HasComponent<SnapEngine::TextRendererComponent>());

		e.GetComponent<SnapEngine::TextRendererComponent>().LineSpacing = *LineSpacing;
	}

	void ScriptGlue::RegisterGlue()
	{
		// Internal Calls
		
		// Input Internal Calls
		SNAP_ADD_INTERNAL_CALL(IsKeyPressed);
		SNAP_ADD_INTERNAL_CALL(IsKeyReleased);

		// Entity Internal Calls
		SNAP_ADD_INTERNAL_CALL(Entity_HasComponent);
		SNAP_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		SNAP_ADD_INTERNAL_CALL(GetScriptInsatnce);

		SNAP_ADD_INTERNAL_CALL(Transform_GetPositionVec3);
		SNAP_ADD_INTERNAL_CALL(Transform_SetPositionVec3);

		SNAP_ADD_INTERNAL_CALL(RigidBody2D_ApplyLinearImpulseToCenter);
		SNAP_ADD_INTERNAL_CALL(RigidBody2D_GetLinearVelocity);

		SNAP_ADD_INTERNAL_CALL(RigidBody2D_GetBodyType);
		SNAP_ADD_INTERNAL_CALL(RigidBody2D_SetBodyType);

		// Text Component
		SNAP_ADD_INTERNAL_CALL(TextComponent_GetText);
		SNAP_ADD_INTERNAL_CALL(TextComponent_SetText);

		SNAP_ADD_INTERNAL_CALL(TextComponent_GetColor);
		SNAP_ADD_INTERNAL_CALL(TextComponent_SetColor);

		SNAP_ADD_INTERNAL_CALL(TextComponent_GetKerningOffset);
		SNAP_ADD_INTERNAL_CALL(TextComponent_SetKerningOffset);

		SNAP_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		SNAP_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

		SNAP_ADD_INTERNAL_CALL(Snap_Sin);
	}
}