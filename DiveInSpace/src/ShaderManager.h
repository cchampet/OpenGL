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
        GBUFFER_TRAVEL_PLANETE,
        GBUFFER_TRAVEL_SPIRAL,
        GBUFFER_TRAVEL_MONOLITHE,
		BLIT,
		DIR_LIGHT,
        POINT_LIGHT,
        SPOT_LIGHT,
		GAMMA,
		SOBEL,
		BLUR,
		COC,
		DOF,
        GLOW,
		EXPLOSION,
        COLORSPACE, 
        STAR
	};

	ShaderManager();

    void addShader(const char* shaderFile, int typemask, ListShaderType shaderType);

	void uploadUniforms(ListShaderType shaderType, glm::vec3 cameraEye, double t);

	void renderTextureWithShader(ListShaderType shaderType, int width, int height, GLuint* bufferTexture, GLuint* vao, int ping, int pong, glm::vec3 cameraEye, double t);
    
    void renderLighting(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t);
    void renderDirLighting(LightManager& lightManager);
    void renderPointLighting(LightManager& lightManager);
    void renderSpotLighting(LightManager& lightManager);

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
    inline float* getTranslateFactor() { return &translateFactor;}
    inline bool* isHalStop() { return &bIsHalStop;}
    inline bool* isMonolitheStop() { return &bIsMonolitheStop;}
    inline float* getSpiralAngle() { return &spiralAngle;}
    inline float* getSpiralRadius() { return &spiralRadius;}

	inline void setBlurSamples(float blur) { blurSamples = blur;}
	inline void setFocusPlane(float focus) { focusPlane = focus;}
	inline void setNearPlane(float near) { nearPlane = near;}
	inline void setFarPlane(float far) { farPlane = far;}
	inline void setGamma(float fGamma) { gamma = fGamma;}
	inline void setSobelCoef(float sobel) { sobelCoef = sobel;}
    inline void setTranslateFactor(float newFactor) { translateFactor = newFactor;}
    inline void setIsHalStop(bool flag) { bIsHalStop = flag;}
    inline void setIsMonolitheStop(bool flag) { bIsMonolitheStop = flag;}
    inline void setSpiralAngle(float angle) { spiralAngle = angle;}
    inline void setSpiralRadius(float radius) { spiralRadius = radius;}

    /**
    * Manage specific scenes
    */
    void updateTravel2Elements(double t){
        sobelCoef = 1.f;
        blurSamples = 15.f;
    }

private:
	
    std::map<ListShaderType, Shader> m_shaderMap;

    // Parameters
    float blurSamples;
	float focusPlane;
	float nearPlane;
	float farPlane;
	float gamma;
	float sobelCoef;
    bool  glow;

    float translateFactor;
    bool  bIsHalStop;
    bool  bIsMonolitheStop;
    float spiralAngle;
    float spiralRadius;

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

    // Location for gbufferTravelPlanete_shader
    GLuint gbufferTravelPlanete_projectionLocation;
    GLuint gbufferTravelPlanete_viewLocation; 
    GLuint gbufferTravelPlanete_objectLocation;
    GLuint gbufferTravelPlanete_timeLocation;  
    GLuint gbufferTravelPlanete_diffuseLocation;
    GLuint gbufferTravelPlanete_specLocation;

    // Location for gbufferTravelSpiral_shader
    GLuint gbufferTravelSpiral_projectionLocation;
    GLuint gbufferTravelSpiral_viewLocation; 
    GLuint gbufferTravelSpiral_objectLocation;
    GLuint gbufferTravelSpiral_timeLocation;  
    GLuint gbufferTravelSpiral_diffuseLocation;
    GLuint gbufferTravelSpiral_specLocation;
    GLuint gbufferTravelSpiral_translateFactorLocation;
    GLuint gbufferTravelSpiral_spiralRadiusLocation;
    GLuint gbufferTravelSpiral_spiralAngleLocation;

    // Location for gbufferTravelMonolythe_shader
    GLuint gbufferTravelMonolithe_projectionLocation;
    GLuint gbufferTravelMonolithe_viewLocation; 
    GLuint gbufferTravelMonolithe_objectLocation;
    GLuint gbufferTravelMonolithe_timeLocation;  
    GLuint gbufferTravelMonolithe_diffuseLocation;
    GLuint gbufferTravelMonolithe_specLocation;
    GLuint gbufferTravelMonolithe_translateFactorLocation;
    GLuint gbufferTravelMonolithe_isMonolitheStopLocation;

    // Location for blit_shader
    GLuint blit_tex1Location;

    // Location for dirLight shader
    GLuint dirLight_materialLocation; 
    GLuint dirLight_normalLocation; 
    GLuint dirLight_depthLocation; 
    GLuint dirLight_inverseViewProjectionLocation; 
    GLuint dirLight_cameraPositionLocation; 
    GLuint dirLight_lightDirectionLocation; 
    GLuint dirLight_lightDiffuseColorLocation; 
    GLuint dirLight_lightSpecularColorLocation; 
    GLuint dirLight_lightIntensityLocation;
    GLuint dirLight_timeLocation;
    GLuint dirLight_projectionLocation;

    // Location for pointLight shader
    GLuint pointLight_materialLocation; 
    GLuint pointLight_normalLocation; 
    GLuint pointLight_depthLocation; 
    GLuint pointLight_inverseViewProjectionLocation; 
    GLuint pointLight_cameraPositionLocation; 
    GLuint pointLight_lightPositionLocation; 
    GLuint pointLight_lightDiffuseColorLocation; 
    GLuint pointLight_lightSpecularColorLocation; 
    GLuint pointLight_lightIntensityLocation;
    GLuint pointLight_timeLocation;
    GLuint pointLight_projectionLocation;

    // Location for spotLight shader
    GLuint spotLight_materialLocation; 
    GLuint spotLight_normalLocation; 
    GLuint spotLight_depthLocation; 
    GLuint spotLight_inverseViewProjectionLocation; 
    GLuint spotLight_cameraPositionLocation; 
    GLuint spotLight_lightPositionLocation; 
    GLuint spotLight_lightDiffuseColorLocation; 
    GLuint spotLight_lightSpecularColorLocation; 
    GLuint spotLight_lightIntensityLocation;
    GLuint spotLight_lightDirectionLocation;
    GLuint spotLight_lightExternalAngleLocation;
    GLuint spotLight_lightInternalAngleLocation;
    GLuint spotLight_timeLocation;
    GLuint spotLight_projectionLocation;

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

    // Location for glow_shader
    GLuint glow_colorLocation;
    GLuint glow_blurLocation;

    // Location for explosion_shader
    GLuint explosion_channelLocation;
    GLuint explosion_timeLocation;

    // Location for colorspace shader
    GLuint colorspace_resolutionLocation;
    GLuint colorspace_timeLocation;

    // Location for star shader
    GLuint star_resolutionLocation;
    GLuint star_timeLocation;


};

#endif