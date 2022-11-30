#pragma once
#include <Snap/Core/Core.h>
#include "Buffer.h"

namespace SnapEngine
{

	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddVertexBuffer(const SnapPtr<VertexBuffer>& vertexbuffer) = 0;
		virtual void AddIndexBuffer(const SnapPtr<IndexBuffer>& indexbuffer) = 0;

		virtual inline const SnapPtr<IndexBuffer>& GetIndexBuffer() const = 0;

		static VertexArray* Creat();
	};
}