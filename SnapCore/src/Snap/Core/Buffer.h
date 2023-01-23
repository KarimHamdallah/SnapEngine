#pragma once
#include <stdint.h>
#include <cstring>

namespace SnapEngine
{
	struct Buffer
	{
		uint8_t* m_Data = nullptr;
		uint64_t m_Size = 0;

		Buffer() = default;
		Buffer(uint64_t Size) { Allocate(Size); }
		Buffer(const Buffer&) = default;

		static Buffer Copy(const Buffer& other)
		{
			Buffer result(other.m_Size);
			memcpy(result.m_Data, other.m_Data, other.m_Size);
			return result;
		}

		void Allocate(uint64_t Size)
		{
			Release();
			
			m_Data = new uint8_t[Size];
			m_Size = Size;
		}

		void Release()
		{
			if (m_Data)
			{
				delete[] m_Data;
				m_Data = nullptr;
			}
			m_Size = 0;
		}

		template <typename T>
		T As()
		{
			return (T)m_Data;
		}

		operator bool() const
		{
			return (bool)m_Data;
		}
	};

	struct ScopedBuffer
	{
		ScopedBuffer(uint64_t size)
			: m_Buffer(size)
		{}

		ScopedBuffer(Buffer buffer)
			: m_Buffer(buffer)
		{}

		~ScopedBuffer() { m_Buffer.Release(); }

		uint8_t* Data() { return m_Buffer.m_Data; }
		uint64_t Size() { return m_Buffer.m_Size; }

	private:
		Buffer m_Buffer;
	};
}