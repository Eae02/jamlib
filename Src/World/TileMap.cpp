#include "TileMap.hpp"
#include "../Graphics/Texture.hpp"
#include "../Graphics/Shader.hpp"
#include "../Graphics/Graphics.hpp"

#include <iostream>
#include <cstring>

namespace jm
{
#pragma pack(push, 1)
	struct TileMapVertex
	{
		float pos[2];
		float texCoord[2];
	};
#pragma pack(pop)
	
	TileMap::TileMap(uint32_t width, uint32_t height)
		: m_width(width), m_height(height),
		  m_vertexBuffer(BufferFlags::VertexBuffer | BufferFlags::AllowUpdate, 0),
		  m_indexBuffer(BufferFlags::IndexBuffer | BufferFlags::AllowUpdate, 0)
	{
		m_tileData = std::make_unique<uint32_t[]>(width * height);
		std::memset(m_tileData.get(), 0, width * height * sizeof(uint32_t));
		
		m_vertexLayout.InitBinding(0, sizeof(TileMapVertex), InputRate::Vertex);
		m_vertexLayout.InitAttribute(0, 0, DataType::Float32, 2, offsetof(TileMapVertex, pos));
		m_vertexLayout.InitAttribute(1, 0, DataType::Float32, 2, offsetof(TileMapVertex, texCoord));
	}
	
	void TileMap::SetTile(int x, int y, const jm::TileSet& tileSet, jm::TileID tileID, TileFlags flags)
	{
		if (!InRange(x, y))
			Panic("TileMap::GetTile out of range");
		
		size_t tileSetIdx = m_tileSets.size();
		for (size_t i = 0; i < m_tileSets.size(); i++)
		{
			if (m_tileSets[i].tileSet == &tileSet)
			{
				tileSetIdx = i;
				break;
			}
		}
		
		if (tileSetIdx == m_tileSets.size())
		{
			if (m_tileSets.size() == 254)
			{
				std::cerr << "Too many tilesets, max is 254\n";
				std::abort();
			}
			
			m_tileSets.push_back({ 0, 0, &tileSet });
		}
		
		m_tileData[x + y * m_width] = (tileSetIdx + 1) | ((uint32_t)flags << 8U) | (tileID << 11U);
		m_outOfDate = true;
	}
	
	std::tuple<const TileSet*, TileID, TileFlags> TileMap::GetTile(int x, int y) const
	{
		if (!InRange(x, y))
			Panic("TileMap::GetTile out of range");
		
		const uint32_t data = m_tileData[x + y * m_width];
		
		const uint32_t tileSetIdx = data & 0xFFU;
		if (tileSetIdx == 0)
			return { nullptr, 0, TileFlags::None };
		
		return { m_tileSets[tileSetIdx - 1].tileSet, data >> 11U, (TileFlags)((data >> 8U) & 0b111U) };
	}
	
	void TileMap::BuildMesh()
	{
		if (!m_outOfDate)
			return;
		m_outOfDate = false;
		
		std::vector<TileMapVertex> vertices;
		std::vector<std::vector<uint32_t>> indices(m_tileSets.size());
		
		//Generates vertices and indices
		uint32_t totalIndices = 0;
		uint32_t* tilePtr = m_tileData.get();
		for (uint32_t y = 0; y < m_height; y++)
		{
			for (uint32_t x = 0; x < m_width; x++, tilePtr++)
			{
				uint32_t tile = *tilePtr;
				uint32_t tileSetIdx = (tile & 0xFFU);
				if (tileSetIdx == 0)
					continue;
				tileSetIdx--;
				const TileSet& tileSet = *m_tileSets[tileSetIdx].tileSet;
				
				const glm::ivec2 pixelOffset = tileSet.GetTile(tile >> 11U).pixelOffset;
				const float uvScaleX = 1.0f / tileSet.GetTexture().Width();
				const float uvScaleY = 1.0f / tileSet.GetTexture().Height();
				
				//Initializes UV coordinates
				glm::vec2 uvs[2][2];
				for (int dx = 0; dx < 2; dx++)
				{
					for (int dy = 0; dy < 2; dy++)
					{
						int iy = tileSet.GetTexture().Height() - (pixelOffset.y + dy * tileSet.TileHeight());
						uvs[dx][dy].x = (float)(pixelOffset.x + dx * tileSet.TileWidth()) * uvScaleX;
						uvs[dx][dy].y = (float)iy * uvScaleY;
					}
				}
				
				//Applies tile flipping to the UV coordinates
				if (tile & ((uint32_t)TileFlags::FlippedDiag << 8U))
				{
					std::swap(uvs[1][0], uvs[0][1]);
				}
				if (tile & ((uint32_t)TileFlags::FlippedX << 8U))
				{
					std::swap(uvs[0][0], uvs[1][0]);
					std::swap(uvs[0][1], uvs[1][1]);
				}
				if (!(tile & ((uint32_t)TileFlags::FlippedY << 8U)))
				{
					std::swap(uvs[0][0], uvs[0][1]);
					std::swap(uvs[1][0], uvs[1][1]);
				}
				
				static const uint32_t REL_INDICES[] = { 0, 1, 2, 1, 3, 2 };
				for (uint32_t idx : REL_INDICES)
				{
					indices[tileSetIdx].push_back(vertices.size() + idx);
				}
				totalIndices += std::size(REL_INDICES);
				
				for (int dx = 0; dx < 2; dx++)
				{
					for (int dy = 0; dy < 2; dy++)
					{
						TileMapVertex& vertex = vertices.emplace_back();
						vertex.texCoord[0] = uvs[dx][dy].x;
						vertex.texCoord[1] = uvs[dx][dy].y;
						vertex.pos[0] = (float)(x + dx);
						vertex.pos[1] = (float)(y + dy);
					}
				}
			}
		}
		
		std::vector<uint32_t> compactedIndices(totalIndices);
		uint32_t indicesOutIdx = 0;
		for (size_t i = 0; i < indices.size(); i++)
		{
			std::copy_n(indices[i].begin(), indices[i].size(), &compactedIndices[indicesOutIdx]);
			m_tileSets[i].firstIndex = indicesOutIdx;
			m_tileSets[i].numIndices = indices[i].size();
			indicesOutIdx += indices[i].size();
		}
		
		m_hasAnyGeometry = totalIndices > 0;
		if (totalIndices == 0)
			return;
		
		//Uploads vertices
		const uint64_t reqVertexBufferSize = vertices.size() * sizeof(TileMapVertex);
		if (m_vertexBuffer.Size() < reqVertexBufferSize)
		{
			m_vertexBuffer.Realloc(reqVertexBufferSize, vertices.data());
			m_vertexLayout.SetVertexBuffer(0, m_vertexBuffer, 0);
		}
		else
		{
			m_vertexBuffer.Update(0, reqVertexBufferSize, vertices.data());
		}
		
		//Uploads indices
		const uint64_t reqIndexBufferSize = totalIndices * sizeof(uint32_t);
		if (m_indexBuffer.Size() < reqIndexBufferSize)
		{
			m_indexBuffer.Realloc(reqIndexBufferSize, compactedIndices.data());
			m_vertexLayout.SetIndexBuffer(m_indexBuffer);
		}
		else
		{
			m_indexBuffer.Update(0, reqIndexBufferSize, compactedIndices.data());
		}
	}
	
	const char* TileMap::DefaultVertexShader = R"(
layout(location=0) in vec2 position_in;
layout(location=1) in vec2 texCoord_in;

out vec2 vTexCoord;

uniform mat3 uTransform;

void main()
{
	vTexCoord = texCoord_in;
	gl_Position = vec4((uTransform * vec3(position_in, 1)).xy, 0, 1);
}
)";
	
	static const char* defaultFragmentShader = R"(
in vec2 vTexCoord;

layout(location=0) out vec4 color_out;

uniform sampler2D tileSetTex;

void main()
{
	color_out = texture(tileSetTex, vTexCoord);
}
)";
	
	static Shader* defaultTileMapShader;
	static int transformUniformLocation;
	
	void CreateDefaultTileMapShader()
	{
		defaultTileMapShader = new Shader;
		defaultTileMapShader->AddVertexShader(TileMap::DefaultVertexShader);
		defaultTileMapShader->AddFragmentShader(defaultFragmentShader);
		defaultTileMapShader->Link();
		
		defaultTileMapShader->SetUniformI(defaultTileMapShader->GetUniformLocation("tileSetTex"), 0);
		transformUniformLocation = defaultTileMapShader->GetUniformLocation("uTransform");
	}
	
	void DestroyDefaultTileMapShader()
	{
		delete defaultTileMapShader;
	}
	
	void TileMap::Draw(const glm::mat3& transform)
	{
		defaultTileMapShader->SetUniformM3(transformUniformLocation, transform);
		
		Draw(*defaultTileMapShader);
	}
	
	void TileMap::Draw(const Shader& shader)
	{
		BuildMesh();
		
		if (!m_hasAnyGeometry)
			return;
		
		shader.Bind();
		
		m_vertexLayout.Bind();
		
		for (const TileSetEntry& tileSet : m_tileSets)
		{
			tileSet.tileSet->GetTexture().Bind(0);
			jm::DrawIndexed(DrawTopology::TriangleList, IndexType::UInt32, 0, tileSet.numIndices);
		}
	}
}
