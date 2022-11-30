#pragma once
#include <Snap/Renderer/Shader.h>

namespace SnapEngine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath, bool isDefault = false);
		OpenGLShader(const std::string& Name, const std::string& VertexShaderSource, const std::string& FragmentShaderSource, bool isDefault = false);

		virtual ~OpenGLShader();

		bool Is_null();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void Delete() override;

		virtual void UploadVec4(const char* varName, const glm::vec4& vec4) const override;
		virtual void UploadVec3(const char* varName, const glm::vec3& vec3) const override;
		virtual void UploadVec2(const char* varName, const glm::vec2& vec2) const override;
		virtual void UploadFloat(const char* varName, float value) const override;
		virtual void UploadInt(const char* varName, int value) const override;
		virtual void UploadIntArray(const char* varName, int length, const int* array) const override;
		virtual void UploadUInt(const char* varName, unsigned int value) const override;

		virtual void UploadMat4(const char* varName, const glm::mat4& mat4) const override;
		virtual void UploadMat3(const char* varName, const glm::mat3& mat3) const override;

		static void ClearAllShaderVariables();

	private:
	};
}