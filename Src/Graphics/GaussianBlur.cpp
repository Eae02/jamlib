#include "GaussianBlur.hpp"

#include <sstream>

namespace jm
{
	static const char* FS_SOURCE = R"(
in vec2 vTexCoord;

out vec4 color_out;

uniform float uKernel[BLUR_RADIUS + 1];
uniform vec2 uBlurVector;

uniform sampler2D inputTex;

void main()
{
	color_out = vec4(0.0);
	for (int i = -BLUR_RADIUS; i <= BLUR_RADIUS; i++)
	{
		color_out += texture(inputTex, vTexCoord + uBlurVector * float(i)) * uKernel[abs(i)];
	}
}
)";
	
	GaussianBlur::GaussianBlur(int blurRadius, float sigmaFactor)
	{
		std::ostringstream fsStream;
		fsStream << "#define BLUR_RADIUS " << blurRadius << FS_SOURCE;
		std::string fsCode = fsStream.str();
		
		m_shader.AddVertexShader(FullscreenQuadVS);
		m_shader.AddFragmentShader(fsCode);
		m_shader.Link();
		
		m_shader.SetUniformI(m_shader.GetUniformLocation("inputTex"), 0);
		
		float sigma = sigmaFactor * blurRadius;
		float s = -0.5f / (sigma * sigma);
		float sum = 0.0f;
		
		std::vector<float> kernel(blurRadius + 1);
		
		for (int x = -blurRadius; x <= blurRadius; x++)
		{
			float val = exp(x * x * s);
			sum += val;
			if (x >= 0)
				kernel[x] = val;
		}
		
		for (float& v : kernel)
		{
			v /= sum;
		}
		
		m_shader.SetUniformF(m_shader.GetUniformLocation("uKernel"), 1, kernel.size(), kernel.data());
		
		m_blurVectorUniformLocation = m_shader.GetUniformLocation("uBlurVector");
	}
	
	const Texture2D& GaussianBlur::RenderBlur(const Texture2D& input)
	{
		if (!m_outputTexture.has_value() || m_outputTexture->Width() != input.Width() || m_outputTexture->Height() != input.Height())
		{
			m_outputTexture = Texture2D(input.Width(), input.Height(), input.GetFormat(), 1);
		}
		
		RenderBlur(input, &*m_outputTexture);
		return *m_outputTexture;
	}
	
	void GaussianBlur::RenderBlur(const Texture2D& input, Texture2D* output)
	{
		if (!m_tempTexture.has_value() || m_tempTexture->Width() != input.Width() || m_tempTexture->Height() != input.Height())
		{
			m_tempTexture = Texture2D(input.Width(), input.Height(), input.GetFormat(), 1);
		}
		
		m_shader.Bind();
		m_vertexLayout.Bind();
		
		SetRenderTarget(&*m_tempTexture);
		input.Bind(0);
		m_shader.SetUniformF(m_blurVectorUniformLocation, glm::vec2(1.0f / input.Width(), 0));
		Draw(DrawTopology::TriangleList, 0, 3);
		
		SetRenderTarget(output);
		m_tempTexture->Bind(0);
		m_shader.SetUniformF(m_blurVectorUniformLocation, glm::vec2(0, 1.0f / input.Height()));
		Draw(DrawTopology::TriangleList, 0, 3);
	}
}
