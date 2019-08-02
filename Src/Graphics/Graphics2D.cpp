#include "Graphics2D.hpp"
#include "Shader.hpp"

#include <glm/gtx/matrix_transform_2d.hpp>

namespace jm
{
	static const char* VertexShaderCode = R"(
layout(location=0) in vec2 position_in;
layout(location=1) in vec2 texCoord_in;
layout(location=2) in vec4 color_in;

out vec2 vTexCoord;
out vec4 vColor;

uniform mat3 transform;

void main()
{
	vTexCoord = texCoord_in;
	vColor = color_in;
	gl_Position = vec4((transform * vec3(position_in, 1.0)).xy, 0.0, 1.0);
}
)";
	
	static const char* FragmentShaderCode = R"(
uniform sampler2D uTexture;

in vec2 vTexCoord;
in vec4 vColor;

layout(location=0) out vec4 color_out;

uniform int redToAlpha;

void main()
{
	color_out = vColor;
	if (redToAlpha == 1)
	{
		color_out.a *= texture(uTexture, vTexCoord).r;
	}
	else
	{
		color_out *= texture(uTexture, vTexCoord);
	}
}
)";
	
	static Shader* shader;
	static int transformUniformLocation;
	static int redToAlphaUniformLocation;
	
	static Texture2D* whitePixelTexture;
	
	void Graphics2D::InitStatic()
	{
		shader = new Shader;
		shader->AddVertexShader(VertexShaderCode);
		shader->AddFragmentShader(FragmentShaderCode);
		shader->Link();
		
		shader->SetUniformI(shader->GetUniformLocation("uTexture"), 0);
		
		transformUniformLocation = shader->GetUniformLocation("transform");
		redToAlphaUniformLocation = shader->GetUniformLocation("redToAlpha");
		
		whitePixelTexture = new Texture2D(1, 1, Format::RGBA8_UNorm, 1);
		const uint32_t whitePixelData = 0xFFFFFFFFU;
		whitePixelTexture->SetData(0, 0, 0, 1, 1, DataType::UInt8Norm, 4, &whitePixelData);
	}
	
	void Graphics2D::DestroyStatic()
	{
		delete shader;
		delete whitePixelTexture;
	}
	
	Graphics2D::Graphics2D()
		: m_vertexBuffer(BufferFlags::VertexBuffer | BufferFlags::AllowUpdate, 0),
		  m_indexBuffer(BufferFlags::IndexBuffer | BufferFlags::AllowUpdate, 0)
	{
		m_vertexLayout.InitAttribute(0, 0, DataType::Float32, 2, offsetof(Vertex, position));
		m_vertexLayout.InitAttribute(1, 0, DataType::Float32, 2, offsetof(Vertex, texCoord));
		m_vertexLayout.InitAttribute(2, 0, DataType::UInt8Norm, 4, offsetof(Vertex, color));
		m_vertexLayout.InitBinding(0, sizeof(Vertex), InputRate::Vertex);
	}
	
	void Graphics2D::Begin()
	{
		m_batches.clear();
		m_indices.clear();
		m_vertices.clear();
	}
	
	void Graphics2D::InitBatch(const Texture2D& texture, bool redToAlpha)
	{
		bool needsNewBatch = true;
		if (!m_batches.empty() && m_batches.back().texture == &texture &&
			m_batches.back().redToAlpha == redToAlpha && m_batches.back().sampler == m_sampler)
		{
			if (!m_batches.back().enableScissor && m_scissorStack.empty())
			{
				needsNewBatch = false;
			}
			else if (m_batches.back().enableScissor && !m_scissorStack.empty() &&
			         m_batches.back().scissor == m_scissorStack.back())
			{
				needsNewBatch = false;
			}
		}
		
		if (needsNewBatch)
		{
			Batch& batch = m_batches.emplace_back();
			batch.redToAlpha = redToAlpha;
			batch.texture = &texture;
			batch.sampler = m_sampler;
			batch.firstIndex = static_cast<uint32_t>(m_indices.size());
			batch.numIndices = 0;
			if ((batch.enableScissor = !m_scissorStack.empty()))
			{
				batch.scissor = m_scissorStack.back();
			}
		}
	}
	
	void Graphics2D::AddQuadIndices()
	{
		uint32_t i0 = m_vertices.size();
		
		uint32_t indices[] = { 0, 1, 2, 1, 2, 3 };
		for (uint32_t i : indices)
		{
			m_indices.push_back(i0 + i);
		}
		
		m_batches.back().numIndices += 6;
	}
	
	void Graphics2D::Sprite(const Texture2D& texture, const glm::vec2& position, const glm::vec4& color,
		const Rectangle& texRectangle, float scale, SpriteFlags flipFlags, float rotation, glm::vec2 origin)
	{
		InitBatch(texture, (uint32_t)flipFlags & (uint32_t)SpriteFlags::RedToAlpha);
		
		AddQuadIndices();
		
		float uOffsets[] = { 0, texRectangle.w };
		float vOffsets[] = { 0, texRectangle.h };
		
		if ((uint32_t)flipFlags & (uint32_t)SpriteFlags::FlipX)
		{
			std::swap(uOffsets[0], uOffsets[1]);
			origin.x = texRectangle.w - origin.x;
		}
		
		if ((uint32_t)flipFlags & (uint32_t)SpriteFlags::FlipY)
		{
			std::swap(vOffsets[0], vOffsets[1]);
			origin.y = texRectangle.h - origin.y;
		}
		
		const float cosR = std::cos(rotation);
		const float sinR = std::sin(rotation);
		
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				const float u = (texRectangle.x + uOffsets[x]) / texture.Width();
				const float v = (texRectangle.y + vOffsets[y]) / texture.Height();
				
				const float offX = texRectangle.w * x - origin.x;
				const float offY = texRectangle.h * y - origin.y;
				const float rOffX = offX * cosR - offY * sinR;
				const float rOffY = offX * sinR + offY * cosR;
				
				m_vertices.emplace_back(position + glm::vec2(rOffX, rOffY) * scale, glm::vec2(u, v), color);
			}
		}
	}
	
	void Graphics2D::Sprite(const Texture2D& texture, const Rectangle& rectangle, const glm::vec4& color,
		const Rectangle& texRectangle, SpriteFlags flipFlags)
	{
		InitBatch(texture, (uint32_t)flipFlags & (uint32_t)SpriteFlags::RedToAlpha);
		
		AddQuadIndices();
		
		float uOffsets[] = { 0, texRectangle.w };
		float vOffsets[] = { texRectangle.h, 0 };
		
		if ((uint32_t)flipFlags & (uint32_t)SpriteFlags::FlipX)
			std::swap(uOffsets[0], uOffsets[1]);
		if ((uint32_t)flipFlags & (uint32_t)SpriteFlags::FlipY)
			std::swap(vOffsets[0], vOffsets[1]);
		
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				const float u = (texRectangle.x + uOffsets[x]) / texture.Width();
				const float v = (texRectangle.y + vOffsets[y]) / texture.Height();
				m_vertices.emplace_back(glm::vec2(rectangle.x + rectangle.w * x, rectangle.y + rectangle.h * y),
					glm::vec2(u, v), color);
			}
		}
	}
	
	void Graphics2D::BorderRect(const Rectangle& rectangle, const glm::vec4& color, float width)
	{
		Line({ rectangle.x, rectangle.y }, { rectangle.MaxX(), rectangle.y }, color, width);
		Line({ rectangle.MaxX(), rectangle.y }, { rectangle.x + rectangle.w, rectangle.MaxY() }, color, width);
		Line({ rectangle.MaxX(), rectangle.MaxY() }, { rectangle.x, rectangle.MaxY() }, color, width);
		Line({ rectangle.x, rectangle.MaxY() }, { rectangle.x, rectangle.y }, color, width);
	}
	
	void Graphics2D::Line(const glm::vec2& begin, const glm::vec2& end, const glm::vec4& color, float width)
	{
		InitBatch(*whitePixelTexture, false);
		
		AddQuadIndices();
		
		glm::vec2 d = glm::normalize(end - begin);
		glm::vec2 dO(d.y, -d.x);
		
		for (int s = 0; s < 2; s++)
		{
			m_vertices.emplace_back(begin + dO * (width * (s * 2 - 1)), glm::vec2(0, 0), color);
		}
		for (int s = 0; s < 2; s++)
		{
			m_vertices.emplace_back(end + dO * (width * (s * 2 - 1)), glm::vec2(0, 0), color);
		}
	}
	
	void Graphics2D::FilledRect(const Rectangle& rectangle, const glm::vec4& color)
	{
		InitBatch(*whitePixelTexture, false);
		
		AddQuadIndices();
		
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				m_vertices.emplace_back(glm::vec2(rectangle.x + rectangle.w * x, rectangle.y + rectangle.h * y),
					glm::vec2(0, 0), color);
			}
		}
	}
	
	void Graphics2D::PushScissor(int x, int y, int width, int height)
	{
		if (m_scissorStack.empty())
		{
			m_scissorStack.push_back({x, y, width, height});
		}
		else
		{
			int ix = std::max(x, m_scissorStack.back().x);
			int iy = std::max(y, m_scissorStack.back().y);
			int iw = std::min(x + width, m_scissorStack.back().x + m_scissorStack.back().width) - ix;
			int ih = std::min(y + height, m_scissorStack.back().y + m_scissorStack.back().height) - iy;
			m_scissorStack.push_back({ ix, iy, iw, ih });
		}
	}
	
	static const BlendState* blendStates[] = 
	{
		&AlphaBlend,
		&AlphaBlendPreMultiplied,
		&AdditiveBlend,
		nullptr
	};
	
	void Graphics2D::End(int screenWidth, int screenHeight, const glm::mat3& matrix, Blend blend)
	{
		if (m_batches.empty())
			return;
		
		SetBlendState(blendStates[(int)blend]);
		
		shader->Bind();
		m_vertexLayout.Bind();
		
		//Reallocates the vertex buffer if it's too small
		if (m_vertexBufferCapacity < m_vertices.size())
		{
			m_vertexBufferCapacity = RoundToNextMultiple<uint32_t>(m_vertices.size(), 1024);
			m_vertexBuffer.Realloc(m_vertexBufferCapacity * sizeof(Vertex), nullptr);
		}
		
		//Reallocates the index buffer if it's too small
		if (m_indexBufferCapacity < m_indices.size())
		{
			m_indexBufferCapacity = RoundToNextMultiple<uint32_t>(m_indices.size(), 1024);
			m_indexBuffer.Realloc(m_indexBufferCapacity * sizeof(uint32_t), nullptr);
		}
		
		m_vertexBuffer.Update(0, m_vertices.size() * sizeof(Vertex), m_vertices.data());
		m_indexBuffer.Update(0, m_indices.size() * sizeof(uint32_t), m_indices.data());
		
		m_vertexLayout.SetVertexBuffer(0, m_vertexBuffer, 0);
		m_vertexLayout.SetIndexBuffer(m_indexBuffer);
		
		shader->SetUniformM3(transformUniformLocation, matrix);
		
		int32_t currentRedToAlpha = -1;
		for (const Batch& batch : m_batches)
		{
			int32_t newRedToAlpha = batch.redToAlpha;
			if (newRedToAlpha != currentRedToAlpha)
			{
				shader->SetUniformI(redToAlphaUniformLocation, batch.redToAlpha);
				currentRedToAlpha = newRedToAlpha;
			}
			
			if (batch.enableScissor)
				SetScissor(batch.scissor.x, batch.scissor.y, batch.scissor.width, batch.scissor.height);
			else
				SetScissor(0, 0, screenWidth, screenHeight);
			
			batch.texture->Bind(0);
			
			BindSampler(batch.sampler, 0);
			
			DrawIndexed(DrawTopology::TriangleList, IndexType::UInt32, batch.firstIndex * sizeof(uint32_t), batch.numIndices);
		}
		
		SetBlendState(nullptr);
	}
	
	void Graphics2D::End(int screenWidth, int screenHeight, Blend blend)
	{
		glm::mat3 transform = glm::translate(glm::mat3(1), glm::vec2(-1)) *
			glm::scale(glm::mat3(1), glm::vec2(2.0f / screenWidth, 2.0f / screenHeight));
		
		End(screenWidth, screenHeight, transform, blend);
	}
	
	Graphics2D::Vertex::Vertex(const glm::vec2& _position, const glm::vec2& _texCoord, const glm::vec4& _color)
		: position(_position), texCoord(_texCoord)
	{
		for (int i = 0; i < 4; i++)
		{
			color[i] = static_cast<uint8_t>(std::round(_color[i] * 255.0f));
		}
	}
	
	glm::mat3 MakeViewMatrix2D(glm::vec2 centerWorld, float zoom, float rotation, int screenWidth, int screenHeight)
	{
		return
			glm::scale(glm::mat3(1), glm::vec2((zoom * 2) / screenWidth, (zoom * 2) / screenHeight)) *
			glm::rotate(glm::mat3(1), rotation) *
			glm::translate(glm::mat3(1), -centerWorld);
	}
	
	glm::mat3 MakeInverseViewMatrix2D(glm::vec2 centerWorld, float zoom, float rotation, int screenWidth, int screenHeight)
	{
		return
			glm::translate(glm::mat3(1), centerWorld) *
			glm::rotate(glm::mat3(1), -rotation) *
			glm::scale(glm::mat3(1), glm::vec2(screenWidth / (zoom * 2), screenHeight / (zoom * 2)));
	}
	
	glm::vec2 UnprojectScreen2D(const glm::mat3& inverseViewMatrix, glm::vec2 screenCoords)
	{
		glm::vec3 ndc(2 * screenCoords.x / CurrentRTWidth() - 1, 1 - 2 * screenCoords.y / CurrentRTHeight(), 1);
		return glm::vec2(inverseViewMatrix * ndc);
	}
}
