#include "TileSolidityMap.hpp"
#include "TileMap.hpp"
#include "../Graphics/Graphics2D.hpp"

#include <queue>

namespace jm
{
	TileSolidityMap::TileSolidityMap(uint32_t width, uint32_t height, float tileWidth, float tileHeight, glm::vec2 offset)
		: m_tileWidth(tileWidth), m_tileHeight(tileHeight), m_offset(offset),
		  m_width(width), m_height(height), m_isSolid(width * height, false), m_hitboxes(width * height)
	{
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				m_hitboxes[y * width + x] = Rectangle(x * tileWidth + offset.x, y * tileHeight + offset.y, tileWidth, tileHeight);
			}
		}
	}
	
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
					
					if (tileSet != nullptr)
					{
						const Tile& tile = tileSet->GetTile(tileId);
						if (tile.data & dataMask)
						{
							SetIsSolid(dst.x, dst.y, true);
							m_hitboxes[dst.y * m_width + dst.x] = jm::Rectangle(
								((float)dst.x + tile.hitbox.x / (float)tileSet->TileWidth()) * m_tileWidth + m_offset.x,
								((float)dst.y + tile.hitbox.y / (float)tileSet->TileHeight()) * m_tileHeight + m_offset.y,
								(m_tileWidth * tile.hitbox.w) / (float)tileSet->TileWidth(),
								(m_tileHeight * tile.hitbox.h) / (float)tileSet->TileHeight()
							);
						}
					}
				}
			}
		}
	}
	
	bool TileSolidityMap::IntersectsSolid(const Rectangle& source) const
	{
		glm::vec2 min = ToLocal(source.Min());
		glm::vec2 max = ToLocal(source.Max());
		
		const int maxX = std::ceil(max.x);
		const int maxY = std::ceil(max.y);
		for (int y = std::floor(min.y); y < maxY; y++)
		{
			for (int x = std::floor(min.x); x < maxX; x++)
			{
				if (IsSolid(x, y) && source.Intersects(m_hitboxes[y * m_width + x]))
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
		if (len > 0.01f)
		{
			glm::vec2 dir = (end - start) / len;
			
			float a = 0;
			while (a < len)
			{
				glm::vec2 pos = start + dir * a;
				assert(InRange(std::floor(pos.x), std::floor(pos.y)));
				if (IsSolid(std::floor(pos.x), std::floor(pos.y)))
					return true;
				
				auto CalcStep = [&](float d, float s)
				{
					if (std::abs(d) < 0.001f)
						return INFINITY;
					return ((d > 0 ? (std::floor(s) + 1) : (std::ceil(s) - 1)) - s) / d;
				};
				
				float stepX = CalcStep(dir.x, pos.x);
				float stepY = CalcStep(dir.y, pos.y);
				a += std::min(stepX, stepY) + 0.001f;
			}
		}
		
		return IsSolid(std::floor(end.x), std::floor(end.y));
	}
	
	std::pair<bool, float> TileSolidityMap::ClipX(const Rectangle& originRect, float moveX) const
	{
		glm::vec2 oMin = ToLocal(originRect.Min());
		glm::vec2 oMax = ToLocal(originRect.Max());
		const float localMove = moveX / m_tileWidth;
		float clippedMove = moveX;
		
		bool wasClipped = false;
		
		const int minY = (int)std::floor(oMin.y);
		const int maxY = (int)std::ceil(oMax.y);
		if (moveX > 0)
		{
			const int endX = (int)std::floor(oMax.x + localMove);
			
			for (int x = (int)std::floor(oMax.x); x <= endX; x++)
			{
				for (int y = minY; y < maxY; y++)
				{
					if (IsSolid(x, y))
					{
						auto [rClipped, rClippedMove] = Rectangle::ClipX(originRect, m_hitboxes[x + y * m_width], moveX);
						if (rClipped && std::abs(rClippedMove) < std::abs(clippedMove))
						{
							clippedMove = rClippedMove;
							wasClipped = true;
						}
					}
				}
			}
		}
		else if (moveX < 0)
		{
			const int endX = (int)std::floor(oMin.x + localMove);
			
			for (int x = (int)std::floor(oMin.x); x >= endX; x--)
			{
				for (int y = minY; y < maxY; y++)
				{
					if (IsSolid(x, y))
					{
						auto [rClipped, rClippedMove] = Rectangle::ClipX(originRect, m_hitboxes[x + y * m_width], moveX);
						if (rClipped && std::abs(rClippedMove) < std::abs(clippedMove))
						{
							clippedMove = rClippedMove;
							wasClipped = true;
						}
					}
				}
			}
		}
		
		return std::make_pair(wasClipped, clippedMove);
	}
	
	std::pair<bool, float> TileSolidityMap::ClipY(const Rectangle& originRect, float moveY) const
	{
		glm::vec2 oMin = ToLocal(originRect.Min());
		glm::vec2 oMax = ToLocal(originRect.Max());
		const float localMove = moveY / m_tileHeight;
		float clippedMove = moveY;
		
		bool wasClipped = false;
		
		const int minX = (int)std::floor(oMin.x);
		const int maxX = (int)std::ceil(oMax.x);
		if (moveY > 0)
		{
			const int endY = (int)std::floor(oMax.y + localMove);
			
			for (int y = (int)std::floor(oMax.y); y <= endY; y++)
			{
				for (int x = minX; x < maxX; x++)
				{
					if (IsSolid(x, y))
					{
						auto [rClipped, rClippedMove] = Rectangle::ClipY(originRect, m_hitboxes[x + y * m_width], moveY);
						if (rClipped && std::abs(rClippedMove) < std::abs(clippedMove))
						{
							clippedMove = rClippedMove;
							wasClipped = true;
						}
					}
				}
			}
		}
		else if (moveY < 0)
		{
			const int endY = (int)std::floor(oMin.y + localMove);
			
			for (int y = (int)std::floor(oMin.y); y >= endY; y--)
			{
				for (int x = minX; x < maxX; x++)
				{
					if (IsSolid(x, y))
					{
						auto [rClipped, rClippedMove] = Rectangle::ClipY(originRect, m_hitboxes[x + y * m_width], moveY);
						if (rClipped && std::abs(rClippedMove) < std::abs(clippedMove))
						{
							clippedMove = rClippedMove;
							wasClipped = true;
						}
					}
				}
			}
		}
		
		return std::make_pair(wasClipped, clippedMove);
	}
	
	glm::vec2 TileSolidityMap::Clip(Rectangle originRect, glm::vec2 move, bool& clippedX, bool& clippedY) const
	{
		constexpr float MARGIN = 0.01f;
		
		auto [clippedY2, moveY] = ClipY({ originRect.x + MARGIN, originRect.y, originRect.w - MARGIN * 2, originRect.h }, move.y);
		clippedY = clippedY2;
		
		originRect.y += moveY;
		auto [clippedX2, moveX] = ClipX({ originRect.x, originRect.y + MARGIN, originRect.w, originRect.h - MARGIN * 2 }, move.x);
		clippedX = clippedX2;
		
		return glm::vec2(moveX, moveY);
	}
	
	void TileSolidityMap::DrawCollision(Graphics2D& gfx) const
	{
		for (uint32_t y = 0; y < m_height; y++)
		{
			for (uint32_t x = 0; x < m_width; x++)
			{
				if (IsSolidUnchecked(x, y))
				{
					gfx.BorderRect(m_hitboxes[x + m_width * y], glm::vec4(0.5f, 0, 0, 0.5f), 0.5f);
				}
			}
		}
	}
}
