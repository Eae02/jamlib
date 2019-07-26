#pragma once

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#define JM_USE_GLES
#endif

#ifdef JM_USE_GLES
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#define GL_TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define JM_MODERN_GL(code, fallback) fallback
#else
#include <GL/gl3w.h>
#define JM_MODERN_GL(code, fallback) if (jm::detail::hasModernGL) { code } else { fallback }
#endif

namespace jm::detail
{
	extern bool hasModernGL;
	
	extern float maxAnistropy;
	
	bool InitializeOpenGL(bool debug);
}
