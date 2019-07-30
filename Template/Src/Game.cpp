struct Game : jm::Game
{
	virtual void RunFrame(float dt) override
	{
		jm::ClearColor(glm::vec4(jm::ParseHexColor(0x0066ff), 1));
	}
};

JM_ENTRY_POINT(Game)
