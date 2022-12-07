#include "SnapPCH.h"
#include "Scene.h"
#include <Snap/Scene/Entity.h>

#include <Snap/Scene/Comps/Components.h>
#include <Snap/Renderer/RendererCommand.h>
#include <Snap/Renderer/Renderer2D.h>

namespace SnapEngine
{
    Entity& Scene::CreatEntity(
        const std::string& name,
        const glm::vec3& Position,
        const glm::vec3& Scale,
        const glm::vec3& Rotation
    )
    {
        Entity* entity = new Entity(registry.create(), this);
        entity->AddComponent<TransformComponent>(Position, Scale, Rotation);
        entity->AddComponent<TagComponent>(name);
        entity->GetComponent<TagComponent>().m_Tag.empty() ? std::string("Entity_") + std::to_string(EntityCounter++) : name;
        Entities[name] = SnapPtr<Entity>(entity);
        return *entity;
    }

    Entity& Scene::GetEntity(const std::string& name)
    {
        return *Entities[name];
    }

    void Scene::DestroyEntity(Entity& entity)
    {
        auto it = Entities.find(entity.GetComponent<TagComponent>().m_Tag);
        Entities.erase(it);
        registry.destroy(entity);
    }

    void Scene::Update(const TimeStep& Time)
    {
        { // Update Scripts
            registry.view<CppScriptComponent>().each([=](auto entity, auto& cppSc)
                {
                    if (!cppSc.m_Instance)
                    {
                        cppSc.m_Instance = cppSc.InstantiateScriptFuncPtr();
                        cppSc.m_Instance->m_Entity = Entity{ entity, this };

                        cppSc.m_Instance->Start();
                    }

                    cppSc.m_Instance->Update(Time);
                });
        }
    }

    void Scene::Render()
    {
        SceneCamera* MainCamera = nullptr;
        glm::mat4 MainCameraTransform;
        {// Get Main Camera

            auto& group = registry.view<TransformComponent, CameraComponent>();

            for (auto entity : group)
            {
                auto [transform, cam] = group.get<TransformComponent, CameraComponent>(entity);

                if (cam.m_IsMain)
                {
                    MainCamera = &cam.m_Camera;
                    MainCameraTransform = transform.m_Transform;
                    break;
                }
            }
        }

        if (MainCamera)
        {
            Renderer2D::ResetStats();
            RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
            RendererCommand::Clear();

            // Send Projection And View Matrix To Renderer2D
            Renderer2D::Begin({ MainCamera->GetProjectionMatrix(), glm::inverse(MainCameraTransform) });

            {// Render Sprites

                auto& group = registry.view<TransformComponent, SpriteRendererComponent>();

                for (auto entity : group)
                {
                    auto [transform, sprite_renderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                    Renderer2D::DrawQuad(transform, sprite_renderer);
                }
            }

            Renderer2D::End();
        }
    }

    void Scene::ResizeViewPort(uint32_t Width, uint32_t Height)
    {
        {// Get Main Camera

            auto& group = registry.view<CameraComponent>();

            for (auto entity : group)
            {
                auto& cam = group.get<CameraComponent>(entity);

                if (!cam.m_FixedAspectRatio)
                {
                    // Update OrthoGraphicCamera Projection Matrix
                    cam.m_Camera.SetViewPortSize(Width, Height);
                }
            }
        }
    }
}