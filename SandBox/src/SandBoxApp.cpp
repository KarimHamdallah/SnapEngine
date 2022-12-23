#include <SnapEngine.h>
#include <Snap/ECS/EntityManager.h>
#include <Snap/ECS/Components.h>

class ExampleLayer : public SnapEngine::Layer
{
public:
	ExampleLayer()
		: Layer("ExampleLayer")
	{

		Registry = SnapEngine::SnapPtr<EntityManager>(new EntityManager());

		Entity& Player = Registry->CreatEntity();
		Player.AddComponent<TagComponent>("Player");
		Player.AddComponent<TransformComponent>(glm::vec3(1.0f, 20.0f, 15.0f), glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		Player.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f));

		Entity& Enemy = Registry->CreatEntity();
		Enemy.AddComponent<TagComponent>("Enemy");
		Enemy.AddComponent<TransformComponent>(glm::vec3(30.0f, 7.0f, 130.0f), glm::vec3(70.0f, 18.0f, 11.0f), glm::vec3(1.0f, 1.5f, 3.0f));
	    
		Entity& Enemy2 = Registry->CreatEntity();
		Enemy2.AddComponent<TagComponent>("Enemy");
		Enemy2.AddComponent<TransformComponent>(glm::vec3(122.0f, 7.0f, 130.0f), glm::vec3(70.0f, 18.0f, 11.0f), glm::vec3(1.0f, 1.5f, 3.0f));

		Registry->Each<TransformComponent>([=](uint32_t entityID, TransformComponent& comp)
			{
				comp.m_Position.x++;
			});

		glm::vec3 pos1 = Player.GetComponent<TransformComponent>().m_Position;
		glm::vec3 pos2 = Enemy.GetComponent<TransformComponent>().m_Position;
		glm::vec3 pos3 = Enemy2.GetComponent<TransformComponent>().m_Position;

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