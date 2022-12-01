#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Renderer/Texture.h>

namespace SnapEngine
{
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
		inline uint32_t GetID() const { return m_ID; }
		inline uint32_t GetTextureID() const { return m_TextureID; }

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		static FrameBuffer* Creat(uint32_t Width, uint32_t Height);
	protected:
		uint32_t m_ID = 0u;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		uint32_t m_TextureID = 0u;
		uint32_t m_DepthTextureID = 0u;
	};
}