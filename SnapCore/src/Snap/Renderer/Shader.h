#pragma once
#include <Snap/Core/Core.h>
#include <glm/glm.hpp>

namespace SnapEngine
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void Delete() = 0;

		// get
		inline unsigned int getID() const { return m_ID; }
		inline int getStartIndex() const { return StartIndex; }
		inline bool getIsDefault() const { return IsDefault; }
		inline std::string getFilePath() const { return Filepath; }
		inline std::string getName() const { return name; }

		// set
		inline void setID(unsigned int id) { m_ID = id; }
		inline void setIsDefault(bool is_default) { IsDefault = is_default; }
		inline void setName(std::string _name) { name = _name; }

		virtual void UploadVec4(const char* varName, const glm::vec4& vec4) const = 0;
		virtual void UploadVec3(const char* varName, const glm::vec3& vec3) const = 0;
		virtual void UploadVec2(const char* varName, const glm::vec2& vec2) const = 0;
		virtual void UploadFloat(const char* varName, float value) const = 0;
		virtual void UploadInt(const char* varName, int value) const = 0;
		virtual void UploadIntArray(const char* varName, int length, const int* array) const = 0;
		virtual void UploadUInt(const char* varName, unsigned int value) const = 0;
		virtual void UploadMat4(const char* varName, const glm::mat4& mat4) const = 0;
		virtual void UploadMat3(const char* varName, const glm::mat3& mat3) const = 0;

		static SnapPtr<Shader> Creat(const std::string& Name, const std::string& VretexShaderSource, const std::string& FragmentShaderSource);
		static SnapPtr<Shader> Creat(const std::string& ShaderFilePath);

	protected:
		uint32_t m_ID = 0u;
		int StartIndex = -1; // This is the start index in the global shader variables vector
		bool IsDefault = false;
		std::string Filepath;
		std::string name;
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& Name, const SnapPtr<Shader>& Shader);
		void Add(const SnapPtr<Shader>& Shader);


		SnapPtr<Shader> Load(const std::string& Name, const std::string& VretexShaderSource, const std::string& FragmentShaderSource);
		SnapPtr<Shader> Load(const std::string& ShaderFilePath);
		
		SnapPtr<Shader> Get(const std::string& Name);

		bool Existed(const std::string& Name) const;
	private:
		std::unordered_map<std::string, SnapPtr<Shader>> m_Shaders;
	};
}