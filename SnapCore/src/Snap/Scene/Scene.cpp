#include "SnapPCH.h"
#include "Scene.h"
#include <Snap/Scene/Entity.h>

#include <Snap/Scene/Comps/Components.h>
#include <Snap/Renderer/RendererCommand.h>
#include <Snap/Renderer/Renderer2D.h>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace SnapEngine
{
    static b2BodyType RgidBody2DTypeToBox2DType(RigidBody2DComponent::RigidBodyType Type)
    {
        switch (Type)
        {
        case SnapEngine::RigidBody2DComponent::RigidBodyType::STATIC: return b2BodyType::b2_staticBody;
        case SnapEngine::RigidBody2DComponent::RigidBodyType::DYNAMIC: return b2BodyType::b2_dynamicBody;
        case SnapEngine::RigidBody2DComponent::RigidBodyType::KINEMATIC: return b2BodyType::b2_kinematicBody;
        }

        SNAP_ASSERT_MSG(false, "Unkown Type!");
        return b2_staticBody;
    }

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
        entity->GetComponent<TagComponent>().m_Tag.empty() ? std::string("Entity_") + std::to_string(EntityCounter) : name;
        m_Entities[EntityCounter] = SnapPtr<Entity>(entity);
        EntityCounter++;
        return *entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        registry.destroy(entity);
    }

    void Scene::UpdateEditor(const TimeStep& Time, const EditorCamera& Camera)
    {
        // Send Projection And View Matrix To Renderer2D
        Renderer2D::Begin(Camera);

        {// Render Sprites

            auto& group = registry.view<TransformComponent, SpriteRendererComponent>();

            for (auto entity : group)
            {
                auto [transform, sprite_renderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawSprite(transform, sprite_renderer, (int)entity);
            }
        }

        Renderer2D::End();
    }

    void Scene::UpdateRunTime(const TimeStep& Time)
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

        { // Physics
            const uint32_t VelocityIterations = 6;
            const uint32_t PositionIterations = 2;

            m_PhysicsWorld->Step(Time, VelocityIterations, PositionIterations);

            // Get transform From Box2D
            auto& group = registry.view<TransformComponent, RigidBody2DComponent>();
            for (auto entity : group)
            {
                Entity e = { entity, this };
                auto& [transform, rb2d] = group.get<TransformComponent, RigidBody2DComponent>(entity);

                b2Body* body = (b2Body*)rb2d.RunTimeBody;
                const auto& Position = body->GetPosition();
                SNAP_DEBUG("Rotaion {0}", body->GetAngle());
                transform.m_Position.x = Position.x;
                transform.m_Position.y = Position.y;
                transform.m_Rotation.z = glm::degrees(body->GetAngle());
            }
        }
    }

    void Scene::RunTimeRender()
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
                    MainCameraTransform = transform.GetTransformMatrix();
                    break;
                }
            }
        }

        if (MainCamera)
        {
            // Send Projection And View Matrix To Renderer2D
            Renderer2D::Begin({ MainCamera->GetProjectionMatrix(), glm::inverse(MainCameraTransform) });

            {// Render Sprites

                auto& group = registry.view<TransformComponent, SpriteRendererComponent>();

                for (auto entity : group)
                {
                    auto [transform, sprite_renderer] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                    Renderer2D::DrawSprite(transform, sprite_renderer, (int)entity);
                }
            }
        }
    }

    void Scene::ResizeViewPort(uint32_t Width, uint32_t Height)
    {
        m_ViewPortWidth = Width;
        m_ViewPortHeight = Height;
        {// Get Cameras

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

    void Scene::ProcessEvents(IEvent* e)
    {
        registry.view<CppScriptComponent>().each([=](auto entity, auto& cppSc)
            {
                if (!cppSc.m_Instance)
                {
                    cppSc.m_Instance = cppSc.InstantiateScriptFuncPtr();
                    cppSc.m_Instance->m_Entity = Entity{ entity, this };

                    cppSc.m_Instance->Start();
                }

                cppSc.m_Instance->ProcessEvents(*e);
            });
    }

    Entity Scene::GetMainCameraEntity()
    {
        auto& group = registry.view<TransformComponent, CameraComponent>();

        for (auto entity : group)
        {
            auto [transform, cam] = group.get<TransformComponent, CameraComponent>(entity);

            if (cam.m_IsMain)
                return Entity{ entity, this };
        }
            
        return {};
    }

    void Scene::OnRunTimeStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

        auto& group = registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : group)
        {
            Entity e = { entity, this };
            auto& [transform, rb2d] = group.get<TransformComponent, RigidBody2DComponent>(entity);

            
            b2BodyDef Def;
            Def.position.Set(transform.m_Position.x, transform.m_Position.y);
            Def.angle = glm::radians(transform.m_Rotation.z);
            Def.type = RgidBody2DTypeToBox2DType(rb2d.m_Type);
            b2Body* body = m_PhysicsWorld->CreateBody(&Def);
            body->SetFixedRotation(rb2d.m_FixedRotation);
            rb2d.RunTimeBody = body;

            if (e.HasComponent<BoxCollider2DComponent>())
            {
                auto& collider = e.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape shape;
                shape.SetAsBox(collider.m_Size.x * transform.m_Scale.x, collider.m_Size.y * transform.m_Scale.y);
                b2FixtureDef Def;
                Def.shape = &shape;
                Def.density = collider.m_Density;
                Def.friction = collider.m_Friction;
                Def.restitution = collider.m_Restitution;
                Def.restitutionThreshold = collider.m_RestitutionThreshold;
                collider.RunTimeFixture = body->CreateFixture(&Def);
            }
        }
    }

    void Scene::OnRunTimeStop()
    {
        { // Delete Bodies 
            auto& group = registry.view<RigidBody2DComponent>();
            for (auto entity : group)
            {
                auto& rb2d = group.get<RigidBody2DComponent>(entity);
                m_PhysicsWorld->DestroyBody((b2Body*)rb2d.RunTimeBody);
            }
        }

        delete m_PhysicsWorld;
    }


    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(false);
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        component.m_Camera.SetViewPortSize(m_ViewPortWidth, m_ViewPortHeight);
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CppScriptComponent>(Entity entity, CppScriptComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
    {
    }
    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
    {
    }
}