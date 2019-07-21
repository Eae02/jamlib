#include <vector>
#include "OpenGL.hpp"
#include "GLHandle.hpp"

namespace jm::detail
{
	template <>
	void DestroyGLObject<GLObjectTypes::Buffer>(uint32_t handle) noexcept
	{
		glDeleteBuffers(1, &handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Program>(uint32_t handle) noexcept
	{
		glDeleteProgram(handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Shader>(uint32_t handle) noexcept
	{
		glDeleteShader(handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::VertexArray>(uint32_t handle) noexcept
	{
		glDeleteVertexArrays(1, &handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Texture>(uint32_t handle) noexcept
	{
		glDeleteTextures(1, &handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Framebuffer>(uint32_t handle) noexcept
	{
		glDeleteFramebuffers(1, &handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Sampler>(uint32_t handle) noexcept
	{
		glDeleteSamplers(1, &handle);
	}
	
	template <>
	void DestroyGLObject<GLObjectTypes::Query>(uint32_t handle) noexcept
	{
		glDeleteQueries(1, &handle);
	}
}
