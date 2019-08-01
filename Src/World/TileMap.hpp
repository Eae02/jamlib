#pragma once

#include "TileSet.hpp"
#include "../API.hpp"
#include "../Graphics/VertexLayout.hpp"
#include "../Graphics/Buffer.hpp"

#include <vector>
#include <tuple>
#include <memory>
#include <cstdint>
#include <stdint-gcc.h>

namespace jm
{
	enum class TileFlags
	{
		None        = 0,
		FlippedX    = 1, //Flipped along the x axis
		FlippedY    = 2, //Flipped along the y axis
		FlippedDiag = 4  //Flipped along the antidiagonal
	};
	JM_BIT_FIELD(TileFlags)
	
	class JAPI TileMap
	{
	public:
		TileMap(uint32_t width, uint32_t height);
		
		void SetTile(int x, int y, const TileSet& tileSet, TileID tileID, TileFlags flags = TileFlags::None);
		
		std::tuple<const TileSet*, TileID, TileFlags> GetTile(int x, int y) const;
		
		bool InRange(int x, int y) const
		{
			return x >= 0 && y >= 0 && x < (int)m_width && y < (int)m_height;
		}
		
		void BuildMesh();
		
		void Draw(const glm::mat3& transform);
		
		void Draw(const class Shader& shader);
		
		uint32_t GetWidth() const
		{ return m_width; }
		
		uint32_t GetHeight() const
		{ return m_height; }
		
		static const char* DefaultVertexShader;
		
	private:
		uint32_t m_width;
		uint32_t m_height;
		
		struct TileSetEntry
		{
			uint32_t firstIndex;
			uint32_t numIndices;
			const TileSet* tileSet;
		};
		
		std::vector<TileSetEntry> m_tileSets;
		
		std::unique_ptr<uint32_t[]> m_tileData;
		
		bool m_outOfDate = false;
		bool m_hasAnyGeometry = false;
		
		jm::Buffer m_vertexBuffer;
		jm::Buffer m_indexBuffer;
		jm::VertexLayout m_vertexLayout;
	};
}
