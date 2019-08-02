#include "Graphics.hpp"
#include "OpenGL.hpp"
#include "Texture.hpp"

#include <iostream>

namespace jm
{
	void ClearColor(const glm::vec4& color)
	{
		glClearBufferfv(GL_COLOR, 0, &color.x);
	}
	
	void ClearDepth(float depth)
	{
		glClearBufferfv(GL_DEPTH, 0, &depth);
	}
	
	static const GLenum topologyTranslation[] = 
	{
		/* TriangleList  */ GL_TRIANGLES,
		/* TriangleStrip */ GL_TRIANGLE_STRIP,
		/* LineList      */ GL_LINES,
		/* LineStrip     */ GL_LINE_STRIP,
		/* LineLoop      */ GL_LINE_LOOP,
		/* PointList     */ GL_POINTS
	};
	
	void Draw(DrawTopology topology, uint32_t firstVertex, uint32_t numVertices, uint32_t numInstances)
	{
		glDrawArraysInstanced(topologyTranslation[(int)topology], firstVertex, numVertices, numInstances);
	}
	
	static const GLenum indexTypeTranslation[] = 
	{
		/* UInt8  */ GL_UNSIGNED_BYTE,
		/* UInt16 */ GL_UNSIGNED_SHORT,
		/* UInt32 */ GL_UNSIGNED_INT,
	};
	
	void DrawIndexed(DrawTopology topology, IndexType indexType, uint32_t indexBufferOffset, uint32_t numIndices,
		uint32_t numInstances)
	{
		glDrawElementsInstanced(topologyTranslation[(int)topology], numIndices, indexTypeTranslation[(int)indexType],
			reinterpret_cast<void*>(indexBufferOffset), numInstances);
	}
	
	int detail::defaultRTWidth = 0;
	int detail::defaultRTHeight = 0;
	
	int detail::currentRTWidth = 0;
	int detail::currentRTHeight = 0;
	
	void SetRenderTarget(Texture2D* color, Texture2D* depth)
	{
		if (color == nullptr)
		{
			detail::currentRTWidth = detail::defaultRTWidth;
			detail::currentRTHeight = detail::defaultRTHeight;
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			const uint32_t depthUID = depth ? depth->UniqueID() : 0;
			
			Texture2D::Framebuffer* framebuffer = nullptr;
			for (Texture2D::Framebuffer& fb : color->m_framebuffers)
			{
				if (fb.depthUID == depthUID)
				{
					framebuffer = &fb;
					break;
				}
			}
			
			if (framebuffer == nullptr)
			{
				framebuffer = &color->m_framebuffers.emplace_back();
				framebuffer->depthUID = depthUID;
				
				GLuint fbo;
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color->Handle(), 0);
				if (depth != nullptr)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->Handle(), 0);
				}
				
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cerr << "Framebuffer incomplete!" << std::endl;
					std::abort();
				}
				
				framebuffer->framebuffer = fbo;
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer.Get());
			}
			
			detail::currentRTWidth = color->Width();
			detail::currentRTHeight = color->Height();
		}
		
		ResetViewport();
		ResetScissor();
	}
	
	glm::vec3 ParseHexColor(uint32_t hex)
	{
		glm::vec3 color;
		color.r = ((hex & 0xFF0000U) >> (8U * 2)) / 255.0f;
		color.g = ((hex & 0x00FF00U) >> (8U * 1)) / 255.0f;
		color.b = ((hex & 0x0000FFU) >> (8U * 0)) / 255.0f;
		return color;
	}
	
	static int currentViewportX = -1;
	static int currentViewportY = -1;
	static int currentViewportW = -1;
	static int currentViewportH = -1;
	
	void SetViewport(int x, int y, int w, int h)
	{
		if (x != currentViewportX || y != currentViewportY || w != currentViewportW || h != currentViewportH)
		{
			glViewport(x, y, w, h);
			currentViewportX = x;
			currentViewportY = y;
			currentViewportW = w;
			currentViewportH = h;
		}
	}
	
	static bool scissorEnabled = false;
	static int currentScissorX = -1;
	static int currentScissorY = -1;
	static int currentScissorW = -1;
	static int currentScissorH = -1;
	
	void SetScissor(int x, int y, int w, int h)
	{
		bool scissorShouldEnable = x != currentViewportX || y != currentViewportY || w != currentViewportW || h != currentViewportH;
		
		if (scissorShouldEnable)
		{
			if (!scissorEnabled)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			if (x != currentScissorX || y != currentScissorY || w != currentScissorW || h != currentScissorH)
			{
				glScissor(x, y, w, h);
				currentScissorX = x;
				currentScissorY = y;
				currentScissorW = w;
				currentScissorH = h;
			}
		}
		else if (scissorEnabled)
		{
			glDisable(GL_SCISSOR_TEST);
		}
	}
	
	const BlendState AlphaBlend =
	{
		BlendFunction::Add,
		BlendFactor::SrcAlpha,
		BlendFactor::OneMinusSrcAlpha
	};
	
	const BlendState AlphaBlendPreMultiplied =
	{
		BlendFunction::Add,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha
	};
	
	const BlendState AdditiveBlend =
	{
		BlendFunction::Add,
		BlendFactor::One,
		BlendFactor::One
	};
	
	inline static GLenum TranslateBlendFunction(BlendFunction blendFunction)
	{
		switch (blendFunction)
		{
		case BlendFunction::Add: return GL_FUNC_ADD;
		case BlendFunction::Subtract: return GL_FUNC_SUBTRACT;
		case BlendFunction::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendFunction::Min: return GL_MIN;
		case BlendFunction::Max: return GL_MAX;
		}
		
		std::abort();
	}
	
	inline static GLenum TranslateBlendFactor(BlendFactor blendFactor)
	{
		switch (blendFactor)
		{
		case BlendFactor::Zero: return GL_ZERO;
		case BlendFactor::One: return GL_ONE;
		case BlendFactor::SrcColor: return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor: return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha: return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor: return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha: return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
		}
		
		std::abort();
	}
	
	static bool isBlendEnabled = false;
	
	void SetBlendState(const BlendState* blendState)
	{
		if (blendState == nullptr)
		{
			if (isBlendEnabled)
				glDisable(GL_BLEND);
			return;
		}
		
		if (!isBlendEnabled)
			glEnable(GL_BLEND);
		
		glBlendEquationSeparate(
			TranslateBlendFunction(blendState->functionRGB),
			TranslateBlendFunction(blendState->functionA)
		);
		
		glBlendFuncSeparate(
			TranslateBlendFactor(blendState->srcFactorRGB),
			TranslateBlendFactor(blendState->dstFactorRGB),
			TranslateBlendFactor(blendState->srcFactorA),
			TranslateBlendFactor(blendState->dstFactorA)
		);
	}
}
