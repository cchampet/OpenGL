#ifndef _H_TEXTURE_MANAGER_H_
#define _H_TEXTURE_MANAGER_H_


#include "glew/glew.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "glm/vec3.hpp" // glm::vec3
#include "ShaderManager.h"

struct TextureManager 
{

	void loadTextures(GLuint* tab, size_t size);

	void loadBufferTextures(GLuint* gbufferTextures, size_t size, int width, int height);

	void loadFxTextures(GLuint* fxBufferTextures, size_t size, int width, int height);

	void renderMainScreen(ShaderManager& shaderManager, int width, int height, GLuint bufferTexture, GLuint* vao, glm::vec3 cameraEye, double t);
	void renderDebugScreens(size_t nbMiniatures, int width, int height, GLuint* bufferTexture, GLuint* vao);
};



#endif 