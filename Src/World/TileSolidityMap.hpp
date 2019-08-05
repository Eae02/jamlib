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
		TileSolidityMap(uint32_t width, uint32_t height, float tileWidth, float tileHeight,
			glm::vec2 offset = glm::vec2(0));
		
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
		
		float ToLocalX(float v) const { return (v - m_offset.x) / m_tileWidth; }
		float ToLocalY(float v) const { return (v - m_offset.y) / m_tileHeight; }
		glm::vec2 ToLocal(glm::vec2 v) const { return (v - m_offset) / glm::vec2(m_tileWidth, m_tileHeight); };
		
		float TileWidth() const
		{
			return m_tileWidth;
		}
		
		float TileHeight() const
		{
			return m_tileHeight;
		}
		
		glm::vec2 Offset() const
		{
			return m_offset;
		}
		
		void DrawCollision(class Graphics2D& gfx) const;
		
	private:
		float m_tileWidth = 1;
		float m_tileHeight = 1;
		glm::vec2 m_offset;
		uint32_t m_width;
		uint32_t m_height;
		std::vector<bool> m_isSolid;
		std::vector<Rectangle> m_hitboxes;
	};
}
