#pragma once

#include "API.hpp"
#include "Rectangle.hpp"

#include <glm/glm.hpp>
#include <pcg_random.hpp>
#include <random>
#include <functional>

namespace jm
{
	struct ParticleEmitterType
	{
		std::function<glm::vec2(pcg32_fast&)> positionGenerator;
		float directionMin, directionMax;
		float speedMin, speedMax;
		float rotationMin, rotationMax;
		float rotationVelMin, rotationVelMax;
		float sizeMin, sizeMax;
		float finalSizeScaleMin, finalSizeScaleMax;
		float opacityMin, opacityMax;
		float finalOpacityScaleMin, finalOpacityScaleMax;
		float lifeTimeMin, lifeTimeMax;
		glm::vec2 gravity;
		float drag;
		bool blendAdditive = false;
		bool directionToRotation = false;
		float emissionRate = 1;
		
		class Texture2D* texture;
		std::vector<Rectangle> textureAreas;
	};
}
