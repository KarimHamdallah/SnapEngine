#include <SnapEngine.h>

class ExampleLayer : public SnapEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(400.0f, -400.0f, 300.0f, -300.0f, 0.0f, 1.0f)
	{
		m_Texture = SnapEngine::SnapPtr<SnapEngine::Texture2D>(SnapEngine::Texture2D::Creat("assets/SpriteSheet.png"));
		m_SubTexture = SnapEngine::SubTexture2D::CreatFromCoords(m_Texture, { 1.0f, 2.0f }, { 400.0f, 600.0f });
	}

	~ExampleLayer() {}

	void Update(SnapEngine::TimeStep Time) override
	{
		// Update Frame
		/*
		m_Position += m_Velocity * Time.GetSeconds();
		
		if (m_Position.x + m_Scale.x * 0.5f >= HalfWidth) { m_Position.x = HalfWidth - m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
		if (m_Position.x - m_Scale.x * 0.5f <= -HalfWidth) { m_Position.x = -HalfWidth + m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
		if (m_Position.y + m_Scale.y * 0.5f >= HalfHeight) { m_Position.y = HalfHeight - m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
		if (m_Position.y - m_Scale.y * 0.5f <= -HalfHeight) { m_Position.y = -HalfHeight + m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
		*/
		// Render
		SnapEngine::Renderer2D::ResetStats();
		SnapEngine::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		SnapEngine::RendererCommand::Clear();

		SnapEngine::Renderer2D::Begin(m_Camera);
		{
			SnapEngine::Renderer2D::DrawQuad(m_SubTexture,
				m_Position, m_Scale);
		}
		SnapEngine::Renderer2D::End();
	}

	void ImGuiRender() override
	{
	}

	void ProcessEvent(SnapEngine::IEvent& e) override
	{
		SnapEngine::EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<SnapEngine::WindowResizeEvent>(SNAP_BIND_FUNCTION(ExampleLayer::WindowResizeCallBack));
		//dispatcher.DispatchEvent<SnapEngine::MouseScrollEvent>(SNAP_BIND_FUNCTION(ExampleLayer::MouseScrollCallBack));
	}

private:
	SnapEngine::OrthoGraphicsCamera m_Camera;
	SnapEngine::SnapPtr<SnapEngine::Texture2D> m_Texture;
	SnapEngine::SnapPtr<SnapEngine::SubTexture2D> m_SubTexture;

	// Rendering
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale = { 100.0f, 100.0f, 1.0f };
	glm::vec3 m_Velocity = { 100.0f, 50.0f, 0.0f };

	// Event variables
	float Width = 800.0f;
	float Height = 600.0f;
	float HalfWidth = 400.0f;
	float HalfHeight = 300.0f;

	float m_Zoom = 1.0f;

private:
	// Event Dispatch CallBacks
	bool WindowResizeCallBack(SnapEngine::WindowResizeEvent& e)
	{
		Width = (float)e.GetWidth();
		Height = (float)e.GetHeight();
		HalfWidth = Width * 0.5f;
		HalfHeight = Height * 0.5f;

		// Change Camera Projection According To New Width and New Height
		m_Camera = SnapEngine::OrthoGraphicsCamera(HalfWidth, -HalfWidth, HalfHeight, -HalfHeight, 0.0f, 1.0f);

		return false;
	}

	bool MouseScrollCallBack(SnapEngine::MouseScrollEvent& e)
	{
		m_Zoom -= e.GetOffsetY() * 0.01f;
		Width *= m_Zoom;
		Height *= m_Zoom;

		SNAP_DEBUG("Zoom = {}", m_Zoom);

		// Change Camera Projection According To New Width and New Height
		m_Camera = SnapEngine::OrthoGraphicsCamera(HalfWidth, -HalfWidth, HalfHeight, -HalfHeight, 0.0f, 1.0f);
		
		return false;
	}
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