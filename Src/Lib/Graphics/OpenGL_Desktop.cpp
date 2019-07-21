#include "OpenGL.hpp"

#include <iostream>
#include <string_view>

#ifndef JM_USE_GLES
namespace jm::detail
{
	bool hasGL45;
	
	static std::string glVendorName;
	
	void OpenGLMessageCallback(GLenum, GLenum type, GLuint id, GLenum severity, GLsizei length,
	                           const GLchar* message, const void*)
	{
		if (id == 1286)
			return;
		
		if (glVendorName == "Intel Open Source Technology Center")
		{
			if (id == 17 || id == 14) //Clearing integer framebuffer attachments.
				return;
		}
		
		const char* severityString = "N";
		if (severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR)
		{
			severityString = "E";
		}
		else if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM)
		{
			severityString = "W";
		}
		
		std::string_view messageView(message, static_cast<size_t>(length));
		
		//Some vendors include a newline at the end of the message. This removes the newline if present.
		if (messageView.back() == '\n')
		{
			messageView = messageView.substr(0, messageView.size() - 1);
		}
		
		std::cout << "GL[" << severityString << id << "]: " << messageView << std::endl;
		
		if (type == GL_DEBUG_TYPE_ERROR)
			std::abort();
	}
	
	bool InitializeOpenGL(bool debug)
	{
		if (gl3wInit())
		{
			std::cerr << "Error initializing OpenGL" << std::endl;
			return false;
		}
		
		glVendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		
		if (debug && glDebugMessageCallback)
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
		
		hasGL45 = gl3wIsSupported(4, 5);
		
		return true;
	}
}
#endif
