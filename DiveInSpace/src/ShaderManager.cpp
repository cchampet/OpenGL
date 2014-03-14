#include "ShaderManager.h"

#include "Geometry.h"

#include <cstdlib> //EXIT_FAILURE
#include <cstdio> //fprintf

ShaderManager::ShaderManager() {
    blurSamples = 2.0;
    focusPlane = 5.0;
    nearPlane = 1.0;
    farPlane = 50.0;
    gamma = 1.0;
    sobelCoef = 1.0;

}

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

    // Initialize locations
    switch(shaderType) {
        case GBUFFER:
            gbuffer_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            gbuffer_viewLocation = glGetUniformLocation(shader.program, "View");
            gbuffer_objectLocation = glGetUniformLocation(shader.program, "Object");
            gbuffer_timeLocation = glGetUniformLocation(shader.program, "Time");
            gbuffer_diffuseLocation = glGetUniformLocation(shader.program, "Diffuse");
            gbuffer_specLocation = glGetUniformLocation(shader.program, "Spec");

            break;

        case BLIT:
            blit_tex1Location = glGetUniformLocation(shader.program, "Texture1");

            break;

        case LIGHT:
            lighting_materialLocation = glGetUniformLocation(shader.program, "Material");
            lighting_normalLocation = glGetUniformLocation(shader.program, "Normal");
            lighting_depthLocation = glGetUniformLocation(shader.program, "Depth");
            lighting_inverseViewProjectionLocation = glGetUniformLocation(shader.program, "InverseViewProjection");
            lighting_cameraPositionLocation = glGetUniformLocation(shader.program, "CameraPosition");
            lighting_lightPositionLocation = glGetUniformLocation(shader.program, "LightPosition");
            lighting_lightColorLocation = glGetUniformLocation(shader.program, "LightColor");
            lighting_lightIntensityLocation = glGetUniformLocation(shader.program, "LightIntensity");
            break;

        case GAMMA:
            gamma_tex1Location = glGetUniformLocation(shader.program, "Texture1");
            gamma_gammaLocation = glGetUniformLocation(shader.program, "Gamma");

            break;

        case SOBEL:
            sobel_tex1Location = glGetUniformLocation(shader.program, "Texture1");
            sobel_sobelLocation = glGetUniformLocation(shader.program, "SobelCoef");

            break;

        case BLUR:
            blur_tex1Location = glGetUniformLocation(shader.program, "Texture1");
            blur_directionLocation = glGetUniformLocation(shader.program, "Direction");
            blur_samplesLocation = glGetUniformLocation(shader.program, "SampleCount");

            break;

        case COC:
            coc_depthLocation = glGetUniformLocation(shader.program, "Depth");
            coc_screenToViewLocation = glGetUniformLocation(shader.program, "ScreenToView");
            coc_focusLocation = glGetUniformLocation(shader.program, "Focus");
            coc_nearPlaneLocation = glGetUniformLocation(shader.program, "Near");
            coc_farPlaneLocation = glGetUniformLocation(shader.program, "Far");

            break;

        case  DOF:
            dof_colorLocation = glGetUniformLocation(shader.program, "Color");
            dof_blurLocation = glGetUniformLocation(shader.program, "Blur");
            dof_cocLocation = glGetUniformLocation(shader.program, "CoC");

            break;

        case EXPLOSION:
            explosion_channelLocation = glGetUniformLocation(shader.program, "Channel");
            explosion_timeLocation = glGetUniformLocation(shader.program, "Time");

            break;
    }
}

void ShaderManager::uploadUniforms(ListShaderType shaderType, glm::vec3 cameraEye, double t) {
    // Initialize locations
    switch(shaderType) {
        case GBUFFER:
            glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(ShaderManager::projection));
            glUniformMatrix4fv(gbuffer_viewLocation, 1, 0, glm::value_ptr(worldToView));
            glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
            glUniform1f(gbuffer_timeLocation, t);
            glUniform1i(gbuffer_diffuseLocation, 0);
            glUniform1i(gbuffer_specLocation, 1);

            break;

        case BLIT:
            glUniform1i(blit_tex1Location, 0);

            break;

        case LIGHT:
            glUniform1i(lighting_materialLocation, 0);
            glUniform1i(lighting_normalLocation, 1);
            glUniform1i(lighting_depthLocation, 2);
            glUniform3fv(lighting_cameraPositionLocation, 1, glm::value_ptr(cameraEye));
            glUniformMatrix4fv(lighting_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));

            break;

        case GAMMA:
            glUniform1i(gamma_tex1Location, 0);
            glUniform1f(gamma_gammaLocation, gamma);

            break;

        case SOBEL:
            glUniform1i(sobel_tex1Location, 0);
            glUniform1f(sobel_sobelLocation, sobelCoef);
            break;

        case BLUR:
            glUniform1i(blur_tex1Location, 0);
            glUniform1i(blur_samplesLocation, static_cast<int>(blurSamples));

            break;

        case COC:
            glUniform1i(coc_depthLocation, 2);
            glUniformMatrix4fv(coc_screenToViewLocation, 1, 0, glm::value_ptr(screenToView));
            glUniform1f(coc_focusLocation, focusPlane);
            glUniform1f(coc_nearPlaneLocation, nearPlane);
            glUniform1f(coc_farPlaneLocation, farPlane);

            break;

        case  DOF:
            glUniform1i(dof_colorLocation, 0);
            glUniform1i(dof_blurLocation, 1);
            glUniform1i(dof_cocLocation, 2);

            break;

         case  EXPLOSION:
            glUniform1i(explosion_channelLocation, 0);
            glUniform1i(explosion_timeLocation, t);

            break;
    }
}

void ShaderManager::updateLightingUniformsTD(LightManager* lightManager, double tl) {
    glUniform3fv(lighting_lightPositionLocation, 1, lightManager->getCustomPositionOfLight(tl));
    glUniform3fv(lighting_lightColorLocation, 1, lightManager->getCustomColorOfLight(tl));
    glUniform1f(lighting_lightIntensityLocation, lightManager->getIntensityOfLights());
}

void ShaderManager::updateLightingUniformsHall(LightManager* lightManager, double i) {
    glUniform3fv(lighting_lightPositionLocation, 1, lightManager->getCustomPositionOfLight(i));
    glUniform3fv(lighting_lightColorLocation, 1, lightManager->getRedColor());
    glUniform1f(lighting_lightIntensityLocation, lightManager->getIntensityOfLights());
}

void ShaderManager::renderTextureWithShader(ListShaderType shaderType, int width, int height, GLuint* bufferTexture, GLuint* vao, int ping, int pong, glm::vec3 cameraEye, double t) {

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, bufferTexture[ping], 0);

    // Clear the front buffer
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shader and upload uniforms
    glUseProgram(getShader(shaderType).program);
    uploadUniforms(shaderType, cameraEye, t);

    switch(shaderType) {
        case DOF:
            glActiveTexture(GL_TEXTURE0);//color
            glBindTexture(GL_TEXTURE_2D, bufferTexture[1]);

            glActiveTexture(GL_TEXTURE1);//blur
            glBindTexture(GL_TEXTURE_2D, bufferTexture[3]);

            glActiveTexture(GL_TEXTURE2);//coc
            glBindTexture(GL_TEXTURE_2D, bufferTexture[2]);

            break;

        default:
            break;

    }    

    // Bind textures we want to render
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTexture[pong]);   

    // Draw scene
    glBindVertexArray(vao[2]);
    glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
}

void ShaderManager::renderLightingTD(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureToWrite, 0);

    // Clear the front buffer
    glViewport( 0, 0, width, height );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind lighting shader
    glUseProgram(shaderManager.getShader(ShaderManager::LIGHT).program);

    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::LIGHT, cameraEye, t);

    // Bind textures we want to render
    // Bind color to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[0]);        
    // Bind normal to unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[1]);    
    // Bind depth to unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[2]);        

    // Blit above the rest
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Deferred lights
    for (int i = 0; i < (int) lightManager.getNbLights(); ++i)
    {
        float tl = t * i;
        shaderManager.updateLightingUniformsTD(&lightManager, tl);

        // Draw quad
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }

    glDisable(GL_BLEND);
}

void ShaderManager::renderLightingHall(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureToWrite, 0);

    // Clear the front buffer
    glViewport( 0, 0, width, height );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind lighting shader
    glUseProgram(shaderManager.getShader(ShaderManager::LIGHT).program);

    // Upload uniforms
    shaderManager.uploadUniforms(ShaderManager::LIGHT, cameraEye, t);

    // Bind textures we want to render
    // Bind color to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[0]);        
    // Bind normal to unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[1]);    
    // Bind depth to unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texturesToRead[2]);        

    // Blit above the rest
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Deferred lights
    for (int i = 0; i < (int) lightManager.getNbLights(); ++i)
    {
        shaderManager.updateLightingUniformsHall(&lightManager, i);

        // Draw quad
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }

    glDisable(GL_BLEND);
}

void ShaderManager::computeCoc(int width, int height, GLuint bufferTextureToRead, GLuint bufferTextureToWrite, GLuint* vao, glm::vec3 cameraEye, double t){

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, bufferTextureToWrite, 0);

    // Clear the front buffer
    glViewport( 0, 0, width, height );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shader and upload uniforms
    glUseProgram(getShader(ShaderManager::COC).program);
    uploadUniforms(ShaderManager::COC, cameraEye, t);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTextureToRead);        

    // Draw scene
    glBindVertexArray(vao[2]);
    glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
}


Shader& ShaderManager::getShader(ListShaderType shaderType){
	return m_shaderMap.at(shaderType);
}

void ShaderManager::getCameraMatrices(float widthf, float heightf, glm::vec3 cameraEye, glm::vec3 cameraO, glm::vec3 cameraUp) {
    projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 100.f); 
    worldToView = glm::lookAt(cameraEye, cameraO, cameraUp);
    worldToScreen = projection * worldToView;
    screenToWorld = glm::transpose(glm::inverse(worldToScreen));
    screenToView = glm::inverse(projection);
}
