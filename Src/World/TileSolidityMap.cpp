#include "TileSolidityMap.hpp"
#include "TileMap.hpp"

#include <queue>

namespace jm
{
	TileSolidityMap::TileSolidityMap(uint32_t width, uint32_t height)
		: m_width(width), m_height(height), m_isSolid(width * height, false) { }
	
	void TileSolidityMap::Apply(const TileMap& tileMap, uint32_t dataMask, glm::ivec2 dstOffset)
	{
		for (uint32_t y = 0; y < tileMap.Height(); y++)
		{
			for (uint32_t x = 0; x < tileMap.Width(); x++)
			{
				glm::ivec2 dst = glm::ivec2(x, y) + dstOffset;
				if (InRange(dst.x, dst.y))
				{
					auto [tileSet, tileId, tileFlags] = tileMap.GetTile(x, y);
					
					if (tileSet != nullptr && tileSet->GetTile(tileId).data & dataMask)
					{
						SetIsSolid(x, y, true);
					}
				}
			}
		}
	}
	
	bool TileSolidityMap::IntersectsSolid(const Rectangle& source) const
	{
		const int maxX = std::ceil(ToLocalX(source.MaxX()));
		const int maxY = std::ceil(ToLocalY(source.MaxY()));
		for (int y = std::floor(ToLocalY(source.y)); y < maxY; y++)
		{
			for (int x = std::floor(ToLocalX(source.x)); x < maxX; x++)
			{
				if (IsSolidUnchecked(x, y))
				{
					return true;
				}
			}
		}
		return false;
	}
	
	bool TileSolidityMap::LineIntersectsSolid(glm::vec2 start, glm::vec2 end) const
	{
		start = ToLocal(start);
		end = ToLocal(end);
		
		float len = glm::distance(start, end);
		glm::vec2 dir = (end - start) / len;
		
		float a = 0;
		while (a < len)
		{
			glm::vec2 pos = start + dir * a;
			if (IsSolid(std::floor(pos.x), std::floor(pos.y)))
				return true;
			float stepX = ((dir.x > 0 ? (std::floor(pos.x) + 1) : (std::ceil(pos.x) - 1)) - pos.x) / dir.x;
			float stepY = ((dir.y > 0 ? (std::floor(pos.y) + 1) : (std::ceil(pos.y) - 1)) - pos.y) / dir.y;
			a += std::min(stepX, stepY) + 0.001f;
		}
		
		return IsSolid(std::floor(end.x), std::floor(end.y));
	}
	
	std::pair<bool, float> TileSolidityMap::ClipX(const Rectangle& originRect, float moveX) const
	{
		glm::vec2 oMin = ToLocal(originRect.Min());
		glm::vec2 oMax = ToLocal(originRect.Max());
		moveX /= tileWidth;
		
		bool wasClipped = false;
		
		const int minY = (int)std::floor(oMin.y);
		const int maxY = (int)std::ceil(oMax.y);
		if (moveX > 0)
		{
			const int endX = (int)std::floor(oMax.x + moveX);
			
			for (int x = (int)std::ceil(oMax.x); x <= endX && !wasClipped; x++)
			{
				for (int y = minY; y < maxY; y++)
				{
					if (IsSolid(x, y))
					{
						moveX = (float)x - oMax.x;
						wasClipped = true;
						break;
					}
				}
			}
		}
		else if (moveX < 0)
		{
			const int endX = (int)std::floor(oMin.x + moveX);
			
			for (int x = (int)std::floor(oMin.x) - 1; x >= endX && !wasClipped; x--)
			{
				for (int y = minY; y < maxY; y++)
				{
					if (IsSolid(x, y))
					{
						moveX = (float)(x + 1) - oMin.x;
						wasClipped = true;
						break;
					}
				}
			}
		}
		
		return std::make_pair(wasClipped, moveX * tileWidth);
	}
	
	std::pair<bool, float> TileSolidityMap::ClipY(const Rectangle& originRect, float moveY) const
	{
		glm::vec2 oMin = ToLocal(originRect.Min());
		glm::vec2 oMax = ToLocal(originRect.Max());
		moveY /= tileHeight;
		
		bool wasClipped = false;
		
		const int minX = (int)std::floor(oMin.x);
		const int maxX = (int)std::ceil(oMax.x);
		if (moveY > 0)
		{
			const int endY = (int)std::floor(oMax.y + moveY);
			
			for (int y = (int)std::ceil(oMax.y); y <= endY && !wasClipped; y++)
			{
				for (int x = minX; x < maxX; x++)
				{
					if (IsSolid(x, y))
					{
						moveY = (float)y - oMax.y;
						wasClipped = true;
						break;
					}
				}
			}
		}
		else if (moveY < 0)
		{
			if (oMin.y + moveY < 0)
			{
				moveY = 0 - oMin.y;
				wasClipped = true;
			}
			else
			{
				const int endY = (int)std::floor(oMin.y + moveY);
				
				for (int y = (int)std::floor(oMin.y) - 1; y >= endY && !wasClipped; y--)
				{
					for (int x = minX; x < maxX; x++)
					{
						if (IsSolid(x, y))
						{
							moveY = (float)(y + 1) - oMin.y;
							wasClipped = true;
							break;
						}
					}
				}
			}
		}
		
		return std::make_pair(wasClipped, moveY * tileHeight);
	}
	
	glm::vec2 TileSolidityMap::Clip(Rectangle originRect, glm::vec2 move, bool& clippedX, bool& clippedY) const
	{
		auto [clippedY2, moveY] = ClipY(originRect, move.y);
		clippedY = clippedY2;
		
		originRect.y += moveY;
		auto [clippedX2, moveX] = ClipX(originRect, move.x);
		clippedX = clippedX2;
		
		return glm::vec2(moveX, moveY);
	}
}
