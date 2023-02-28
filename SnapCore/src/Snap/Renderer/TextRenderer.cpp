#include "SnapPCH.h"
#include "TextRenderer.h"

#include <Snap/Renderer/VertexArray.h>
#include <Snap/Renderer/Shader.h>
#include <Snap/Renderer/RendererCommand.h>
#include "MSDFData.h"

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
		s_Data->m_Font->GetAtlasTexture()->Bind();

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

	void TextBatchRenderer::RenderText(const std::string& Text, const glm::mat4 Transform, const glm::vec4& Color, float KerningOffset, float LineSpacing, int EntityID)
	{
		const auto& fontGeometry = s_Data->m_Font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		SnapPtr<Texture2D> fontAtlas = s_Data->m_Font->GetAtlasTexture();

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;
		float lineHeightOffset = 0.0f;

		for (size_t i = 0; i < Text.size(); i++)
		{
			char character = Text[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + lineHeightOffset + LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				double advance = fontGeometry.getGlyph(' ')->getAdvance();
				if (i < Text.size() - 1)
				{
					char nextCharacter = Text[i + 1];
					fontGeometry.getAdvance(advance, character, nextCharacter);
				}
				
				x += fsScale * advance + KerningOffset;
				continue;
			}

			if (character == '\t')
			{
				double advance = fontGeometry.getGlyph(' ')->getAdvance();
				x += 4.0f * (fsScale * advance + KerningOffset);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			if (character == '\t')
				glyph = fontGeometry.getGlyph(' ');

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->getWidth();
			float texelHeight = 1.0f / fontAtlas->getHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			s_Data->m_GlyphVerticesFirst->Position = Transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_Data->m_GlyphVerticesFirst->Color = Color;
			s_Data->m_GlyphVerticesFirst->TexCoords = texCoordMin;
			s_Data->m_GlyphVerticesFirst->Entity_ID = EntityID;
			s_Data->m_GlyphVerticesFirst++;

			s_Data->m_GlyphVerticesFirst->Position = Transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_Data->m_GlyphVerticesFirst->Color = Color;
			s_Data->m_GlyphVerticesFirst->TexCoords = { texCoordMin.x, texCoordMax.y };
			s_Data->m_GlyphVerticesFirst->Entity_ID = EntityID;
			s_Data->m_GlyphVerticesFirst++;

			s_Data->m_GlyphVerticesFirst->Position = Transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_Data->m_GlyphVerticesFirst->Color = Color;
			s_Data->m_GlyphVerticesFirst->TexCoords = texCoordMax;
			s_Data->m_GlyphVerticesFirst->Entity_ID = EntityID;
			s_Data->m_GlyphVerticesFirst++;

			s_Data->m_GlyphVerticesFirst->Position = Transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_Data->m_GlyphVerticesFirst->Color = Color;
			s_Data->m_GlyphVerticesFirst->TexCoords = { texCoordMax.x, texCoordMin.y };
			s_Data->m_GlyphVerticesFirst->Entity_ID = EntityID;
			s_Data->m_GlyphVerticesFirst++;

			s_Data->m_Stats.GlyphCount++;

			if (i < Text.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = Text[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + KerningOffset;
			}
		}
	}

	void TextBatchRenderer::RenderText(const glm::mat4 Transform, const TextRendererComponent& TextRenderer, int EntityID)
	{
		RenderText(TextRenderer.m_TextString, Transform, TextRenderer.m_Color, TextRenderer.KerningOffset, TextRenderer.LineSpacing, EntityID);
	}
}