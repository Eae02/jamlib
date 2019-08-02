#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/color_space.hpp>

#include "Texture.hpp"
#include "../API.hpp"

namespace jm
{
	enum class DrawTopology
	{
		TriangleList,
		TriangleStrip,
		LineList,
		LineStrip,
		LineLoop,
		PointList
	};
	
	enum class IndexType
	{
		UInt8,
		UInt16,
		UInt32,
	};
	
	JAPI void ClearColor(const glm::vec4& color);
	JAPI void ClearDepth(float depth = 1.0f);
	
	JAPI void Draw(DrawTopology topology, uint32_t firstVertex, uint32_t numVertices, uint32_t numInstances = 1);
	
	JAPI void DrawIndexed(DrawTopology topology, IndexType indexType, uint32_t indexBufferOffset,
		uint32_t numIndices, uint32_t numInstances = 1);
	
	namespace detail
	{
		JAPI extern int defaultRTWidth;
		JAPI extern int defaultRTHeight;
		JAPI extern int currentRTWidth;
		JAPI extern int currentRTHeight;
	}
	
	inline int DefaultRTWidth() { return detail::defaultRTWidth; }
	inline int DefaultRTHeight() { return detail::defaultRTHeight; }
	inline int CurrentRTWidth() { return detail::currentRTWidth; }
	inline int CurrentRTHeight() { return detail::currentRTHeight; }
	
	JAPI void SetViewport(int x, int y, int w, int h);
	
	inline void ResetViewport()
	{
		SetViewport(0, 0, detail::currentRTWidth, detail::currentRTHeight);
	}
	
	JAPI void SetScissor(int x, int y, int w, int h);
	
	inline void ResetScissor()
	{
		SetScissor(0, 0, detail::currentRTWidth, detail::currentRTHeight);
	}
	
	JAPI void SetRenderTarget(Texture2D* color, Texture2D* depth = nullptr);
	
	enum class BlendFunction
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};
	
	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha
	};
	
	struct BlendState
	{
		BlendFunction functionRGB;
		BlendFunction functionA;
		BlendFactor srcFactorRGB;
		BlendFactor srcFactorA;
		BlendFactor dstFactorRGB;
		BlendFactor dstFactorA;
		
		inline BlendState() noexcept
			: BlendState(BlendFunction::Add, BlendFactor::One, BlendFactor::Zero) { }
		
		inline BlendState(BlendFunction _function, BlendFactor _srcFactor, BlendFactor _dstFactor) noexcept
			: functionRGB(_function), functionA(_function),
			  srcFactorRGB(_srcFactor), srcFactorA(_srcFactor),
			  dstFactorRGB(_dstFactor), dstFactorA(_dstFactor) { }
		
		inline BlendState(BlendFunction _functionRGB, BlendFunction _functionA,
		                  BlendFactor _srcFactorRGB, BlendFactor _srcFactorA,
		                  BlendFactor _dstFactorRGB, BlendFactor _dstFactorA) noexcept
			: functionRGB(_functionRGB), functionA(_functionA),
			  srcFactorRGB(_srcFactorRGB), srcFactorA(_srcFactorA),
			  dstFactorRGB(_dstFactorRGB), dstFactorA(_dstFactorA) { }
		
		inline bool operator==(const BlendState& other) const noexcept
		{
			return functionRGB == other.functionRGB &&
			       functionA == other.functionA &&
			       srcFactorRGB == other.srcFactorRGB &&
			       srcFactorA == other.srcFactorA &&
			       dstFactorRGB == other.dstFactorRGB &&
			       dstFactorA == other.dstFactorA;
		}
		
		inline bool operator!=(const BlendState& other) const noexcept
		{
			return !operator==(other);
		}
	};
	
	JAPI extern const BlendState AlphaBlend;
	JAPI extern const BlendState AlphaBlendPreMultiplied;
	JAPI extern const BlendState AdditiveBlend;
	
	JAPI void SetBlendState(const BlendState* blendState);
	
	JAPI glm::vec3 ParseHexColor(uint32_t hex);
	
	/**
	 * Parses the provided hex color and converts
	 * the result from sRGB to linear color space.
	 * @param hex An RGB hex color
	 */
	inline glm::vec3 ParseHexColorSRGB(uint32_t hex)
	{
		return glm::convertSRGBToLinear(ParseHexColor(hex));
	}
}
