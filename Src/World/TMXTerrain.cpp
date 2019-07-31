#include "TMXTerrain.hpp"
#include "../Asset.hpp"

#include <tinyxml2.h>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace jm
{
	static TileSet LoadTileSetXML(const tinyxml2::XMLElement& tileSetXml, const std::string& name)
	{
		const uint32_t tileWidth  = tileSetXml.UnsignedAttribute("tilewidth", 1);
		const uint32_t tileHeight = tileSetXml.UnsignedAttribute("tileheight", 1);
		const uint32_t spacing    = tileSetXml.UnsignedAttribute("spacing", 0);
		const uint32_t margin     = tileSetXml.UnsignedAttribute("margin", 0);
		
		const tinyxml2::XMLElement* imageEl = tileSetXml.FirstChildElement("image");
		if (imageEl == nullptr)
		{
			Panic(Concat({ "Invalid tile set '", name, "', missing image element." }));
		}
		
		const char* imageName = imageEl->Attribute("source");
		if (imageName == nullptr)
		{
			Panic(Concat({ "Invalid tile set '", name, "', missing image name." }));
		}
		
		std::string fullImageName = Concat({ ParentPath(name, true), imageName });
		const Texture2D& texture = GetAsset<Texture2D>(fullImageName);
		
		TileSet tileSet(texture, tileWidth, tileHeight, margin, margin, spacing, spacing);
		
		//Adds tiles
		for (uint32_t y = 0; y < texture.Height(); y += tileHeight)
		{
			for (uint32_t x = 0; x < texture.Width(); x += tileWidth)
			{
				tileSet.AddTile(x, y, 0);
			}
		}
		
		return tileSet;
	}
	
	TileSet LoadTileSetAsset(gsl::span<const char> fileData, const std::string& name)
	{
		tinyxml2::XMLDocument document;
		if (document.Parse(fileData.data(), fileData.size()) != tinyxml2::XML_SUCCESS)
		{
			Panic(Concat({ "Invalid tile set xml '", name, "': ", document.ErrorStr() }));
		}
		
		return LoadTileSetXML(*document.FirstChildElement("tileset"), name);
	}
	
	static std::vector<char> DecodeLayerDataPlain(const tinyxml2::XMLElement& dataXml, int width, int height)
	{
		std::vector<char> ids(width * height * sizeof(uint32_t), 0);
		uint32_t* idsPtr = reinterpret_cast<uint32_t*>(ids.data());
		
		int remainingIds = width * height;
		for (auto tileEl = dataXml.FirstChildElement("tile"); tileEl && remainingIds > 0;)
		{
			*(idsPtr++) = tileEl->UnsignedAttribute("gid");
			tileEl = tileEl->NextSiblingElement("tile");
			remainingIds--;
		}
		
		return ids;
	}
	
	static std::vector<char> DecodeLayerDataBase64(const tinyxml2::XMLElement& dataXml)
	{
		return Base64Decode(TrimString(dataXml.GetText()));
	}
	
	TMXTerrain TMXTerrain::LoadAsset(gsl::span<const char> fileData, const std::string& name)
	{
		tinyxml2::XMLDocument document;
		if (document.Parse(fileData.data(), fileData.size()) != tinyxml2::XML_SUCCESS)
		{
			Panic(Concat({ "Invalid terrain xml '", name, "': ", document.ErrorStr() }));
		}
		
		auto FormatError = [&] (std::string_view text)
		{
			Panic(Concat({ "Invalid terrain file '", name, "': ", text }));
		};
		
		const tinyxml2::XMLElement& mapElement = *document.FirstChildElement("map");
		
		TMXTerrain terrain;
		terrain.m_mapWidth   = mapElement.IntAttribute("width");
		terrain.m_mapHeight  = mapElement.IntAttribute("height");
		terrain.m_tileWidth  = mapElement.IntAttribute("tilewidth", 1);
		terrain.m_tileHeight = mapElement.IntAttribute("tileheight", 1);
		
		//Loads tile sets
		std::vector<std::pair<int, const TileSet*>> tileSetsByFirstGId;
		for (auto tileSetEl = mapElement.FirstChildElement("tileset"); tileSetEl; tileSetEl = tileSetEl->NextSiblingElement("tileset"))
		{
			int firstGId;
			if (tileSetEl->QueryIntAttribute("firstgid", &firstGId))
				FormatError("Missing 'firstgid'");
			
			const TileSet* tileSet;
			
			const char* sourceName = tileSetEl->Attribute("source");
			if (sourceName == nullptr)
			{
				tileSet = &terrain.m_ownedTileSets.emplace_back(LoadTileSetXML(*tileSetEl, name));
			}
			else
			{
				std::string fullSourceName = Concat({ ParentPath(name, true), sourceName });
				tileSet = &GetAsset<TileSet>(fullSourceName);
			}
			
			tileSetsByFirstGId.emplace_back(firstGId, tileSet);
		}
		std::sort(tileSetsByFirstGId.begin(), tileSetsByFirstGId.end());
		
		//Loads layers
		for (auto el = mapElement.FirstChildElement(); el; el = el->NextSiblingElement())
		{
			bool isTileLayer = std::strcmp(el->Name(), "layer") == 0;
			bool isObjectLayer = std::strcmp(el->Name(), "objectgroup") == 0;
			
			if ((!isTileLayer && !isObjectLayer) || !el->IntAttribute("visible", 1))
				continue;
			
			TMXLayer layer;
			layer.id = el->IntAttribute("id", 0);
			layer.offset.x = el->IntAttribute("offsetx", 0);
			layer.offset.y = el->IntAttribute("offsety", 0);
			
			if (const char* layerName = el->Attribute("name"))
				layer.name = layerName;
			
			if (isTileLayer)
			{
				const tinyxml2::XMLElement* dataEl = el->FirstChildElement("data");
				if (!dataEl)
					continue;
				
				//Loads tile data
				const char* encodingStr = dataEl->Attribute("encoding");
				std::vector<char> data;
				if (encodingStr == nullptr)
				{
					data = DecodeLayerDataPlain(*dataEl, terrain.m_mapWidth, terrain.m_mapHeight);
				}
				else if (std::strcmp(encodingStr, "base64") == 0)
				{
					data = DecodeLayerDataBase64(*dataEl);
				}
				else
				{
					std::cout << "Unknown encoding for layer '" << layer.name << "' in '" << name << "': " << encodingStr << std::endl;
					continue;
				}
				
				//TODO: Decompress data
				
				const uint32_t* tilePtr = reinterpret_cast<uint32_t*>(data.data());
				
				layer.tileMap = TileMap(terrain.m_mapWidth, terrain.m_mapHeight);
				for (int y = terrain.m_mapHeight - 1; y >= 0; y--)
				{
					for (int x = 0; x < terrain.m_mapWidth; x++, tilePtr++)
					{
						static constexpr uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
						static constexpr uint32_t FLIPPED_VERTICALLY_FLAG   = 0x40000000;
						static constexpr uint32_t FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
						
						uint32_t maskedId = *tilePtr & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
						
						auto tileSetIt = std::lower_bound(tileSetsByFirstGId.begin(), tileSetsByFirstGId.end(),
							std::pair<int, const jm::TileSet*>(maskedId, nullptr));
						if (tileSetIt == tileSetsByFirstGId.end())
							continue;
						
						TileFlags tileFlags = TileFlags::None;
						if (*tilePtr & FLIPPED_HORIZONTALLY_FLAG)
							tileFlags |= TileFlags::FlippedX;
						if (*tilePtr & FLIPPED_VERTICALLY_FLAG)
							tileFlags |= TileFlags::FlippedY;
						if (*tilePtr & FLIPPED_DIAGONALLY_FLAG)
							tileFlags |= TileFlags::FlippedDiag;
						
						layer.tileMap->SetTile(x, y, *tileSetIt->second, maskedId - tileSetIt->first, tileFlags);
					}
				}
			}
			else if (isObjectLayer)
			{
				
			}
			
			terrain.m_layers.push_back(std::move(layer));
		}
		
		return terrain;
	}
	
	void TMXTerrain::Draw(const glm::mat3& transform)
	{
		glm::vec2 tileSize = glm::vec2(m_tileWidth, m_tileHeight);
		glm::mat3 fullTransform = glm::scale(transform, tileSize);
		
		for (TMXLayer& layer : m_layers)
		{
			glm::mat3 layerTransform = glm::translate(fullTransform, glm::vec2(layer.offset) / tileSize);
			
			if (layer.tileMap.has_value())
			{
				layer.tileMap->Draw(layerTransform);
			}
		}
	}
	
	void RegisterTiledAssetLoaders()
	{
		RegisterAssetLoader<TMXTerrain>("tmx", &TMXTerrain::LoadAsset);
		RegisterAssetLoader<TileSet>("tsx", &LoadTileSetAsset);
	}
	
	void ParseProperties(TMXPropertySet& set, const void* elementVoid)
	{
		if (elementVoid == nullptr)
			return;
		//const tinyxml2::XMLElement& element = *(const tinyxml2::XMLElement*)elementVoid;
		
		
	}
	
	TMXPropertyValue TMXPropertySet::GetProperty(std::string_view propName) const
	{
		auto it = std::lower_bound(m_properties.begin(), m_properties.end(), propName, [] (const auto& a, const auto& b)
		{
			return a.first < b;
		});
		if (it == m_properties.end() || it->first != propName)
			return std::monostate();
		return it->second;
	}
	
	std::optional<float> TMXPropertySet::GetPropertyFloat(std::string_view propName)
	{
		TMXPropertyValue prop = GetProperty(propName);
		if (float* val = std::get_if<float>(&prop))
			return *val;
		if (int* val = std::get_if<int>(&prop))
			return *val;
		if (bool* val = std::get_if<bool>(&prop))
			return *val;
		
		try
		{
			return std::stof(std::get<std::string>(prop));
		}
		catch (...) { }
		
		return { };
	}
	
	std::optional<int> TMXPropertySet::GetPropertyInt(std::string_view propName)
	{
		TMXPropertyValue prop = GetProperty(propName);
		if (int* val = std::get_if<int>(&prop))
			return *val;
		if (float* val = std::get_if<float>(&prop))
			return std::round(*val);
		if (bool* val = std::get_if<bool>(&prop))
			return *val;
		
		try
		{
			return std::stoi(std::get<std::string>(prop));
		}
		catch (...) { }
		
		return { };
	}
	
	std::optional<std::string> TMXPropertySet::GetPropertyString(std::string_view propName)
	{
		TMXPropertyValue prop = GetProperty(propName);
		if (int* val = std::get_if<int>(&prop))
			return std::to_string(*val);
		if (float* val = std::get_if<float>(&prop))
			return std::to_string(*val);
		if (bool* val = std::get_if<bool>(&prop))
			return *val ? "true" : "false";
		if (std::string* val = std::get_if<std::string>(&prop))
			return *val;
		
		return { };
	}
}
