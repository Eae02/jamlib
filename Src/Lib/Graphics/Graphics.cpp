#include "Graphics.hpp"
#include "OpenGL.hpp"

namespace jm
{
	void ClearColor(const glm::vec4& color)
	{
		glClearBufferfv(GL_COLOR, 0, &color.x);
	}
	
	void ClearDepth(float depth)
	{
		glClearBufferfv(GL_DEPTH, 0, &depth);
	}
	
	void SetRenderTarget(Texture2D* color, Texture2D* depth)
	{
		
	}
}
