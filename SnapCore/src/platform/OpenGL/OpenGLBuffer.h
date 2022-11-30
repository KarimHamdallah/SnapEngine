#pragma once

#include <Snap/Renderer/Buffer.h>

namespace SnapEngine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, size_t size);
		OpenGLVertexBuffer(size_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		inline virtual BufferLayout GetBufferLayout() const override { return m_BufferLayout; }
		inline virtual void SetBufferLayout(const BufferLayout& layout) override { m_BufferLayout = layout; }

	private:
		uint32_t m_ID = 0u;
		BufferLayout m_BufferLayout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		inline virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_ID = 0u;
		uint32_t m_Count = 0u;
	};
}