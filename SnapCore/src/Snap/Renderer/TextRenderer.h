#include <glm/glm.hpp>

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

	struct RendererCamera
	{
		glm::mat4 Projection;
		glm::mat4 View;
	};

	class TextBatchRenderer
	{
	public:
		void Init();
		static void Begin(const RendererCamera& RendererCam);
		static void End();
	};
}