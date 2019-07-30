#pragma once

#include "../API.hpp"
#include "../Rectangle.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "VertexLayout.hpp"
#include "Graphics.hpp"

#include <glm/glm.hpp>

namespace jm
{
	enum class SpriteFlags
	{
		None = 0,
		FlipX = 1,
		FlipY = 2,
		RedToAlpha = 4
	};
	
	JM_BIT_FIELD(SpriteFlags)
	
	class JAPI Graphics2D
	{
	public:
		Graphics2D();
		
		void Begin();
		
		void PushScissor(int x, int y, int width, int height);
		
		void PopScissor()
		{
			m_scissorStack.pop_back();
		}
		
		/**
		 * Adds a sprite to the spritebatch.
		 * @param texture The texture to use for the sprite.
		 * @param position The position of the origin in input space.
		 * @param color Constant color (in linear space) which will be multiplied with the texture color.
		 * @param scale Scale factor.
		 * @param flags Controls sprite texture flipping.
		 * @param rotation Angle of rotation, specified clockwise in radians.
		 * @param origin Sprite origin in texture space.
		 */
		void Sprite(const Texture2D& texture, const glm::vec2& position, const glm::vec4& color, float scale = 1,
			SpriteFlags flags = SpriteFlags::None, float rotation = 0, const glm::vec2& origin = { })
		{
			Sprite(texture, position, color, Rectangle(0, 0, (float)texture.Width(), (float)texture.Height()), scale,
			       flags, rotation, origin);
		}
		
		void Sprite(const Texture2D& texture, const glm::vec2& position, const glm::vec4& color,
			const Rectangle& texRectangle, float scale = 1, SpriteFlags flipFlags = SpriteFlags::None,
			float rotation = 0, glm::vec2 origin = { });
		
		void Sprite(const Texture2D& texture, const Rectangle& rectangle, const glm::vec4& color,
			SpriteFlags flags = SpriteFlags::None)
		{
			Sprite(texture, rectangle, color, Rectangle(0, 0, (float)texture.Width(), (float)texture.Height()), flags);
		}
		
		void Sprite(const Texture2D& texture, const Rectangle& rectangle, const glm::vec4& color,
			const Rectangle& texRectangle, SpriteFlags flags = SpriteFlags::None);
		
		void BorderRect(const Rectangle& rectangle, const glm::vec4& color, float width = 1);
		
		void FilledRect(const Rectangle& rectangle, const glm::vec4& color);
		
		void Line(const glm::vec2& begin, const glm::vec2& end, const glm::vec4& color, float width = 1);
		
		void End()
		{
			End(CurrentRTWidth(), CurrentRTHeight());
		}
		
		void End(const glm::mat3& matrix)
		{
			End(CurrentRTWidth(), CurrentRTHeight(), matrix);
		}
		
		void End(int screenWidth, int screenHeight);
		
		void End(int screenWidth, int screenHeight, const glm::mat3& matrix);
		
		bool Empty() const
		{
			return m_batches.empty();
		}
		
		void SetSampler(const Sampler* sampler)
		{
			m_sampler = sampler;
		}
		
		static void InitStatic();
		static void DestroyStatic();
		
	private:
		void InitBatch(const Texture2D& texture, bool redToAlpha);
		void AddQuadIndices();
		
		struct Vertex
		{
			glm::vec2 position;
			glm::vec2 texCoord;
			uint8_t color[4];
			
			Vertex(const glm::vec2& _position, const glm::vec2& _texCoord, const glm::vec4& _color);
		};
		
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		
		struct ScissorRectangle
		{
			int x;
			int y;
			int width;
			int height;
			
			bool operator==(const ScissorRectangle& rhs) const
			{
				return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
			}
			
			bool operator!=(const ScissorRectangle& rhs) const
			{
				return !(rhs == *this);
			}
		};
		
		struct Batch
		{
			const Texture2D* texture;
			const Sampler* sampler;
			bool redToAlpha;
			uint32_t firstIndex;
			uint32_t numIndices;
			bool enableScissor;
			ScissorRectangle scissor;
		};
		
		const Sampler* m_sampler = nullptr;
		
		std::vector<Batch> m_batches;
		
		std::vector<ScissorRectangle> m_scissorStack;
		
		uint32_t m_vertexBufferCapacity = 0;
		uint32_t m_indexBufferCapacity = 0;
		Buffer m_vertexBuffer;
		Buffer m_indexBuffer;
		
		VertexLayout m_vertexLayout;
	};
	
	JAPI glm::mat3 MakeViewMatrix2D(glm::vec2 centerWorld, float zoom, float rotation, int screenWidth, int screenHeight);
	
	inline glm::mat3 MakeViewMatrix2D(glm::vec2 centerWorld, float zoom, float rotation)
	{
		return MakeViewMatrix2D(centerWorld, zoom, rotation, CurrentRTWidth(), CurrentRTHeight());
	}
}
