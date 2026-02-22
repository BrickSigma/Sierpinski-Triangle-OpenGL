#include "shaders/shader.h"

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#define INFO_LOG_SIZE 512

static int gl_success;
static char info_log[INFO_LOG_SIZE];

Shader *LoadShader(const char *path, GLenum type)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("Could not open shader file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(fsize + 1);
    if (buffer == NULL)
    {
        perror("Could not allocate memory for buffer");
        fclose(file);
        return NULL;
    }
    fread(buffer, fsize, 1, file);
    fclose(file);

    buffer[fsize] = 0;

    Shader *shader = (Shader *)malloc(sizeof(Shader));
    if (shader == NULL)
    {
        perror("Could not allocate memory for shader");
        free(buffer);
        return NULL;
    }
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, (const char *const *)&buffer, NULL);
    glCompileShader(*shader);

    free(buffer);

    // Check if the shader compiled without errors
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &gl_success);
    if (!gl_success)
    {
        glGetShaderInfoLog(*shader, INFO_LOG_SIZE, NULL, info_log);
        printf("Could not compile shader: %s\n", info_log);
        free(shader);
        return NULL;
    }

    return shader;
}

void DeleteShader(Shader *shader)
{
    glDeleteShader(*shader);
    free(shader);
}

ShaderProgram *CreateShaderProgram(Shader *vert, Shader *frag)
{
    ShaderProgram *program = (ShaderProgram *)malloc(sizeof(ShaderProgram));
    if (program == NULL)
    {
        perror("Could not allocate memory for shader program");
        return NULL;
    }
    *program = glCreateProgram();
    glAttachShader(*program, *vert);
    glAttachShader(*program, *frag);
    glLinkProgram(*program);

    glGetShaderiv(*program, GL_LINK_STATUS, &gl_success);
    if (!gl_success)
    {
        glGetShaderInfoLog(*program, INFO_LOG_SIZE, NULL, info_log);
        printf("Could not compile shader: %s\n", info_log);
        free(program);
        return NULL;
    }

    return program;
}

ShaderProgram *LoadShaderProgram(const char *vert, const char *frag)
{
    Shader *vertex_shader = LoadShader(vert, GL_VERTEX_SHADER);
    if (vertex_shader == NULL)
    {
        printf("Could not load vertex shader!\n");
        return NULL;
    }

    Shader *fragment_shader = LoadShader(frag, GL_FRAGMENT_SHADER);
    if (fragment_shader == NULL)
    {
        printf("Could not load fragment shader!\n");
        DeleteShader(vertex_shader);
        return NULL;
    }

    ShaderProgram *program = CreateShaderProgram(vertex_shader, fragment_shader);
    if (program == NULL)
    {
        printf("Could not link shader program!\n");
    }

    DeleteShader(vertex_shader);
    DeleteShader(fragment_shader);

    return program;
}

void UseShaderProgram(ShaderProgram *program)
{
    glUseProgram(*program);
}

void DeleteShaderProgram(ShaderProgram *program)
{
    glDeleteProgram(*program);
    free(program);
}
