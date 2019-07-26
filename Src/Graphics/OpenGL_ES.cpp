#include "OpenGL.hpp"

#ifdef JM_USE_GLES
#include <SDL_video.h>

namespace jm::detail
{
	bool hasModernGL = false;
	
	float maxAnistropy;
	
	bool InitializeOpenGL(bool debug)
	{
		if (SDL_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		{
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnistropy);
		}
		
		return true;
	}
}
#endif
