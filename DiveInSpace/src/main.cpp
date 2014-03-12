#include "glew/glew.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "GL/glfw.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "ShaderManager.h"
#include "Camera.h"
#include "Shader.h"
#include "ImGui.h"
#include "LightManager.h"
#include "Geometry.h"
#include "TextureManager.h"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

int main( int argc, char **argv )
{
    int width = 1024, height=768;
    float widthf = (float) width, heightf = (float) height;
    double t;
    float fps = 0.f;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }

    // Force core profile on Mac OSX
#ifdef __APPLE__
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( width, height, 0,0,0,0, 24, 0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );

        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwSetWindowTitle( "002_forward_a" );


    // Core profile is flagged as experimental in glew
#ifdef __APPLE__
    glewExperimental = GL_TRUE;
#endif
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
          exit( EXIT_FAILURE );
    }

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    // Init viewer structures
    Camera camera;
    camera.camera_defaults();
    init_imgui();
    GUIStates guiStates;
    init_gui_states(guiStates);

    // Init shader structures
    ShaderManager shaderManager;
    LightManager lightManager(10.f);

    TextureManager textureManager;

    // Load images and upload textures
    GLuint textures[3];
    textureManager.loadTextures(textures, 3);


    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Shaders ------------------------------------------ */
    /* --------------------------------------------------------------------------------------------- */
    shaderManager.addShader("shaders/gbuffer.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GBUFFER);
    shaderManager.addShader("shaders/blit.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLIT);
    shaderManager.addShader("shaders/light.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::LIGHT);
    shaderManager.addShader("shaders/gamma.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GAMMA);
    shaderManager.addShader("shaders/sobel.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::SOBEL);
    shaderManager.addShader("shaders/blur.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLUR);
    shaderManager.addShader("shaders/coc.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::COC);
    shaderManager.addShader("shaders/dof.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::DOF);


    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Geometry ----------------------------------------- */
    /* --------------------------------------------------------------------------------------------- */
    // Vertex Array Object
    GLuint vao[3];
    glGenVertexArrays(3, vao);

    // Vertex Buffer Objects
    GLuint vbo[12];
    glGenBuffers(12, vbo);

    // Cube
    glBindVertexArray(vao[0]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);

    // Plane
    glBindVertexArray(vao[1]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);

    // Quad
    glBindVertexArray(vao[2]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* --------------------------------------------------------------------------------------------- */
    /* ----------------------------------- Manage FrameBuffer -------------------------------------- */
    /* --------------------------------------------------------------------------------------------- */

    //
    // Create gbuffer frame buffer object
    //
    GLuint gbufferFbo;
    GLuint gbufferTextures[3];
    GLuint gbufferDrawBuffers[2];

    textureManager.loadBufferTextures(gbufferTextures, 3, width, height);

    // Create Framebuffer Object
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, gbufferTextures[0], 0);
    gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, gbufferTextures[1], 0);
    gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        exit( EXIT_FAILURE );
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* --------------------------------------------------------------------------------------------- */
    /* ----------------------------------- Manage FXBuffer -------------------------------------- */
    /* --------------------------------------------------------------------------------------------- */
    GLuint fxBufferFbo;
    GLuint fxBufferTextures[4];

    textureManager.loadFxTextures(fxBufferTextures, 4, width, height);

    // Create Framebuffer Object
    glGenFramebuffers(1, &fxBufferFbo);


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        exit( EXIT_FAILURE );
    }

    do
    {
        t = glfwGetTime();

        // Mouse states
        int leftButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements
        int altPressed = glfwGetKey(GLFW_KEY_LSHIFT);
        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            int x; int y;
            glfwGetMousePos(&x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS)
        {
            int mousex; int mousey;
            glfwGetMousePos(&mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera.camera_zoom(zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera.camera_turn(diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera.camera_pan(diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }
  
        // Get camera matrices
        shaderManager.getCameraMatrices(widthf, heightf, camera.m_eye, camera.m_o, camera.m_up);


        /* --------------------------------------------------------------------------------------------- */
        /* ----------------------------------- Remplissage Frame Buffer -------------------------------- */
        /* --------------------------------------------------------------------------------------------- */
       
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
        glDrawBuffers(2, gbufferDrawBuffers);

        // Viewport 
        glViewport(0, 0, width, height );

        // Default states
        glEnable(GL_DEPTH_TEST);

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind gbuffer shader
        glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER).program);

        // Upload uniforms
        shaderManager.uploadUniforms(ShaderManager::GBUFFER, camera.m_eye, t);
        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        // Render vaos
        glBindVertexArray(vao[0]);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 32);
        glBindVertexArray(vao[1]);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        /* --------------------------------------------------------------------------------------------- */
        /* -------------------------------------- Rendu/Affichage -------------------------------------- */
        /* --------------------------------------------------------------------------------------------- */

       //shaderManager.renderLighting(fxBufferFbo, fxBufferTextures, width, height, &lightManager);
        // Start FX 
        glBindFramebuffer(GL_FRAMEBUFFER, fxBufferFbo);
        
        //
        // Lighting pass
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[0], 0);

        // Clear the front buffer
        glViewport( 0, 0, width, height );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::LIGHT).program);

        // Upload uniforms
        shaderManager.uploadUniforms(ShaderManager::LIGHT, camera.m_eye, t);

        // Bind textures we want to render
        // Bind color to unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);        
        // Bind normal to unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);    
        // Bind depth to unit 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);        

        // Blit above the rest
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // Deferred lights
        for (int i = 0; i < (int) lightManager.getNbLights(); ++i)
        {
            float tl = t * i;

            shaderManager.updateLightingUniforms(&lightManager, tl);

            // Draw quad
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        glDisable(GL_BLEND);

        // //
        // // COC
        // //
        //renderTextureWithShader(ShaderManager::COC, width, height, gbufferTextures, vao, 2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[2], 0);

        // Clear the front buffer
        glViewport( 0, 0, width, height );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::COC).program);

        // Upload uniforms
        shaderManager.uploadUniforms(ShaderManager::COC, camera.m_eye, t);

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);        

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        // //
        // // Sobel pass
        // //
        shaderManager.renderTextureWithShader(ShaderManager::SOBEL, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);

        // //
        // // BLUR
        // //

        shaderManager.renderTextureWithShader(ShaderManager::BLUR, width, height, fxBufferTextures, vao, 3, 1, camera.m_eye, t);

        // //
        // // DOF
        // //
        shaderManager.renderTextureWithShader(ShaderManager::DOF, width, height, fxBufferTextures, vao, 0, 1, camera.m_eye, t);

        // //
        // // Gamma pass
        // //
        shaderManager.renderTextureWithShader(ShaderManager::GAMMA, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);
   

        // Unbind framebuffer : now that we render all the textures, we can debind the fxBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //
        // Display main screen 
        //
        glViewport( 0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderManager.getShader(ShaderManager::BLIT).program);
        //glUniform1i(blit_tex1Location, 0);
        shaderManager.uploadUniforms(ShaderManager::BLIT, camera.m_eye, t);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[0]);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //
        // Display miniatures (à la noix) 
        //

        // Diffuse
        glViewport(0, 0, width/5, height/5);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Specular
        glViewport(width/5, 0, width/5, height/5);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Depth
        glViewport(width/5 * 2, 0, width/5, height/5);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Coc
        glViewport(width/5 * 3, 0, width/5, height/5);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[2]);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Blur
        glViewport(width/5 * 4, 0, width/5, height/5);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[3]);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
#if 1
        // Draw UI
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);

        unsigned char mbut = 0;
        int mscroll = 0;
        int mousex; int mousey;
        glfwGetMousePos(&mousex, &mousey);
        mousey = height - mousey;

        if( leftButton == GLFW_PRESS )
            mbut |= IMGUI_MBUT_LEFT;

        imguiBeginFrame(mousex, mousey, mbut, mscroll);
        int logScroll = 0;
        char lineBuffer[512];
        imguiBeginScrollArea("Dive In Space", width - 210, height - 350, 200, 300, &logScroll);
        sprintf(lineBuffer, "FPS %f", fps);
        imguiLabel(lineBuffer);
        imguiSlider("Lights", lightManager.getNbLightsAdress(), 0.0, 100.0, 1.0);
        imguiSlider("Gamma", shaderManager.getGamma(), 0.0, 3.0, 0.1);
        imguiSlider("Sobel", shaderManager.getSobelCoeff(), 0.0, 1.0, 1.0);
        imguiSlider("Blur Samples", shaderManager.getBlurSamples(), 1.0, 100.0, 1.0);
        imguiSlider("Focus plane", shaderManager.getFocusPlane(), 1.0, 100.0, 1.0);
        imguiSlider("Near plane", shaderManager.getNearPlane(), 1.0, 100.0, 1.0);
        imguiSlider("Far plane", shaderManager.getFarPlane(), 1.0, 100.0, 1.0);
        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height); 

        glDisable(GL_BLEND);
#endif
        
        // Check for errors
        GLenum err = glGetError();
        if(err != GL_NO_ERROR){
            fprintf(stderr, "OpenGL Error : %s\n", gluErrorString(err));
            
        }

        // Swap buffers
        glfwSwapBuffers();

        double newTime = glfwGetTime();
        fps = 1.f/ (newTime - t);
    } 
    // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Clean UI
    imguiRenderGLDestroy();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
