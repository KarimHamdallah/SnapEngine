#pragma once

#include "RendererCommand.h"

#include "OrthoGraphicCamera.h"
#include <Snap/Renderer/Shader.h>
#include <Snap/Renderer/Texture.h>

namespace SnapEngine
{
	class Renderer
	{
	public:

		static void Init();
		static void Begin(OrthoGraphicsCamera& Camera);
		static void End();

		static void SetViewPort(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height);

		static void Submit(const SnapPtr<Shader>& Shader, const SnapPtr<VertexArray>& vertexArray, const glm::mat4& TransformationMatrix = glm::mat4(1.0f));
		static void Submit(const SnapPtr<Shader>& Shader, const SnapPtr<VertexArray>& vertexArray,
			const glm::mat4& TransformationMatrix, const SnapPtr<Texture>& Texture);


		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:

		struct SceneData
		{
			glm::mat4 CameraProjectionViewMatrix = glm::mat4(1.0f);
		};

		static SceneData* m_SceneData;
	};
}