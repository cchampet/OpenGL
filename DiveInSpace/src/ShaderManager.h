#ifndef _H_SHADER_MANAGER_H_
#define _H_SHADER_MANAGER_H_

#include <map>

#include "Shader.h"

/**
* Manage all the shaders.
*/
struct ShaderManager
{
	/**
	* List of shader available
	*/
	enum ListShaderType{
		GBUFFER,
		BLIT,
		LIGHT,
		GAMMA,
		SOBEL,
		BLUR,
		COC,
		DOF
	};

    void addShader(const char* shaderFile, int typemask, ListShaderType shaderType);
	Shader& getShader(ListShaderType shaderType);

private:
	
    std::map<ListShaderType, Shader> m_shaderMap;
};

#endif