#include "GridPathFinder.hpp"
#include "TileSolidityMap.hpp"

#include <algorithm>

namespace jm
{
	static const std::pair<glm::ivec2, float> toNeighbors[] = 
	{
		{ glm::ivec2(-1,  0), 1 },
		{ glm::ivec2( 1,  0), 1 },
		{ glm::ivec2( 0, -1), 1 },
		{ glm::ivec2( 0,  1), 1 },
		{ glm::ivec2(-1,  1), M_SQRT2 },
		{ glm::ivec2( 1,  1), M_SQRT2 },
		{ glm::ivec2( 1, -1), M_SQRT2 },
		{ glm::ivec2(-1, -1), M_SQRT2 }
	};
	
	GridPathFinder::FindResult GridPathFinder::FindPath(const TileSolidityMap& map, glm::vec2 source, glm::vec2 dest, float maxLength)
	{
		glm::vec2 localSource = map.ToLocal(source);
		glm::vec2 localDest = map.ToLocal(dest);
		
		const glm::ivec2 sourceI = glm::ivec2(glm::floor(localSource));
		const glm::ivec2 destI = glm::ivec2(glm::floor(localDest));
		
		if (!map.InRange(sourceI.x, sourceI.y) || map.IsSolidUnchecked(sourceI.x, sourceI.y))
		{
			return FindResult::InvalidSource;
		}
		if (!map.InRange(destI.x, destI.y) || map.IsSolidUnchecked(destI.x, destI.y))
		{
			return FindResult::InvalidDestination;
		}
		
		//Creates a heap node for the given grid node and cost to reach that grid node.
		auto MakeHeapNode = [&] (glm::ivec2 node, float cost)
		{
			const glm::ivec2 complete = node - destI;
			const float h = std::sqrt((float)(complete.x * complete.x + complete.y * complete.y));
			return HeapNode { node, cost, cost + h };
		};
		
		m_nodes.resize(map.Width() * map.Height());
		for (Node& node : m_nodes)
			node.minCost = INFINITY;
		
		m_heap.clear();
		m_heap.reserve(m_nodes.size() * 2);
		m_heap.push_back(MakeHeapNode(sourceI, 0));
		m_nodes[sourceI.y * map.Width() + sourceI.x].minCost = 0;
		
		//A*
		while (!m_heap.empty() && m_heap[0].node != destI)
		{
			HeapNode cur = m_heap[0];
			std::pop_heap(m_heap.begin(), m_heap.end());
			m_heap.pop_back();
			
			if (cur.cost > m_nodes[cur.node.y * map.Width() + cur.node.x].minCost)
				continue;
			
			for (std::pair<glm::ivec2, float> toNeighbor : toNeighbors)
			{
				glm::ivec2 n = cur.node + toNeighbor.first;
				if (!map.InRange(n.x, n.y) || map.IsSolidUnchecked(n.x, n.y) ||
				    map.IsSolidUnchecked(cur.node.x, n.y) || map.IsSolidUnchecked(n.x, cur.node.y))
				{
					continue;
				}
				
				uint32_t idx = n.y * map.Width() + n.x;
				float cost = cur.cost + toNeighbor.second;
				if (cost < m_nodes[idx].minCost && cost < maxLength)
				{
					m_nodes[idx].minCost = cost;
					m_nodes[idx].prev = cur.node;
					m_heap.push_back(MakeHeapNode(n, cost));
					std::push_heap(m_heap.begin(), m_heap.end());
				}
			}
		}
		
		if (m_heap[0].node != destI)
			return FindResult::NoPath;
		
		//Constructs a temporary path back
		m_tempPath.clear();
		m_tempPath.push_back(dest);
		for (glm::ivec2 n = m_nodes[destI.y * map.Width() + destI.x].prev; n != sourceI;
			n = m_nodes[n.y * map.Width() + n.x].prev)
		{
			m_tempPath.push_back((glm::vec2(n) + 0.5f) * glm::vec2(map.tileWidth, map.tileHeight) + map.offset);
		}
		m_tempPath.push_back(source);
		
		//Smooths the path back by removing unnecessary points
		m_finalPath.clear();
		m_finalPath.push_back(m_tempPath.back());
		for (int64_t i = (int64_t)m_tempPath.size() - 2; i >= 0; i--)
		{
			if (map.LineIntersectsSolid(m_finalPath.back(), m_tempPath[i]))
			{
				m_finalPath.emplace_back(m_tempPath[i + 1]);
			}
		}
		m_finalPath.emplace_back(m_tempPath[0]);
		
		return FindResult::Success;
	}
}
