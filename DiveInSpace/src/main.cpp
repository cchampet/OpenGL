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

#include "Camera.h"
#include "Shader.h"
#include "ShaderManager.h"

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

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;    



struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;


void init_gui_states(GUIStates & guiStates)
{
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}

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

    if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len))
    {
        fprintf(stderr, "Could not init GUI renderer.\n");
        exit(EXIT_FAILURE);
    }

    // Init viewer structures
    Camera camera;
    camera.camera_defaults();
    GUIStates guiStates;
    init_gui_states(guiStates);

    // Init shader structures
    ShaderManager shaderManager;

    // GUI
    float numLights = 10.f;

    // Load images and upload textures
    GLuint textures[3];
    glGenTextures(3, textures);
    int x;
    int y;
    int comp; 

    // Diffuse texture
    unsigned char * diffuse = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);

    // Specular texture
    unsigned char * spec = stbi_load("textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);


    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Shaders ------------------------------------------ */
    /* --------------------------------------------------------------------------------------------- */
    shaderManager.addShader("shaders/gbuffer.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GBUFFER);
    // Compute locations for gbuffer_shader
    GLuint gbuffer_projectionLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "Projection");
    GLuint gbuffer_viewLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "View");
    GLuint gbuffer_objectLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "Object");
    GLuint gbuffer_timeLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "Time");
    GLuint gbuffer_diffuseLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "Diffuse");
    GLuint gbuffer_specLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GBUFFER).program, "Spec");

    shaderManager.addShader("shaders/blit.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLIT);
    // Compute locations for blit_shader
    GLuint blit_tex1Location = glGetUniformLocation(shaderManager.getShader(ShaderManager::BLIT).program, "Texture1");

    shaderManager.addShader("shaders/light.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::LIGHT);
    // Compute locations for lighting_shader
    GLuint lighting_materialLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "Material");
    GLuint lighting_normalLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "Normal");
    GLuint lighting_depthLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "Depth");
    GLuint lighting_inverseViewProjectionLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "InverseViewProjection");
    GLuint lighting_cameraPositionLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "CameraPosition");
    GLuint lighting_lightPositionLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "LightPosition");
    GLuint lighting_lightColorLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "LightColor");
    GLuint lighting_lightIntensityLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::LIGHT).program, "LightIntensity");

    shaderManager.addShader("shaders/gamma.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::GAMMA);
    // Compute locations for gamma_shader
    GLuint gamma_tex1Location = glGetUniformLocation(shaderManager.getShader(ShaderManager::GAMMA).program, "Texture1");
    GLuint gamma_gammaLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::GAMMA).program, "Gamma");


    shaderManager.addShader("shaders/sobel.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::SOBEL);
    // Compute locations for sobel_shader
    GLuint sobel_tex1Location = glGetUniformLocation(shaderManager.getShader(ShaderManager::SOBEL).program, "Texture1");
    GLuint sobel_sobelLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::SOBEL).program, "SobelCoef");

    shaderManager.addShader("shaders/blur.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::BLUR);
    // Compute locations for blur_shader
    GLuint blur_tex1Location = glGetUniformLocation(shaderManager.getShader(ShaderManager::BLUR).program, "Texture1");
    GLuint blur_directionLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::BLUR).program, "Direction");
    GLuint blur_samplesLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::BLUR).program, "SampleCount");

    shaderManager.addShader("shaders/coc.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::COC);
    // Compute locations for coc_shader
    GLuint coc_depthLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::COC).program, "Depth");
    GLuint coc_screenToViewLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::COC).program, "ScreenToView");
    GLuint coc_focusLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::COC).program, "Focus");
    GLuint coc_nearPlaneLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::COC).program, "Near");
    GLuint coc_farPlaneLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::COC).program, "Far");

    shaderManager.addShader("shaders/dof.glsl", Shader::VERTEX_SHADER | Shader::FRAGMENT_SHADER, ShaderManager::DOF);
    // Compute locations for dof_shader
    GLuint dof_colorLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::DOF).program, "Color");
    GLuint dof_blurLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::DOF).program, "Blur");
    GLuint dof_cocLocation = glGetUniformLocation(shaderManager.getShader(ShaderManager::DOF).program, "CoC");


    /* --------------------------------------------------------------------------------------------- */
    /* ------------------------------------------ Geometry ----------------------------------------- */
    /* --------------------------------------------------------------------------------------------- */
    

    // Load geometry
    int   cube_triangleCount = 12;
    int   cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
    int   plane_triangleCount = 2;
    int   plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float plane_uvs[] = {0.f, 0.f, 0.f, 10.f, 10.f, 0.f, 10.f, 10.f};
    float plane_vertices[] = {-50.0, -1.0, 50.0, 50.0, -1.0, 50.0, -50.0, -1.0, -50.0, 50.0, -1.0, -50.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

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
    glGenTextures(3, gbufferTextures);

    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create depth texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
    glGenTextures(4, fxBufferTextures);

    // Create first ping-pong texture
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[0]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create second ping-pong texture
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[1]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture => Coc
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[2]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture => Blur
    glBindTexture(GL_TEXTURE_2D, fxBufferTextures[3]);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create Framebuffer Object
    glGenFramebuffers(1, &fxBufferFbo);


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        exit( EXIT_FAILURE );
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Settings
    float blurSamples = 2.0;
    float focusPlane = 5.0;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    float gamma = 1.0;
    float sobelCoef = 1.0;

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
        glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 100.f); 
        glm::mat4 worldToView = glm::lookAt(camera.m_eye, camera.m_o, camera.m_up);
        glm::mat4 objectToWorld;
        glm::mat4 worldToScreen = projection * worldToView;
        glm::mat4 screenToWorld = glm::transpose(glm::inverse(worldToScreen));
        glm::mat4 screenToView = glm::inverse(projection);


        /* --------------------------------------------------------------------------------------------- */
        /* ----------------------------------- Remplissage Frame Buffer -------------------------------- */
        /* --------------------------------------------------------------------------------------------- */
       
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
        glDrawBuffers(2, gbufferDrawBuffers);

        // Viewport 
        glViewport( 0, 0, width, height  );

        // Default states
        glEnable(GL_DEPTH_TEST);

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind gbuffer shader
        glUseProgram(shaderManager.getShader(ShaderManager::GBUFFER).program);

        // Upload uniforms
        glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(projection));
        glUniformMatrix4fv(gbuffer_viewLocation, 1, 0, glm::value_ptr(worldToView));
        glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
        glUniform1f(gbuffer_timeLocation, t);
        glUniform1i(gbuffer_diffuseLocation, 0);
        glUniform1i(gbuffer_specLocation, 1);

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
        glUniform1i(lighting_materialLocation, 0);
        glUniform1i(lighting_normalLocation, 1);
        glUniform1i(lighting_depthLocation, 2);
        glUniform3fv(lighting_cameraPositionLocation, 1, glm::value_ptr(camera.m_eye));
        glUniformMatrix4fv(lighting_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));

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
        for (int i = 0; i < (int) numLights; ++i)
        {
            float tl = t * i;

            //Update light uniforms
            float lightPosition[3] = { sinf(tl) * 10.f, -0.5f, cosf(tl) * 10.f};
            float lightColor[3] = {sinf(tl) *  1.f, 1.f - cosf(tl), -sinf(tl)};
            float lightIntensity = 10.0;

            glUniform3fv(lighting_lightPositionLocation, 1, lightPosition);
            glUniform3fv(lighting_lightColorLocation, 1, lightColor);
            glUniform1f(lighting_lightIntensityLocation, lightIntensity);

            // Draw quad
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        }

        glDisable(GL_BLEND);

        //
        // COC
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[2], 0);

        // Clear the front buffer
        glViewport( 0, 0, width, height );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::COC).program);

        // Upload uniforms
        glUniform1i(coc_depthLocation, 2);
        glUniformMatrix4fv(coc_screenToViewLocation, 1, 0, glm::value_ptr(screenToView));
        glUniform1f(coc_focusLocation, focusPlane);
        glUniform1f(coc_nearPlaneLocation, nearPlane);
        glUniform1f(coc_farPlaneLocation, farPlane);

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);        

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);


        //
        // Sobel pass
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[1], 0);     

        // Clear the front buffer
        glViewport( 0, 0, width, height );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::SOBEL).program);

        // Upload uniforms
        glUniform1i(sobel_tex1Location, 0);
        glUniform1f(sobel_sobelLocation, sobelCoef);

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[0]); 

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //
        // BLUR
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[3], 0);     

        // Clear the front buffer
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::BLUR).program);

        // Upload uniforms
        glUniform1i(blur_tex1Location, 0);
        glUniform1i(blur_samplesLocation, static_cast<int>(blurSamples));

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[1]);

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //
        // DOF
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[0], 0);     

        // Clear the front buffer
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::DOF).program);

        // Upload uniforms
        glUniform1i(dof_colorLocation, 0);
        glUniform1i(dof_blurLocation, 1);
        glUniform1i(dof_cocLocation, 2);

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE0);//color
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[1]);
        glActiveTexture(GL_TEXTURE1);//blur
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[3]);
        glActiveTexture(GL_TEXTURE2);//coc
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[2]);

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //
        // Gamma pass
        //
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxBufferTextures[1], 0);     

        // Clear the front buffer
        glViewport( 0, 0, width, height );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind lighting shader
        glUseProgram(shaderManager.getShader(ShaderManager::GAMMA).program);

        // Upload uniforms
        glUniform1i(gamma_tex1Location, 0);
        glUniform1f(gamma_gammaLocation, gamma);

        // Bind textures we want to render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[0]);

        // Draw scene
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

   

        // Unbind framebuffer : now that we render all the textures, we can debind the fxBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //
        // Display main screen 
        //
        glViewport( 0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderManager.getShader(ShaderManager::BLIT).program);
        glUniform1i(blit_tex1Location, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[1]);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        //
        // Display miniatures (à la noix) 
        //

        // Diffuse
        glViewport( 0, 0, width/5, height/5  );
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Specular
        glViewport( width/5, 0, width/5, height/5  );
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Depth
        glViewport( width/5 * 2, 0, width/5, height/5  );
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Coc
        glViewport( width/5 * 3, 0, width/5, height/5  );
        glBindTexture(GL_TEXTURE_2D, fxBufferTextures[2]);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        // Blur
        glViewport( width/5 * 4, 0, width/5, height/5  );
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
        imguiSlider("Lights", &numLights, 0.0, 100.0, 1.0);
        imguiSlider("Gamma", &gamma, 0.0, 3.0, 0.1);
        imguiSlider("Sobel", &sobelCoef, 0.0, 1.0, 1.0);
        imguiSlider("Blur Samples", &blurSamples, 1.0, 100.0, 1.0);
        imguiSlider("Focus plane", &focusPlane, 1.0, 100.0, 1.0);
        imguiSlider("Near plane", &nearPlane, 1.0, 100.0, 1.0);
        imguiSlider("Far plane", &farPlane, 1.0, 100.0, 1.0);
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
