#pragma once
#include <Snap/Core/Core.h>
#include <glm/glm.hpp>
#include <Snap/Renderer/VertexArray.h>

namespace SnapEngine
{
	class RendererAPI
	{
	public:
		enum class API { None = 0, OpenGL = 1 };

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height) = 0;

		virtual void DrawIndexed(const SnapPtr<VertexArray>& vertexArray, uint32_t count = 0) = 0;

		virtual void DrawLines(const SnapPtr<VertexArray>& vertexArray, uint32_t VerticesCount) = 0;

		virtual void SetLineWidth(float LineWidth = 1.0f) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}