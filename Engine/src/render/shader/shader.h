#pragma once

#include <glm/glm.hpp>
#include <engineincl.h>

namespace Techless {
	struct ShaderSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	enum class ShaderType {
		None = -1,
		Vertex = 0,
		Fragment = 1,
		Compute = 2
	};
	
	class Shader {
	public:
		Shader();
		Shader(const std::string& Path);
		~Shader();

		void ImportShader(const std::string& Path);
		
		void Bind() const;
		void Unbind() const;

		void SetUniform4f(const std::string& Name, float v0, float v1, float v2, float v3);
		void SetUniform1i(const std::string& Name, int v0);
		void SetUniform1iv(const std::string& Name, int* Values, int Size);
		void SetUniformMat4f(const std::string& Name, const glm::mat4& Matrix);
	private:
		unsigned int RendererID;
		std::string CurrentPath;

		std::unordered_map<std::string, int> UniformCache;

	private:
		unsigned int GetUniformLocation(const std::string& Name);

		ShaderSource ParseShader(const std::string& FilePath);
		unsigned int CompileShader(unsigned int Type, const std::string& SourceString);
		unsigned int CreateShader(const std::string& VertexShaderSource, const std::string& FragmentShaderSource);
	};
}