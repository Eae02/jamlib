#pragma once

#include "API.hpp"

#include <glm/glm.hpp>
#include <chrono>
#include <utility>

namespace jm
{
	class JAPI Camera2D
	{
	public:
		Camera2D();
		
		void Shake(float magnitude, float duration);
		
		glm::mat3 ViewMatrix(glm::vec2 target, float zoom = 1, float rotation = 0) const
		{
			return ViewMatrixAndInverse(target, zoom, rotation).first;
		}
		
		std::pair<glm::mat3, glm::mat3> ViewMatrixAndInverse(glm::vec2 target, float zoom = 1, float rotation = 0) const;
		
		float shakeRoughness;
		float rotationShakeMagnitude;
		
	private:
		float m_shakeMagnitude = 0;
		float m_shakeFalloff = 0;
		std::chrono::high_resolution_clock::time_point m_shakeBeginTime;
	};
}
