#pragma once

#include <memory>
#include "API.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define JM_ENTRY_POINT(game) \
extern "C" void EMSCRIPTEN_KEEPALIVE WebMain() { \
	jm::Init(0, nullptr); \
	jm::Run(new game()); \
}
#else
#define JM_ENTRY_POINT(game) \
int main(int argc, char** argv) { \
	jm::Init(argc - 1, argv + 1); \
	jm::Run(new game()); \
}
#endif

namespace jm
{
	class JAPI Game
	{
	public:
		Game() = default;
		
		virtual ~Game() = default;
		
		virtual void RunFrame(float dt) = 0;
		
		virtual void OnResize(int newWidth, int newHeight) { }
	};
	
	JAPI void Init(int argc, char** argv);
	JAPI void Run(Game* game);
}
