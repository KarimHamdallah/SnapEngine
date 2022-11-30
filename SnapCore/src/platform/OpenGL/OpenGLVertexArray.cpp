#include "SnapPCH.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace SnapEngine
{
	static GLenum ShaderDatTypeToGLDataType(ShaderDataType type)
	{
		switch (type)
		{
		case SnapEngine::ShaderDataType::Float:             return GL_FLOAT;
		case SnapEngine::ShaderDataType::Float2:			return GL_FLOAT;
		case SnapEngine::ShaderDataType::Float3:			return GL_FLOAT;
		case SnapEngine::ShaderDataType::Float4:			return GL_FLOAT;
		case SnapEngine::ShaderDataType::Mat3:				return GL_FLOAT;
		case SnapEngine::ShaderDataType::Mat4:				return GL_FLOAT;
		case SnapEngine::ShaderDataType::Int:				return GL_INT;
		case SnapEngine::ShaderDataType::Int2:				return GL_INT;
		case SnapEngine::ShaderDataType::Int3:				return GL_INT;
		case SnapEngine::ShaderDataType::Int4:				return GL_INT;
		case SnapEngine::ShaderDataType::Bool:				return GL_BOOL;
		}

		SNAP_ASSERT_MSG(false, "UnKnown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_ID);
		glBindVertexArray(m_ID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_ID);
	}

	void OpenGLVertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const SnapPtr<VertexBuffer>& vertexbuffer)
	{
		SNAP_ASSERT_MSG(vertexbuffer->GetBufferLayout().GetElements().size(), "VertexBuffer Has no Layout!");
		
		glBindVertexArray(m_ID);
		vertexbuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexbuffer->GetBufferLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDatTypeToGLDataType(element.m_Type),
				element.m_Normalize ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.m_Offset
			);

			index++;
		}

		m_VertexBuffers.push_back(vertexbuffer);
	}

	void OpenGLVertexArray::AddIndexBuffer(const SnapPtr<IndexBuffer>& indexbuffer)
	{
		glBindVertexArray(m_ID);
		indexbuffer->Bind();

		m_IndexBuffer = indexbuffer;
	}
}