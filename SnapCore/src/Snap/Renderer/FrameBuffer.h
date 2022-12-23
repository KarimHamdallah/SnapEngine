#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/Renderer/Texture.h>

namespace SnapEngine
{
	enum FrameBufferTextureFormat
	{
		None = 0,

		// Color Attachment
		RGBA8 = 1,
		RED_INTEGER = 2,

		// Depth/stencil
		DEPTH24STECNCIL8 = 3,

		Depth = DEPTH24STECNCIL8
	};

	struct FrameBufferTextureSpecs
	{
		FrameBufferTextureSpecs() = default;
		FrameBufferTextureSpecs(FrameBufferTextureFormat TextureFormat)
			: m_TextureFormat(TextureFormat)
		{}

		FrameBufferTextureFormat m_TextureFormat;
	};

	struct FrameBufferAttachment
	{
		FrameBufferAttachment() = default;
		FrameBufferAttachment(const std::initializer_list<FrameBufferTextureSpecs>& Attachments)
			: m_Attachments(Attachments)
		{}

		std::vector<FrameBufferTextureSpecs> m_Attachments;
	};

	struct FrameBufferSpecifications
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		FrameBufferAttachment Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false; // vulkan
	};


	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() {}

		inline uint32_t GetWidth() const { return m_Specs.Width; }
		inline uint32_t GetHeight() const { return m_Specs.Height; }
		inline uint32_t GetID() const { return m_ID; }
		inline uint32_t GetColorAttachmentTextureID(uint32_t index = 0) const { SNAP_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }
		inline uint32_t GetDepthAttachmentTextureID() const { return m_DepthAttachment; }


		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual int ReadPixel(uint32_t AttachmentIndex, uint32_t x, uint32_t y) = 0;
		virtual void ClearAttachment(uint32_t AttachmentIndex, int value) = 0;
		
		static FrameBuffer* Creat(const FrameBufferSpecifications& specs);
	protected:
		uint32_t m_ID = 0u;
		FrameBufferSpecifications m_Specs;

		std::vector<FrameBufferTextureSpecs> m_ColorAttachmentSpecs;
		FrameBufferTextureSpecs m_DepthAttachmentSpecs = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0u;
	};
}