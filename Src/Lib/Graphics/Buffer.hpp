#pragma once

#include "GLHandle.hpp"
#include "Format.hpp"
#include "../API.hpp"
#include "../Utils.hpp"

namespace jm
{
	enum class BufferFlags
	{
		None          = 0,
		VertexBuffer  = 1,
		IndexBuffer   = 2,
		UniformBuffer = 4,
		AllowUpdate   = 8
	};
	
	JM_BIT_FIELD(BufferFlags)
	
	class JAPI Buffer
	{
	public:
		Buffer(BufferFlags flags, uint64_t size, const void* data = nullptr);
		
		void Realloc(uint64_t size, const void* data = nullptr);
		
		void Update(uint64_t offset, uint64_t range, const void* data);
		
		void BindUBO(int binding) const;
		
		uint32_t Handle() const { return m_handle.Get(); }
		
		uint64_t Size() const { return m_size; }
		
	private:
		detail::GLHandle<detail::GLObjectTypes::Buffer> m_handle;
		uint64_t m_size;
		uint32_t m_target;
		uint32_t m_glUsageFlags;
	};
}
