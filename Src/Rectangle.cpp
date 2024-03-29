#include "Rectangle.hpp"

namespace jm
{
	bool Rectangle::Contains(float px, float py) const
	{
		return px >= x && px <= MaxX() && py >= y && py <= MaxY();
	}
	
	bool Rectangle::Contains(glm::vec2 point) const
	{
		return point.x >= x && point.x <= MaxX() && point.y >= y && point.y <= MaxY();
	}
	
	bool Rectangle::Contains(const Rectangle& other) const
	{
		return x < other.x && y < other.y && MaxX() > other.MaxX() && MaxY() > other.MaxY();
	}
	
	bool Rectangle::Intersects(const Rectangle& other) const
	{
		return !(other.x >= MaxX() ||
			other.MaxX() <= x ||
			other.y >= MaxY() ||
			other.MaxY() <= y);
	}
	
	std::array<Rectangle, 4> Rectangle::Divide() const
	{
		std::array<Rectangle, 4> result;
		
		float divW = w / 2.0f;
		float divH = h / 2.0f;
		
		for (unsigned int i = 0; i < 4; i++)
		{
			result[i].x = x + (i % 2) * divW;
			result[i].y = y + (i / 2) * divH;
			
			result[i].w = divW;
			result[i].h = divH;
		}
		
		return result;
	}
	
	Rectangle Rectangle::CreateCentered(float x, float y, float w, float h)
	{
		return Rectangle(x - (w / 2.0f), y - (h / 2.0f), w, h);
	}
	
	Rectangle Rectangle::Mix(const Rectangle& a, const Rectangle& b, float x)
	{
		return { glm::mix(a.x, b.x, x), glm::mix(a.y, b.y, x), glm::mix(a.w, b.w, x), glm::mix(a.h, b.h, x) };
	}
	
	Rectangle Rectangle::Inflated(float amount) const
	{
		return Rectangle(x - amount, y - amount, w + amount * 2, h + amount * 2);
	}
	
	Rectangle Rectangle::FromMinMax(glm::vec2 min, glm::vec2 max)
	{
		return Rectangle(
			std::min(min.x, max.x),
			std::min(min.y, max.y),
			std::abs(max.x - min.x),
			std::abs(max.y - min.y)
		);
	}
	
	constexpr float SMALL = 1E-3f;
	
	std::pair<bool, float> Rectangle::ClipX(const Rectangle& originRect, const Rectangle& solidRect, float moveX)
	{
		if (originRect.y > solidRect.MaxY() + SMALL || originRect.MaxY() < solidRect.y - SMALL)
			return { false, moveX };
		
		if (moveX < 0 && originRect.x > solidRect.MaxX() && originRect.x + moveX - SMALL < solidRect.MaxX())
		{
			return { true, solidRect.MaxX() - originRect.x + SMALL };
		}
		
		if (moveX > 0 && originRect.MaxX() < solidRect.x && originRect.MaxX() + moveX + SMALL > solidRect.x)
		{
			return { true, solidRect.x - originRect.MaxX() - SMALL };
		}
		
		return { false, moveX };
	}
	
	std::pair<bool, float> Rectangle::ClipY(const Rectangle& originRect, const Rectangle& solidRect, float moveY)
	{
		if (originRect.x > solidRect.MaxX() + SMALL || originRect.MaxX() < solidRect.x - SMALL)
			return { false, moveY };
		
		if (moveY < 0 && originRect.y > solidRect.MaxY() && originRect.y + moveY - SMALL < solidRect.MaxY())
		{
			return { true, solidRect.MaxY() - originRect.y + SMALL };
		}
		
		if (moveY > 0 && originRect.MaxY() < solidRect.y && originRect.MaxY() + moveY + SMALL > solidRect.y)
		{
			return { true, solidRect.y - originRect.MaxY() - SMALL };
		}
		
		return { false, moveY };
	}
}
