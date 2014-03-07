#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include "glew/glew.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/**
* A GLSL shader.
*/
struct Shader
{
    int compile_and_link_shader(int typeMask, const char * sourceBuffer, int bufferSize);
	int destroy_shader();
	int load_shader_from_file(const char * path, int typemask);

    enum ShaderType
    {
        VERTEX_SHADER = 1,
        FRAGMENT_SHADER = 2,
        GEOMETRY_SHADER = 4
    };

    GLuint program;
};

#endif