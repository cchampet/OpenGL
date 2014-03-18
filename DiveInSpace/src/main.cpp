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

#include <time.h>
#include <sstream>

#include "ShaderManager.h"
#include "Camera.h"
#include "Shader.h"
#include "ImGui.h"
#include "LightManager.h"
#include "Geometry.h"
#include "TextureManager.h"


#define MODE_HAL    0
#define MODE_HAL2   0
#define MODE_TRAVEL 1

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

    glfwSetWindowTitle( "Dive in Space" );


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
    LightManager lightManager;
    #if MODE_HAL == 1
        lightManager.createHalLights();
    #endif
    
    #if MODE_TRAVEL == 1
        lightManager.createTravelLights();
    #endif

    #if MODE_HAL2 == 1
        lightManager.createHalLights2();
    #endif

    TextureManager textureManager;

    // Load images and upload textures
    GLuint textures[3];
    textureManager.loadTextures(textures, 3);


    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Shaders ------------------------------------------ */
    /* --------------------------------------------------------------------------------------------- */
    shaderManager.addShader("shaders/gbuffer.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GBUFFER);
    shaderManager.addShader("shaders/gbuffer_travel.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GBUFFER_TRAVEL);
    shaderManager.addShader("shaders/blit.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLIT);
    //Light
    shaderManager.addShader("shaders/dirLight.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::DIR_LIGHT);
    shaderManager.addShader("shaders/pointLight.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::POINT_LIGHT);
    shaderManager.addShader("shaders/spotLight.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::SPOT_LIGHT);
    //Post-processing
    shaderManager.addShader("shaders/gamma.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GAMMA);
    shaderManager.addShader("shaders/sobel.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::SOBEL);
    shaderManager.addShader("shaders/blur.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLUR);
    shaderManager.addShader("shaders/coc.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::COC);
    shaderManager.addShader("shaders/dof.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::DOF);
    //Not working
    shaderManager.addShader("shaders/explosion.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::EXPLOSION);

    shaderManager.addShader("shaders/colorSpace.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::COLORSPACE);
    shaderManager.addShader("shaders/star.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::STAR);
    
    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Geometry ----------------------------------------- */
    /* --------------------------------------------------------------------------------------------- */
    // Vertex Array Object
    GLuint vao[5];
    glGenVertexArrays(5, vao);

    // Vertex Buffer Objects]
    GLuint vbo[18];
    glGenBuffers(18, vbo);

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


    // Sphere
    // Fill sphere data
    int tt;
    // Construit l'ensemble des vertex
    std::vector<glm::vec3> spherePositions;
    std::vector<glm::vec3> sphereNormals;
    std::vector<glm::vec2> sphereUv;
    int sphere_nb_vertices = 0;
    for(GLsizei j = 0; j <= discLong; ++j) {
        GLfloat cosTheta = cos(-M_PI / 2 + j * dTheta);
        GLfloat sinTheta = sin(-M_PI / 2 + j * dTheta);
        for(GLsizei i = 0; i <= discLat; ++i) {
            sphereUv.push_back(glm::vec2(i * rcpLat, 1.f - j * rcpLong));
            glm::vec3 normal(sin(i * dPhi) * cosTheta, sinTheta, cos(i * dPhi) * cosTheta);
            sphereNormals.push_back(normal);
            spherePositions.push_back(sphere_radius * normal);
            sphere_nb_vertices++;
        }
    }

    float sphere_vertices[sphere_nb_vertices*3];
    float sphere_uv[sphere_nb_vertices*2];
    float sphere_normals[sphere_nb_vertices*3];
    int sphere_triangleList[discLong * discLat * 6];

    tt = 0;
    int uu = 0;
    for(size_t i = 0; i < spherePositions.size(); ++i){
        sphere_vertices[tt] = spherePositions[i].x;
        sphere_normals[tt] = sphereNormals[i].x; tt++;
        sphere_uv[uu] = sphereUv[i].x; uu++;

        sphere_vertices[tt] = spherePositions[i].y; 
        sphere_normals[tt] = sphereNormals[i].y; tt++;
        sphere_uv[uu] = sphereUv[i].y; uu++;

        sphere_vertices[tt] = spherePositions[i].z; 
        sphere_normals[tt] = sphereNormals[i].z; tt++;
    }

    // Fill sphere index
    tt = 0;
    for(GLsizei j = 0; j < discLong; ++j) {
        GLsizei offset = j * (discLat + 1);
        for(GLsizei i = 0; i < discLat; ++i) {
            
            sphere_triangleList[tt] = offset + i;                       tt++;
            sphere_triangleList[tt] = offset + (i + 1);                 tt++;
            sphere_triangleList[tt] = offset + discLat + 1 + (i + 1);   tt++;

            sphere_triangleList[tt] = offset + i;                       tt++;
            sphere_triangleList[tt] = offset + discLat + 1 + (i + 1);   tt++;
            sphere_triangleList[tt] = offset + i + discLat + 1;         tt++;
        }
    }
    glBindVertexArray(vao[3]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_triangleList), sphere_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_uv), sphere_uv, GL_STATIC_DRAW);



    /* --------------------------------------------- */
    /* -------------------- HAL -------------------- */
    /* --------------------------------------------- */

    // Plane
    glBindVertexArray(vao[4]);

    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[14]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hal_triangleList), hal_triangleList, GL_STATIC_DRAW);
    
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hal_vertices), hal_vertices, GL_STATIC_DRAW);
    
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hal_normals), hal_normals, GL_STATIC_DRAW);
    
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[17]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hal_uvs), hal_uvs, GL_STATIC_DRAW);

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
    /**
    * 0 / 1 => ping pong textures
    * 2 => Coc
    * 3 => Blur
    */
    textureManager.loadFxTextures(fxBufferTextures, 4, width, height);

    // Create Framebuffer Object
    glGenFramebuffers(1, &fxBufferFbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
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
        #if MODE_HAL == 1
            textureManager.fillFrameBufferHal(gbufferFbo, gbufferDrawBuffers, width, height, shaderManager, textures, vao, camera.m_eye, t);
        #endif
        
        #if MODE_TRAVEL == 1
            textureManager.fillFrameBufferTravel(gbufferFbo, gbufferDrawBuffers, width, height, shaderManager, textures, vao, camera.m_eye, t);
        #endif
        
        #if MODE_HAL2 == 1
            textureManager.fillFrameBufferHal2(gbufferFbo, gbufferDrawBuffers, width, height, shaderManager, textures, vao, camera.m_eye, t);
        #endif
        
        /* --------------------------------------------------------------------------------------------- */
        /* -------------------------------------- Rendu/Affichage -------------------------------------- */
        /* --------------------------------------------------------------------------------------------- */
        
        //
        // Start FX 
        //
        glBindFramebuffer(GL_FRAMEBUFFER, fxBufferFbo);
            
            // Lighting
            #if MODE_HAL == 1
                lightManager.updateHalLights(t);
            #endif

            #if MODE_TRAVEL == 1
                //lightManager.updateTravelights(t);
            #endif

            #if MODE_HAL2 == 1
                lightManager.updateHalLights2(t);
            #endif

            shaderManager.renderLighting(shaderManager, lightManager, width, height, gbufferTextures, fxBufferTextures[0], vao, camera.m_eye, t);

            // Explosion pass
            //shaderManager.renderTextureWithShader(ShaderManager::EXPLOSION, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);

            // ColorSpace pass
            shaderManager.renderTextureWithShader(ShaderManager::COLORSPACE, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);
            
            // Star pass
             shaderManager.renderTextureWithShader(ShaderManager::STAR, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);

            // // COC
            // shaderManager.computeCoc(width, height, gbufferTextures[2], fxBufferTextures[2], vao, camera.m_eye, t);

            // // Sobel pass
            // shaderManager.renderTextureWithShader(ShaderManager::SOBEL, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);

            // // BLUR
            // shaderManager.renderTextureWithShader(ShaderManager::BLUR, width, height, fxBufferTextures, vao, 3, 1, camera.m_eye, t);

            // // DOF
            // shaderManager.renderTextureWithShader(ShaderManager::DOF, width, height, fxBufferTextures, vao, 0, 1, camera.m_eye, t);   

            // // Gamma pass
            // shaderManager.renderTextureWithShader(ShaderManager::GAMMA, width, height, fxBufferTextures, vao, 1, 0, camera.m_eye, t);
        //
        // Unbind framebuffer : now that we render all the textures, we can debind the fxBuffer
        //
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Display main screen 
        textureManager.renderMainScreen(shaderManager, width, height, fxBufferTextures[1], vao, camera.m_eye, t);
        // Display debug (pas à la noix non non non !)
        textureManager.renderDebugScreens(3, width, height, gbufferTextures, vao);
#if 1
        drawGUI(width, height, shaderManager, lightManager, fps, leftButton);
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
