#include "JamLib.hpp"

struct Game : jm::Game
{
	Game()
	{
		emitter = std::make_shared<jm::ParticleEmitter>(jm::GetAsset<jm::ParticleEmitterType>("ParticleEmitter.ype"));
		particleManager.AddEmitter(emitter);
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
		
		particleManager.Update(dt);
		particleManager.Draw(jm::MakeViewMatrix2D(glm::vec2(0), 1, 0));
	}
	
	std::shared_ptr<jm::ParticleEmitter> emitter;
	jm::ParticleManager particleManager;
	
	float rx = 10;
	float vx = 0;
};

JM_ENTRY_POINT(Game)
