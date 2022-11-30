#include "SnapPCH.h"
#include "Renderer.h"

namespace SnapEngine
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::Init()
	{
		RendererCommand::Init();
	}

	void Renderer::Begin(OrthoGraphicsCamera& Camera)
	{
		m_SceneData->CameraProjectionViewMatrix = Camera.GetProjectionViewMatrix();
	}

	void Renderer::End()
	{

	}

	void Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height)
	{
		RendererCommand::SetViewPort(x, y, Width, Height);
	}

	void Renderer::Submit(const SnapPtr<Shader>& Shader, const SnapPtr<VertexArray>& vertexArray, const glm::mat4& TransformationMatrix)
	{
		Shader->Bind();
		Shader->UploadMat4("u_ProjectionView", m_SceneData->CameraProjectionViewMatrix);
		Shader->UploadMat4("u_Transformation", TransformationMatrix);
		RendererCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(const SnapPtr<Shader>& Shader, const SnapPtr<VertexArray>& vertexArray,
		const glm::mat4& TransformationMatrix,
		const SnapPtr<Texture>& Texture)
	{
		Shader->Bind();
		Shader->UploadMat4("u_ProjectionView", m_SceneData->CameraProjectionViewMatrix);
		Shader->UploadMat4("u_Transformation", TransformationMatrix);
		Shader->UploadInt("u_Texture", 0);

		Texture->Bind();

		RendererCommand::DrawIndexed(vertexArray);
	}
}