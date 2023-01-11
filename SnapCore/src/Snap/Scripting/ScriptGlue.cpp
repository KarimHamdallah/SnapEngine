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

namespace Scripting
{
	std::unordered_map< MonoType*, std::function<bool(SnapEngine::Entity)>> s_EntityHasComponentFunc;

	static bool IsKeyPressed(SnapEngine::Key key)
	{
		return SnapEngine::Input::IsKeyPressed(key);
	}

	template<typename T>
	static void SaveComponentMonoTypeWithItsHasComponentRelatedFunction()
	{
		std::string TypenName = typeid(T).name();
		size_t pos = TypenName.find_last_of(":");
		std::string ComponentTypenName = TypenName.substr(pos + 1);
		std::string ManagedName = fmt::format("SnapEngine.{}", ComponentTypenName.c_str());
		
		// Get SnapEngine.ComponentTypenName Mono Type
		MonoType* ManagedType = mono_reflection_type_from_name(ManagedName.data(), ScriptingEngine::GetCoreAssemblyImage());

		// Save HasComponent Functions With MonoType* Key
		s_EntityHasComponentFunc[ManagedType] = [](SnapEngine::Entity entity) { return entity.HasComponent<T>(); };
	}

	void ScriptGlue::RegisterComponents()
	{
		SaveComponentMonoTypeWithItsHasComponentRelatedFunction<SnapEngine::TransformComponent>();
		SaveComponentMonoTypeWithItsHasComponentRelatedFunction<SnapEngine::RigidBody2DComponent>();
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

	static void RigidBody_ApplyLinearImpulseToCenter(SnapEngine::UUID EntityID, glm::vec2* impulse, bool wake)
	{
		SnapEngine::Scene* scene = ScriptingEngine::GetSceneContext();
		SnapEngine::Entity e = scene->GetEntityWithUUID(EntityID);
		b2Body* Body = (b2Body*)e.GetComponent<SnapEngine::RigidBody2DComponent>().RunTimeBody;
		Body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	void ScriptGlue::RegisterGlue()
	{
		// Internal Calls
		
		// Input Internal Calls
		SNAP_ADD_INTERNAL_CALL(IsKeyPressed);

		// Entity Internal Calls
		SNAP_ADD_INTERNAL_CALL(Entity_HasComponent);

		SNAP_ADD_INTERNAL_CALL(Transform_GetPositionVec3);
		SNAP_ADD_INTERNAL_CALL(Transform_SetPositionVec3);

		SNAP_ADD_INTERNAL_CALL(RigidBody_ApplyLinearImpulseToCenter);

		SNAP_ADD_INTERNAL_CALL(Snap_Sin);
	}
}