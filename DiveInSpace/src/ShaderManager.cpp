#include "ShaderManager.h"

#include <cstdlib> //EXIT_FAILURE
#include <cstdio> //fprintf

void ShaderManager::addShader(const char* shaderFile, int typemask, ListShaderType shaderType){
	// Try to load and compile shader
    Shader shader;
    int status = shader.load_shader_from_file(shaderFile, typemask);
    if( status == -1 ) {
        fprintf(stderr, "Error on loading  %s\n", shaderFile);
        exit(EXIT_FAILURE);
    }
    // Insert shader in the map of shaders
    m_shaderMap.insert(std::pair<ListShaderType, Shader>(shaderType, shader));
}

Shader& ShaderManager::getShader(ListShaderType shaderType){
	return m_shaderMap.at(shaderType);
}