#ifndef _H_TEXTURE_MANAGER_H_
#define _H_TEXTURE_MANAGER_H_


#include "glew/glew.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

struct TextureManager 
{

	void loadTextures(GLuint* tab, size_t size);

	void loadBufferTextures(GLuint* gbufferTextures, size_t size, int width, int height);

	void loadFxTextures(GLuint* fxBufferTextures, size_t size, int width, int height);
};



#endif 