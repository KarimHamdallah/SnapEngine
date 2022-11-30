#include "SnapPCH.h"
#include "Shader.h"

#include <Snap/Renderer/RendererAPI.h>
#include <platform/OpenGL/OpenGLShader.h>

namespace SnapEngine
{
	SnapPtr<Shader> Shader::Creat(const std::string& Name, const std::string& VretexShaderSource, const std::string& FragmentShaderSource)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return std::make_shared<OpenGLShader>(Name, VretexShaderSource, FragmentShaderSource);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}

	SnapPtr<Shader> Shader::Creat(const std::string& ShaderFilePath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return std::make_shared<OpenGLShader>(ShaderFilePath);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& Name, const SnapPtr<Shader>& Shader)
	{
		SNAP_ASSERT_MSG(m_Shaders.find(Name) == m_Shaders.end(), "Shader With name" + Name + "is already existed!");
		m_Shaders[Name] = Shader;
	}

	void ShaderLibrary::Add(const SnapPtr<Shader>& Shader)
	{
		std::string name = Shader->getName();
		Add(name, Shader);
	}

	SnapPtr<Shader> ShaderLibrary::Load(const std::string& Name, const std::string& VretexShaderSource, const std::string& FragmentShaderSource)
	{
		SnapPtr<Shader> shader = SnapPtr<Shader>(Shader::Creat(Name, VretexShaderSource, FragmentShaderSource));
		Add(Name, shader);
		return shader;
	}

	SnapPtr<Shader> ShaderLibrary::Load(const std::string& ShaderFilePath)
	{
		SnapPtr<Shader> shader = SnapPtr<Shader>(Shader::Creat(ShaderFilePath));
		Add(shader);
		return shader;
	}

	SnapPtr<Shader> ShaderLibrary::Get(const std::string& Name)
	{
		SNAP_ASSERT_MSG(m_Shaders.find(Name) != m_Shaders.end(), "Shader With name" + Name + "is not existed!");
		return m_Shaders[Name];
	}

	bool ShaderLibrary::Existed(const std::string& Name) const
	{
		return m_Shaders.find(Name) != m_Shaders.end();
	}
}