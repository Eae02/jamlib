#include "Buffer.hpp"
#include "OpenGL.hpp"

namespace jm
{
	Buffer::Buffer(BufferFlags flags, uint64_t size, const void* data)
		: m_size(size)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		m_handle = buffer;
		
		int numIncompFlags = 0;
		
		m_target = GL_ARRAY_BUFFER;
		if (HasFlag(flags, BufferFlags::VertexBuffer))
		{
			numIncompFlags++;
		}
		if (HasFlag(flags, BufferFlags::IndexBuffer))
		{
			m_target = GL_ELEMENT_ARRAY_BUFFER;
			numIncompFlags++;
		}
		if (HasFlag(flags, BufferFlags::UniformBuffer))
		{
			m_target = GL_UNIFORM_BUFFER;
			numIncompFlags++;
		}
		
		if (numIncompFlags > 1)
		{
			std::cerr << "Multiple incompatible buffer flags specified." << std::endl;
			std::abort();
		}
		
		m_glUsageFlags = GL_STATIC_DRAW;
		if (HasFlag(flags, BufferFlags::AllowUpdate))
			m_glUsageFlags = GL_DYNAMIC_DRAW;
		
		if (size != 0)
		{
			Realloc(size, data);
		}
	}
	
	void Buffer::Realloc(uint64_t size, const void* data)
	{
		glBindBuffer(m_target, m_handle.Get());
		glBufferData(m_target, size, data, m_glUsageFlags);
	}
	
	void Buffer::Update(uint64_t offset, uint64_t range, const void* data)
	{
		glBindBuffer(m_target, m_handle.Get());
		glBufferSubData(m_target, offset, range, data);
	}
	
	void Buffer::BindUBO(int binding) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_handle.Get());
	}
}
