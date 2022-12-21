#include "SnapPCH.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace SnapEngine
{
    static GLenum TextureTarget(bool IsMultiSampled)
    {
        return IsMultiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void BindTexture(bool IsMultiSampled, uint32_t ID)
    {
        glBindTexture(TextureTarget(IsMultiSampled), ID);
    }

    static bool IsDepthFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
           case FrameBufferTextureFormat::DEPTH24STECNCIL8: return true;
        }

        return false;
    }

    static void CreatTextures(bool IsMultiSampled, uint32_t* outID, uint32_t count)
    {
        glCreateTextures(TextureTarget(IsMultiSampled), count, outID);
    }

    static void AttachColorTexture(uint32_t ID, int Samples, GLenum Format, uint32_t Width, uint32_t Height, int index)
    {
        bool IsMultiSampled = Samples > 1;

        if (IsMultiSampled)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE);
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(IsMultiSampled), ID, 0);
    }

    static void AttachDepthTexture(uint32_t ID, int Samples, GLenum Format, GLenum AttachmentType, uint32_t Width, uint32_t Height)
    {
        bool IsMultiSampled = Samples > 1;

        if (IsMultiSampled)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE);
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, TextureTarget(IsMultiSampled), ID, 0);
    }

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
	{
        m_Specs = specs;

        for (auto format : m_Specs.Attachments.m_Attachments)
        {
            if (!IsDepthFormat(format.m_TextureFormat))
                m_ColorAttachmentSpecs.emplace_back(format);
            else
                m_DepthAttachmentSpecs = format;
        }

        Invalidate();
	}

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_ID)
        {
            glDeleteFramebuffers(1, &m_ID);
            
            for (auto& TextureID : m_ColorAttachments)
                glDeleteTextures(1, &TextureID);

            glDeleteTextures(1, &m_DepthAttachment);

           m_ID = m_DepthAttachment = 0u;

           m_ColorAttachments.clear();
        }

        glGenFramebuffers(1, &m_ID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

        // Attachments
        bool IsMultiSampled = m_Specs.Samples > 1;

        // create color attachment textures
        if (m_ColorAttachmentSpecs.size())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
            
            CreatTextures(IsMultiSampled, m_ColorAttachments.data(), m_ColorAttachments.size());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                BindTexture(IsMultiSampled, m_ColorAttachments[i]);
                switch (m_ColorAttachmentSpecs[i].m_TextureFormat)
                {
                case FrameBufferTextureFormat::RGBA8 :
                {
                    AttachColorTexture(m_ColorAttachments[i], m_Specs.Samples,
                        GL_RGBA8, m_Specs.Width, m_Specs.Height, i);
                }
                break;
                default:
                    break;
                }
            }
        }

        // create depth attachment texture
        if (m_DepthAttachmentSpecs.m_TextureFormat != FrameBufferTextureFormat::None)
        {
            CreatTextures(IsMultiSampled, &m_DepthAttachment, 1);
            BindTexture(IsMultiSampled, m_DepthAttachment);

            switch (m_DepthAttachmentSpecs.m_TextureFormat)
            {
            case FrameBufferTextureFormat::DEPTH24STECNCIL8:
            {
                AttachDepthTexture(m_DepthAttachment, m_Specs.Samples, GL_DEPTH24_STENCIL8,
                    GL_DEPTH_STENCIL_ATTACHMENT, m_Specs.Width, m_Specs.Height);
            }
            break;
            default:
                break;
            }
        }

        if (m_ColorAttachments.size() > 1)
        {
            SNAP_ASSERT_MSG(m_ColorAttachments.size() <= 4, "FrameBuffer Color Attachments Exceeding Max Attachments Which Is 4 Color Attachments");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers); // Enable Rendering of All Color Attachments
        }
        else if (m_ColorAttachments.empty())
        {
            // Only Depth Pass
            glDrawBuffer(GL_NONE);
        }

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            SNAP_ASSERT_MSG(false, "Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_ID);

        for (auto& TextureID : m_ColorAttachments)
            glDeleteTextures(1, &TextureID);

        glDeleteTextures(1, &m_DepthAttachment);

        m_ID = m_DepthAttachment = 0u;

        m_ColorAttachments.clear();
    }

    void OpenGLFrameBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
        glViewport(0, 0, m_Specs.Width, m_Specs.Height);
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

        m_Specs.Width = Width;
        m_Specs.Height = Height;

        Invalidate();
    }
}