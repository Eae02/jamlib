#pragma once

#include <variant>
#include <list>
#include <optional>
#include <variant>
#include <gsl/span>

#include "TileMap.hpp"

namespace jm
{
	using TMXPropertyValue = std::variant<std::monostate, bool, int, float, std::string, glm::vec4>;
	
	class JAPI TMXPropertySet
	{
	public:
		TMXPropertySet() = default;
		
		TMXPropertyValue GetProperty(std::string_view propName) const;
		
		std::optional<float> GetPropertyFloat(std::string_view propName) const;
		std::optional<int> GetPropertyInt(std::string_view propName) const;
		std::optional<std::string> GetPropertyString(std::string_view propName) const;
		
	private:
		friend void ParseProperties(TMXPropertySet& set, const void* elementVoid);
		
		std::vector<std::pair<std::string, TMXPropertyValue>> m_properties;
	};
	
	class JAPI TMXLayer
	{
	public:
		friend class TMXTerrain;
		
		int id;
		std::string name;
		glm::ivec2 offset;
		std::optional<TileMap> tileMap;
		bool visible;
		TMXPropertySet properties;
	};
	
	struct TMXShape
	{
		std::string name;
		Rectangle rect;
		TMXPropertySet properties;
	};
	
	class JAPI TMXTerrain
	{
	public:
		static TMXTerrain LoadAsset(gsl::span<const char> fileData, const std::string& name);
		
		void Draw(const glm::mat3& transform);
		
		int MapWidth() const { return m_mapWidth; }
		int MapHeight() const { return m_mapHeight; }
		
		int TileWidth() const { return m_tileWidth; }
		int TileHeight() const { return m_tileHeight; }
		glm::ivec2 TileSize() const { return glm::ivec2(m_tileWidth, m_tileHeight); }
		
		class TileSolidityMap MakeSolidityMap(uint32_t dataMask) const;
		
		const TMXShape* GetShapeByName(std::string_view name) const
		{
			auto span = GetShapesByName(name);
			return span.empty() ? nullptr : &span[0];
		}
		
		gsl::span<const TMXShape> GetShapesByName(std::string_view name) const;
		
		TMXLayer* GetLayerByName(std::string_view name);
		
	private:
		TMXTerrain() = default;
		
		int m_mapWidth;
		int m_mapHeight;
		int m_tileWidth;
		int m_tileHeight;
		
		std::list<TileSet> m_ownedTileSets;
		std::vector<TMXLayer> m_layers;
		
		std::vector<TMXShape> m_shapes;
	};
}
