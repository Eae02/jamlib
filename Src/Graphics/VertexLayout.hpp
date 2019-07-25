#pragma once

#include "Buffer.hpp"

#include <vector>
#include <bitset>

namespace jm
{
	enum class InputRate
	{
		Vertex = 0,
		Instance = 1
	};
	
	class VertexLayout
	{
	public:
		VertexLayout();
		
		void InitAttribute(int location, int binding, DataType dataType, int components, uint32_t offset);
		
		void InitBinding(int binding, uint32_t stride, InputRate inputRate);
		
		void SetVertexBuffer(int binding, const Buffer& buffer, uint32_t offset);
		
		void SetIndexBuffer(const Buffer& buffer);
		
		void Bind() const;
		
	private:
		detail::GLHandle<detail::GLObjectTypes::VertexArray> m_handle;
		uint32_t m_uniqueID;
		uint32_t m_bindingStride[8];
		
#ifdef __EMSCRIPTEN__
		struct Attrib
		{
			int location;
			int binding;
			uint32_t dataType;
			bool normalized;
			int components;
			uint32_t offset;
		};
		std::vector<Attrib> m_attributes;
		std::bitset<8> m_bindingDivisor;
#endif
	};
}
