#pragma once

#include "../API.hpp"
#include "Graphics.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "VertexLayout.hpp"
#include "Shader.hpp"

#include <optional>

namespace jm
{
	class JAPI GaussianBlur
	{
	public:
		explicit GaussianBlur(int blurRadius, float sigmaFactor = 0.5f);
		
		const Texture2D& RenderBlur(const Texture2D& input);
		
		void RenderBlur(const Texture2D& input, Texture2D* output);
		
	private:
		int m_blurRadius;
		Buffer m_kernelBuffer;
		
		Shader m_shader;
		int m_blurVectorUniformLocation;
		
		VertexLayout m_vertexLayout;
		
		std::optional<Texture2D> m_tempTexture;
		std::optional<Texture2D> m_outputTexture;
	};
}
