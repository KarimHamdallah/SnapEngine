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
		s_Data->m_CircleShader = Shader::Creat("assets/Shaders/CircleShader.glsl");
		s_Data->m_LineShader = Shader::Creat("assets/Shaders/LineShader.glsl");

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
			BufferElement(ShaderDataType::Float,  "aTilingFactor"),
			BufferElement(ShaderDataType::Int,  "aEntityID")
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


		// Cricle
		s_Data->m_CircleVertexArray = SnapPtr<VertexArray>(VertexArray::Creat());
		// Dynamic Vertex Buffer Setup
		s_Data->Circle_VBO = SnapPtr<VertexBuffer>(VertexBuffer::Creat(sizeof(CircleVertex) * s_Data->MaxVertices));
		BufferLayout Circle_VBO_Layout =
		{
			BufferElement(ShaderDataType::Float3, "aPosition"),
			BufferElement(ShaderDataType::Float3, "aLocalPosition"),
			BufferElement(ShaderDataType::Float4, "aColor"),
			BufferElement(ShaderDataType::Float,  "aThickness"),
			BufferElement(ShaderDataType::Float,  "aFade"),
			BufferElement(ShaderDataType::Int,  "aEntityID")
		};
		s_Data->Circle_VBO->SetBufferLayout(Circle_VBO_Layout);
		// Add Vertex Buffer To Vertex Array
		s_Data->m_CircleVertexArray->AddVertexBuffer(s_Data->Circle_VBO);
		// Add Index Buffer To Vertex Array
		s_Data->m_CircleVertexArray->AddIndexBuffer(Quad_EBO);


		// Line
		s_Data->m_LineVertexArray = SnapPtr<VertexArray>(VertexArray::Creat());
		// Dynamic Vertex Buffer Setup
		s_Data->Line_VBO = SnapPtr<VertexBuffer>(VertexBuffer::Creat(sizeof(LineVertex) * s_Data->MaxVertices));
		BufferLayout Line_VBO_Layout =
		{
			BufferElement(ShaderDataType::Float3, "aPosition"),
			BufferElement(ShaderDataType::Float4, "aColor"),
			BufferElement(ShaderDataType::Int,  "aEntityID")
		};
		s_Data->Line_VBO->SetBufferLayout(Line_VBO_Layout);
		// Add Vertex Buffer To Vertex Array
		s_Data->m_LineVertexArray->AddVertexBuffer(s_Data->Line_VBO);




		int max_vertices = s_Data->MaxVertices;
		s_Data->QuadVertexBatchBuffer = new QuadVertex[max_vertices];
		s_Data->CircleVertexBatchBuffer = new CircleVertex[max_vertices];
		s_Data->LineVertexBatchBuffer = new LineVertex[max_vertices];

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

	void Renderer2D::Begin(const EditorCamera& Camera)
	{
		Begin(RendererCamera{ Camera.GetProjectionMatrix(), Camera.GetViewMatrix() });
	}

	void Renderer2D::Begin(const RendererCamera& Camera)
	{
		s_Data->m_ProjectionViewMatrix = Camera.Projection * Camera.View;
		
		// Quad
		// Point To First Element of QuadVertexBatchBuffer Array
		s_Data->QuadVertexPtr = s_Data->QuadVertexBatchBuffer;

		auto Shader = s_Data->m_BatchRendererShader;
		Shader->Bind();
		Shader->UploadMat4("u_ProjectionView", s_Data->m_ProjectionViewMatrix);
		int Samplers[s_Data->MaxTextureSlots];
		for (int i = 0; i < s_Data->MaxTextureSlots; i++)
			Samplers[i] = i;
		Shader->UploadIntArray("u_Textures", s_Data->MaxTextureSlots, Samplers);
		Shader->UnBind();


		// Circle
		// Point To First Element of CircleVertexBatchBuffer Array
		s_Data->CircleVertexPtr = s_Data->CircleVertexBatchBuffer;

		auto CircleShader = s_Data->m_CircleShader;
		CircleShader->Bind();
		CircleShader->UploadMat4("u_ProjectionView", s_Data->m_ProjectionViewMatrix);
		CircleShader->UnBind();

		// Line
		// Point To First Element of LineVertexBatchBuffer Array
		s_Data->LineVertexPtr = s_Data->LineVertexBatchBuffer;

		auto LineShader = s_Data->m_LineShader;
		LineShader->Bind();
		LineShader->UploadMat4("u_ProjectionView", s_Data->m_ProjectionViewMatrix);
		LineShader->UnBind();


		/// ////////////////// Textures /////////////////
		s_Data->TextureSlotIndex = 1; // Reset TextureSlotIndex to 1
	}

	void Renderer2D::End()
	{
		// Update Gpu Vertex Buffer Data
		uint32_t data_size = (uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertexBatchBuffer;
		s_Data->Quad_VBO->SetData(s_Data->QuadVertexBatchBuffer, data_size);
		
		uint32_t circle_data_size = (uint8_t*)s_Data->CircleVertexPtr - (uint8_t*)s_Data->CircleVertexBatchBuffer;
		s_Data->Circle_VBO->SetData(s_Data->CircleVertexBatchBuffer, circle_data_size);

		uint32_t line_data_size = (uint8_t*)s_Data->LineVertexPtr - (uint8_t*)s_Data->LineVertexBatchBuffer;
		s_Data->Line_VBO->SetData(s_Data->LineVertexBatchBuffer, line_data_size);

		Flush();
	}

	void Renderer2D::Flush()
	{
		s_Data->m_BatchRendererShader->Bind();
		// Bind Textures
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
			s_Data->TextureSlots[i]->Bind(i);

		// Quad Draw Call
		RendererCommand::DrawIndexed(s_Data->m_QuadVertexArray, s_Data->Stats.m_QuadCount * 6);
		s_Data->Stats.m_DrawCalls++;




		// Circle Draw Call
		s_Data->m_CircleShader->Bind();
		RendererCommand::DrawIndexed(s_Data->m_CircleVertexArray, s_Data->Stats.m_CircleCount * 6);
		s_Data->Stats.m_DrawCalls++;

		// Line Draw Call
		s_Data->m_LineShader->Bind();
		RendererCommand::SetLineWidth(s_Data->m_LineWidth);
		RendererCommand::DrawLines(s_Data->m_LineVertexArray, s_Data->Stats.m_LineCount * 2);
		s_Data->Stats.m_DrawCalls++;
	}

	void Renderer2D::CircleFlushAndReset()
	{
		End();

		s_Data->Stats.m_CircleCount = 0;

		// Point To First Element of CircleVertexBatchBuffer Array
		s_Data->CircleVertexPtr = s_Data->CircleVertexBatchBuffer;
	}

	void Renderer2D::FlushAndReset()
	{
		End();


		s_Data->Stats.m_QuadCount = 0;

		// Point To First Element of QuadVertexBatchBuffer Array
		s_Data->QuadVertexPtr = s_Data->QuadVertexBatchBuffer;

		s_Data->TextureSlotIndex = 1; // Reset TextureSlotIndex to 1
	}
	
	void Renderer2D::LineFlushAndReset()
	{
		End();

		s_Data->Stats.m_LineCount = 0;

		// Point To First Element of LineVertexBatchBuffer Array
		s_Data->LineVertexPtr = s_Data->LineVertexBatchBuffer;
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

















	void Renderer2D::DrawRect(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color, int EntityID)
	{
		glm::vec3 p0 = glm::vec3(Position.x - Size.x * 0.5f, Position.y + Size.y * 0.5f, Position.z); // Top Left
		glm::vec3 p1 = glm::vec3(Position.x + Size.x * 0.5f, Position.y + Size.y * 0.5f, Position.z); // Top Right
		glm::vec3 p2 = glm::vec3(Position.x + Size.x * 0.5f, Position.y - Size.y * 0.5f, Position.z); // Bottom Right
		glm::vec3 p3 = glm::vec3(Position.x - Size.x * 0.5f, Position.y - Size.y * 0.5f, Position.z); // Bottom Left
		
		
		DrawLine(p0, p1, Color, EntityID);
		DrawLine(p1, p2, Color, EntityID);
		DrawLine(p2, p3, Color, EntityID);
		DrawLine(p0, p3, Color, EntityID);
	}



	void Renderer2D::DrawRect(const glm::mat4& Transform, const glm::vec4& Color, int EntityID)
	{
		glm::vec3 p0 = Transform * s_Data->QuadVertices[0];
		glm::vec3 p1 = Transform * s_Data->QuadVertices[1];
		glm::vec3 p2 = Transform * s_Data->QuadVertices[2];
		glm::vec3 p3 = Transform * s_Data->QuadVertices[3];

		DrawLine(p0, p1, Color, EntityID);
		DrawLine(p1, p2, Color, EntityID);
		DrawLine(p2, p3, Color, EntityID);
		DrawLine(p0, p3, Color, EntityID);
	}


	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& Color, int EntityID)
	{
		if (s_Data->Stats.m_LineCount * 2 >= s_Data->MaxVertices)
			LineFlushAndReset();

		// Push Line (2 Vertices) To LineVertexBacthBuffer
		s_Data->LineVertexPtr->m_Position = p0;
		s_Data->LineVertexPtr->m_Color = Color;
		s_Data->LineVertexPtr->m_EntityID = EntityID;
		s_Data->LineVertexPtr++;

		s_Data->LineVertexPtr->m_Position = p1;
		s_Data->LineVertexPtr->m_Color = Color;
		s_Data->LineVertexPtr->m_EntityID = EntityID;
		s_Data->LineVertexPtr++;

		s_Data->Stats.m_LineCount++; // Add Circle
	}


	void Renderer2D::DrawCircle(const glm::mat4& Transform, const glm::vec4& Color, float Thickness, float Fade, int EntityID)
	{
		if (s_Data->Stats.m_CircleCount * 6 >= s_Data->MaxIndices)
			CircleFlushAndReset();

		const int QuadVerticesCount = 4;

		for (int i = 0; i < QuadVerticesCount; i++)
		{
			// Push Quad (4 Vertices) To CircleVertexBacthBuffer
			s_Data->CircleVertexPtr->m_Position = Transform * s_Data->QuadVertices[i]; // Top Right
			s_Data->CircleVertexPtr->m_LocalPosition = s_Data->QuadVertices[i] * 2.0f;
			s_Data->CircleVertexPtr->m_Color = Color;
			s_Data->CircleVertexPtr->m_Thickness = Thickness;
			s_Data->CircleVertexPtr->m_Fade = Fade;
			s_Data->CircleVertexPtr->m_EntityID = EntityID;
			s_Data->CircleVertexPtr++;
		}

		s_Data->Stats.m_CircleCount++; // Add Circle
	}

	void Renderer2D::DrawQuad(const glm::mat4& Transform, const glm::vec4& Color, int EntityID)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f; // White Texture
		float TilingFactor = 1.0f;

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
			s_Data->QuadVertexPtr->m_EntityID = EntityID;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}

	void Renderer2D::DrawQuad(SnapPtr<Texture2D> Tex, const glm::mat4& Transform, const glm::vec4& Color, float TilingFactor, int EntityID)
	{
		if (s_Data->Stats.m_QuadCount * 6 >= s_Data->MaxIndices)
			FlushAndReset();

		float tex_index = 0.0f; // White Texture

		// Find Passed Texture in s_Data->TextureSlots Array
		// if you find it give me it's index
		for (int i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (s_Data->TextureSlots[i]->IsEqual(*Tex.get()))
			{
				tex_index = (float)i;
				break;
			}
		}

		// if ypu didn't find it in s_Data->TextureSlots Array Add it at TextureSlotIndex
		if (tex_index == 0.0f)
		{
			tex_index = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = Tex;
			s_Data->TextureSlotIndex++;
		}



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
			s_Data->QuadVertexPtr->m_EntityID = EntityID;
			s_Data->QuadVertexPtr++;
		}

		s_Data->Stats.m_QuadCount++; // Add Quad
	}


	void Renderer2D::DrawSprite(const glm::mat4& Transform, SpriteRendererComponent& SpriteRenderer, int EntityID)
	{
		if(!SpriteRenderer.m_Texture)
			DrawQuad(Transform, SpriteRenderer, EntityID);
		else
			DrawQuad(SpriteRenderer.m_Texture, Transform, SpriteRenderer.m_Color, SpriteRenderer.m_TilingFactor, EntityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& Transform, CircleRendererComponent& CircleRenderer, int EntityID)
	{
		DrawCircle(Transform, CircleRenderer.m_Color, CircleRenderer.m_Thickness, CircleRenderer.m_Fade, EntityID);
	}
}