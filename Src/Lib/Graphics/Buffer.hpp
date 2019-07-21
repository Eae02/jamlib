#pragma once

#include "GLHandle.hpp"
#include "Format.hpp"
#include "../API.hpp"

namespace jm
{
	class JAPI Buffer
	{
	public:
		enum UsageFlags
		{
			USAGE_VERTEX_BUFFER  = 1,
			USAGE_INDEX_BUFFER   = 2,
			USAGE_UNIFORM_BUFFER = 4,
			USAGE_UPDATE         = 8
		};
		
		Buffer(UsageFlags flags, uint64_t size, const void* data = nullptr);
		
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
