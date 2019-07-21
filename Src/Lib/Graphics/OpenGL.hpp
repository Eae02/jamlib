#pragma once

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#define JM_USE_GLES
#endif

#ifdef JM_USE_GLES
#include <GLES3/gl32.h>
#define JM_GL45(code, fallback) fallback
#else
#include <GL/gl3w.h>
#define JM_GL45(code, fallback) if (jm::detail::hasGL45) { code } else { fallback }
#endif

namespace jm::detail
{
	extern bool hasGL45;
	
	bool InitializeOpenGL(bool debug);
}
