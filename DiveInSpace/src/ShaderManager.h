#ifndef _H_SHADER_MANAGER_H_
#define _H_SHADER_MANAGER_H_

#include <map>

#include "glew/glew.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Shader.h"
#include "LightManager.h"


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
		DOF,
		EXPLOSION
	};

	ShaderManager();

    void addShader(const char* shaderFile, int typemask, ListShaderType shaderType);


	void uploadUniforms(ListShaderType shaderType, glm::vec3 cameraEye, double t);
    void updateLightingUniformsTD(LightManager* lightManager, double t);
    void updateLightingUniformsHal(LightManager* lightManager, int i, double t);

	void renderTextureWithShader(ListShaderType shaderType, int width, int height, GLuint* bufferTexture, GLuint* vao, int ping, int pong, glm::vec3 cameraEye, double t);
    void renderLightingTD(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t);
    void renderLightingHal(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t);
    void computeCoc(int width, int height, GLuint bufferTextureToRead, GLuint bufferTextureToWrite, GLuint* vao, glm::vec3 cameraEye, double t);

	// Getters & Setters
    Shader& getShader(ListShaderType shaderType);
    void getCameraMatrices(float widthf, float heightf, glm::vec3 cameraEye, glm::vec3 cameraO, glm::vec3 cameraUp);

	inline float* getBlurSamples() { return &blurSamples;}
	inline float* getFocusPlane() { return &focusPlane;}
	inline float* getNearPlane() { return &nearPlane;}
	inline float* getFarPlane() { return &farPlane;}
	inline float* getGamma() { return &gamma;}
	inline float* getSobelCoeff() { return &sobelCoef;}

	inline void setBlurSamples(float blur) { blurSamples = blur;}
	inline void setFocusPlane(float focus) { focusPlane = focus;}
	inline void setNearPlane(float near) { nearPlane = near;}
	inline void setFarPlane(float far) { farPlane = far;}
	inline void setGamma(float fGamma) { gamma = fGamma;}
	inline void setSobelCoef(float sobel) { sobelCoef = sobel;}

private:
	
    std::map<ListShaderType, Shader> m_shaderMap;

    // Parameters
    float blurSamples;
	float focusPlane;
	float nearPlane;
	float farPlane;
	float gamma;
	float sobelCoef;


    // Location for camera
    glm::mat4 projection;
    glm::mat4 worldToView;
    glm::mat4 objectToWorld;
    glm::mat4 worldToScreen;
    glm::mat4 screenToWorld;
    glm::mat4 screenToView;

    // Location for gbuffer_shader
    GLuint gbuffer_projectionLocation;
    GLuint gbuffer_viewLocation; 
    GLuint gbuffer_objectLocation;
    GLuint gbuffer_timeLocation;  
    GLuint gbuffer_diffuseLocation;
    GLuint gbuffer_specLocation;

    // Location for blit_shader
    GLuint blit_tex1Location;

    // Location for lighting_shader
    GLuint lighting_materialLocation; 
    GLuint lighting_normalLocation; 
    GLuint lighting_depthLocation; 
    GLuint lighting_inverseViewProjectionLocation; 
    GLuint lighting_cameraPositionLocation; 
    GLuint lighting_lightPositionLocation; 
    GLuint lighting_lightColorLocation; 
    GLuint lighting_lightIntensityLocation;

    // Location for gamma_shader
    GLuint gamma_tex1Location;
    GLuint gamma_gammaLocation;

    // Location for sobel_shader
    GLuint sobel_tex1Location;
    GLuint sobel_sobelLocation;

    // Location for blur_shader
    GLuint blur_tex1Location;
    GLuint blur_directionLocation;
    GLuint blur_samplesLocation;

    // Location for coc_shader
    GLuint coc_depthLocation;
    GLuint coc_screenToViewLocation;
    GLuint coc_focusLocation;
    GLuint coc_nearPlaneLocation;
    GLuint coc_farPlaneLocation;

    // Location for dof_shader
    GLuint dof_colorLocation;
    GLuint dof_blurLocation;
    GLuint dof_cocLocation;

    // Location for explosion_shader
    GLuint explosion_channelLocation;
    GLuint explosion_timeLocation;


};

#endif