//
//  main.cpp
//  SecondLab(OpenGL)
//
//  Created by Apple on 07/05/2018.
//  Copyright © 2018 Kek. All rights reserved.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 2560
#define SCREEN_HEIGHT 1600

using namespace std;

int main(void) {
    
    GLFWwindow *window;
    
    if (!glfwInit()){
        return -1;
    }
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello world", NULL, NULL);
    
    if (!window){
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    float vertices[] = {
        0, 300, 0.0, //top corner
        300, 300, 0.0,
        300, 0, 0.0, //bottom left corner
        0, 0, 0.0
    };
    
    GLfloat pointVertex[] = {
        SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2
    };
    
    GLfloat pointVertex2[] = {
        SCREEN_WIDTH * 0.75, SCREEN_HEIGHT / 2
    };
    
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    while (!glfwWindowShouldClose(window)){
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //render gl
        glEnableClientState(GL_VERTEX_ARRAY);
        glPointSize(50);
        glVertexPointer(2, GL_FLOAT, 0, pointVertex);
        glDrawArrays(GL_POINTS, 0, 1);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glPointSize(20);
        glVertexPointer(2, GL_FLOAT, 0, pointVertex2);
        glDrawArrays(GL_POINTS, 0, 1);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
        
    }
    glfwTerminate();
    return 0;
}
