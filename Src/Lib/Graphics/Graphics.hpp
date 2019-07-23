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
