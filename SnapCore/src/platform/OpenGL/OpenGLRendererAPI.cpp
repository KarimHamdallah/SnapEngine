#include "SnapPCH.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h> 

namespace SnapEngine
{
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height)
	{
		glViewport(x, y, Width, Height);
	}

	void OpenGLRendererAPI::DrawIndexed(const SnapPtr<VertexArray>& vertexArray, uint32_t count)
	{
		int index_count = count > 0 ? count : /*vertexArray->GetIndexBuffer()->GetCount()*/0;

		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}