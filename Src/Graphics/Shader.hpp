#pragma once

#include "GLHandle.hpp"
#include "../API.hpp"

#include <string_view>
#include <string>
#include <gsl/span>
#include <glm/glm.hpp>

namespace jm
{
	class JAPI Shader
	{
	public:
		Shader();
		
		void AddVertexShader(std::string_view code);
		void AddFragmentShader(std::string_view code);
		
		void Link();
		
		void Bind() const;
		
		int GetUniformLocation(const std::string& name) const
		{
			return GetUniformLocation(name.c_str());
		}
		
		int GetUniformLocation(const char* name) const;
		
		void SetUniformBlockBinding(const std::string& name, int binding)
		{
			SetUniformBlockBinding(name.c_str(), binding);
		}
		
		void SetUniformBlockBinding(const char* name, int binding);
		
	private:
		detail::GLHandle<detail::GLObjectTypes::Program> m_program;
		uint32_t m_uniqueID;
	};
	
	JAPI void SetUniformF(int location, int components, int elements, const float* value);
	JAPI void SetUniformI(int location, int components, int elements, const int32_t* value);
	JAPI void SetUniformU(int location, int components, int elements, const uint32_t* value);
	
	JAPI void SetUniformM2(int location, gsl::span<const glm::mat2> matrices);
	JAPI void SetUniformM3(int location, gsl::span<const glm::mat3> matrices);
	JAPI void SetUniformM4(int location, gsl::span<const glm::mat4> matrices);
	
	inline void SetUniformF(int location, float value)
	{
		SetUniformF(location, 1, 1, &value);
	}
	
	inline void SetUniformF(int location, const glm::vec2& value)
	{
		SetUniformF(location, 2, 1, &value.x);
	}
	
	inline void SetUniformF(int location, const glm::vec3& value)
	{
		SetUniformF(location, 3, 1, &value.x);
	}
	
	inline void SetUniformF(int location, const glm::vec4& value)
	{
		SetUniformF(location, 4, 1, &value.x);
	}
	
	inline void SetUniformI(int location, int32_t value)
	{
		SetUniformI(location, 1, 1, &value);
	}
	
	inline void SetUniformI(int location, const glm::ivec2& value)
	{
		SetUniformI(location, 2, 1, &value.x);
	}
	
	inline void SetUniformI(int location, const glm::ivec3& value)
	{
		SetUniformI(location, 3, 1, &value.x);
	}
	
	inline void SetUniformI(int location, const glm::ivec4& value)
	{
		SetUniformI(location, 4, 1, &value.x);
	}
	
	inline void SetUniformU(int location, uint32_t value)
	{
		SetUniformU(location, 1, 1, &value);
	}
	
	inline void SetUniformU(int location, const glm::uvec2& value)
	{
		SetUniformU(location, 2, 1, &value.x);
	}
	
	inline void SetUniformU(int location, const glm::uvec3& value)
	{
		SetUniformU(location, 3, 1, &value.x);
	}
	
	inline void SetUniformU(int location, const glm::uvec4& value)
	{
		SetUniformU(location, 4, 1, &value.x);
	}
	
	inline void SetUniformM2(int location, const glm::mat2& matrix)
	{
		SetUniformM2(location, gsl::make_span(&matrix, 1));
	}
	
	inline void SetUniformM3(int location, const glm::mat3& matrix)
	{
		SetUniformM3(location, gsl::make_span(&matrix, 1));
	}
	
	inline void SetUniformM4(int location, const glm::mat4& matrix)
	{
		SetUniformM4(location, gsl::make_span(&matrix, 1));
	}
}
