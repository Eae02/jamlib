#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include "../API.hpp"
#include "../Rectangle.hpp"

namespace jm
{
	using TileID = uint32_t;
	
	struct Tile
	{
		glm::ivec2 pixelOffset;
		uint32_t data;
		Rectangle hitbox;
	};
	
	class JAPI TileSet
	{
	public:
		TileSet(const class Texture2D& texture, int tileWidth, int tileHeight,
			int marginX = 0, int marginY = 0, int spacingX = 0, int spacingY = 0)
		    : m_texture(&texture), m_tileWidth(tileWidth), m_tileHeight(tileHeight),
		      m_marginX(marginX), m_marginY(marginY), m_spacingX(spacingX), m_spacingY(spacingY) { }
		
		TileID AddTile(int x, int y, uint32_t data)
		{
			return AddTile(x, y, data, Rectangle(0, 0, (float)m_tileWidth, (float)m_tileHeight));
		}
		
		TileID AddTile(int x, int y, uint32_t data, const Rectangle& hitbox);
		
		const Tile& GetTile(TileID id) const
		{
			return m_tiles.at(id);
		}
		
		int TileWidth() const { return m_tileWidth; }
		int TileHeight() const { return m_tileHeight; }
		
		int MarginX() const { return m_marginX; }
		int MarginY() const { return m_marginY; }
		
		int SpacingX() const { return m_spacingX; }
		int SpacingY() const { return m_spacingY; }
		
		size_t NumTiles() const { return m_tiles.size(); }
		
		const class Texture2D& GetTexture() const { return *m_texture; }
		
	private:
		const class Texture2D* m_texture;
		
		int m_tileWidth;
		int m_tileHeight;
		int m_marginX;
		int m_marginY;
		int m_spacingX;
		int m_spacingY;
		
		std::vector<Tile> m_tiles;
	};
}
