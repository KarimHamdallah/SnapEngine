#include "SnapPCH.h"
#include "OpenGLTexture.h"
#include <Snap/Core/asserts.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace SnapEngine
{
	static GLenum ToGL(WrapMode wrapMode)
	{
		switch (wrapMode)
		{
		case WrapMode::Repeat:
			return GL_REPEAT;
		case WrapMode::None:
			return GL_NONE;
		case WrapMode::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		default:
			SNAP_WARN("Unknown glWrapMode");
		}

		return GL_NONE;
	}

	static GLenum ToGL(FilterMode filterMode)
	{
		switch (filterMode)
		{
		case FilterMode::Linear:
			return GL_LINEAR;
		case FilterMode::Nearest:
			return GL_NEAREST;
		case FilterMode::None:
			return GL_NONE;
		default:
			SNAP_WARN("Unknown glFilterMode");
		}

		return GL_NONE;
	}

	static GLenum ToGL(ByteFormat format)
	{
		switch (format)
		{
		case ByteFormat::RGBA8:
			return GL_RGBA8;
		case ByteFormat::RGB8:
			return GL_RGB8;
		case ByteFormat::RGBA:
			return GL_RGBA;
		case ByteFormat::RGB:
			return GL_RGB;
		case ByteFormat::GREEN:
			return GL_GREEN;
		case ByteFormat::BLUE:
			return GL_BLUE;
		case ByteFormat::R32UI:
			return GL_R32UI;
		case ByteFormat::RED:
			return GL_RED;
		case ByteFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		case ByteFormat::DEPTH24_STENCIL8:
			return GL_DEPTH24_STENCIL8;
		case ByteFormat::None:
			return GL_NONE;
		default:
			SNAP_WARN("Unknown glByteFormat");
		}

		return GL_NONE;
	}


	static GLenum ToGLDataType(ByteFormat format)
	{
		switch (format)
		{
		case ByteFormat::RGBA8:
			return GL_FLOAT;
		case ByteFormat::RGB8:
			return GL_FLOAT;
		case ByteFormat::RGBA:
			return GL_FLOAT;
		case ByteFormat::RGB:
			return GL_FLOAT;
		case ByteFormat::R32UI:
			return GL_UNSIGNED_INT;
		case ByteFormat::RED_INTEGER:
			return GL_UNSIGNED_INT;
		case ByteFormat::None:
			return GL_NONE;
		default:
			SNAP_WARN("Unknown glByteFormat");
		}

		return GL_NONE;
	}

	static bool ByteFormatIsInt(ByteFormat format)
	{
		switch (format)
		{
		case ByteFormat::RGBA8:
			return false;
		case ByteFormat::RGB8:
			return false;
		case ByteFormat::RGBA:
			return false;
		case ByteFormat::RGB:
			return false;
		case ByteFormat::R32UI:
			return true;
		case ByteFormat::RED_INTEGER:
			return true;
		case ByteFormat::None:
			return GL_NONE;
		default:
			SNAP_WARN("Unknown glByteFormat");
		}

		return false;
	}

	OpenGLTexture::OpenGLTexture(const std::string& filepath, const Textureprops& props, bool flip)
	{

		stbi_set_flip_vertically_on_load(flip);

		int channels, Width, Height;
		unsigned char* pixels = stbi_load(filepath.c_str(), &Width, &Height, &channels, 0);

		SNAP_ASSERT_MSG((pixels != nullptr), "STB failed to load image at path: " + filepath + "\n-> STB Failure Reason: " + std::string(stbi_failure_reason()));
		
		m_Filepath = filepath;
		m_Width = Width;
		m_Height = Height;

		int bytesPerPixel = channels;

		if (bytesPerPixel == 4)
		{
			m_Props.m_InternalFormat = ByteFormat::RGBA8;
			m_Props.m_ExternalFormat = ByteFormat::RGBA;
		}
		else if (bytesPerPixel == 3)
		{
			m_Props.m_InternalFormat = ByteFormat::RGB8;
			m_Props.m_ExternalFormat = ByteFormat::RGB;
		}
		else
		{
			SNAP_WARN("Unknown number of channels {0} in image {1}.", channels, filepath.c_str());
			return;
		}

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// Bind Texture Parameters
		if (m_Props.m_WrapS != WrapMode::None) // WrapS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGL(m_Props.m_WrapS));
		if (m_Props.m_WrapT != WrapMode::None) // WrapT
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGL(m_Props.m_WrapT));
		if (m_Props.m_MinFilter != FilterMode::None) // MinFilter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGL(m_Props.m_MinFilter));
		if (m_Props.m_MagFilter != FilterMode::None) // MagFiter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGL(m_Props.m_MagFilter));

		uint32_t internalFormat = ToGL(m_Props.m_InternalFormat);
		uint32_t externalFormat = ToGL(m_Props.m_ExternalFormat);

		SNAP_ASSERT_MSG(internalFormat != GL_NONE && externalFormat != GL_NONE, "Tried to load image from file, but failed to identify internal format for image " + filepath);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, externalFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}

	OpenGLTexture::OpenGLTexture(uint32_t Width, uint32_t Height, const Textureprops& props, bool flip)
	{
		m_Width = Width;
		m_Height = Height;
		
		m_Props.m_InternalFormat = ByteFormat::RGBA8;
		m_Props.m_ExternalFormat = ByteFormat::RGBA;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// Bind Texture Parameters
		if (m_Props.m_WrapS != WrapMode::None) // WrapS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGL(m_Props.m_WrapS));
		if (m_Props.m_WrapT != WrapMode::None) // WrapT
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGL(m_Props.m_WrapT));
		if (m_Props.m_MinFilter != FilterMode::None) // MinFilter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGL(m_Props.m_MinFilter));
		if (m_Props.m_MagFilter != FilterMode::None) // MagFiter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGL(m_Props.m_MagFilter));

		uint32_t internalFormat = ToGL(m_Props.m_InternalFormat);
		uint32_t externalFormat = ToGL(m_Props.m_ExternalFormat);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, externalFormat, GL_UNSIGNED_BYTE, 0);
	}

	OpenGLTexture::OpenGLTexture(uint32_t Width, uint32_t Height, void* Data, const Textureprops& props)
	{
		m_Width = Width;
		m_Height = Height;

		m_Props.m_InternalFormat = ByteFormat::RED;
		m_Props.m_ExternalFormat = ByteFormat::RED;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		// Bind Texture Parameters
		if (m_Props.m_WrapS != WrapMode::None) // WrapS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGL(m_Props.m_WrapS));
		if (m_Props.m_WrapT != WrapMode::None) // WrapT
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGL(m_Props.m_WrapT));
		if (m_Props.m_MinFilter != FilterMode::None) // MinFilter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGL(m_Props.m_MinFilter));
		if (m_Props.m_MagFilter != FilterMode::None) // MagFiter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGL(m_Props.m_MagFilter));

		uint32_t internalFormat = ToGL(m_Props.m_InternalFormat);
		uint32_t externalFormat = ToGL(m_Props.m_ExternalFormat);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, externalFormat, GL_UNSIGNED_BYTE, Data);
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		GLenum dataformat = ToGL(m_Props.m_ExternalFormat);
		int bpc = dataformat == GL_RGBA ? 4 : 3;
		SNAP_ASSERT_MSG(size == m_Width * m_Height * bpc, "Size not Correct Should be equal to width * height * byte per channel");
		
		glBindTexture(GL_TEXTURE_2D, m_ID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataformat, GL_UNSIGNED_BYTE, data);
	}

	bool OpenGLTexture::Is_null() const
	{
		return m_ID == 0u;
	}

	void OpenGLTexture::Bind(uint32_t Slot) const
	{
		glBindTextureUnit(Slot, m_ID);
	}

	void OpenGLTexture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void OpenGLTexture::Delete()
	{
		if (!Is_null())
		{
			glDeleteTextures(1, &m_ID);
			m_ID = 0u;
		}
	}

	OpenGLTexture::~OpenGLTexture()
	{
		Delete();
	}
}