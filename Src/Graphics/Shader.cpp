#include "Shader.hpp"
#include "OpenGL.hpp"

#include <vector>
#include <sstream>
#include <iostream>

namespace jm
{
	static uint32_t nextUniqueID = 1;
	static uint32_t currentShader = 0;
	
	Shader::Shader()
		: m_uniqueID(nextUniqueID++)
	{
		m_program = glCreateProgram();
	}
	
#ifdef JM_USE_GLES
	static const char* VS_PREFIX = "#version 300 es\n";
	static const char* FS_PREFIX = "#version 300 es\nprecision highp float;\n";
#else
	static const char* VS_PREFIX = "#version 420 core\n";
	static const char* FS_PREFIX = "#version 420 core\n";
#endif
	
	static inline void AttachShaderStage(GLuint program, GLenum stage, const char* prefix, std::string_view code)
	{
		std::ostringstream sourceStream;
		sourceStream << prefix << "#line 1\n" << code;
		std::string source = sourceStream.str();
		
		GLuint shader = glCreateShader(stage);
		
		const char* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, nullptr);
		
		glCompileShader(shader);
		
		//Checks the shader's compile status.
		GLint compileStatus = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (!compileStatus)
		{
			GLint infoLogLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
			
			std::vector<char> infoLog(static_cast<size_t>(infoLogLen) + 1);
			glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog.data());
			infoLog.back() = '\0';
			
			std::cerr << "Shader failed to compile: " << infoLog.data() << "\nCode:\n" << source;
			std::abort();
		}
		
		glAttachShader(program, shader);
	}
	
	void Shader::AddVertexShader(std::string_view code)
	{
		AttachShaderStage(m_program.Get(), GL_VERTEX_SHADER, VS_PREFIX, code);
	}
	
	void Shader::AddFragmentShader(std::string_view code)
	{
		AttachShaderStage(m_program.Get(), GL_FRAGMENT_SHADER, FS_PREFIX, code);
	}
	
	void Shader::Link()
	{
		glLinkProgram(m_program.Get());
		
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(m_program.Get(), GL_LINK_STATUS, &linkStatus);
		if (!linkStatus)
		{
			GLint infoLogLen = 0;
			glGetProgramiv(m_program.Get(), GL_INFO_LOG_LENGTH, &infoLogLen);
			
			std::vector<char> infoLog(static_cast<size_t>(infoLogLen) + 1);
			glGetProgramInfoLog(m_program.Get(), infoLogLen, nullptr, infoLog.data());
			infoLog.back() = '\0';
			
			std::cerr << "Shader program failed to link:  " << infoLog.data() << "\n";
			std::abort();
		}
	}
	
	int Shader::GetUniformLocation(const char* name) const
	{
		return glGetUniformLocation(m_program.Get(), name);
	}
	
	void Shader::Bind() const
	{
		if (currentShader != m_uniqueID)
		{
			glUseProgram(m_program.Get());
			currentShader = m_uniqueID;
		}
	}
	
	void Shader::SetUniformBlockBinding(const char* name, int binding)
	{
		int index = glGetUniformBlockIndex(m_program.Get(), name);
		if (index == -1)
			std::cerr << "Uniform block " << name << " not found.";
		else
			glUniformBlockBinding(m_program.Get(), index, binding);
	}
	
	static PFNGLUNIFORM1FVPROC setUniformFProc[5];
	static PFNGLUNIFORM1IVPROC setUniformIProc[5];
	static PFNGLUNIFORM1UIVPROC setUniformUProc[5];
	
	void InitSetUniform()
	{
		setUniformFProc[1] = glUniform1fv;
		setUniformFProc[2] = glUniform2fv;
		setUniformFProc[3] = glUniform3fv;
		setUniformFProc[4] = glUniform4fv;
		
		setUniformIProc[1] = glUniform1iv;
		setUniformIProc[2] = glUniform2iv;
		setUniformIProc[3] = glUniform3iv;
		setUniformIProc[4] = glUniform4iv;
		
		setUniformUProc[1] = glUniform1uiv;
		setUniformUProc[2] = glUniform2uiv;
		setUniformUProc[3] = glUniform3uiv;
		setUniformUProc[4] = glUniform4uiv;
	}
	
	void SetUniformF(int location, int components, int elements, const float* value)
	{
		setUniformFProc[components](location, elements, value);
	}
	
	void SetUniformI(int location, int components, int elements, const int32_t* value)
	{
		setUniformIProc[components](location, elements, value);
	}
	
	void SetUniformU(int location, int components, int elements, const uint32_t* value)
	{
		setUniformUProc[components](location, elements, value);
	}
	
	void SetUniformM2(int location, gsl::span<const glm::mat2> matrices)
	{
		glUniformMatrix2fv(location, matrices.size(), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
	}
	
	void SetUniformM3(int location, gsl::span<const glm::mat3> matrices)
	{
		glUniformMatrix3fv(location, matrices.size(), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
	}
	
	void SetUniformM4(int location, gsl::span<const glm::mat4> matrices)
	{
		glUniformMatrix4fv(location, matrices.size(), GL_FALSE, reinterpret_cast<const float*>(matrices.data()));
	}
}
