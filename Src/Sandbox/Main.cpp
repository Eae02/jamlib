#include "../Lib/Game.hpp"
#include "../Lib/Input.hpp"
#include "../Lib/Graphics/Graphics.hpp"
#include "../Lib/Graphics/Graphics2D.hpp"

struct Game : jm::Game
{
	virtual void RunFrame(float dt) override
	{
		if (jm::IsButtonDown(jm::Button::LeftArrow))
			vx -= dt * 200;
		else if (jm::IsButtonDown(jm::Button::RightArrow))
			vx += dt * 200;
		else
			vx -= vx * dt * 10;
		
		vx = glm::clamp<float>(vx, -100, 100);
		
		rx += vx * dt;
		
		jm::ClearColor(glm::vec4(jm::ParseHexColor(0x0066ff), 1));
		
		gfx.Begin();
		
		gfx.DrawRect(jm::Rectangle(rx, 10, 50, 100), glm::vec4(1.0f));
		
		gfx.End();
	}
	
	jm::Graphics2D gfx;
	
	float rx = 10;
	float vx = 0;
};

JM_ENTRY_POINT(Game)
