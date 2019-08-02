#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <future>

#include "../API.hpp"

namespace jm
{
	class JAPI GridPathFinder
	{
	public:
		enum class FindResult
		{
			Success,
			NoPath,
			InvalidSource,
			InvalidDestination
		};
		
		GridPathFinder() = default;
		
		FindResult FindPath(const class TileSolidityMap& map, glm::vec2 localSource, glm::vec2 localDest,
			float maxLength = INFINITY);
		
		const std::vector<glm::vec2>& Path() const
		{
			return m_finalPath;
		}
		
	private:
		struct Node
		{
			float minCost;
			glm::ivec2 prev;
		};
		
		struct HeapNode
		{
			glm::ivec2 node;
			float cost;
			float costWithHeuristic;
			
			bool operator<(const HeapNode& other) const
			{
				return costWithHeuristic > other.costWithHeuristic;
			}
		};
		
		std::vector<HeapNode> m_heap;
		
		std::vector<Node> m_nodes;
		
		std::vector<glm::vec2> m_tempPath;
		std::vector<glm::vec2> m_finalPath;
	};
}
