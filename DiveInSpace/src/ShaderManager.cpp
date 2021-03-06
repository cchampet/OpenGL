#include "ShaderManager.h"

#include "Geometry.h"

#include <cstdlib> //EXIT_FAILURE
#include <cstdio> //fprintf

const float TIME_SCALE_FOR_RECORDING = 0.25f;

ShaderManager::ShaderManager() {
    //FX
    blurSamples = 1.0;
    focusPlane = 5.0;
    nearPlane = 1.0;
    farPlane = 50.0;
    gamma = 1.0;
    sobelCoef = 0.0;
    //Specific elements
    translateFactor = 5.0; //for Travel_Planetes & Travel_Monolithe
    bIsHalStop = false; //for Hal
    bIsMonolitheStop = false; //for Monolithe
    spiralAngle = 1.1f; //for Travel_Planetes
    spiralRadius = 1.05f; //for Travel_Planetes
    starSize = 12.f; //for Star
    numStar = 50.f; //for Star
    distanceFactor = 1.f; //for Star
    dominantColor = glm::vec4(0.5,0.3,0.2, 1.0); //for Star
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

        case GBUFFER_IMAC:
            gbuffer_imac_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            gbuffer_imac_viewLocation = glGetUniformLocation(shader.program, "View");
            gbuffer_imac_objectLocation = glGetUniformLocation(shader.program, "Object");
            gbuffer_imac_timeLocation = glGetUniformLocation(shader.program, "Time");
            gbuffer_imac_diffuseLocation = glGetUniformLocation(shader.program, "Diffuse");
            gbuffer_imac_specLocation = glGetUniformLocation(shader.program, "Spec");

            break;


        case GBUFFER_TRAVEL_PLANETE:
            gbufferTravelPlanete_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            gbufferTravelPlanete_viewLocation = glGetUniformLocation(shader.program, "View");
            gbufferTravelPlanete_objectLocation = glGetUniformLocation(shader.program, "Object");
            gbufferTravelPlanete_timeLocation = glGetUniformLocation(shader.program, "Time");
            gbufferTravelPlanete_diffuseLocation = glGetUniformLocation(shader.program, "Diffuse");
            gbufferTravelPlanete_specLocation = glGetUniformLocation(shader.program, "Spec");

            break;

        case GBUFFER_TRAVEL_SPIRAL:
            gbufferTravelSpiral_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            gbufferTravelSpiral_viewLocation = glGetUniformLocation(shader.program, "View");
            gbufferTravelSpiral_objectLocation = glGetUniformLocation(shader.program, "Object");
            gbufferTravelSpiral_timeLocation = glGetUniformLocation(shader.program, "Time");
            gbufferTravelSpiral_diffuseLocation = glGetUniformLocation(shader.program, "Diffuse");
            gbufferTravelSpiral_specLocation = glGetUniformLocation(shader.program, "Spec");
            gbufferTravelSpiral_translateFactorLocation = glGetUniformLocation(shader.program, "TranslateFactor");
            gbufferTravelSpiral_spiralRadiusLocation = glGetUniformLocation(shader.program, "SpiralRadius");
            gbufferTravelSpiral_spiralAngleLocation = glGetUniformLocation(shader.program, "SpiralAngle");

            break;

        case GBUFFER_TRAVEL_MONOLITHE:
            gbufferTravelMonolithe_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            gbufferTravelMonolithe_viewLocation = glGetUniformLocation(shader.program, "View");
            gbufferTravelMonolithe_objectLocation = glGetUniformLocation(shader.program, "Object");
            gbufferTravelMonolithe_timeLocation = glGetUniformLocation(shader.program, "Time");
            gbufferTravelMonolithe_diffuseLocation = glGetUniformLocation(shader.program, "Diffuse");
            gbufferTravelMonolithe_specLocation = glGetUniformLocation(shader.program, "Spec");
            gbufferTravelMonolithe_translateFactorLocation = glGetUniformLocation(shader.program, "TranslateFactor");
            gbufferTravelMonolithe_isMonolitheStopLocation = glGetUniformLocation(shader.program, "IsMonolitheStop");

            break;

        case BLIT:
            blit_tex1Location = glGetUniformLocation(shader.program, "Texture1");

            break;

        case DIR_LIGHT:
            dirLight_materialLocation = glGetUniformLocation(shader.program, "Material");
            dirLight_normalLocation = glGetUniformLocation(shader.program, "Normal");
            dirLight_depthLocation = glGetUniformLocation(shader.program, "Depth");
            dirLight_timeLocation = glGetUniformLocation(shader.program, "Time");
            dirLight_inverseViewProjectionLocation = glGetUniformLocation(shader.program, "InverseViewProjection");
            dirLight_cameraPositionLocation = glGetUniformLocation(shader.program, "CameraPosition");
            dirLight_lightDirectionLocation = glGetUniformLocation(shader.program, "LightDirection");
            dirLight_lightDiffuseColorLocation = glGetUniformLocation(shader.program, "LightDiffuseColor");
            dirLight_lightSpecularColorLocation = glGetUniformLocation(shader.program, "LightSpecularColor");
            dirLight_lightIntensityLocation = glGetUniformLocation(shader.program, "LightIntensity");
            dirLight_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            
            break;

        case POINT_LIGHT:
            pointLight_materialLocation = glGetUniformLocation(shader.program, "Material");
            pointLight_normalLocation = glGetUniformLocation(shader.program, "Normal");
            pointLight_depthLocation = glGetUniformLocation(shader.program, "Depth");
            pointLight_timeLocation = glGetUniformLocation(shader.program, "Time");
            pointLight_inverseViewProjectionLocation = glGetUniformLocation(shader.program, "InverseViewProjection");
            pointLight_cameraPositionLocation = glGetUniformLocation(shader.program, "CameraPosition");
            pointLight_lightPositionLocation = glGetUniformLocation(shader.program, "LightPosition");
            pointLight_lightDiffuseColorLocation = glGetUniformLocation(shader.program, "LightDiffuseColor");
            pointLight_lightSpecularColorLocation = glGetUniformLocation(shader.program, "LightSpecularColor");
            pointLight_lightIntensityLocation = glGetUniformLocation(shader.program, "LightIntensity");
            pointLight_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            
            break;

        case SPOT_LIGHT:
            spotLight_materialLocation = glGetUniformLocation(shader.program, "Material");
            spotLight_normalLocation = glGetUniformLocation(shader.program, "Normal");
            spotLight_depthLocation = glGetUniformLocation(shader.program, "Depth");
            spotLight_timeLocation = glGetUniformLocation(shader.program, "Time");
            spotLight_inverseViewProjectionLocation = glGetUniformLocation(shader.program, "InverseViewProjection");
            spotLight_cameraPositionLocation = glGetUniformLocation(shader.program, "CameraPosition");
            spotLight_lightPositionLocation = glGetUniformLocation(shader.program, "LightPosition");
            spotLight_lightDirectionLocation = glGetUniformLocation(shader.program, "LightDirection");
            spotLight_lightDiffuseColorLocation = glGetUniformLocation(shader.program, "LightDiffuseColor");
            spotLight_lightSpecularColorLocation = glGetUniformLocation(shader.program, "LightSpecularColor");
            spotLight_lightIntensityLocation = glGetUniformLocation(shader.program, "LightIntensity");
            spotLight_lightExternalAngleLocation = glGetUniformLocation(shader.program, "LightExternalAngle");
            spotLight_lightInternalAngleLocation = glGetUniformLocation(shader.program, "LightInternalAngle");
            spotLight_projectionLocation = glGetUniformLocation(shader.program, "Projection");
            
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

        case  GLOW:
            glow_colorLocation = glGetUniformLocation(shader.program, "TextureColor");
            glow_blurLocation = glGetUniformLocation(shader.program, "TextureBlur");

            break;

        case EXPLOSION:
            explosion_channelLocation = glGetUniformLocation(shader.program, "Channel");
            explosion_timeLocation = glGetUniformLocation(shader.program, "Time");

            break;

        case COLORSPACE:
            colorspace_resolutionLocation = glGetUniformLocation(shader.program, "Resolution");
            colorspace_timeLocation = glGetUniformLocation(shader.program, "Time");

            break;

        case STAR:
            star_resolutionLocation = glGetUniformLocation(shader.program, "Resolution");
            star_timeLocation = glGetUniformLocation(shader.program, "Time");
            star_starSizeLocation = glGetUniformLocation(shader.program, "StarSize");
            star_numStarLocation = glGetUniformLocation(shader.program, "NumStar");
            star_distanceFactorLocation = glGetUniformLocation(shader.program, "DistanceFactor");
            star_dominantColorLocation = glGetUniformLocation(shader.program, "DominantColor");

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
            glUniform1f(gbuffer_timeLocation, t*TIME_SCALE_FOR_RECORDING);
            glUniform1i(gbuffer_diffuseLocation, 0);
            glUniform1i(gbuffer_specLocation, 1);

            break;

        case GBUFFER_IMAC:
            glUniformMatrix4fv(gbuffer_imac_projectionLocation, 1, 0, glm::value_ptr(ShaderManager::projection));
            glUniformMatrix4fv(gbuffer_imac_viewLocation, 1, 0, glm::value_ptr(worldToView));
            glUniformMatrix4fv(gbuffer_imac_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
            glUniform1f(gbuffer_imac_timeLocation, t);
            glUniform1i(gbuffer_imac_diffuseLocation, 0);
            glUniform1i(gbuffer_imac_specLocation, 1);

            break;

        case GBUFFER_TRAVEL_PLANETE:
            glUniformMatrix4fv(gbufferTravelPlanete_projectionLocation, 1, 0, glm::value_ptr(ShaderManager::projection));
            glUniformMatrix4fv(gbufferTravelPlanete_viewLocation, 1, 0, glm::value_ptr(worldToView));
            glUniformMatrix4fv(gbufferTravelPlanete_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
            glUniform1f(gbufferTravelPlanete_timeLocation, t*TIME_SCALE_FOR_RECORDING);
            glUniform1i(gbufferTravelPlanete_diffuseLocation, 0);
            glUniform1i(gbufferTravelPlanete_specLocation, 1);

            break;

        case GBUFFER_TRAVEL_SPIRAL:
            glUniformMatrix4fv(gbufferTravelSpiral_projectionLocation, 1, 0, glm::value_ptr(ShaderManager::projection));
            glUniformMatrix4fv(gbufferTravelSpiral_viewLocation, 1, 0, glm::value_ptr(worldToView));
            glUniformMatrix4fv(gbufferTravelSpiral_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
            glUniform1f(gbufferTravelSpiral_timeLocation, t*TIME_SCALE_FOR_RECORDING);
            glUniform1i(gbufferTravelSpiral_diffuseLocation, 0);
            glUniform1i(gbufferTravelSpiral_specLocation, 1);
            glUniform1f(gbufferTravelSpiral_translateFactorLocation, translateFactor);
            glUniform1f(gbufferTravelSpiral_spiralRadiusLocation, spiralRadius);
            glUniform1f(gbufferTravelSpiral_spiralAngleLocation, spiralAngle);

            break;

        case GBUFFER_TRAVEL_MONOLITHE:
            glUniformMatrix4fv(gbufferTravelMonolithe_projectionLocation, 1, 0, glm::value_ptr(ShaderManager::projection));
            glUniformMatrix4fv(gbufferTravelMonolithe_viewLocation, 1, 0, glm::value_ptr(worldToView));
            glUniformMatrix4fv(gbufferTravelMonolithe_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
            glUniform1f(gbufferTravelMonolithe_timeLocation, t*TIME_SCALE_FOR_RECORDING);
            glUniform1i(gbufferTravelMonolithe_diffuseLocation, 0);
            glUniform1i(gbufferTravelMonolithe_specLocation, 1);
            glUniform1f(gbufferTravelMonolithe_translateFactorLocation, translateFactor);
            glUniform1i(gbufferTravelMonolithe_isMonolitheStopLocation, static_cast<int>(*isMonolitheStop()));

            break;

        case BLIT:
            glUniform1i(blit_tex1Location, 0);

            break;

        case DIR_LIGHT:
            glUniform1i(dirLight_materialLocation, 0);
            glUniform1i(dirLight_normalLocation, 1);
            glUniform1i(dirLight_depthLocation, 2);
            glUniform3fv(dirLight_cameraPositionLocation, 1, glm::value_ptr(cameraEye));
            glUniformMatrix4fv(dirLight_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
            glUniform1f(dirLight_timeLocation, t*TIME_SCALE_FOR_RECORDING);

            break;

        case POINT_LIGHT:
            glUniform1i(pointLight_materialLocation, 0);
            glUniform1i(pointLight_normalLocation, 1);
            glUniform1i(pointLight_depthLocation, 2);
            glUniform3fv(pointLight_cameraPositionLocation, 1, glm::value_ptr(cameraEye));
            glUniformMatrix4fv(pointLight_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
            glUniform1f(pointLight_timeLocation, t*TIME_SCALE_FOR_RECORDING);

            break;

        case SPOT_LIGHT:
            glUniform1i(spotLight_materialLocation, 0);
            glUniform1i(spotLight_normalLocation, 1);
            glUniform1i(spotLight_depthLocation, 2);
            glUniform3fv(spotLight_cameraPositionLocation, 1, glm::value_ptr(cameraEye));
            glUniformMatrix4fv(spotLight_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
            glUniform1f(spotLight_timeLocation, t*TIME_SCALE_FOR_RECORDING);

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

        case  GLOW:
            glUniform1i(glow_colorLocation, 0);
            glUniform1i(glow_blurLocation, 1);

            break;

        case  EXPLOSION:
            glUniform1i(explosion_channelLocation, 0);
            glUniform1i(explosion_timeLocation, t*TIME_SCALE_FOR_RECORDING);

            break;

        case  COLORSPACE:
            glUniform3fv(colorspace_resolutionLocation, 1, glm::value_ptr(glm::vec3(800, 800, 800)));
            glUniform1f(colorspace_timeLocation, t*TIME_SCALE_FOR_RECORDING);

            break;

        case  STAR:
            glUniform3fv(star_resolutionLocation, 1, glm::value_ptr(glm::vec3(800, 800, 800)));
            glUniform1f(star_timeLocation, t*TIME_SCALE_FOR_RECORDING);
            glUniform1f(star_starSizeLocation, starSize);
            glUniform1i(star_numStarLocation, numStar);
            glUniform1f(star_distanceFactorLocation, distanceFactor);
            glUniform4fv(star_dominantColorLocation, 1, glm::value_ptr(dominantColor));

            break;
  
    }
}

void ShaderManager::renderDirLighting(LightManager& lightManager) {
    for (unsigned int i = 0; i < lightManager.getNumDirLight(); ++i)
    {
        glm::vec3 dir = lightManager.getDLDirection(i);
        glm::vec3 diff = lightManager.getDLDiffuse(i);
        glm::vec3 spec = lightManager.getDLSpec(i);
        float lightDirection[3] = {dir.x, dir.y, dir.z};
        float lightDiffuseColor[3] = {diff.r, diff.g, diff.b};
        float lightSpecColor[3] = {spec.r, spec.g, spec.b};
        glUniform3fv(dirLight_lightDirectionLocation, 1, lightDirection);
        glUniform3fv(dirLight_lightDiffuseColorLocation, 1, lightDiffuseColor);
        glUniform3fv(dirLight_lightSpecularColorLocation, 1, lightSpecColor);
        glUniform1f(dirLight_lightIntensityLocation, lightManager.getDLIntensity(i));
        
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
}

void ShaderManager::renderPointLighting(LightManager& lightManager) {
    for (unsigned int i = 0; i < lightManager.getNumPointLight(); ++i)
    {
        glm::vec3 pos = lightManager.getPLPosition(i);
        glm::vec3 diff = lightManager.getPLDiffuse(i);
        glm::vec3 spec = lightManager.getPLSpec(i);
        float lightPosition[3] = {pos.x, pos.y, pos.z};
        float lightDiffuseColor[3] = {diff.r, diff.g, diff.b};
        float lightSpecColor[3] = {spec.r, spec.g, spec.b};
        glUniform3fv(pointLight_lightPositionLocation, 1, lightPosition);
        glUniform3fv(pointLight_lightDiffuseColorLocation, 1, lightDiffuseColor);
        glUniform3fv(pointLight_lightSpecularColorLocation, 1, lightSpecColor);
        glUniform1f(pointLight_lightIntensityLocation, lightManager.getPLIntensity(i));
        
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
}

void ShaderManager::renderSpotLighting(LightManager& lightManager) {
    for (unsigned int i = 0; i < lightManager.getNumSpotLight(); ++i)
    {
        glm::vec3 pos = lightManager.getSPLPosition(i);
        glm::vec3 dir = lightManager.getSPLDirection(i);
        glm::vec3 diff = lightManager.getSPLDiffuse(i);
        glm::vec3 spec = lightManager.getSPLSpec(i);
        float lightPosition[3] = {pos.x, pos.y, pos.z};
        float lightDirection[3] = {dir.x, dir.y, dir.z};
        float lightDiffuseColor[3] = {diff.r, diff.g, diff.b};
        float lightSpecColor[3] = {spec.r, spec.g, spec.b};
        glUniform3fv(spotLight_lightPositionLocation, 1, lightPosition);
        glUniform3fv(spotLight_lightDirectionLocation, 1, lightDirection);
        glUniform3fv(spotLight_lightDiffuseColorLocation, 1, lightDiffuseColor);
        glUniform3fv(spotLight_lightSpecularColorLocation, 1, lightSpecColor);
        glUniform1f(spotLight_lightIntensityLocation, lightManager.getSPLIntensity(i));
        glUniform1f(spotLight_lightExternalAngleLocation, lightManager.getSPLExternalAngle(i));
        glUniform1f(spotLight_lightInternalAngleLocation, lightManager.getSPLInternalAngle(i));
        
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
    }
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
            glBindTexture(GL_TEXTURE_2D, bufferTexture[pong]);
            glActiveTexture(GL_TEXTURE1);//blur
            glBindTexture(GL_TEXTURE_2D, bufferTexture[3]);
            glActiveTexture(GL_TEXTURE2);//coc
            glBindTexture(GL_TEXTURE_2D, bufferTexture[2]);

            break;

        case GLOW:
            glActiveTexture(GL_TEXTURE0);//color
            glBindTexture(GL_TEXTURE_2D, bufferTexture[4]);
            glActiveTexture(GL_TEXTURE1);//blur
            glBindTexture(GL_TEXTURE_2D, bufferTexture[3]);

            break;

        default:
            // Bind textures we want to render
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bufferTexture[pong]);

            break;
    }

    // Draw scene
    glBindVertexArray(vao[2]);
    glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
}

void ShaderManager::renderLighting(ShaderManager& shaderManager, LightManager& lightManager,  int width, int height, GLuint* texturesToRead, GLuint textureToWrite, GLuint* vao, glm::vec3 cameraEye, double t){
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureToWrite, 0);

    // Clear the front buffer
    glViewport( 0, 0, width, height );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
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

    // Deferred lights on quad
    glBindVertexArray(vao[2]);

    // Bind dir_light_shader shader
    glUseProgram(shaderManager.getShader(ShaderManager::DIR_LIGHT).program);
    shaderManager.uploadUniforms(ShaderManager::DIR_LIGHT, cameraEye, t);
    renderDirLighting(lightManager);
    
    // Bind point_light_shader shader
    glUseProgram(shaderManager.getShader(ShaderManager::POINT_LIGHT).program);
    shaderManager.uploadUniforms(ShaderManager::POINT_LIGHT, cameraEye, t);
    renderPointLighting(lightManager);

    // Bind spot_light_shader shader
    glUseProgram(shaderManager.getShader(ShaderManager::SPOT_LIGHT).program);
    shaderManager.uploadUniforms(ShaderManager::SPOT_LIGHT, cameraEye, t);
    renderSpotLighting(lightManager);

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
    projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 10000.f); 
    worldToView = glm::lookAt(cameraEye, cameraO, cameraUp);
    worldToScreen = projection * worldToView;
    screenToWorld = glm::transpose(glm::inverse(worldToScreen));
    screenToView = glm::inverse(projection);
}
