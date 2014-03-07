#include "Shader.h"

#include <stdio.h> //FILE, fopen, fseek, ftell, stderr...

int Shader::compile_and_link_shader(int typeMask, const char * sourceBuffer, int bufferSize)
{
    // Create program object
    program = glCreateProgram();
    
    //Handle Vertex Shader
    GLuint vertexShaderObject ;
    if (typeMask & Shader::VERTEX_SHADER)
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
        glAttachShader(program, vertexShaderObject);
    }

    // Handle Geometry shader
    GLuint geometryShaderObject ;
    if (typeMask & Shader::GEOMETRY_SHADER)
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
        glAttachShader(program, geometryShaderObject);
    }


    // Handle Fragment shader
    GLuint fragmentShaderObject ;
    if (typeMask && Shader::FRAGMENT_SHADER)
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
        glAttachShader(program, fragmentShaderObject);
    }


    // Bind attribute location
    glBindAttribLocation(program,  0,  "VertexPosition");
    glBindAttribLocation(program,  1,  "VertexNormal");
    glBindAttribLocation(program,  2,  "VertexTexCoord");
    glBindFragDataLocation(program, 0, "Color");
    glBindFragDataLocation(program, 1, "Normal");

    // Link attached shaders
    glLinkProgram(program);

    // Clean
    if (typeMask & Shader::VERTEX_SHADER)
    {
        glDeleteShader(vertexShaderObject);
    }
    if (typeMask && Shader::GEOMETRY_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }
    if (typeMask && Shader::FRAGMENT_SHADER)
    {
        glDeleteShader(fragmentShaderObject);
    }

    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        fprintf(stderr, "Error in linking shaders : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);        
    if (status == GL_FALSE)
        return -1;


    return 0;
}

int Shader::destroy_shader()
{
    glDeleteProgram(program);
    program = 0;
    return 0;
}

int Shader::load_shader_from_file(const char * path, int typemask)
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
    status = compile_and_link_shader( typemask, buffer, fileSize );
    delete[] buffer;
    return status;
}