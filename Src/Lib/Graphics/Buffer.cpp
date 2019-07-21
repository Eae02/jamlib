#include "Buffer.hpp"
#include "OpenGL.hpp"

namespace jm
{
	Buffer::Buffer(UsageFlags flags, uint64_t size, const void* data)
		: m_size(size)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		m_handle = buffer;
		
		int numIncompFlags = 0;
		
		m_target = GL_ARRAY_BUFFER;
		if (flags & USAGE_VERTEX_BUFFER)
		{
			numIncompFlags++;
		}
		if (flags & USAGE_INDEX_BUFFER)
		{
			m_target = GL_ELEMENT_ARRAY_BUFFER;
			numIncompFlags++;
		}
		if (flags & USAGE_UNIFORM_BUFFER)
		{
			m_target = GL_UNIFORM_BUFFER;
			numIncompFlags++;
		}
		
		m_glUsageFlags = GL_STATIC_DRAW;
		if (flags & USAGE_UPDATE)
			m_glUsageFlags = GL_DYNAMIC_DRAW;
		
		Realloc(size, data);
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
