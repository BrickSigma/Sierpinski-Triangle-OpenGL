#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

// Used for an individual shader
typedef unsigned int Shader;

// Used for a full shader program linked together
typedef unsigned int ShaderProgram;

// Loads a shader and compiles it
Shader *LoadShader(const char *path, GLenum type);

// Deletes a shader
void DeleteShader(Shader *shader);

// Create a shader program from a vertext and fragment shader
ShaderProgram *CreateShaderProgram(Shader *vert, Shader *frag);

// Loads a shader program using vertext and fragment shader source files
ShaderProgram *LoadShaderProgram(const char *vert, const char *frag);

// Make OpenGL use the input shader program
void UseShaderProgram(ShaderProgram *program);

// Deletes a shader program
void DeleteShaderProgram(ShaderProgram *program);

#endif