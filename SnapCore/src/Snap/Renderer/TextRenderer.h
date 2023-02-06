#pragma once
#include <glm/glm.hpp>
#include <Snap/Renderer/Font.h>

#define VERTICES_PER_GLYPH 4
#define INDICES_PER_GLYPH 6
#define MAX_GLYPHS_PER_BATCH 10000

namespace SnapEngine
{
	struct GlyphVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoords;

		int Entity_ID = -1;
	};

	struct TextBatchRendererStats
	{
		uint32_t GlyphCount = 0;
		uint32_t DrawCalls = 0;
	};

	struct RendererCamera
	{
		glm::mat4 Projection;
		glm::mat4 View;
	};

	class TextBatchRenderer
	{
	public:
		static void Init();
		static void Begin(const RendererCamera& RendererCam, const SnapPtr<Font>& Font);
		static void End();
		static void ResetStats();
		static void ShutDown();


		static void RenderText(const std::string& Text, const glm::vec2& Position, float scale, const glm::vec4& Color, int EntityID = -1);
	private:
		static void Flush();
	};
}