#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <GLUT/glut.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <stack>
#include <memory>
#include <stdio.h>
#include <fstream>
#include "../SOIL.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800


GLfloat rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
GLfloat transX = 0.0f, transY = 0.0f, transZ = 0.0f;
GLfloat figureScale = 2.0f;
GLfloat offset = 0.0f;
GLfloat matrix[16];
GLfloat t = 30;
GLint n = 4;
GLint parts = 1;
GLenum m = GL_LINE;
bool is_d = true;
float k_z = 3.0/4.0;


using namespace std;

void projection() {
    GLfloat teta = asin(k_z / sqrt(2));
    GLfloat fi = asin(k_z / sqrt(2 - k_z * k_z));
    // GETTING THE MATRIX
    matrix[0]  = cos(fi); matrix[1]  = sin(fi)*sin(teta);  matrix[2]  = sin(fi)*cos(teta);   matrix[3]  = 0;
    matrix[4]  = 0;       matrix[5]  = cos(teta);          matrix[6]  = -sin(teta);          matrix[7]  = 0;
    matrix[8]  = sin(fi); matrix[9]  = -cos(fi)*sin(teta); matrix[10] = -cos(teta)*cos(fi);  matrix[11] = 0;
    matrix[12] = 0;       matrix[13] = 0;                  matrix[14] = 0;                   matrix[15] = 1;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    const GLfloat speed = 10;
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_DOWN:
                rotX += speed;
                break;
            case GLFW_KEY_RIGHT:
                rotY += speed;
                break;
            case GLFW_KEY_LEFT:
                rotY -= speed;
                break;
            case GLFW_KEY_D:
                rotZ -= speed;
                break;
            case GLFW_KEY_SPACE:
                (m == GL_LINE)? m = GL_FILL : m = GL_LINE;
                break;
            case GLFW_KEY_UP:
                rotX -= speed;
                break;
            case GLFW_KEY_A:
                rotZ += speed;
                break;
            case GLFW_KEY_Q:
                transX -= speed*5;
                break;
            case GLFW_KEY_W:
                transX += speed*5;
                break;
            case GLFW_KEY_E:
                transY += speed*5;
                break;
            case GLFW_KEY_R:
                transY -= speed*5;
                break;
            case GLFW_KEY_MINUS:
                n--;
                break;
            case GLFW_KEY_1:
                parts++;
                break;
            case GLFW_KEY_2:
                (parts < 2)? parts = 1 : parts--;
                break;
            case GLFW_KEY_5:
                t += 5;
                break;
            case GLFW_KEY_6:
                t -= 5;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_T:
                transZ -= speed*5;
                break;
            case GLFW_KEY_Y:
                transZ += speed*5;
                break;
            case GLFW_KEY_B:
                (figureScale > 0.9) ? figureScale = 3 : figureScale += 0.1;
                break;
            case GLFW_KEY_S:
                (figureScale < 0.2f) ? figureScale = 0.1 : figureScale -= 0.1;
                break;
            case GLFW_KEY_0:
                is_d = !is_d;
                break;
            case GLFW_KEY_EQUAL:
                n++;
                break;
        }
        
    }
    
}


void display() {
    
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(40.0f, -40.0f, -40.0f);
    glVertex3f(40.0f, 40.0f, -40.0f);
    glVertex3f(-40.0f, 40.0f, -40.0f);
    glVertex3f(-40.0f, -40.0f, -40.0f);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(40.0f, -40.0f, 40.0f);
    glVertex3f(40.0f, 40.0f, 40.0f);
    glVertex3f(-40.0f, 40.0f, 40.0f);
    glVertex3f(-40.0f, -40.0f, 40.0f);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(40.0f, -40.0f, -40.0f);
    glVertex3f(40.0f, 40.0f, -40.0f);
    glVertex3f(40.0f, 40.0f, 40.0f);
    glVertex3f(40.0f, -40.0f, 40.0f);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-40.0f, -40.0f, 40.0f);
    glVertex3f(-40.0f, 40.0f, 40.0f);
    glVertex3f(-40.0f, 40.0f, -40.0f);
    glVertex3f(-40.0f, -40.0f, -40.0f);
    glEnd();
    
    // FRONT
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(40.0f, 40.0f, 40.0f);
    glVertex3f(40.0f, 40.0f, -40.0f);
    glVertex3f(-40.0f, 40.0f, -40.0f);
    glVertex3f(-40.0f, 40.0f, 40.0f);
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(40.0f, -40.0f, -40.0f);
    glVertex3f(40.0f, -40.0f, 40.0f);
    glVertex3f(-40.0f, -40.0f, 40.0f);
    glVertex3f(-40.0f, -40.0f, -40.0f);
    glEnd();
}


void sample() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(100, 50, 0);
    display();
}

void drawConicalCylinder(GLdouble a, GLdouble b, int points_am, bool isFill) {
    // константы необходимые для отрисовки
    double step = (double)1 / (double)points_am;
    double h = 100;
    double d = double(2) * h / parts;
    double c = double(t) / parts;
    
    GLenum rememberM = m;
    if (!isFill) {
        m = GL_LINE;
    }
    glPolygonMode(GL_FRONT_AND_BACK, m);
    m = rememberM;
    
    // Нижняя крышка
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(t/2, 0, -h);
    for (double angle = -step * M_PI; angle < 2 * M_PI; angle += step * M_PI) {
        glColor3f((cos(angle) + 1) / 2, (sin(angle) + 1) / 2, 0.7f);
        glVertex3f(t/2 + a*sin(angle), b*cos(angle), -h);
    }
    glEnd();
    
    // Делим на части
    for (double i = 0; i < parts; i++) {
        glBegin(GL_QUAD_STRIP);
        for (double angle = 0; angle <= 2 * M_PI; angle += step * M_PI) {
            glColor3f((sin(angle) + 1) / 2, (cos(angle) + 1) / 2, 0.7f);
            glVertex3f(-t/2 + c * i + a*sin(angle), b*cos(angle), h - (i * d));
            glVertex3f(-t/2 + c * (i + 1) + a*sin(angle), b*cos(angle), h - ((i + 1) * d));
        }
        glVertex3f(-t/2 + c * i + a*sin(0), b*cos(0), h - (i * d));
        glVertex3f(-t/2 + c * (i + 1) + a*sin(0), b*cos(0), h - ((i + 1) * d));
        glEnd();
    }
    
    // Верхняя крышка
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-t/2, 0, h);
    for (double angle = -step * M_PI; angle < 2 * M_PI; angle += step * M_PI) {
        glColor3f((cos(angle) + 1) / 2, (sin(angle) + 1) / 2, 0.7f);
        glVertex3f(-t/2 + a*sin(angle), b*cos(angle), h);
    }
    glEnd();
}




int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    
    if (!glfwInit())
    {
        return -1;
    }
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Khusnutdinov_lab#3", NULL, NULL);
    
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    
    
    int screenWidth, screenHeight;
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);
    
    projection();
    
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0.0f, 0.0f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, 0, screenHeight, -screenWidth, screenWidth);
        if (is_d) {
            glMultMatrixf(matrix);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        GLfloat halfSW = screenWidth / 2;
        GLfloat halfSH = screenHeight / 2;
        
        
        // Отрисовка
        
        glPushMatrix();
        glTranslatef(halfSW + transX, halfSH + transY, -500 - transZ);
        glRotatef(rotY, 0, 1, 0);
        glRotatef(rotZ, 0, 0, 1);
        glRotatef(rotX, 1, 0, 0);
        glScalef(figureScale, figureScale, figureScale);
        
        drawConicalCylinder(50, 50, n, true);
        sample();
        glTranslatef(-halfSW, -halfSH, 500);
        
        glPopMatrix();
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
