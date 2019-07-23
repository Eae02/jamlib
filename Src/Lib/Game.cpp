#include "Game.hpp"
#include "Input.hpp"
#include "Graphics/OpenGL.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Graphics2D.hpp"

#include <SDL.h>
#include <iostream>

namespace jm
{
	static SDL_Window* window;
	static SDL_GLContext glContext;
	static Game* game;
	static bool shouldClose = false;
	static bool firstMouseMotionEvent = true;
	static bool firstControllerAxisEvent = true;
	
	extern SDL_GameController* activeController;
	
	Button TranslateSDLKey(SDL_Scancode scancode);
	Button TranslateSDLControllerButton(int button);
	Button TranslateSDLMouseButton(int button);
	void AddGameController(SDL_GameController* controller);
	
	void InitSetUniform();
	
	extern int defaultRTWidth;
	extern int defaultRTHeight;
	
	void Init(int argc, char** argv)
	{
		if (SDL_Init(SDL_INIT_VIDEO))
		{
			std::cerr << "SDL failed to initialize: " << SDL_GetError() << std::endl;
			std::exit(1);
		}
		
		bool glDebug = false;
		for (int i = 0; i < argc; i++)
		{
			if (std::strcmp(argv[i], "--dbg") == 0)
				glDebug = true;
		}
		
#ifdef __EMSCRIPTEN__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
		uint32_t glContextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
		if (glDebug)
			glContextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, glContextFlags);
#endif
		
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		
		detail::currentIS = new InputState;
		detail::previousIS = new InputState;
		
		glContext = SDL_GL_CreateContext(window);
		if (glContext == nullptr)
		{
			std::cerr << "Error creating OpenGL context: " << SDL_GetError() << "\n";
			std::abort();
		}
		
		detail::InitializeOpenGL(glDebug);
		
		InitSetUniform();
		Graphics2D::InitStatic();
	}
	
	inline void Uninit()
	{
		delete detail::currentIS;
		delete detail::previousIS;
		Graphics2D::DestroyStatic();
	}
	
	void ButtonDownEvent(Button button, bool isRepeat)
	{
		if (!isRepeat && button != Button::Unknown && !detail::currentIS->IsButtonDown(button))
			detail::currentIS->OnButtonDown(button);
	}
	
	void ButtonUpEvent(Button button, bool isRepeat)
	{
		if (!isRepeat && button != Button::Unknown && detail::currentIS->IsButtonDown(button))
			detail::currentIS->OnButtonUp(button);
	}
	
	static std::chrono::high_resolution_clock::time_point lastFrameStart;
	
	void RunOneFrame()
	{
		auto thisFrameStart = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration_cast<std::chrono::nanoseconds>(thisFrameStart - lastFrameStart).count() * 1E-9f;
		lastFrameStart = thisFrameStart;
		
		*detail::previousIS = *detail::currentIS;
		detail::currentIS->cursorDeltaX = 0;
		detail::currentIS->cursorDeltaY = 0;
		detail::inputtedText.clear();
		
		SDL_GL_GetDrawableSize(window, &detail::defaultRTWidth, &detail::defaultRTHeight);
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				shouldClose = true;
				break;
			case SDL_KEYDOWN:
				ButtonDownEvent(TranslateSDLKey(event.key.keysym.scancode), event.key.repeat);
				
				if (RelativeMouseModeActive() && !event.key.repeat &&
				    event.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					bool rel = SDL_GetRelativeMouseMode();
					SDL_SetRelativeMouseMode((SDL_bool)(!rel));
				}
				break;
			case SDL_KEYUP:
				ButtonUpEvent(TranslateSDLKey(event.key.keysym.scancode), event.key.repeat);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				if (SDL_GameControllerFromInstanceID(event.cbutton.which) == activeController)
				{
					ButtonDownEvent(TranslateSDLControllerButton(event.cbutton.button), false);
				}
				break;
			case SDL_CONTROLLERBUTTONUP:
				if (SDL_GameControllerFromInstanceID(event.cbutton.which) == activeController)
				{
					ButtonUpEvent(TranslateSDLControllerButton(event.cbutton.button), false);
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				if (SDL_GameControllerFromInstanceID(event.caxis.which) == activeController)
				{
					const ControllerAxis axis = (ControllerAxis)event.caxis.axis;
					const float valueF = event.caxis.value / SDL_JOYSTICK_AXIS_MAX;
					if (firstControllerAxisEvent)
					{
						detail::previousIS->OnAxisMoved(axis, valueF);
						firstControllerAxisEvent = false;
					}
					detail::currentIS->OnAxisMoved(axis, valueF);
				}
				break;
			case SDL_CONTROLLERDEVICEADDED:
				AddGameController(SDL_GameControllerFromInstanceID(event.cdevice.which));
				break;
			case SDL_MOUSEBUTTONDOWN:
				ButtonDownEvent(TranslateSDLMouseButton(event.button.button), false);
				break;
			case SDL_MOUSEBUTTONUP:
				ButtonUpEvent(TranslateSDLMouseButton(event.button.button), false);
				break;
			case SDL_MOUSEMOTION:
				if (firstMouseMotionEvent)
				{
					detail::previousIS->cursorX = event.motion.x;
					detail::previousIS->cursorY = event.motion.y;
					firstMouseMotionEvent = false;
				}
				detail::currentIS->cursorX = event.motion.x;
				detail::currentIS->cursorY = event.motion.y;
				detail::currentIS->cursorDeltaX += event.motion.xrel;
				detail::currentIS->cursorDeltaY += event.motion.yrel;
				break;
			case SDL_MOUSEWHEEL:
				detail::currentIS->scrollX += event.wheel.x;
				detail::currentIS->scrollY += event.wheel.y;
				break;
			case SDL_TEXTINPUT:
				detail::inputtedText.append(event.text.text);
				break;
			}
		}
		
		SetRenderTarget(nullptr);
		
		game->RunFrame(dt);
		
		SDL_GL_SwapWindow(window);
	}
	
	void Run(Game* _game)
	{
		lastFrameStart = std::chrono::high_resolution_clock::now();
		
		game = _game;
		
#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop([]
		{
			RunOneFrame();
		}, 0, 0);
#else
		while (!shouldClose)
		{
			RunOneFrame();
		}
		
		Uninit();
#endif
	}
}
