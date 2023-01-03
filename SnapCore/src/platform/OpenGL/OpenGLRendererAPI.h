#pragma once
#include <Snap/Renderer/RendererAPI.h>


namespace SnapEngine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI() = default;

		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height) override;

		virtual void DrawIndexed(const SnapPtr<VertexArray>& vertexArray, uint32_t count = 0) override;

		virtual void DrawLines(const SnapPtr<VertexArray>& vertexArray, uint32_t VerticesCount) override;

		virtual void SetLineWidth(float LineWidth = 1.0f) override;
	};
}