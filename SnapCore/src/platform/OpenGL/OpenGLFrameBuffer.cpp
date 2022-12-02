#include "SnapPCH.h"
#include "OpenGLFrameBuffer.h"

#include <Snap/Core/asserts.h>
#include <glad/glad.h>

namespace SnapEngine
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(uint32_t Width, uint32_t Height)
	{
        Invalidate(Width, Height);
	}

    void OpenGLFrameBuffer::Invalidate(uint32_t Width, uint32_t Height)
    {
        m_Width = Width;
        m_Height = Height;

        if (m_ID)
        {
            glDeleteFramebuffers(1, &m_ID);
            glDeleteTextures(1, &m_TextureID);
            glDeleteTextures(1, &m_DepthTextureID);

           m_ID = m_TextureID = m_DepthTextureID = 0u;
        }

        glGenFramebuffers(1, &m_ID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

        // create a color attachment texture

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);


        // create a depth attachment texture
        glGenTextures(1, &m_DepthTextureID);
        glBindTexture(GL_TEXTURE_2D, m_DepthTextureID);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureID, 0);

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            SNAP_ASSERT_MSG(false, "Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteTextures(1, &m_TextureID);
        glDeleteTextures(1, &m_DepthTextureID);

        glDeleteFramebuffers(1, &m_ID);
    }

    void OpenGLFrameBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
        glViewport(0, 0, m_Width, m_Height);
    }
    void OpenGLFrameBuffer::UnBind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Resize(uint32_t Width, uint32_t Height)
    {
        if (Width == 0 || Height == 0)
        {
            SNAP_ERROR("Attemp To Resize Frame Buffer To {0}, {1}", Width, Height);
            return;
        }
        Invalidate(Width, Height);
    }
}