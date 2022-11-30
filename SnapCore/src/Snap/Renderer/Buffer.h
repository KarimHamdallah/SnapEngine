#pragma once
#include <Snap/Core/asserts.h>

namespace SnapEngine
{
	enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

	inline uint32_t GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		   case SnapEngine::ShaderDataType::Float:              return 4;
		   case SnapEngine::ShaderDataType::Float2:			    return 4 * 2;
		   case SnapEngine::ShaderDataType::Float3:			    return 4 * 3;
		   case SnapEngine::ShaderDataType::Float4:			    return 4 * 4;
		   case SnapEngine::ShaderDataType::Mat3:				return 4 * 3 * 3;
		   case SnapEngine::ShaderDataType::Mat4:				return 4 * 4 * 4;
		   case SnapEngine::ShaderDataType::Int:				return 4;
		   case SnapEngine::ShaderDataType::Int2:				return 4 * 2;
		   case SnapEngine::ShaderDataType::Int3:				return 4 * 3;
		   case SnapEngine::ShaderDataType::Int4:				return 4 * 4;
		   case SnapEngine::ShaderDataType::Bool:				return 1;
		}

		SNAP_ASSERT_MSG(false, "UnKnown ShaderDataType!");
		return 0;
	}

	inline uint32_t GetShaderDataTypeCount(ShaderDataType type)
	{
		switch (type)
		{
		   case SnapEngine::ShaderDataType::Float:              return 1;
		   case SnapEngine::ShaderDataType::Float2:			    return 2;
		   case SnapEngine::ShaderDataType::Float3:			    return 3;
		   case SnapEngine::ShaderDataType::Float4:			    return 4;
		   case SnapEngine::ShaderDataType::Mat3:				return 3 * 3;
		   case SnapEngine::ShaderDataType::Mat4:				return 4 * 4;
		   case SnapEngine::ShaderDataType::Int:				return 1;
		   case SnapEngine::ShaderDataType::Int2:				return 2;
		   case SnapEngine::ShaderDataType::Int3:				return 3;
		   case SnapEngine::ShaderDataType::Int4:				return 4;
		   case SnapEngine::ShaderDataType::Bool:				return 1;
		}

		SNAP_ASSERT_MSG(false, "UnKnown ShaderDataType!");
		return 0;
	}

	class BufferElement
	{
	public:
		std::string m_Name;
		ShaderDataType m_Type;
		uint32_t m_Size;
		uint32_t m_Offset;
		bool m_Normalize;

		BufferElement(ShaderDataType Type, const std::string& Name, bool Normalize = false)
			: m_Name(Name), m_Type(Type), m_Size(GetShaderDataTypeSize(Type)), m_Offset(0), m_Normalize(Normalize)
		{}

		inline uint32_t GetComponentCount() const { return GetShaderDataTypeCount(m_Type); }
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_BufferElements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() { return m_BufferElements; }
		inline const uint32_t GetStride() const { return m_Stride; }

		inline std::vector<BufferElement>::iterator begin() { return m_BufferElements.begin(); }
		inline std::vector<BufferElement>::iterator end() { return m_BufferElements.end(); }

		inline std::vector<BufferElement>::const_iterator begin() const { return m_BufferElements.begin(); }
		inline std::vector<BufferElement>::const_iterator end() const { return m_BufferElements.end(); }

	private:
		std::vector<BufferElement> m_BufferElements;
		uint32_t m_Stride = 0;

		void CalculateOffsetAndStride()
		{
			uint32_t Offset = 0;
			for (auto& element : m_BufferElements)
			{
				element.m_Offset = Offset;
				Offset += element.m_Size;
				m_Stride += element.m_Size;
			}
		}
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual BufferLayout GetBufferLayout() const = 0;
		virtual void SetBufferLayout(const BufferLayout& layout) = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static VertexBuffer* Creat(float* vertices, size_t size);
		static VertexBuffer* Creat(size_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Creat(uint32_t* indices, uint32_t count);
	};
}