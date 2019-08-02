#pragma once

#include "../API.hpp"
#include "../Utils.hpp"
#include "Rectangle.hpp"

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace jm
{
	class JAPI TileSolidityMap
	{
	public:
		TileSolidityMap(uint32_t width, uint32_t height);
		
		void Apply(const class TileMap& tileMap, uint32_t dataMask, glm::ivec2 dstOffset = { });
		
		std::pair<bool, float> ClipX(const Rectangle& originRect, float moveX) const;
		std::pair<bool, float> ClipY(const Rectangle& originRect, float moveY) const;
		
		glm::vec2 Clip(const Rectangle& originRect, glm::vec2 move) const
		{
			bool clippedX, clippedY;
			return Clip(originRect, move, clippedX, clippedY);
		}
		
		glm::vec2 Clip(Rectangle originRect, glm::vec2 move, bool& clippedX, bool& clippedY) const;
		
		bool IntersectsSolid(const Rectangle& rectangle) const;
		
		bool LineIntersectsSolid(glm::vec2 start, glm::vec2 end) const;
		
		void SetIsSolid(int x, int y, bool isSolid)
		{
			if (!InRange(x, y))
				Panic("TileSolidityMap::SetIsSolid out of range");
			m_isSolid[y * m_width + x] = isSolid;
		}
		
		void SetIsSolidUnchecked(int x, int y, bool isSolid)
		{
			m_isSolid[y * m_width + x] = isSolid;
		}
		
		bool IsSolid(int x, int y) const
		{
			if (!InRange(x, y))
				return false;
			return m_isSolid[y * m_width + x];
		}
		
		bool IsSolidUnchecked(int x, int y) const
		{
			return m_isSolid[y * m_width + x];
		}
		
		bool InRange(int x, int y) const
		{
			return x >= 0 && y >= 0 && x < (int)m_width && y < (int)m_height;
		}
		
		uint32_t Width() const
		{ return m_width; }
		
		uint32_t Height() const
		{ return m_height; }
		
		float tileWidth = 1;
		float tileHeight = 1;
		glm::vec2 offset;
		
		float ToLocalX(float v) const { return (v - offset.x) / tileWidth; }
		float ToLocalY(float v) const { return (v - offset.y) / tileHeight; }
		glm::vec2 ToLocal(glm::vec2 v) const { return (v - offset) / glm::vec2(tileWidth, tileHeight); };
		
	private:
		uint32_t m_width;
		uint32_t m_height;
		std::vector<bool> m_isSolid;
	};
}
