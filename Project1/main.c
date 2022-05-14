#include <GL/glew.h> // должен всегда быть первым
#include <GL/freeglut.h>
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

GLuint vao;
GLuint vbo;
GLuint ibo;
GLuint projMatrLoc;
GLuint rotMatrLoc;


void setUniformLocations() // заносим юниформ переменные
{
    projMatrLoc = glGetUniformLocation(prog, "projMatr");
    rotMatrLoc = glGetUniformLocation(prog, "rotMatr");
}

void createBuffer();
void initVao();

void init() 
{
    createBuffer(); // функция создания вершин
    initVao();

    //  создаём шейдеры 
    GLuint shaders[] = 
    {
        createShader("shader.vs", GL_VERTEX_SHADER),
        createShader("shader.fs", GL_FRAGMENT_SHADER)
    };
    int len = sizeof(shaders) / sizeof(shaders[0]); // определяем колво шейдеров
    createProg(shaders, len); // создаём программу, содержащую эти шейдеры
    int i = 0;
    for (; i < len; i++) // удаление шейдеров
    {
        glDeleteShader(shaders[i]);
    }

    setUniformLocations();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void createBuffer() // функция создания вершин
{
    // массив индексов
    GLushort indices[] = 
    {
        0, 1, 2,
        0, 2, 3,
        3, 2, 6,
        3, 6, 7,
        4, 5, 1,
        4, 1, 0,
        7, 6, 5,
        7, 5, 4,
        1, 5, 6,
        1, 6, 2,
        4, 0, 3,
        4, 3, 7
    };

    float vertices[] = 
    {
        // vertices
        // front vertices:
        -1.0f, -1.0f, 1.0f,// left-bottom
        -1.0f,  1.0f, 1.0f,// left-top
         1.0f,  1.0f, 1.0f,// right-top
         1.0f, -1.0f, 1.0f,// right-bottom
        // back vertices:
        -1.0f, -1.0f, -1.0f,// left-bottom
        -1.0f,  1.0f, -1.0f,// left-top
         1.0f,  1.0f, -1.0f,// right-top
         1.0f, -1.0f, -1.0f,// right-bottom

         // colors
         
         //1.0f, 0.0f, 0.0f, 1.0f, // red
         //0.0f, 1.0f, 0.0f, 1.0f, // green
         //1.0f, 0.0f, 1.0f, 1.0f, // purple
         //1.0f, 1.0f, 0.0f, 1.0f, // yellow
         //0.0f, 1.0f, 1.0f, 1.0f, // aqua
         //0.5f, 0.0f, 0.0f, 1.0f, // dark-red
         //0.0f, 0.5f, 0.0f, 1.0f, // dark-green
         //0.0f, 0.0f, 0.5f, 1.0f  // dark-blue

         
         1.0f, 0.0f, 0.0f, 1.0f, // red
         1.0f, 0.0f, 0.0f, 1.0f, // red
         0.0f, 1.0f, 0.0f, 1.0f, // green
         0.0f, 1.0f, 0.0f, 1.0f, // green
         0.0f, 0.0f, 1.0f, 0.0f, // blue
         0.0f, 0.0f, 1.0f, 0.0f, // blue
         1.0f, 1.0f, 0.0f, 1.0f, // dark-green
         1.0f, 1.0f, 0.0f, 1.0f  // dark-blue
    };

    glGenBuffers(1, &vbo); // просим создать 1 объект и идентификатор положить в переменную vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // просим опенгл сохранить эту ссылку в эрей бафер
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // функция заполнения, которой говорим, что переменная на которую указывает эрей бафер, должна быть размером sizeof(vertices), а также, что данные должны быть скопиированы, с помощью GL_STATIC_DRAW гвоорим, что данные изменять не будем  
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // массив индексов ibo
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // такая же функция заполнения
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initVao() {
    glGenVertexArrays(1, &vao); // генерируем объект вао
    glBindVertexArray(vao); // бинди, чтобы настройки применялись

    // vertex array object will store next things:
    // 1. Binding to GL_ELEMENT_ARRAY_BUFFER
    // 2. Enabled vertex attrib arrays
    // 3. Vertex attrib pointer settings

    glEnableVertexAttribArray(0); // сохраняем эти настройки в вао
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glVertexAttribPointer will use GL_ARRAY_BUFFER
    // , which was set by glBindBuffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(8 * 3 * sizeof(float)));
    // Binding to GL_ARRAY_BUFFER will not be stored in
    // the vertex array object,
    // but it was already defined for glVertexAttribPointer.
    glBindBuffer(GL_ARRAY_BUFFER, 0); // убираем буфер

    // This will be used by glDrawElements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // биндим ещё один буфер

    glBindVertexArray(0); // убираем буфер

}

float* rotationMatr(float elapsed) 
{
    float part = fmodf(elapsed, 5.0f) / 5.0f; //обновление каждые 5 секунд
    float angle = 3.14159f * 2.0f * part;
    
    // вращение по у
    /*float res[] = 
    {
        cosf(angle), 0, -sinf(angle), 0,
        0, 1, 0, 0,
        sinf(angle), 0, cosf(angle), 0,
        0, 0, 0, 1
    };*/

    // вращение по х
    /*float res[] =
    {
        1, 0, 0, 0,
        0, cosf(angle), -sinf(angle), 0,
        0, sinf(angle), cosf(angle), 0,
        0, 0, 0, 1
    };*/

    // вращение по z
    float res[] =
    {
        cosf(angle), -sinf(angle), 0, 0,
        sinf(angle), cosf(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    float* matr = malloc(sizeof(float) * 16); // выделяем место
    memcpy(matr, res, sizeof(float) * 16);// копируем
    return matr;
}

void display() {
    glClearColor(0.0f, 1.0f, 1.0f, 0.0f); // цвет для очистки окна
    glClearDepth(1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// функция очистки, используюящая цвет из буфера
    glUseProgram(prog); // объект, в котором скомбинированы шейдеры
    glBindBuffer(GL_ARRAY_BUFFER, 0); //задействуем массив вершин

    glBindVertexArray(vao);
    float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // берём время
    float* matr = rotationMatr(elapsed); // отправляем
    glUniformMatrix4fv(rotMatrLoc, 1, GL_FALSE, matr);
    free(matr); // стираем
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0); // рисовать по индексам

    glBindVertexArray(0); // деактивируем
    glUseProgram(0); // деактивируем
    glutSwapBuffers(); // чтобы содержимаое появилось на окне, нужно свапнуть буферы
    glutPostRedisplay();
}

float toRad(float deg) // конвертация в радианы
{
    return deg * 3.141593f / 180.0f;
}

void reshape(int w, int h) // функция изменения размера окна
{
    float n = 0.2f; // расположение ближней плоскости
    float f = 1000.0f;// расположение дальней плоскости
    float fov = toRad(90); // поле зрение
    float aspect = w / (float)h;
    float t = n * tanf(fov / 2.0f);
    float r = t * aspect;

    // проекционная матрица (пишем по столбцам)
    float projMatr[] = 
    {
        n / r, 0,   0,              0,
        0,   n / t, 0,              0,
        0,   0,   (f + n) / (n - f),    -1,
        0,   0,   2.0f * n * f / (n - f), 0
    };

    //printf("n=%.1f  f=%.1f  fov=%.2f  aspect=%.2f  t=%.2f  r=%.2f\n", n, f, fov, aspect, t, r);
    glUseProgram(prog); // указываем программу
    glUniformMatrix4fv(projMatrLoc, 1, GL_FALSE, projMatr); // передаём значения в projMatrLoc
    glUseProgram(0); // деактивируем
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);// указания, как нужно выполнять трансформацию в оконные кординаты
}

int main(int argc, char* argv[]) 
{
    glutInit(&argc, argv);//инициализируем фриглут
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL); // используем двойную буферизацию
    glutInitWindowSize(600, 600);// создаём окно 600 на 600
    glutInitWindowPosition(100, 100);// позиция 100 100
    glutCreateWindow("кубик");// создание окна с именем 

    GLenum res = glewInit(); // проверка на ошибки
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(res));
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    init();
    glutMainLoop(); //запустить цикл обработки сообщений
    return 0;
}
