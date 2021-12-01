
#include <GL/glew.h>
#include <engineincl.h>

#include "render/renderer.h"
#include "shader.h"

namespace Techless {
	Shader::Shader()
		: CurrentPath(""), RendererID(0) {}

	Shader::Shader(const std::string& Path)
	{
        ImportShader(Path);
	}

    Shader::~Shader() 
    {
        glDeleteProgram(RendererID);
    }

	void Shader::Bind() const 
    {
        glUseProgram(RendererID);
	}

	void Shader::Unbind() const
    {
        glUseProgram(0);
	}

    // Uniforms
	void Shader::SetUniform4f(const std::string& Name, float v0, float v1, float v2, float v3)
    {
        glUniform4f(GetUniformLocation(Name) , v0, v1, v2, v3);
	}
    
    void Shader::SetUniform1i(const std::string& Name, int v0)
    {
        glUniform1i(GetUniformLocation(Name) , v0);
	}
    
    void Shader::SetUniform1iv(const std::string& Name, int* Values, int Size)
    {
        glUniform1iv(GetUniformLocation(Name) , Size, Values);
	}

    void Shader::SetUniformMat4f(const std::string& Name, const glm::mat4& Matrix)
    {
        glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, &Matrix[0][0]);
    }

	unsigned int Shader::GetUniformLocation(const std::string& Name) 
    {
        if (UniformCache.find(Name) != UniformCache.end()) 
            return UniformCache[Name];
        
        int Loc = glGetUniformLocation(RendererID, Name.c_str());

        if (Loc != -1)
            UniformCache[Name] = Loc;
        return Loc;
	}

    // Shader importation and compilation
    void Shader::ImportShader(const std::string& Path)
    {
        auto ShaderSource = ParseShader(Path);
        unsigned int Program = CreateShader(ShaderSource.VertexSource, ShaderSource.FragmentSource);

        RendererID = Program;
        CurrentPath = Path;
    }

    ShaderSource Shader::ParseShader(const std::string& FilePath)
    {
        std::ifstream Stream(FilePath);

        std::string Line;
        std::stringstream ss[2];

        ShaderType Type = ShaderType::None;
        while (getline(Stream, Line))
        {
            if (Line.find("#shader") != std::string::npos)
            {
                if (Line.find("vertex") != std::string::npos)
                    Type = ShaderType::Vertex;
                else if (Line.find("fragment") != std::string::npos)
                    Type = ShaderType::Fragment;
            }
            /*else if (Line.find("uniform") != std::string::npos) 
            {
                auto FirstSpace = Line.find(" ");
                auto Type = Line.substr

                if (FirstSpace) {

                }
            }*/
            else
            {
                ss[(int)Type] << Line << "\n";
            }
        }

        return { ss[0].str(), ss[1].str() };
    }

    unsigned int Shader::CompileShader(unsigned int Type, const std::string& SourceString)
    {
        unsigned int ShaderID = glCreateShader(Type);
        const char* Source = SourceString.c_str();

        glShaderSource(ShaderID, 1, &Source, nullptr);
        glCompileShader(ShaderID);

        int Result;
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

        if (Result == GL_FALSE) {
            int Length;
            glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &Length);

            char* Message = (char*)alloca(Length * sizeof(char));
            glGetShaderInfoLog(ShaderID, Length, &Length, Message);

            std::cout << (Type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader failed to compile: " << Message << std::endl;

            glDeleteShader(ShaderID);

            return 0;
        }

        return ShaderID;
    }

    unsigned int Shader::CreateShader(const std::string& VertexShaderSource, const std::string& FragmentShaderSource)
    {
        unsigned int Program = glCreateProgram();
        unsigned int VertexShader = CompileShader(GL_VERTEX_SHADER, VertexShaderSource);
        unsigned int FragmentShader = CompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);

        glAttachShader(Program, VertexShader);
        glAttachShader(Program, FragmentShader);
        glLinkProgram(Program);
        glValidateProgram(Program);

        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);

        return Program;
    }
}