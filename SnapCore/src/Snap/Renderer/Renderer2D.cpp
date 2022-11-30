#include "SnapPCH.h"
#include "Renderer2D.h"

#include <Snap/Renderer/RendererCommand.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace SnapEngine
{
	Renderer2D::Renderer2D_Data* Renderer2D::s_Data = new Renderer2D::Renderer2D_Data();
	
	void Renderer2D::Init()
	{
		// Init BatchRenderer2D Shader
		s_Data->m_BatchRendererShader = Shader::Creat("assets/Shaders/QuadBatchRenderer.glsl");

		// Init White Texture
		s_Data->m_WhiteTexture = SnapPtr<Texture2D>(Texture2D::Creat(1, 1));
		uint32_t WhitePixel =  0xffffffff;
		s_Data->m_WhiteTexture->SetData(&WhitePixel, sizeof(uint32_t));


		///////////////// Quad Batch Renderer Setup ////////////////////

		// Init Quad VBO, EBO, VAO
		s_Data->m_QuadVertexArray = SnapPtr<VertexArray>(VertexArray::Creat());
		
		// Dynamic Vertex Buffer Setup
		s_Data->Quad_VBO = SnapPtr<VertexBuffer>(VertexBuffer::Creat(sizeof(QuadVertex) * s_Data->MaxVertices));

		BufferLayout Quad_VBO_Layout =
		{
			BufferElement(ShaderDataType::Float3, "aPosition"),
			BufferElement(ShaderDataType::Float4, "aColor"),
			BufferElement(ShaderDataType::Float2, "aTexCoords"),
			BufferElement(ShaderDataType::Float,  "aTexID"),
			BufferElement(ShaderDataType::Float,  "aTilingFactor")
		};

		s_Data->Quad_VBO->SetBufferLayout(Quad_VBO_Layout);
		
		// Add Vertex Buffer To Vertex Array
		s_Data->m_QuadVertexArray->AddVertexBuffer(s_Data->Quad_VBO);

		// Creat Index Buffer
		int c = s_Data->MaxIndices;
		uint32_t* Indices = new uint32_t[c];
		int offset = 0;
		for (size_t i = 0; i < c; i += 6)
		{
			Indices[i + 0] = 0 + offset;
			Indices[i + 1] = 1 + offset;
			Indices[i + 2] = 3 + offset;

			Indices[i + 3] = 1 + offset;
			Indices[i + 4] = 2 + offset;
			Indices[i + 5] = 3 + offset;

			offset += 4;
		}

		// Index Buffer Setup
		SnapPtr<IndexBuffer> Quad_EBO = SnapPtr<IndexBuffer>(IndexBuffer::Creat(Indices, s_Data->MaxIndices));


		// Add Index Buffer To Vertex Array
		s_Data->m_QuadVertexArray->AddIndexBuffer(Quad_EBO);
		
		delete[] Indices;

		int max_vertices = s_Data->MaxVertices;
		s_Data->QuadVertexBatchBuffer = new QuadVertex[max_vertices];

		/////////////// Textures ////////////////
		s_Data->TextureSlots[0] = s_Data->m_WhiteTexture; // Set index 0 to white texture


		s_Data->QuadVertices[0] = { 0.5f, 0.5f, 0.0f, 1.0f }; // Top Right
		s_Data->QuadVertices[1] = { 0.5f, -0.5f, 0.0f, 1.0f }; // Bottom Right
		s_Data->QuadVertices[2] = { -0.5f, -0.5f, 0.0f, 1.0f }; // Bottom Left
		s_Data->QuadVertices[3] = { -0.5f, 0.5f, 0.0f, 1.0f }; // Top Left
	}

	void Renderer2D::ShutDown()
	{
		delete s_Data;
	}

	void Renderer2D::Begin(const OrthoGraphicsCamera& Camera)
	{
		// Point To First Element of QuadVertexBatchBuffer Array
		s_Data->QuadVertexPtr = s_Data->QuadVertexBatchBuffer;

		s_Data->m_ProjectionViewMatrix = Camera.GetProjectionViewMatrix();

		auto Shader = s_Data->m_BatchRendererShader;

		Shader->Bind();
		Shader->UploadMat4("u_ProjectionView", s_Data->m_ProjectionViewMatrix);

		int Samplers[s_Data->MaxTextureSlots];
		for (int i = 0; i < s_Data->MaxTextureSlots; i++)
			Samplers[i] = i;

		Shader->UploadIntArray("u_Textures", s_Data->MaxTextureSlots, Samplers);

		// Should Be in Function ResetStats() and Called From user
		/*
		// ResetData
		s_Data->Stats.m_QuadCount = 0;
		s_Data->Stats.m_DrawCalls = 0;
		*/

		/// ////////////////// Textures /////////////////
		s_Data->TextureSlotIndex = 1; // Reset TextureSlotIndex to 1
	}

	void Renderer2D::End()
	{
		// Update Gpu Vertex Buffer Data
		uint32_t data_size = (uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertexBatchBuffer;
		s_Data->Quad_VBO->SetData(s_Data->QuadVertexBatchBuffer, data_size);

		Flush();
	}

	void Renderer2D::Flush()
	{
		s_Data->m_BatchRendererShader->Bind();
		// Bind Textures
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			s_Data->TextureSlots[i]->Bind(i);

		// Draw Call
		RendererCommand::DrawIndexed(s_Data->m_QuadVertexArray, s_Data->Stats.m_QuadCount * 6);
		s_Data->Stats.m_DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		End();


		s_Data->Stats.m_QuadCount = 0;

		// Point To First Element of QuadVertexBatchBuffer Array
		s_Data->QuadVertexPtr = s_Data->QuadVertexBatchBuffer;

		s_Data->TextureSlotIndex = 1; // Reset TextureSlotIndex to 1
	}

	void Renderer2D::DrawQuad(const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color, float TilingFactor)
	{

		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f; // White Texture

		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::scale(glm::mat4(1.0f), Scale);

		//                                Top Right      Bottom Right      Bottom Left      Top Left
		const glm::vec2 TexCoords[] = { {1.0f, 1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i]; // Top Right
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}

	void Renderer2D::DrawQuad(const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color, float TilingFactor)
	{

		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f; // White Texture

		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), Scale);

		//                                Top Right      Bottom Right      Bottom Left      Top Left
		const glm::vec2 TexCoords[] = { {1.0f, 1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i];
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}

	void Renderer2D::DrawQuad(const SnapPtr<Texture2D>& Texture, const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color, float TilingFactor)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f;
		// Find Passed Texture in s_Data->TextureSlots Array
		// if you find it give me it's index
		for (int i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i]->IsEqual(*Texture.get()))
			{
				tex_index = (float)i;
				break;
			}
		}

		// if ypu didn't find it in s_Data->TextureSlots Array Add it at TextureSlotIndex
		if (tex_index == 0.0f)
		{
			tex_index = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = Texture;
			s_Data->TextureSlotIndex++;
		}


		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::scale(glm::mat4(1.0f), Scale);

		//                                Top Right      Bottom Right      Bottom Left      Top Left
		const glm::vec2 TexCoords[] = { {1.0f, 1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i];
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}
	void Renderer2D::DrawQuad(const SnapPtr<Texture2D>& Texture, const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color, float TilingFactor)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f;
		// Find Passed Texture in s_Data->TextureSlots Array
		// if you find it give me it's index
		for (int i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i]->IsEqual(*Texture.get()))
			{
				tex_index = (float)i;
				break;
			}
		}

		// if ypu didn't find it in s_Data->TextureSlots Array Add it at TextureSlotIndex
		if (tex_index == 0.0f)
		{
			tex_index = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = Texture;
			s_Data->TextureSlotIndex++;
		}


		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), Scale);

		//                                Top Right      Bottom Right      Bottom Left      Top Left
		const glm::vec2 TexCoords[] = { {1.0f, 1.0f}, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i];
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}

	void Renderer2D::DrawQuad(
		const SnapPtr<SubTexture2D>& subtexture,
		const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color, float TilingFactor)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();


		SnapPtr<Texture2D> Texture = subtexture->GetTexture();


		float tex_index = 0.0f;
		// Find Passed Texture in s_Data->TextureSlots Array
		// if you find it give me it's index
		for (int i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i]->IsEqual(*Texture.get()))
			{
				tex_index = (float)i;
				break;
			}
		}

		// if ypu didn't find it in s_Data->TextureSlots Array Add it at TextureSlotIndex
		if (tex_index == 0.0f)
		{
			tex_index = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = Texture;
			s_Data->TextureSlotIndex++;
		}


		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::scale(glm::mat4(1.0f), Scale);

		const glm::vec2* TexCoords = subtexture->GetTexCoords();

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i];
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}
	void Renderer2D::DrawQuad(
		const SnapPtr<SubTexture2D>& subtexture,
		const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color, float TilingFactor)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();


		SnapPtr<Texture2D> Texture = subtexture->GetTexture();


		float tex_index = 0.0f;
		// Find Passed Texture in s_Data->TextureSlots Array
		// if you find it give me it's index
		for (int i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i]->IsEqual(*Texture.get()))
			{
				tex_index = (float)i;
				break;
			}
		}

		// if ypu didn't find it in s_Data->TextureSlots Array Add it at TextureSlotIndex
		if (tex_index == 0.0f)
		{
			tex_index = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = Texture;
			s_Data->TextureSlotIndex++;
		}


		// Transformations
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), Scale);

		const glm::vec2* TexCoords = subtexture->GetTexCoords();

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To QuadVertexBacthBuffer
			s_Data->QuadVertexPtr->m_Position = Transform * s_Data->QuadVertices[i];
			s_Data->QuadVertexPtr->m_Color = Color;
			s_Data->QuadVertexPtr->m_TexCoords = TexCoords[i];
			s_Data->QuadVertexPtr->m_TexID = tex_index;
			s_Data->QuadVertexPtr->m_TilingFactor = TilingFactor;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}
}