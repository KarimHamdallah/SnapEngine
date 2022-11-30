#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Renderer/VertexArray.h>

namespace SnapEngine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddVertexBuffer(const SnapPtr<VertexBuffer>& vertexbuffer) override;
		virtual void AddIndexBuffer(const SnapPtr<IndexBuffer>& indexbuffer) override;

		virtual inline const SnapPtr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		uint32_t m_ID = 0u;
		std::vector<SnapPtr<VertexBuffer>> m_VertexBuffers;
		SnapPtr<IndexBuffer> m_IndexBuffer;
	};
}