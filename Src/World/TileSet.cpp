#include "TileSet.hpp"

namespace jm
{
	TileID TileSet::AddTile(int x, int y, uint32_t data, const Rectangle& hitbox)
	{
		Tile& tile = m_tiles.emplace_back();
		tile.pixelOffset.x = m_marginX + x * (m_tileWidth + m_spacingX);
		tile.pixelOffset.y = m_marginY + y * (m_tileHeight + m_spacingY);
		tile.data = data;
		tile.hitbox = hitbox;
		return m_tiles.size() - 1;
	}
}
