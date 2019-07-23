#include "OpenGL.hpp"

#include <iostream>
#include <string_view>
#include <SDL_video.h>

#ifndef JM_USE_GLES
namespace jm::detail
{
	bool hasModernGL;
	
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
	
	static const char* RequiredExtensions[] =
	{
		"GL_ARB_texture_storage"
	};
	
	static const char* ModernExtensions[] = 
	{
		"GL_ARB_direct_state_access",
		"GL_ARB_buffer_storage"
	};
	
	bool InitializeOpenGL(bool debug)
	{
		if (gl3wInit())
		{
			std::cerr << "Error initializing OpenGL\n";
			return false;
		}
		
		glVendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		
		for (const char* ext : RequiredExtensions)
		{
			if (!SDL_GL_ExtensionSupported(ext))
			{
				std::cerr << "Required OpenGL extension not supported: '" << ext << "'.\n";
				return false;
			}
		}
		
		hasModernGL = true;
		for (const char* ext : ModernExtensions)
		{
			if (!SDL_GL_ExtensionSupported(ext))
			{
				hasModernGL = false;
				break;
			}
		}
		
		if (debug && glDebugMessageCallback)
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		}
		
		return true;
	}
}
#endif
