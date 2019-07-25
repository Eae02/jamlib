#include "VertexLayout.hpp"
#include "OpenGL.hpp"

namespace jm
{
	static uint32_t nextUniqueID = 1;
	static uint32_t currentVAO = 0;
	
	VertexLayout::VertexLayout()
		: m_uniqueID(nextUniqueID++)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		m_handle = vao;
	}
	
	void VertexLayout::Bind() const
	{
		if (currentVAO != m_uniqueID)
		{
			glBindVertexArray(m_handle.Get());
			currentVAO = m_uniqueID;
		}
	}
	
	inline bool IsNormalized(DataType dataType)
	{
		return dataType == DataType::UInt8Norm || dataType == DataType::UInt16Norm || dataType == DataType::UInt32Norm;
	}
	
#ifdef __EMSCRIPTEN__
	void VertexLayout::InitAttribute(int location, int binding, DataType dataType, int components, uint32_t offset)
	{
		Bind();
		glEnableVertexAttribArray(location);
		m_attributes.push_back({ location, binding, detail::GetGLDataType(dataType), IsNormalized(dataType), components, offset });
	}
	
	void VertexLayout::InitBinding(int binding, uint32_t stride, InputRate inputRate)
	{
		m_bindingStride[binding] = stride;
		m_bindingDivisor[binding] = inputRate == InputRate::Instance;
	}
	
	void VertexLayout::SetVertexBuffer(int binding, const Buffer& buffer, uint32_t offset)
	{
		Bind();
		
		glBindBuffer(GL_ARRAY_BUFFER, buffer.Handle());
		
		for (const Attrib& attrib : m_attributes)
		{
			if (attrib.binding == binding)
			{
				glVertexAttribPointer(attrib.location, attrib.components, attrib.dataType, attrib.normalized,
					m_bindingStride[attrib.binding], reinterpret_cast<void*>(offset + attrib.offset));
				glVertexAttribDivisor(attrib.location, m_bindingDivisor[attrib.binding]);
			}
		}
	}
#else
	void VertexLayout::InitAttribute(int location, int binding, DataType dataType, int components, uint32_t offset)
	{
		Bind();
		glEnableVertexAttribArray(location);
		glVertexAttribFormat(location, components, detail::GetGLDataType(dataType), IsNormalized(dataType), offset);
		glVertexAttribBinding(location, binding);
	}
	
	void VertexLayout::InitBinding(int binding, uint32_t stride, InputRate inputRate)
	{
		Bind();
		m_bindingStride[binding] = stride;
		glVertexBindingDivisor(binding, (int)inputRate);
	}
	
	void VertexLayout::SetVertexBuffer(int binding, const Buffer& buffer, uint32_t offset)
	{
		Bind();
		glBindVertexBuffer(binding, buffer.Handle(), offset, m_bindingStride[binding]);
	}
#endif
	
	void VertexLayout::SetIndexBuffer(const Buffer& buffer)
	{
		Bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.Handle());
	}
}
