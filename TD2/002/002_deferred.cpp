#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

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

struct ShaderGLSL
{
    enum ShaderType
    {
        VERTEX_SHADER = 1,
        FRAGMENT_SHADER = 2,
        GEOMETRY_SHADER = 4
    };
    GLuint program;
};

int compile_and_link_shader(ShaderGLSL & shader, int typeMask, const char * sourceBuffer, int bufferSize);
int destroy_shader(ShaderGLSL & shader);
int load_shader_from_file(ShaderGLSL & shader, const char * path, int typemask);
    
struct Camera
{
    float radius;
    float theta;
    float phi;
    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;
};

void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);

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

struct Light {
    Light(glm::vec3 position, glm::vec3 color, float intensity) {
        m_intensity = intensity;
        m_color = color;
        m_position = position;
        m_specCoeff = 10.f;

        m_spotDirection = glm::vec3(0., 0., 0.);
        m_phi = 0;
    }
    
    float m_intensity;
    float m_specCoeff;
    glm::vec3 m_color;
    glm::vec3 m_position;

    // Spot Light
    glm::vec3 m_spotDirection;
    float m_phi; // Angle d'ouverture du spot, en degrée

    GLuint m_intensityLocation;
    GLuint m_lightSpecCoeffLocation;
    GLuint m_lightColorLocation;
    GLuint m_lightPositionLocation;

    GLuint m_spotDirectionLocation;
    GLuint m_spotAngleLocation;
};

Light* createLight(GLuint program, glm::vec3 position, glm::vec3 color, float intensity){
    Light* light = new Light(position, color, intensity);

    light->m_intensityLocation = glGetUniformLocation(program, "LightIntensity");
    light->m_lightSpecCoeffLocation = glGetUniformLocation(program, "LightSpecCoeff");
    light->m_lightColorLocation = glGetUniformLocation(program, "LightColor");
    light->m_lightPositionLocation = glGetUniformLocation(program, "LightPosition");

    light->m_spotDirectionLocation = glGetUniformLocation(program, "SpotDirection");
    light->m_spotAngleLocation = glGetUniformLocation(program, "SpotAngle");

    return light;
}

void sendLight(Light* light){
    glUniform1f(light->m_intensityLocation, light->m_intensity);
    glUniform1f(light->m_lightSpecCoeffLocation, light->m_specCoeff);
    glUniform3fv(light->m_lightColorLocation, 1, glm::value_ptr(light->m_color));
    glUniform3fv(light->m_lightPositionLocation, 1, glm::value_ptr(light->m_position));

    glUniform3fv(light->m_spotDirectionLocation, 1, glm::value_ptr(light->m_spotDirection));
    glUniform1f(light->m_spotAngleLocation, light->m_phi);
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
    glfwEnable(GLFW_STICKY_KEYS);

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
    camera_defaults(camera);
    GUIStates guiStates;
    init_gui_states(guiStates);

    // GUI
    float numLights = 30.f;
    float lightsPerCircle = 10.f;
    float typeLight = 2.f;
    float nbCubes = 100.f;
    float cubesPerCircle = 7.f;
    float spaceBetweenCubes = 1.f;
    float cubesRotation = 1.f;

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

    // Try to load and compile shader
    int status;
    ShaderGLSL gbuffer_shader;
    const char * shaderFileGBuffer = "002/2_gbuffer.glsl";
    //int status = load_shader_from_file(gbuffer_shader, shaderFileGBuffer, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    status = load_shader_from_file(gbuffer_shader, shaderFileGBuffer, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFileGBuffer);
        exit( EXIT_FAILURE );
    }    

    // Compute locations for gbuffer_shader
    GLuint gbuffer_projectionLocation = glGetUniformLocation(gbuffer_shader.program, "Projection");
    GLuint gbuffer_viewLocation = glGetUniformLocation(gbuffer_shader.program, "View");
    GLuint gbuffer_objectLocation = glGetUniformLocation(gbuffer_shader.program, "Object");
    GLuint gbuffer_timeLocation = glGetUniformLocation(gbuffer_shader.program, "Time");
    GLuint gbuffer_nbCubesLocation = glGetUniformLocation(gbuffer_shader.program, "NbCubes");
    GLuint gbuffer_cubesPerCircleLocation = glGetUniformLocation(gbuffer_shader.program, "CubesPerCircle");
    GLuint gbuffer_spaceBetweenCircleLocation = glGetUniformLocation(gbuffer_shader.program, "SpaceBetweenCircle");
    GLuint gbuffer_cubesRotationLocation = glGetUniformLocation(gbuffer_shader.program, "CubesRotation");
    GLuint gbuffer_diffuseLocation = glGetUniformLocation(gbuffer_shader.program, "Diffuse");
    GLuint gbuffer_specLocation = glGetUniformLocation(gbuffer_shader.program, "Spec");

    // Load Blit shader
    ShaderGLSL blit_shader;
    const char * shaderFileBlit = "002/2_blit.glsl";
    //int status = load_shader_from_file(blit_shader, shaderFileBlit, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    status = load_shader_from_file(blit_shader, shaderFileBlit, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFileBlit);
        exit( EXIT_FAILURE );
    }    

    // Compute locations for blit_shader
    GLuint blit_tex1Location = glGetUniformLocation(blit_shader.program, "Texture1");

    // Load  point light accumulation shader
    ShaderGLSL point_lighting_shader;
    const char * shaderFilePointLighting = "002/2_point_light.glsl";
    //int status = load_shader_from_file(point_lighting_shader, shaderFilePointLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    status = load_shader_from_file(point_lighting_shader, shaderFilePointLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFilePointLighting);
        exit( EXIT_FAILURE );
    }    
    // Compute locations for point_lighting_shader
    GLuint point_lighting_materialLocation = glGetUniformLocation(point_lighting_shader.program, "Material");
    GLuint point_lighting_normalLocation = glGetUniformLocation(point_lighting_shader.program, "Normal");
    GLuint point_lighting_depthLocation = glGetUniformLocation(point_lighting_shader.program, "Depth");
    GLuint point_lighting_inverseViewProjectionLocation = glGetUniformLocation(point_lighting_shader.program, "InverseViewProjection");
    GLuint point_lighting_cameraPositionLocation = glGetUniformLocation(point_lighting_shader.program, "CameraPosition");

    // Load directional light accumulation shader
    ShaderGLSL directional_lighting_shader;
    const char * shaderFileDirectionalLighting = "002/2_directional_light.glsl";
    //int status = load_shader_from_file(directional_lighting_shader, shaderFileDirectionalLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    status = load_shader_from_file(directional_lighting_shader, shaderFileDirectionalLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFileDirectionalLighting);
        exit( EXIT_FAILURE );
    }    
    // Compute locations for directional_lighting_shader
    GLuint directional_lighting_materialLocation = glGetUniformLocation(directional_lighting_shader.program, "Material");
    GLuint directional_lighting_normalLocation = glGetUniformLocation(directional_lighting_shader.program, "Normal");
    GLuint directional_lighting_depthLocation = glGetUniformLocation(directional_lighting_shader.program, "Depth");
    GLuint directional_lighting_inverseViewProjectionLocation = glGetUniformLocation(directional_lighting_shader.program, "InverseViewProjection");
    GLuint directional_lighting_cameraPositionLocation = glGetUniformLocation(directional_lighting_shader.program, "CameraPosition");

    // Load spot light accumulation shader
    ShaderGLSL spot_lighting_shader;
    const char * shaderFileSpotLighting = "002/2_spot_light.glsl";
    //int status = load_shader_from_file(spot_lighting_shader, shaderFileSpotLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER | ShaderGLSL::GEOMETRY_SHADER);
    status = load_shader_from_file(spot_lighting_shader, shaderFileSpotLighting, ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  %s\n", shaderFileSpotLighting);
        exit( EXIT_FAILURE );
    }    
    // Compute locations for spot_lighting_shader
    GLuint spot_lighting_materialLocation = glGetUniformLocation(spot_lighting_shader.program, "Material");
    GLuint spot_lighting_normalLocation = glGetUniformLocation(spot_lighting_shader.program, "Normal");
    GLuint spot_lighting_depthLocation = glGetUniformLocation(spot_lighting_shader.program, "Depth");
    GLuint spot_lighting_inverseViewProjectionLocation = glGetUniformLocation(spot_lighting_shader.program, "InverseViewProjection");
    GLuint spot_lighting_cameraPositionLocation = glGetUniformLocation(spot_lighting_shader.program, "CameraPosition");

    // Lights
    Light* pSun = createLight(directional_lighting_shader.program, glm::vec3(0., -4., 0.), glm::vec3(1.0, 1.0, 0.8), 1.3);

    // Load geometry
    // Cube
    int   cube_triangleCount = 12;
    int   cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
    
    // Plane 
    int   plane_triangleCount = 2;
    int   plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float plane_uvs[] = {0.f, 0.f, 0.f, 10.f, 10.f, 0.f, 10.f, 10.f};
    float plane_vertices[] = {-50.0, -2.0, 50.0, 50.0, -2.0, 50.0, -50.0, -2.0, -50.0, 50.0, -2.0, -50.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    
    // Quad
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

    // Init frame buffers
    GLuint gbufferFbo;
    GLuint gbufferTextures[3];
    GLuint gbufferDrawBuffers[2];
    glGenTextures(3, gbufferTextures);

    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create RGBA32F texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create depth texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create Framebuffer Object
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, gbufferTextures[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, gbufferTextures[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);
    gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    // Note : GL_DEPTH_ATTACHMENT is filled automaticaly.

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error on building framebuffer\n");
        exit( EXIT_FAILURE );
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
                camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera_pan(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }
  
        // Get camera matrices
        glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 100.f); 
        glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
        glm::mat4 objectToWorld;
        glm::mat4 worldToScreen = projection * worldToView;
        glm::mat4 screenToWorld = glm::transpose(glm::inverse(worldToScreen));

        // Bind gbuffer shader
        glUseProgram(gbuffer_shader.program);
        
        // Upload uniforms
        glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(projection));
        glUniformMatrix4fv(gbuffer_viewLocation, 1, 0, glm::value_ptr(worldToView));
        glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, glm::value_ptr(objectToWorld));
        glUniform1f(gbuffer_timeLocation, t);
        glUniform1f(gbuffer_nbCubesLocation, nbCubes);
        glUniform1f(gbuffer_cubesPerCircleLocation, cubesPerCircle);
        glUniform1f(gbuffer_spaceBetweenCircleLocation, spaceBetweenCubes);
        glUniform1f(gbuffer_cubesRotationLocation, cubesRotation);
        glUniform1i(gbuffer_diffuseLocation, 0);
        glUniform1i(gbuffer_specLocation, 1);

        /////////////////////////////
        // Draw on the FrameBuffer //
        /////////////////////////////

        // Bind personnal buffer
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
    
        // Default states
        glEnable(GL_DEPTH_TEST);

        // Clear the FrameBuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate buffer list when drawing
        glDrawBuffers(2, gbufferDrawBuffers);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        // Render vaos
        glBindVertexArray(vao[0]);
        glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, nbCubes);
        glBindVertexArray(vao[1]);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // Debind personnal buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /////////////////////////////////////////////////////////////////
        // Compute all lights and render the merge into the main window //
        /////////////////////////////////////////////////////////////////

        // Reset Viewport
        glViewport(0, 0, width, height);
        //camera.setViewport(0, 0, width, height);
        // Clear FrameBuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Disable Depth
        glDisable(GL_DEPTH_TEST);

        // Keyboard events
        int onePressed = glfwGetKey('A');
        int twoPressed = glfwGetKey('Z');
        int threePressed = glfwGetKey('E');

        if (onePressed == GLFW_PRESS)
        {
            typeLight = 0.;

        }
        else if(twoPressed == GLFW_PRESS)
        {
            typeLight = 1.;
        }
        else if(threePressed == GLFW_PRESS)
        {
            typeLight = 2.;
        }

        // Point light
        if(typeLight == 0.){
            glUseProgram(point_lighting_shader.program);

            // Update uniform of the shader
            glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(projection));
            glUniform1i(point_lighting_materialLocation, 0);
            glUniform1i(point_lighting_normalLocation, 1);
            glUniform1i(point_lighting_depthLocation, 2);
            glUniform3fv(point_lighting_cameraPositionLocation, 1, glm::value_ptr(camera.eye));
            glUniformMatrix4fv(point_lighting_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
        }
        // Directional light -> our sun
        else if(typeLight == 1.){
            glUseProgram(directional_lighting_shader.program);
            glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(projection));
            glUniform1i(directional_lighting_materialLocation, 0);
            glUniform1i(directional_lighting_normalLocation, 1);
            glUniform1i(directional_lighting_depthLocation, 2);
            glUniform3fv(directional_lighting_cameraPositionLocation, 1, glm::value_ptr(camera.eye));
            glUniformMatrix4fv(directional_lighting_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
        }
        // Spot light
        else if(typeLight == 2.){
            glUseProgram(spot_lighting_shader.program);

            // Update uniform of the shader
            glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, glm::value_ptr(projection));
            glUniform1i(spot_lighting_materialLocation, 0);
            glUniform1i(spot_lighting_normalLocation, 1);
            glUniform1i(spot_lighting_depthLocation, 2);
            glUniform3fv(spot_lighting_cameraPositionLocation, 1, glm::value_ptr(camera.eye));
            glUniformMatrix4fv(spot_lighting_inverseViewProjectionLocation, 1, 0, glm::value_ptr(screenToWorld));
        }

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);

        // Draw a quad per light
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        sendLight(pSun);
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        float delta = 2 * 3.14f / (lightsPerCircle);
        float count = 0;
        if(lightsPerCircle != 0) {
            count = numLights/lightsPerCircle;
        }

        
        srand(time(NULL));

        // std::cout << numLights/lightsPerCircle << std::endl;
        while(count > 0)
        {        
   
            for (int i = 0; i < lightsPerCircle; ++i)
            {
                // Lumières tournent + rayon
                float c = cos(i * delta + t);// * cos(t);
                float s = sin(i * delta + t);// * cos(t);

                // Lumières en spirales

                // Couleur aléatoire
                float r = (rand() % 10) / 10.f;
                float g = (rand() % 10) / 10.f;
                float b = (rand() % 10) / 10.f;

                glm::vec3 position = glm::vec3( 5*count*c, 0,  5*count*s);

                Light* light; 
                if(typeLight == 0.){
                    light = createLight(point_lighting_shader.program, position, glm::vec3(1.0, 1.0, 1.0), 1.0);
                } 
                else if(typeLight == 1.){
                    light = pSun;
                }
                else if(typeLight == 2.){
                    light = createLight(spot_lighting_shader.program, position, glm::vec3(r, g, b), 1.0);
                    light->m_phi = 60.;
                    light->m_spotDirection = glm::vec3(0, 1, 0);
                }
                sendLight(light);
                glBindVertexArray(vao[2]);
                glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
            }
            count--;
        }
        
       

        glDisable(GL_BLEND); 

        /////////////////////////////////////////////
        // Draw data from FrameBuffer into 3 quads //
        /////////////////////////////////////////////

        glDisable(GL_DEPTH_TEST);

        // Bind blit shader
        glUseProgram(blit_shader.program);
        // Update uniform of the shader
        glUniform1i(blit_tex1Location, 0);

        // Activate the unit texture 0 (TEXTURE0)
        glActiveTexture(GL_TEXTURE0);

        // Quad 1/3 - Diffuse color
        glViewport(0, 0, width/3, height/4);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
        //draw the quad
        glBindVertexArray(vao[2]); 
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // Quad 2/3 - Normal
        glViewport(width/3, 0, width/3, height/4);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
        glBindVertexArray(vao[2]);  // Not necessary because already binded 
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

        // Quad 3/3 - Depth
        glViewport(2*width/3, 0, width/3, height/4);
        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
        glBindVertexArray(vao[2]);  // Not necessary because already binded 
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
        imguiBeginScrollArea("002 - deferred", 0, height/4, 220, 3*height/4-25, &logScroll);
        sprintf(lineBuffer, "FPS %f", fps);
        imguiLabel(lineBuffer);

        // Lights parameters
        imguiSlider("Lights", &numLights, lightsPerCircle, 100.0, 1.0);
        imguiSlider("Lights per circle", &lightsPerCircle, 0.0, numLights, 1.0);

        // Cubes parameters
        imguiSlider("NbCubes", &nbCubes, 0.0, 500.0, 1.0);
        imguiSlider("Cubes per circle", &cubesPerCircle, 1.0, 10.0, 1.0);
        imguiSlider("Space between cubes", &spaceBetweenCubes, 0.0, 5.0, 0.1);
        imguiSlider("Rotation of cubes", &cubesRotation, 0.0, 1.0, 1.0);
        
        // Sun parameters
        if(typeLight == 1.){
            imguiLabel("Sun parameter directional light");
            imguiSlider("pos X", &pSun->m_position.x, -10.0, 10.0, 0.1);
            imguiSlider("pos Y", &pSun->m_position.y, -10.0, 10.0, 0.1);
            imguiSlider("pos Z", &pSun->m_position.z, -10.0, 10.0, 0.1);
            imguiSlider("R", &pSun->m_color.r, 0.0, 1.0, 0.1);
            imguiSlider("G", &pSun->m_color.g, 0.0, 1.0, 0.1);
            imguiSlider("B", &pSun->m_color.b, 0.0, 1.0, 0.1);
            imguiSlider("Intensity", &pSun->m_intensity, 0.0, 10.0, 0.1);
        }
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




int  compile_and_link_shader(ShaderGLSL & shader, int typeMask, const char * sourceBuffer, int bufferSize)
{
    // Create program object
    shader.program = glCreateProgram();
    
    //Handle Vertex Shader
    GLuint vertexShaderObject ;
    if (typeMask & ShaderGLSL::VERTEX_SHADER)
    {
        // Create shader object for vertex shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        // Add #define VERTEX to buffer
        const char * sc[3] = { "#version 150\n", "#define VERTEX\n", sourceBuffer};
        glShaderSource(vertexShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(vertexShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(vertexShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling vertex shader : %s", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, vertexShaderObject);
    }

    // Handle Geometry shader
    GLuint geometryShaderObject ;
    if (typeMask & ShaderGLSL::GEOMETRY_SHADER)
    {
        // Create shader object for Geometry shader
        geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
        // Add #define Geometry to buffer
        const char * sc[3] = { "#version 150\n", "#define GEOMETRY\n", sourceBuffer};
        glShaderSource(geometryShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(geometryShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(geometryShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(geometryShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling Geometry shader : %s \n", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(geometryShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, geometryShaderObject);
    }


    // Handle Fragment shader
    GLuint fragmentShaderObject ;
    if (typeMask && ShaderGLSL::FRAGMENT_SHADER)
    {
        // Create shader object for fragment shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        // Add #define fragment to buffer
        const char * sc[3] = { "#version 150\n", "#define FRAGMENT\n", sourceBuffer};
        glShaderSource(fragmentShaderObject, 
                       3, 
                       sc,
                       NULL);
        // Compile shader
        glCompileShader(fragmentShaderObject);

        // Get error log size and print it eventually
        int logLength;
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char * log = new char[logLength];
            glGetShaderInfoLog(fragmentShaderObject, logLength, &logLength, log);
            fprintf(stderr, "Error in compiling fragment shader : %s \n", log);
            fprintf(stderr, "%s\n%s\n%s", sc[0], sc[1], sc[2]);
            delete[] log;
        }
        // If an error happend quit
        int status;
        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
            return -1;          

        //Attach shader to program
        glAttachShader(shader.program, fragmentShaderObject);
    }


    // Bind attribute location
    glBindAttribLocation(shader.program,  0,  "VertexPosition");
    glBindAttribLocation(shader.program,  1,  "VertexNormal");
    glBindAttribLocation(shader.program,  2,  "VertexTexCoord");
    glBindFragDataLocation(shader.program, 0, "Color");
    glBindFragDataLocation(shader.program, 1, "Normal");

    // Link attached shaders
    glLinkProgram(shader.program);

    // Clean
    if (typeMask & ShaderGLSL::VERTEX_SHADER)
    {
        glDeleteShader(vertexShaderObject);
    }
    if (typeMask && ShaderGLSL::GEOMETRY_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }
    if (typeMask && ShaderGLSL::FRAGMENT_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }

    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(shader.program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(shader.program, logLength, &logLength, log);
        fprintf(stderr, "Error in linking shaders : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(shader.program, GL_LINK_STATUS, &status);        
    if (status == GL_FALSE)
        return -1;


    return 0;
}

int  destroy_shader(ShaderGLSL & shader)
{
    glDeleteProgram(shader.program);
    shader.program = 0;
    return 0;
}

int load_shader_from_file(ShaderGLSL & shader, const char * path, int typemask)
{
    int status;
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
        return -1;
    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    status = compile_and_link_shader( shader, typemask, buffer, fileSize );
    delete[] buffer;
    return status;
}


void camera_compute(Camera & c)
{
    c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;   
    c.eye.y = cos(c.phi) * c.radius + c.o.y ;
    c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;   
    c.up = glm::vec3(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
}

void camera_defaults(Camera & c)
{
    c.phi = 3.14/2.f;
    c.theta = 3.14/2.f;
    c.radius = 10.f;
    camera_compute(c);
}

void camera_zoom(Camera & c, float factor)
{
    c.radius += factor * c.radius ;
    if (c.radius < 0.1)
    {
        c.radius = 10.f;
        c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
    }
    camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
    c.theta += 1.f * theta;
    c.phi   -= 1.f * phi;
    if (c.phi >= (2 * M_PI) - 0.1 )
        c.phi = 0.00001;
    else if (c.phi <= 0 )
        c.phi = 2 * M_PI - 0.1;
    camera_compute(c);
}

void camera_pan(Camera & c, float x, float y)
{
    glm::vec3 up(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(c.o - c.eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    c.up = glm::normalize(glm::cross(side, fwd));
    c.o[0] += up[0] * y * c.radius * 2;
    c.o[1] += up[1] * y * c.radius * 2;
    c.o[2] += up[2] * y * c.radius * 2;
    c.o[0] -= side[0] * x * c.radius * 2;
    c.o[1] -= side[1] * x * c.radius * 2;
    c.o[2] -= side[2] * x * c.radius * 2;       
    camera_compute(c);
}
