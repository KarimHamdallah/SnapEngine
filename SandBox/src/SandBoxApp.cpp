#include <SnapEngine.h>
#include <Snap/ECS/EntityManager.h>
#include <Snap/ECS/Components.h>
#include "Mathf.h"

#define MAX_ENTITY_COUNT 100
#define MAX_COMPONENT_COUNT 5

using namespace enttt;

class ExampleLayer : public SnapEngine::Layer
{
public:
	ExampleLayer()
		: Layer("ExampleLayer")
	{

		Registry = SnapEngine::SnapPtr<EntityManager>(new EntityManager(MAX_ENTITY_COUNT, MAX_COMPONENT_COUNT));

		Entity Player = Registry->CreatEntity();
		Registry->AddComponent<TagComponent>(Player, "Player");
		Registry->AddComponent<TransformComponent>(Player, glm::vec3(1.0f, 20.0f, 15.0f), glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		Registry->AddComponent<SpriteRendererComponent>(Player, glm::vec4(1.0f));

		Entity Enemy = Registry->CreatEntity();
		Registry->AddComponent<TagComponent>(Enemy, "Enemy");
		Registry->AddComponent<TransformComponent>(Enemy, glm::vec3(30.0f, 7.0f, 130.0f), glm::vec3(70.0f, 18.0f, 11.0f), glm::vec3(1.0f, 1.5f, 3.0f));
	    
		Entity Enemy2 = Registry->CreatEntity();
		Registry->AddComponent<TagComponent>(Enemy2, "Enemy");
		Registry->AddComponent<TransformComponent>(Enemy2, glm::vec3(122.0f, 7.0f, 130.0f), glm::vec3(70.0f, 18.0f, 11.0f), glm::vec3(1.0f, 1.5f, 3.0f));

		
		Registry->Each<TransformComponent>([=](uint32_t entityID, TransformComponent& comp)
			{
				comp.m_Position.x++;
			});

		auto& group = Registry->View<TransformComponent>();
		for (auto& entity : group)
		{
			Registry->GetComponent<TransformComponent>(entity).m_Position.x++;
		}


		glm::vec3 pos1 = Registry->GetComponent<TransformComponent>(Player).m_Position;
		glm::vec3 pos2 = Registry->GetComponent<TransformComponent>(Enemy).m_Position;
		glm::vec3 pos3 = Registry->GetComponent<TransformComponent>(Enemy2).m_Position;

		SNAP_DEBUG("Player Pos = {0}, {1}, {2}", pos1.x, pos1.y, pos1.z);
		SNAP_DEBUG("Enemy Pos = {0}, {1}, {2}", pos2.x, pos2.y, pos2.z);
		SNAP_DEBUG("Enemy Pos = {0}, {1}, {2}", pos3.x, pos3.y, pos3.z);
	}

	virtual void Start() override {}
	virtual void Destroy() override {}
	virtual void Update(SnapEngine::TimeStep Time) override {}
	virtual void ImGuiRender() override {}

	virtual void ProcessEvent(SnapEngine::IEvent& e) override {}

private:
	SnapEngine::SnapPtr<EntityManager> Registry;
};

class SandBox : public SnapEngine::Application
{
public:
	SandBox()
	{
		SNAP_INFO("Welcome To First App!");
		PushLayer(new ExampleLayer());
	}

	~SandBox() {}
private:

};

SnapEngine::Application* SnapEngine::CreatApplication()
{
	return new SandBox();
}