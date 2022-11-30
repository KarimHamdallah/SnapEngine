#include "SnapPCH.h"
#include "OpenGLShader.h"
#include <glad/glad.h>
#include <Snap/Core/asserts.h>

#define STRING_HASH_CODE 31

static unsigned int StringHash(const std::string& s)
{
	unsigned int g = STRING_HASH_CODE;
	unsigned int hash = 0;
	for (int i = 0; i < s.length(); i++)
		hash = g * hash + (int)s[i];

	return hash;
}

namespace SnapEngine
{
	// Internal Structures
	struct ShaderVariable
	{
		std::string Name;
		unsigned int Hash;
		GLint VarLocation;
		unsigned int ShaderProgramId;
	};

	// Internal Variables
	static std::vector<ShaderVariable> m_AllShaderVariables;

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		SNAP_ASSERT_MSG(false, "Unkown shader type >> " + type);
		return 0;
	}

	static GLint GetVariableLocation(const OpenGLShader& shader, const char* varName)
	{
		SNAP_ASSERT_MSG(shader.getStartIndex() >= 0 && shader.getStartIndex() < m_AllShaderVariables.size(), "Invalid shader. Cannot find uniform variable " + std::string(varName) + " on this shader.");
		unsigned int hash = StringHash(varName);

		for (int i = shader.getStartIndex(); i < m_AllShaderVariables.size(); i++)
		{
			const ShaderVariable& shaderVar = m_AllShaderVariables[i];
			if (shaderVar.ShaderProgramId != shader.getID())
			{
				SNAP_WARN("Could not find shader variable {} for shader {}", varName, shader.getFilePath().c_str());
				break;
			}

			if (shaderVar.Hash == hash && shaderVar.Name == varName)
			{
				return shaderVar.VarLocation;
			}
		}

		return -1;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath, bool isDefault)
	{
		int LastSlash = filepath.find_last_of("\//");
		LastSlash = LastSlash == std::string::npos ? 0 : LastSlash + 1;
		int LastDot = filepath.rfind(".");

		int Count = LastDot == std::string::npos ? filepath.size() - LastSlash : LastDot - LastSlash;
		name = filepath.substr(LastSlash, Count);

		// read shader source file
		// creat filestream
		std::ifstream file(filepath.c_str(), std::ios::in);
		// check for sucess
		if (file.fail())
		{
			SNAP_ERROR("[ShaderError]:: Can't open shader source file at path : " + filepath + "\n");
			return;
		}

		// read file into string
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string fileSource = buffer.str();
		file.close();

		std::unordered_map<GLenum, std::string> shaderSources; // vertex , fragment
		const char* type = "#type";
		unsigned int typelen = strlen(type);
		size_t pos = fileSource.find(type, 0);

		while (pos != std::string::npos)
		{
			// find first /n after pos
			unsigned int eol = fileSource.find_first_of("\r\n", pos);
			SNAP_ASSERT_MSG(eol != std::string::npos, "[ShaderError]:: syntax Error you may miss #type \n");
			unsigned int begin = pos + typelen + 1;
			std::string shader_type = fileSource.substr(begin, eol - begin);
			SNAP_ASSERT_MSG(ShaderTypeFromString(shader_type), "Invalid shader type specified.");

			size_t nextLinePos = fileSource.find_first_not_of("\r\n", eol); // to skip any \n or \r prsent
			pos = fileSource.find(type, nextLinePos);
			auto value = (pos == std::string::npos) ? fileSource.size() - 1 : pos - (nextLinePos == std::string::npos ? fileSource.size() - 1 : nextLinePos);
			shaderSources[ShaderTypeFromString(shader_type)] = fileSource.substr(nextLinePos, value);
		}

		GLuint program = glCreateProgram();
		SNAP_ASSERT_MSG(shaderSources.size() <= 2, "Shader source must be less than or equal to 2.");


		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources)
		{
			GLenum shaderType = kv.first;
			const std::string& source = kv.second;

			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(shaderType);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				SNAP_ERROR("{}", infoLog.data());
				SNAP_ASSERT_MSG(false, "Shader compilation failed!");
				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);
				// Don't leak shaders either.
				for (auto id : glShaderIDs)
					glDeleteShader(id);

				SNAP_ERROR("{}", infoLog.data());
				SNAP_ASSERT_MSG(false, "Shader linking failed!");
				return;
			}
		}

		int startIndex = m_AllShaderVariables.size();

		// Get all the active vertex attributes and store them in our map of uniform variable locations
		int numUniforms;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

		int maxCharLength;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
		if (numUniforms > 0 && maxCharLength > 0)
		{
			char* charBuffer = new char[maxCharLength];

			for (int i = 0; i < numUniforms; i++)
			{
				int length, size;
				GLenum type;
				glGetActiveUniform(program, i, maxCharLength, &length, &size, &type, charBuffer);
				GLint varLocation = glGetUniformLocation(program, charBuffer);
				ShaderVariable s = ShaderVariable{ std::string(charBuffer), StringHash(charBuffer), varLocation, program };
				m_AllShaderVariables.push_back(s);
			}

			delete charBuffer;
		}


		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(program, id);


		this->m_ID = program;
		this->StartIndex = startIndex;
		this->IsDefault = isDefault;
		this->Filepath = filepath;
	}

	OpenGLShader::OpenGLShader(const std::string& Name, const std::string& VertexShaderSource, const std::string& FragmentShaderSource, bool isDefault)
	{
		name = Name;
		GLuint program = glCreateProgram();

		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		std::unordered_map<GLenum, std::string> ShaderSources;
		ShaderSources[GL_VERTEX_SHADER] = VertexShaderSource;
		ShaderSources[GL_FRAGMENT_SHADER] = FragmentShaderSource;

		for (auto& src : ShaderSources)
		{
			GLenum shaderType = src.first;
			const std::string& source = src.second;

			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(shaderType);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				SNAP_ERROR("{}", infoLog.data());
				SNAP_ASSERT_MSG(false, "Shader compilation failed!");
				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);
				// Don't leak shaders either.
				for (auto id : glShaderIDs)
					glDeleteShader(id);

				SNAP_ERROR("{}", infoLog.data());
				SNAP_ASSERT_MSG(false, "Shader linking failed!");
				return;
			}
		}

		int startIndex = m_AllShaderVariables.size();

		// Get all the active vertex attributes and store them in our map of uniform variable locations
		int numUniforms;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

		int maxCharLength;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
		if (numUniforms > 0 && maxCharLength > 0)
		{
			char* charBuffer = new char[maxCharLength];

			for (int i = 0; i < numUniforms; i++)
			{
				int length, size;
				GLenum type;
				glGetActiveUniform(program, i, maxCharLength, &length, &size, &type, charBuffer);
				GLint varLocation = glGetUniformLocation(program, charBuffer);
				ShaderVariable s = ShaderVariable{ std::string(charBuffer), StringHash(charBuffer), varLocation, program };
				m_AllShaderVariables.push_back(s);
			}

			delete charBuffer;
		}


		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(program, id);


		this->m_ID = program;
		this->StartIndex = startIndex;
		this->IsDefault = isDefault;
	}

	OpenGLShader::~OpenGLShader()
	{
		Delete();
	}

	void OpenGLShader::Delete()
	{
		glDeleteProgram(m_ID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::UploadVec4(const char* varName, const glm::vec4& vec4) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform4f(varLocation, vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void OpenGLShader::UploadVec3(const char* varName, const glm::vec3& vec3) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform3f(varLocation, vec3.x, vec3.y, vec3.z);
	}

	void OpenGLShader::UploadVec2(const char* varName, const glm::vec2& vec2) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform2f(varLocation, vec2.x, vec2.y);
	}

	void OpenGLShader::UploadFloat(const char* varName, float value) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform1f(varLocation, value);
	}

	void OpenGLShader::UploadInt(const char* varName, int value) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform1i(varLocation, value);
	}

	void OpenGLShader::UploadUInt(const char* varName, unsigned int value) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform1ui(varLocation, value);
	}

	void OpenGLShader::UploadMat4(const char* varName, const glm::mat4& mat4) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniformMatrix4fv(varLocation, 1, GL_FALSE, &mat4[0][0]);
	}

	void OpenGLShader::UploadMat3(const char* varName, const glm::mat3& mat3) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniformMatrix3fv(varLocation, 1, GL_FALSE, &mat3[0][0]);
	}

	void OpenGLShader::UploadIntArray(const char* varName, int length, const int* array) const
	{
		int varLocation = glGetUniformLocation(m_ID, varName);
		glUniform1iv(varLocation, length, array);
	}

	bool OpenGLShader::Is_null()
	{
		return m_ID == -1;
	}

	void OpenGLShader::ClearAllShaderVariables()
	{
		m_AllShaderVariables.clear();
	}
}