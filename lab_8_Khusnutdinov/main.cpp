#define GL_SILENCE_DEPRECATION

#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

using namespace std;
using namespace glm;

int width = 800, height = 800;
int d = 800;
float trans_x = 0.0;
float trans_y = 0.0;
float trans_z = 0.0;
float rotate_y = 0;
float rotate_x = 0;
float scalexyz = 0.5;
static const uint32_t cubeVerticesCount = 32;
static const uint32_t cubeIndicesCount = 36;
unsigned int VBO, VAO, EBO;
GLfloat matrix[16];
float k_z = 3.0/4.0;

mat4 Translate(1);
mat4 Translate1(1);
mat4 Projection(1);
mat4 View1(1);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    if (width >= height)
        d = height;
    else
        d = width;
    glViewport(0, 0, width, height);
}

static const float s = 1.0f;

static float cubeVertices[cubeVerticesCount]{
    -s, s, -s, s,
    s, s, -s, s,
    s, -s, -s, s,
    -s, -s, -s, s,
    -s, s, s, s,
    s, s, s, s,
    s, -s, s, s,
    -s, -s, s, s
};

static int cubeIndices[cubeIndicesCount] = {
    0, 1, 2,
    0, 2, 3,
    2, 1, 5,
    2, 5, 6,
    3, 2, 6,
    3, 6, 7,
    0, 3, 7,
    0, 7, 4,
    1, 0, 4,
    1, 4, 5,
    6, 5, 4,
    6, 4, 7,
};

void projection() {

    GLfloat teta = asin(k_z / sqrt(2));
    GLfloat fi = asin(k_z / sqrt(2 - k_z * k_z));
    // GETTING THE MATRIX
    matrix[0]  = cos(fi); matrix[1]  = sin(fi)*sin(teta);  matrix[2]  = sin(fi)*cos(teta);   matrix[3]  = 0;
    matrix[4]  = 0;       matrix[5]  = cos(teta);          matrix[6]  = -sin(teta);          matrix[7]  = 0;
    matrix[8]  = sin(fi); matrix[9]  = -cos(fi)*sin(teta); matrix[10] = -cos(teta)*cos(fi);  matrix[11] = 0;
    matrix[12] = 0;       matrix[13] = 0;                  matrix[14] = 0;                   matrix[15] = 1;
}


void transforming() {
    projection();
    Projection = make_mat4(matrix);
    View1 = rotate(View1, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
    
    Translate = translate(Translate, vec3(trans_x, trans_y, trans_z));
    Translate = scale(Translate, vec3(scalexyz, scalexyz, scalexyz));
    Translate = rotate(Translate, rotate_y, vec3(0.0f, 1.0f, 0.0f));
    Translate = rotate(Translate, rotate_x, vec3(1.0f, 0.0f, 0.0f));
}

int main(int argc, char **argv) {
    
    // set up glfw
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE);
    
    GLFWwindow *window = glfwCreateWindow(width, height, "Khusnutdinov_lab#8", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetKeyCallback(window, key_callback);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

    // Init glew
    glewExperimental = GL_TRUE;
    glewInit();
    
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    // set up shader
    Shader shader1("resources/shaders/core.vs", "resources/shaders/core.frag");
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cubeVerticesCount, cubeVertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * cubeIndicesCount, cubeIndices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    GLint transform = glGetUniformLocation(shader1.Program, "transform");
    GLint view = glGetUniformLocation(shader1.Program, "view");
    GLint projection = glGetUniformLocation(shader1.Program, "projection");
    
    
    transforming();
    
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 0);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader1.Use();
        glUniformMatrix4fv(transform, 1, GL_FALSE, value_ptr(Translate));
        glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(View1));
        glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(Projection));
        
        // drawing with EBO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_UP)
        rotate_x -= 0.001;
    if (key == GLFW_KEY_DOWN)
        rotate_x += 0.001;
    if (key == GLFW_KEY_RIGHT)
        rotate_y += 0.001;
    if (key == GLFW_KEY_LEFT)
        rotate_y -= 0.001;
    
    if (key == '-') {
        scalexyz -= 0.01;
    }
    if (key == '=') {
        scalexyz += 0.01;
    }
    if (key == GLFW_KEY_A)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_Q)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if (key == GLFW_KEY_E)
        trans_y += 0.02;
    if (key == GLFW_KEY_X)
        trans_y -= 0.02;
    if (key == GLFW_KEY_D)
        trans_x += 0.02;
    if (key == GLFW_KEY_S)
        trans_x -= 0.02;
    if (key == GLFW_KEY_G)
        trans_z += 0.02;
    if (key == GLFW_KEY_F)
        trans_z -= 0.02;
    
    Translate = mat4(1);
    Translate = translate(Translate, vec3(trans_x, trans_y, 0.0f));
    Translate = scale(Translate, vec3(scalexyz, scalexyz, scalexyz));
    Translate = rotate(Translate, 10 * rotate_y, vec3(0.0f, 1.0f, 0.0f));
    Translate = rotate(Translate, 10 * rotate_x, vec3(1.0f, 0.0f, 0.0f));
    
}
