#include "Asset.hpp"
#include "Game.hpp"
#include "Input.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Graphics2D.hpp"

struct Game : jm::Game
{
	Game()
	{
		gfx.SetSampler(&jm::Pixel2DSampler());
	}
	
	void RunFrame(float dt) override
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
		
		gfx.Sprite(jm::GetAsset<jm::Texture2D>("Player.png"), glm::vec2(std::round(rx), 10), glm::vec4(1.0f), 2.0f);
		
		gfx.End();
	}
	
	jm::Graphics2D gfx;
	
	float rx = 10;
	float vx = 0;
};

JM_ENTRY_POINT(Game)
