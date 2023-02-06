#include "SnapPCH.h"
#include "TextRenderer.h"

#include <Snap/Renderer/VertexArray.h>
#include <Snap/Renderer/Shader.h>
#include <Snap/Renderer/RendererCommand.h>

namespace SnapEngine
{
	struct RendererData
	{
		SnapPtr<Shader> m_TextBatchShader;
		GlyphVertex* m_GlyphVertices;
		GlyphVertex* m_GlyphVerticesFirst;
		SnapPtr<VertexArray> m_VertexArray;
		SnapPtr<VertexBuffer> VBO;

		SnapPtr<Font> m_Font;
		TextBatchRendererStats m_Stats;

		glm::vec4 GlyphVertices[4];
	};

	static RendererData* s_Data;

	void TextBatchRenderer::Init()
	{
		// Init Data
		s_Data = new RendererData();
		s_Data->m_TextBatchShader = SnapPtr<Shader>(Shader::Creat("assets/Shaders/TextBatchRenderer.glsl"));
		s_Data->m_GlyphVertices = new GlyphVertex[VERTICES_PER_GLYPH * MAX_GLYPHS_PER_BATCH];
		s_Data->m_GlyphVerticesFirst = s_Data->m_GlyphVertices;

		// VertexArray
		s_Data->m_VertexArray = SnapPtr<VertexArray>(VertexArray::Creat());

		s_Data->VBO = SnapPtr<VertexBuffer>(VertexBuffer::Creat(sizeof(GlyphVertex) * VERTICES_PER_GLYPH * MAX_GLYPHS_PER_BATCH));

		BufferLayout BufferLayout =
		{   BufferElement(ShaderDataType::Float3, "aPosition"),
			BufferElement(ShaderDataType::Float4, "aColor"),
			BufferElement(ShaderDataType::Float2, "aTexCoords"),
			BufferElement(ShaderDataType::Int,    "aEntityID")
		};
		s_Data->VBO->SetBufferLayout(BufferLayout);

		s_Data->m_VertexArray->AddVertexBuffer(s_Data->VBO);

		// Creat Index Buffer
		int c = INDICES_PER_GLYPH * MAX_GLYPHS_PER_BATCH;
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

		SnapPtr<IndexBuffer> EBO = SnapPtr<IndexBuffer>(IndexBuffer::Creat(Indices, c));

		s_Data->m_VertexArray->AddIndexBuffer(EBO);

		delete[] Indices;


		s_Data->GlyphVertices[0] = { 0.5f, 0.5f, 0.0f, 1.0f }; // Top Right
		s_Data->GlyphVertices[1] = { 0.5f, -0.5f, 0.0f, 1.0f }; // Bottom Right
		s_Data->GlyphVertices[2] = { -0.5f, -0.5f, 0.0f, 1.0f }; // Bottom Left
		s_Data->GlyphVertices[3] = { -0.5f, 0.5f, 0.0f, 1.0f }; // Top Left
	}

	void TextBatchRenderer::Begin(const RendererCamera& RendererCam, const SnapPtr<Font>& Font)
	{
		s_Data->m_Font = Font;
		// Set Shader Uniforms
		auto& shader = s_Data->m_TextBatchShader;
		shader->Bind();
		glm::mat4 ProjectionViewMatrix = RendererCam.Projection * RendererCam.View;
		shader->UploadMat4("u_ProjectionView", ProjectionViewMatrix);
		shader->UploadInt("u_TextureAtlas", 0);

		// Set Pointer To First Vertex In Buffer
		s_Data->m_GlyphVerticesFirst = s_Data->m_GlyphVertices;
	}

	void TextBatchRenderer::End()
	{
		uint32_t BufferSize = (uint8_t*)s_Data->m_GlyphVerticesFirst - (uint8_t*)s_Data->m_GlyphVertices;
		s_Data->VBO->SetData(s_Data->m_GlyphVertices, BufferSize);
		Flush();
	}

	void TextBatchRenderer::Flush()
	{
		auto& shader = s_Data->m_TextBatchShader;
		shader->Bind();
		s_Data->m_Font->GetFontAtlas()->Bind();

		// Draw Call
		RendererCommand::DrawIndexed(s_Data->m_VertexArray, s_Data->m_Stats.GlyphCount * INDICES_PER_GLYPH);
		s_Data->m_Stats.DrawCalls++;
	}

	void TextBatchRenderer::ResetStats()
	{
		memset(&s_Data->m_Stats, 0, sizeof(TextBatchRendererStats));
	}

	void TextBatchRenderer::ShutDown()
	{
		delete s_Data;
	}

	void TextBatchRenderer::RenderText(const std::string& Text, const glm::vec2& Position, float scale, const glm::vec4& Color, int EntityID)
	{
		//glm::mat4 TransformMatrix = Transform;
		float x = Position.x;
		for (auto Character = Text.begin(); Character < Text.end(); Character++)
		{
			Glyph glyph = s_Data->m_Font->GetGlyph(*Character);
			float xpos = x + glyph.Bearing.x * scale;
			float ypos = Position.y - (glyph.Size.y - glyph.Bearing.y) * scale;

			float w = glyph.Size.x * scale;
			float h = glyph.Size.y * scale;

			glm::vec3 Vertices[4] = {
			{ xpos + w, ypos,     0.0f }, // Top Right
			{ xpos + w, ypos + h, 0.0f }, // Bottom Right
			{ xpos,     ypos + h, 0.0f }, // Bottom Left
			{ xpos,     ypos,     0.0f }  // Top Left
			};

			glm::vec4 TexCoords = s_Data->m_Font->GetCharacterAtlasTexCoords(*Character);
			//                                    Top Right                         Bottom Right                Bottom Left             Top Left
			const glm::vec2 _TexCoords[] = { { TexCoords.z, TexCoords.w }, { TexCoords.z, TexCoords.y }, { TexCoords.x, TexCoords.y }, { TexCoords.x, TexCoords.w } };
			for (size_t vertex = 0; vertex < 4; vertex++)
			{
				s_Data->m_GlyphVerticesFirst->Position = /*TransformMatrix * s_Data->GlyphVertices[vertex]*/Vertices[vertex];
				s_Data->m_GlyphVerticesFirst->Color = Color;
				s_Data->m_GlyphVerticesFirst->TexCoords = _TexCoords[vertex];
				s_Data->m_GlyphVerticesFirst->Entity_ID = EntityID;

				s_Data->m_GlyphVerticesFirst++;
			}
			s_Data->m_Stats.GlyphCount++;
			x += (glyph.Advance >> 6) * scale;
		}
	}
}