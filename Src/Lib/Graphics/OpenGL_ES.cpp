#include "OpenGL.hpp"

#ifdef JM_USE_GLES
namespace jm::detail
{
	bool hasModernGL = false;
	
	bool InitializeOpenGL(bool debug)
	{
		return true;
	}
}
#endif
