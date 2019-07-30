#include "Shader.hpp"
#include "OpenGL.hpp"

#include <vector>
#include <sstream>
#include <iostream>

namespace jm
{
	const char* FullscreenQuadVS = R"(
const vec2 positions[] = vec2[]
(
	vec2(-1, -1),
	vec2(-1,  3),
	vec2( 3, -1)
);

out vec2 vTexCoord;

void main()
{
	gl_Position = vec4(positions[gl_VertexID], 0, 1);
	vTexCoord = gl_Position.xy * 0.5 + 0.5;
}

)";
	
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
			
#ifdef __EMSCRIPTEN__
			for (const auto& callback : m_setUniformCallbacks)
				callback();
			m_setUniformCallbacks.clear();
#endif
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
	
#ifdef __EMSCRIPTEN__
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
	
	template <typename T, typename CB>
	inline void Shader::SetUniformOnBind(int elements, const T* values, CB callback)
	{
		if (currentShader == m_uniqueID)
		{
			callback(values);
		}
		else
		{
			m_setUniformCallbacks.emplace_back([values2 = std::vector<T>(values, values + elements), callback2 = std::move(callback)]
			{
				callback2(values2.data());
			});
		}
	}
	
	void Shader::SetUniformF(int location, int components, int elements, const float* value)
	{
		SetUniformOnBind(elements, value, [=] (const float* value2)
		{
			setUniformFProc[components](location, elements, value2);
		});
	}
	
	void Shader::SetUniformI(int location, int components, int elements, const int32_t* value)
	{
		SetUniformOnBind(elements, value, [=] (const int32_t* value2)
		{
			setUniformIProc[components](location, elements, value2);
		});
	}
	
	void Shader::SetUniformU(int location, int components, int elements, const uint32_t* value)
	{
		SetUniformOnBind(elements, value, [=] (const uint32_t* value2)
		{
			setUniformUProc[components](location, elements, value2);
		});
	}
	
	void Shader::SetUniformM2(int location, gsl::span<const glm::mat2> matrices)
	{
		SetUniformOnBind(matrices.size(), matrices.data(), [location, size=matrices.size()] (const glm::mat2* matrices2)
		{
			glUniformMatrix2fv(location, size, GL_FALSE, reinterpret_cast<const float*>(matrices2));
		});
	}
	
	void Shader::SetUniformM3(int location, gsl::span<const glm::mat3> matrices)
	{
		SetUniformOnBind(matrices.size(), matrices.data(), [location, size=matrices.size()] (const glm::mat3* matrices2)
		{
			glUniformMatrix3fv(location, size, GL_FALSE, reinterpret_cast<const float*>(matrices2));
		});
	}
	
	void Shader::SetUniformM4(int location, gsl::span<const glm::mat4> matrices)
	{
		SetUniformOnBind(matrices.size(), matrices.data(), [location, size=matrices.size()] (const glm::mat4* matrices2)
		{
			glUniformMatrix4fv(location, size, GL_FALSE, reinterpret_cast<const float*>(matrices2));
		});
	}
#else
	static PFNGLPROGRAMUNIFORM1FVPROC setUniformFProc[5];
	static PFNGLPROGRAMUNIFORM1IVPROC setUniformIProc[5];
	static PFNGLPROGRAMUNIFORM1UIVPROC setUniformUProc[5];
	
	void InitSetUniform()
	{
		setUniformFProc[1] = glProgramUniform1fv;
		setUniformFProc[2] = glProgramUniform2fv;
		setUniformFProc[3] = glProgramUniform3fv;
		setUniformFProc[4] = glProgramUniform4fv;
		
		setUniformIProc[1] = glProgramUniform1iv;
		setUniformIProc[2] = glProgramUniform2iv;
		setUniformIProc[3] = glProgramUniform3iv;
		setUniformIProc[4] = glProgramUniform4iv;
		
		setUniformUProc[1] = glProgramUniform1uiv;
		setUniformUProc[2] = glProgramUniform2uiv;
		setUniformUProc[3] = glProgramUniform3uiv;
		setUniformUProc[4] = glProgramUniform4uiv;
	}
	
	void Shader::SetUniformF(int location, int components, int elements, const float* value)
	{
		setUniformFProc[components](m_program.Get(), location, elements, value);
	}
	
	void Shader::SetUniformI(int location, int components, int elements, const int32_t* value)
	{
		setUniformIProc[components](m_program.Get(), location, elements, value);
	}
	
	void Shader::SetUniformU(int location, int components, int elements, const uint32_t* value)
	{
		setUniformUProc[components](m_program.Get(), location, elements, value);
	}
	
	void Shader::SetUniformM2(int location, gsl::span<const glm::mat2> matrices)
	{
		glProgramUniformMatrix2fv(m_program.Get(), location, matrices.size(), GL_FALSE,
		                          reinterpret_cast<const float*>(matrices.data()));
	}
	
	void Shader::SetUniformM3(int location, gsl::span<const glm::mat3> matrices)
	{
		glProgramUniformMatrix3fv(m_program.Get(), location, matrices.size(), GL_FALSE,
		                          reinterpret_cast<const float*>(matrices.data()));
	}
	
	void Shader::SetUniformM4(int location, gsl::span<const glm::mat4> matrices)
	{
		glProgramUniformMatrix4fv(m_program.Get(), location, matrices.size(), GL_FALSE,
		                          reinterpret_cast<const float*>(matrices.data()));
	}
#endif
}
