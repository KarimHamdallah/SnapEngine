#pragma once
#include "RendererAPI.h"

namespace SnapEngine
{
	class RendererCommand
	{
	public:

		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height)
		{
			s_RendererAPI->SetViewPort(x, y, Width, Height);
		}

		inline static void SetClearColor(const glm::vec4& Color)
		{
			s_RendererAPI->SetClearColor(Color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const SnapPtr<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}