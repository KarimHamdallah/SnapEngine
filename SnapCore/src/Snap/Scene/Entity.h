#pragma once
#include <entt.hpp>
#include <Snap/Scene/Scene.h>
#include <Snap/Core/asserts.h>

namespace SnapEngine
{
	class Entity
	{
	public:
		using Handel = entt::entity;

		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(Handel handel, Scene* scene);


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			SNAP_ASSERT_MSG(!HasComponent<T>(), "Component already registerd!");
			T& Comp = mScene->registry.emplace<T>(entity, std::forward<Args>(args)...);
			mScene->OnComponentAdded<T>(*this, Comp);
			return Comp;
		}
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& Comp = mScene->registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
			mScene->OnComponentAdded<T>(*this, Comp);
			return Comp;
		}

		template<typename T>
		void RemoveComponent()
		{
			SNAP_ASSERT_MSG(HasComponent<T>(), "Component already not registerd!");
			mScene->registry.remove<T>(entity);
		}

		template<typename T>
		T& GetComponent()
		{
			SNAP_ASSERT_MSG(HasComponent<T>(), "Can't get component...Component not registerd!");
			return mScene->registry.get<T>(entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return mScene->registry.all_of<T>(entity);
		}

		inline uint32_t GetID() { return static_cast<uint32_t>(entity); }

		explicit operator bool() const;
		operator Handel () const;
		operator uint32_t () const;

		bool operator == (const Entity& other) const;
		bool operator != (const Entity& other) const;

	private:
		Handel entity = entt::null;
		Scene* mScene = nullptr;
	};
}