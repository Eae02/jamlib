#pragma once

#include "../API.hpp"

#include <cstdint>
#include <utility>

namespace jm::detail
{
	enum class GLObjectTypes
	{
		Buffer,
		Program,
		Shader,
		VertexArray,
		Texture,
		Framebuffer,
		Sampler,
		Query
	};
	
	template <GLObjectTypes Type>
	JAPI void DestroyGLObject(uint32_t handle) noexcept;
	
	template <GLObjectTypes Type>
	class GLHandle
	{
	public:
		inline GLHandle()
			: m_handle(-1) { }
		
		inline GLHandle(uint32_t handle) // NOLINT
			: m_handle(handle) { }
		
		inline ~GLHandle() noexcept
		{
			if (m_handle != -1)
			{
				DestroyGLObject<Type>(static_cast<uint32_t>(m_handle));
			}
		}
		
		inline GLHandle(GLHandle&& other) noexcept
		    : m_handle(other.m_handle)
		{
			other.m_handle = -1;
		}
		
		inline GLHandle& operator=(GLHandle&& other) noexcept
		{
			this->~GLHandle();
			m_handle = other.m_handle;
			other.m_handle = -1;
			return *this;
		}
		
		inline void Swap(GLHandle& other)
		{
			std::swap(m_handle, other.m_handle);
		}
		
		inline bool IsNull() const
		{
			return m_handle == -1;
		}
		
		inline uint32_t Get() const
		{
			return static_cast<uint32_t>(m_handle);
		}
		
	private:
		int m_handle;
	};
}
