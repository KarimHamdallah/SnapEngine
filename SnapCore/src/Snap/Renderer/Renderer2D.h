#pragma once
#include "OrthoGraphicCamera.h"
#include <Snap/Renderer/VertexArray.h>
#include <Snap/Renderer/Shader.h>
#include <Snap/Renderer/SubTexture2D.h>
#include <Snap/Scene/Camera.h>
#include <Snap/Scene/EditorCamera.h>

#include <Snap/Scene/Comps/Components.h>

namespace SnapEngine
{
	class Renderer2D
	{
	public:
		struct RendererCamera
		{
			glm::mat4 Projection;
			glm::mat4 View;
		};


		static void Init();

		static void ShutDown();

		static void Begin(const EditorCamera& Camera);
		static void Begin(const RendererCamera& Camera);
		static void End();

		static void DrawQuad(const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color, float TilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color, float TilingFactor = 1.0f);
		static void DrawQuad(const SnapPtr<Texture2D>& Texture, const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f);
		static void DrawQuad(const SnapPtr<Texture2D>& Texture, const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f);
		
		static void DrawQuad(
			const SnapPtr<SubTexture2D>& subtexture,
			const glm::vec3& Position, const glm::vec3& Scale, float Rotation, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f);
		static void DrawQuad(
			const SnapPtr<SubTexture2D>& subtexture,
			const glm::vec3& Position, const glm::vec3& Scale, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f);


		static void DrawRect(const glm::mat4& Transform, const glm::vec4& Color = glm::vec4(1.0f), int EntityID = -1);
		static void DrawRect(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Color = glm::vec4(1.0f), int EntityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& Color = glm::vec4(1.0f), int EntityID = -1);
		static void DrawCircle(const glm::mat4& Transform, const glm::vec4& Color = glm::vec4(1.0f), float Thickness = 1.0f, float Fade = 0.005f, int EntityID = -1);
		static void DrawQuad(const glm::mat4& Transform, const glm::vec4& Color = glm::vec4(1.0f), int EntityID = -1);
		static void DrawQuad(SnapPtr<Texture2D> Tex, const glm::mat4& Transform, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f, int EntityID = -1);
		static void DrawSprite(const glm::mat4& Transform, SpriteRendererComponent& SpriteRenderer, int EntityID);
		static void DrawCircle(const glm::mat4& Transform, CircleRendererComponent& CircleRenderer, int EntityID = -1);


		inline static void SetLineWidth(float LineWidth) { s_Data->m_LineWidth = LineWidth; }
		inline static float GetLineWidth() { return s_Data->m_LineWidth; }

		// TODO:: Remove
		static void DrawQuadTemp(SnapPtr<Texture2D> Tex, const glm::vec4& TexCoords,  const glm::mat4& Transform, const glm::vec4& Color = glm::vec4(1.0f), float TilingFactor = 1.0f, int EntityID = -1);


		static void Flush();
	private:
		
		struct QuadVertex
		{
			glm::vec3 m_Position;
			glm::vec4 m_Color;
			glm::vec2 m_TexCoords;
			float m_TexID;
			float m_TilingFactor;

			// Editor only
			int m_EntityID = -1;
		};

		struct CircleVertex
		{
			glm::vec3 m_Position;
			glm::vec3 m_LocalPosition;
			glm::vec4 m_Color;
			float m_Thickness;
			float m_Fade;

			// Editor only
			int m_EntityID = -1;
		};


		struct LineVertex
		{
			glm::vec3 m_Position;
			glm::vec4 m_Color;

			// Editor only
			int m_EntityID = -1;
		};
		
		struct RendererStatistics
		{
			uint32_t m_QuadCount = 0;
			uint32_t m_CircleCount = 0;
			uint32_t m_LineCount = 0;
			uint32_t m_DrawCalls = 0;

			uint32_t GetTotalQuadsVertixCount() { return m_QuadCount * 4; }
			uint32_t GetTotalQuadsIndicesCount() { return m_QuadCount * 6; }
		};

		struct Renderer2D_Data
		{
			// Quad Batch Renderer
			uint32_t MaxQuads = 10000;
			uint32_t MaxVertices = MaxQuads * 4;
			uint32_t MaxIndices = MaxQuads * 6;
			uint32_t MaxLines = 10000;
			static const uint32_t MaxTextureSlots = 32;

			std::array<SnapPtr<Texture2D>, MaxTextureSlots> TextureSlots;
			uint32_t TextureSlotIndex = 1; // White Texture at slot 0

			// Quad
			QuadVertex* QuadVertexBatchBuffer = nullptr;
			QuadVertex* QuadVertexPtr = nullptr;

			SnapPtr<VertexArray> m_QuadVertexArray;
			SnapPtr<VertexBuffer> Quad_VBO;

			// Circle
			CircleVertex* CircleVertexBatchBuffer = nullptr;
			CircleVertex* CircleVertexPtr = nullptr;

			SnapPtr<VertexArray> m_CircleVertexArray;
			SnapPtr<VertexBuffer> Circle_VBO;

			// Line
			LineVertex* LineVertexBatchBuffer = nullptr;
			LineVertex* LineVertexPtr = nullptr;

			SnapPtr<VertexArray> m_LineVertexArray;
			SnapPtr<VertexBuffer> Line_VBO;

			float m_LineWidth = 1.0f;

			SnapPtr<Shader> m_BatchRendererShader;
			SnapPtr<Shader> m_CircleShader;
			SnapPtr<Shader> m_LineShader;
			SnapPtr<Texture2D> m_WhiteTexture; // 1x1 White Texture 2D used as 1 value in Texture shader

			glm::mat4 m_ProjectionViewMatrix = glm::mat4(1.0f);

			// Quad Vertices Positions in Local Space
			glm::vec4 QuadVertices[4];

			RendererStatistics Stats;
		};

		static Renderer2D_Data* s_Data;
		
		
	public:		
		inline static RendererStatistics GetRenderer2DStats() { return s_Data->Stats; }
		inline static void ResetStats() { memset(&s_Data->Stats, 0, sizeof(RendererStatistics)); }
	private:
		static void LineFlushAndReset();
		static void FlushAndReset();
		static void CircleFlushAndReset();
	};
}