#include "util.h"
#include <stdio.h>

GLuint prog;

char* readFile(const char* fileName) {
    FILE* f = fopen(fileName, "rb");
    if (!f) {
        perror("fopen failed");
        exit(1);
    }
    if (fseek(f, 0, SEEK_END) == -1) // переходим в конец
    {
        perror("fseek failed");
        exit(1);
    }
    long len = ftell(f); // записываем позицию в лен
    if (len == -1) {
        perror("ftell failed");
        exit(1);
    }
    if (fseek(f, 0, SEEK_SET) == -1) {
        perror("fseek failed");
        exit(1);
    }
    char* res = malloc(len + 1); // выделяем память
    if (!res) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    if (fread(res, 1, len, f) != (size_t)len) {
        fprintf(stderr, "fread failed for file %s\n", fileName);
        exit(1);
    }
    if (fclose(f) == EOF) {
        perror("fclose failed");
        exit(1);
    }
    res[len] = 0;
    return res;
}

// приходит имя  и тип шейдера
GLuint createShader(const char* shaderFile, GLenum shaderType) 
{
    const char* strShaderType; // переменная для записи соответствующего имени
    if (shaderType == GL_VERTEX_SHADER) 
    {
        strShaderType = "vertex";
    }
    else if (shaderType == GL_GEOMETRY_SHADER) 
    {
        strShaderType = "geometry";
    }
    else if (shaderType == GL_FRAGMENT_SHADER) 
    {
        strShaderType = "fragment";
    }
    else 
    {
        fprintf(stderr, "Unrecognized shader type\n");
        exit(1);
    }
    GLuint shader = glCreateShader(shaderType); // создаётся объект шейдера данного типа
    if (!shader) 
    {
        fprintf(stderr, "Error creating shader of type %s\n", strShaderType);
        exit(1);
    }
    GLchar* content = readFile(shaderFile); // читаем содержимое файла
    glShaderSource(shader, 1, (const GLchar**)&content, NULL); // задали, что шейдер должен был связан с этим контентом
    free(content); // освобождаем контент 
    glCompileShader(shader); // компилируем шейдеры
    GLint status; // переменная для проверки на ошибки
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) // проверка
    {
        GLint infoLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen); // проверяем, сколько нужно памяти для логирования ошибки
        GLchar* info = malloc(sizeof(GLchar) * (infoLen + 1)); // выделяем память
        glGetShaderInfoLog(shader, infoLen, NULL, info); // получаем информацию
        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, info);
        exit(1);
    }
    return shader;
}

void createProg(GLuint* shaders, int len) 
{
    int i = 0;
    prog = glCreateProgram();// создаём программу
    if (!prog) // проверка, удалось ли создать
    {
        fprintf(stderr, "Failed to create shader program\n");
        exit(1);
    }
    for (; i < len; i++) 
    {
        glAttachShader(prog, shaders[i]); // присоединяем каждый шейдер
    }
    glLinkProgram(prog); // линкуем, чтобы это работало внутри видеоадаптера
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status); // проверка статуса
    if (status == GL_FALSE) 
    {
        GLint infoLen;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
        GLchar* info = malloc(sizeof(GLchar) * (infoLen + 1));
        glGetProgramInfoLog(prog, infoLen, NULL, info);
        fprintf(stderr, "Linker failure: %s\n", info);
        exit(1);
    }
}

