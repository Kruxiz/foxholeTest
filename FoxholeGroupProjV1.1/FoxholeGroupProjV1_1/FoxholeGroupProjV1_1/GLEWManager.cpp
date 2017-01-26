#include "GLEWManager.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <iostream>

namespace GLEWManager{
	void GLEWManager::GLEWInitialise(void)
	{
		// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
			std::cout << "glewInit failed, aborting." << std::endl;
			exit(1);
		}
		std::cout << glGetString(GL_VERSION) << std::endl;
	}
}
