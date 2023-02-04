#include "SnapPCH.h"
#include "TextRenderer.h"

#include <Snap/Renderer/Shader.h>

namespace SnapEngine
{
	struct RendererData
	{
		SnapPtr<Shader> m_TextBatchShader;
		GlyphVertex* m_GlyphVertices;
		GlyphVertex* m_GlyphVerticesFirst;
	};

	static RendererData* s_Data;

	void TextBatchRenderer::Init()
	{
		s_Data = new RendererData();
		s_Data->m_TextBatchShader = SnapPtr<Shader>(Shader::Creat("assets/Shaders/TextShader.glsl"));
		s_Data->m_GlyphVertices = new GlyphVertex[MAX_GLYPHS_PER_BATCH];
		s_Data->m_GlyphVerticesFirst = s_Data->m_GlyphVertices;
	}

	void TextBatchRenderer::Begin(const RendererCamera& RendererCam)
	{
	}

	void TextBatchRenderer::End()
	{
	}
}