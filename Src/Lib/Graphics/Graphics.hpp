#pragma once

#include <glm/glm.hpp>

#include "Texture.hpp"
#include "../API.hpp"

namespace jm
{
	JAPI void ClearColor(const glm::vec4& color);
	JAPI void ClearDepth(float depth = 1.0f);
	
	JAPI void SetRenderTarget(Texture2D* color, Texture2D* depth = nullptr);
}
