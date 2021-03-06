#include "TextureManager.h"
#include <stdio.h> 
#include "stb/stb_image.h"
#include "Geometry.h"

void TextureManager::loadTextures(GLuint* tab, size_t size) {

    glGenTextures(size, tab);

	int x;
    int y;
    int comp; 

    /**
    * Robot
    */

	// Diffuse texture
    unsigned char * robotDiffuse = stbi_load("textures/robot_diffuse.jpg", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tab[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, robotDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    // Specular texture
    unsigned char * robotSpec = stbi_load("textures/robot_spec.jpg", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tab[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, robotSpec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);

    /**
    * Monolithe
    */

    // Diffuse texture
    unsigned char * monolitheDiffuse = stbi_load("textures/monolithe_diffuse.jpg", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tab[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, monolitheDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    // Specular texture
    unsigned char * monolitheSpec = stbi_load("textures/monolithe_spec.jpg", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tab[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, monolitheSpec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);

    /**
    * Planetes
    */

    // Diffuse texture
    unsigned char * planeteDiffuse = stbi_load("textures/planete_diffuse.jpg", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tab[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, planeteDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    // Specular texture
    unsigned char * planeteSpec = stbi_load("textures/planete_spec.jpg", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tab[6]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, planeteSpec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);
}

void TextureManager::loadBufferTextures(GLuint* gbufferTextures, size_t size, int width, int height) {

	glGenTextures(size, gbufferTextures);

    /**
    * Robot
    */
	// Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /**
    * Depth
    */
    // Create depth texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /**
    * Monolithe
    */
    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /**
    * Planetes
    */
    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[6]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void TextureManager::loadFxTextures(GLuint* fxBufferTextures, size_t size, int width, int height) {
	
    glGenTextures(size, fxBufferTextures);

    // Create first ping-pong texture
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create second ping-pong texture
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture => Coc
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture => Blur
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture => Lights
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void TextureManager::fillFrameBufferTravel1(GLuint fbo, GLuint* drawBuffers, int width, int height, ShaderManager& shaderManager, GLuint* bufferTextures, GLuint* vao, glm::vec3 cameraEye, double t){

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffers(2, drawBuffers);

    // Viewport 
    glViewport(0, 0, width, height );

    // Default states
    glEnable(GL_DEPTH_TEST);

    // Clear the front buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /**
    * Spiral
    */
    // Bind gbuffer travelSpiral shader
    glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER_TRAVEL_SPIRAL).program);
    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::GBUFFER_TRAVEL_SPIRAL, cameraEye, t);
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[5]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[6]);
    // Render vaos
    glBindVertexArray(vao[3]); //sphere
    glDrawElementsInstanced(GL_TRIANGLES, sphere_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 5000);
    
    /**
    * Monolithe
    */
    // Bind gbuffer travelMonolithe shader
    glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER_TRAVEL_MONOLITHE).program);
    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::GBUFFER_TRAVEL_MONOLITHE, cameraEye, t);
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[3]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[4]);
    // Render vaos
    glBindVertexArray(vao[0]); //cube
    glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 1);
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureManager::fillFrameBufferTravel2(GLuint fbo, GLuint* drawBuffers, int width, int height, ShaderManager& shaderManager, GLuint* bufferTextures, GLuint* vao, glm::vec3 cameraEye, double t){

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffers(2, drawBuffers);

    // Viewport 
    glViewport(0, 0, width, height );

    // Default states
    glEnable(GL_DEPTH_TEST);

    // Clear the front buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /**
    * Planetes
    */
    // Bind gbuffer travelPlanetes shader
    glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER_TRAVEL_PLANETE).program);
    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::GBUFFER_TRAVEL_PLANETE, cameraEye, t);
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[5]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[6]);
    // Render vaos
    glBindVertexArray(vao[3]); //sphere
    glDrawElementsInstanced(GL_TRIANGLES, sphere_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 10);
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureManager::fillFrameBufferHal(GLuint fbo, GLuint* drawBuffers, int width, int height, ShaderManager& shaderManager, GLuint* bufferTextures, GLuint* vao, glm::vec3 cameraEye, double t){

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffers(2, drawBuffers);

    // Viewport 
    glViewport(0, 0, width, height );

    // Default states
    glEnable(GL_DEPTH_TEST);

    // Clear the front buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind gbuffer shader
    glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER).program);

    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::GBUFFER, cameraEye, t);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[1]);

    // Render vaos
    glBindVertexArray(vao[4]); //Hal Plane
    glDrawElementsInstanced(GL_TRIANGLES, plane_hal_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 1);

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureManager::fillFrameBufferImac(GLuint fbo, GLuint* drawBuffers, int width, int height, ShaderManager& shaderManager, GLuint* bufferTextures, GLuint* vao, glm::vec3 cameraEye, double t){

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffers(2, drawBuffers);

    // Viewport 
    glViewport(0, 0, width, height );

    // Default states
    glEnable(GL_DEPTH_TEST);

    // Clear the front buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind gbuffer shader
    glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER_IMAC).program);

    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::GBUFFER_IMAC, cameraEye, t);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferTextures[1]);

    // Render vaos
    glBindVertexArray(vao[0]); // Cube
    glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 3);

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureManager::renderMainScreen(ShaderManager& shaderManager, int width, int height, GLuint bufferTexture, GLuint* vao, glm::vec3 cameraEye, double t){
    
    glViewport( 0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderManager.getShader(ShaderManager::BLIT).program);
    shaderManager.uploadUniforms(ShaderManager::BLIT, cameraEye, t);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);
    
    glBindVertexArray(vao[2]);
    glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
}

void TextureManager::renderDebugScreens(size_t nbMiniatures, int width, int height, GLuint* bufferTexture, GLuint* vao){
    
    for(size_t i = 0; i < nbMiniatures; ++i){
        
        glViewport( i*(width/nbMiniatures), 0, width/nbMiniatures, height/nbMiniatures  );
        glBindTexture(GL_TEXTURE_2D, bufferTexture[i]);        
        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
}