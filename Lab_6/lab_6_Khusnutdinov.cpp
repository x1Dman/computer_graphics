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

using namespace std;

struct Vertex {
    float x, y, z;
};

float rotY = 0;
float rotX = 0;
float figureScale = 1.0;
float transX = 0.0;
float transY = 0.0;
float transZ = 0.0;
float wid = 1.5;
float inc = 0.0;
float vx = 0.006, vy = 0.008, vz = 0.005;
float k_z = 3.0/4.0;
int steps = 0;
int k = 10, step = 10, flag;
bool lightsOn = false;
bool textureOn = false;
bool loaded = false;
bool moveOn;
bool fl = false;
GLuint texturePlace[1];
GLuint textureID;
GLfloat lightAng = 10.0;
GLfloat matrix[16];
Vertex **figure, **bottom;

// включение текстуры
void texture() {
    int width, height;
    unsigned char* image = SOIL_load_image("/Users/Apple/Desktop/OpenGL/glfw.glfw/glfw.glfw/Khusnutdinov_texture.bmp", &width, &height, 0, SOIL_LOAD_RGB);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
}


void keyboard_callback(GLFWwindow *window, int key, int scancode,
                       int action, int mods);

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    if (width <= height) {
        glViewport(0, 0, width, width);
    }
    else {
        glViewport(0, 0, height, height);
    }
}


// сохранение состояния в файл
void save() {
    FILE *file;
    file = fopen("lab_6_save.txt", "w+");
    if(file == 0) {
        perror("fopen");
        exit(1);
    }
    fprintf(file, "%f\t%f\t", vx, vy);
    fprintf(file, "%d\t%d\t%f\t%f\t", k, step, inc, wid);
    fprintf(file, "%f\t", lightAng);
    fprintf(file, "%d\t%d\t%d\t", lightsOn, textureOn, moveOn);
    for (int i = 0; i <= step; i++) {
        for (int j = 0; j < k; j++) {
            fprintf(file, "%f\t%f\t%f\n", figure[i][j].x, figure[i][j].y, figure[i][j].z);
        }
    }
    for (int i = 0; i <= step; i++) {
        for (int j = 0; j < k; j++) {
            fprintf(file, "%f\t%f\t%f\n", bottom[i][j].x, bottom[i][j].y, bottom[i][j].z);
        }
    }
    fclose(file);
}

// загрузка с файла
void load() {
    FILE *file;
    file = fopen("lab_6_save.txt", "r");
    fscanf(file, "%f\t%f\t", &vx, &vy);
    fscanf(file, "%d\t%d\t%f\t%f\t", &k, &step, &inc, &wid);
    fscanf(file, "%f\t", &lightAng);
    fscanf(file, "%d\t%d\t%d\t", &lightsOn, &textureOn, &moveOn);
    free(figure);
    free(bottom);
    figure = new Vertex *[step + 1];
    for (int i = 0; i < step + 1; i++)
        figure[i] = new Vertex[k];
    bottom = new Vertex *[step + 1];
    for (int i = 0; i < step + 1; i++)
        bottom[i] = new Vertex[k];
    for (int i = 0; i <= step; i++) {
        for (int j = 0; j < k; j++) {
            fscanf(file, "%f\t%f\t%f\n", &figure[i][j].x, &figure[i][j].y, &figure[i][j].z);
        }
    }
    for (int i = 0; i <= step; i++) {
        for (int j = 0; j < k; j++) {
            fscanf(file, "%f\t%f\t%f\n", &bottom[i][j].x, &bottom[i][j].y, &bottom[i][j].z);
        }
    }
    fclose(file);
}

Vertex vmul(Vertex a, Vertex b, Vertex c) {
    Vertex ab;
    ab.x = b.x - a.x;
    ab.y = b.y - a.y;
    ab.z = b.z - a.z;

    Vertex ac;
    ac.x = c.x - a.x;
    ac.y = c.y - a.y;
    ac.z = c.z - a.z;

    Vertex d;
    d.x = ab.y * ac.z - ac.y * ab.z;
    d.y = -ab.x * ac.z + ac.x * ab.z;
    d.z = ab.x * ac.y - ac.x * ab.y;
    return d;
}



void projection() {
    GLfloat teta = asin(k_z / sqrt(2));
    GLfloat fi = asin(k_z / sqrt(2 - k_z * k_z));
    // GETTING THE MATRIX
    matrix[0]  = cos(fi); matrix[1]  = sin(fi)*sin(teta);  matrix[2]  = sin(fi)*cos(teta);   matrix[3]  = 0;
    matrix[4]  = 0;       matrix[5]  = cos(teta);          matrix[6]  = -sin(teta);          matrix[7]  = 0;
    matrix[8]  = sin(fi); matrix[9]  = -cos(fi)*sin(teta); matrix[10] = -cos(teta)*cos(fi);  matrix[11] = 0;
    matrix[12] = 0;       matrix[13] = 0;                  matrix[14] = 0;                   matrix[15] = 1;
}


// рисую полигональной сеткой
void triangle(Vertex **v, int i, int j, int fl) {
    // fl = 0 - для вершины, 1 для дна (круга)
    Vertex n;
    glBegin(GL_TRIANGLES);
    if (fl == 0)
        n = vmul(v[i][j], v[i + 1][j], v[i][(j + 1) % k]);
    else
        n = vmul(v[i][j], v[i][(j + 1) % k], v[i + 1][j]);
    glNormal3f(n.x, n.y, n.z);
    glTexCoord2f(v[i][j].x, v[i][j].y);
    glVertex3f(v[i][j].x, v[i][j].y, v[i][j].z);
    glTexCoord2f(v[i][(j + 1) % k].x, v[i][(j + 1) % k].y);
    glVertex3f(v[i][(j + 1) % k].x, v[i][(j + 1) % k].y, v[i][(j + 1) % k].z);
    glTexCoord2f(v[i + 1][j].x, v[i + 1][j].y);
    glVertex3f(v[i + 1][j].x, v[i + 1][j].y, v[i + 1][j].z);

    if (fl == 0)
        n = vmul(v[i + 1][j], v[i + 1][(j + 1) % k], v[i][(j + 1) % k]);
    else n = vmul(v[i + 1][j], v[i][(j + 1) % k], v[i + 1][(j + 1) % k]);
    glNormal3f(n.x, n.y, n.z);
    glTexCoord2f(v[i + 1][j].x, v[i + 1][j].y);
    glVertex3f(v[i + 1][j].x, v[i + 1][j].y, v[i + 1][j].z);
    glTexCoord2f(v[i + 1][(j + 1) % k].x, v[i + 1][(j + 1) % k].y);
    glVertex3f(v[i + 1][(j + 1) % k].x, v[i + 1][(j + 1) % k].y, v[i + 1][(j + 1) % k].z);
    glTexCoord2f(v[i][(j + 1) % k].x, v[i][(j + 1) % k].y);
    glVertex3f(v[i][(j + 1) % k].x, v[i][(j + 1) % k].y, v[i][(j + 1) % k].z);

    glEnd();
}

void drawFigure() {
    steps++;
    // Top:
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < k; j++) {
            glColor3f(0.8f, 1.0f, 1.5f);
            triangle(figure, i, j, 0);
        }
    }
    // Bottom:
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < k; j++) {
            glColor3f(0.8f, 1.0f, 1.4f);
            triangle(bottom, i, j, 1);
        }
    }
}

void redraw() {
    steps++;
    const float R = 0.4f;
    float r = R;
    float angle = static_cast<float>(M_PI * 2.0f / k);
    for (int i = 0; i < (step + 1); i++) {
        for (int j = 0; j < k; j++) {
            float cy = (1.0f / step) * i - 1.0f;
            float by = -1.0f;
            figure[i][j].y = figureScale * ((1.0f / step) * i - 1.0f);
            figure[i][j].z = figureScale * ((float) cos(j * angle) * R * cy + cy * inc);
            figure[i][j].x = figureScale * ((float) sin(j * angle) * R * wid * cy);

            bottom[i][j].y = figureScale * (-1.0f);
            bottom[i][j].z = figureScale * ((float) cos(j * angle) * r + by * inc);
            bottom[i][j].x = figureScale * ((float) sin(j * angle) * r * wid);

        }
        if (r > 0)
            r = r - R / step;
    }
}

// включение света
void enableLight() {
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {0.5, 0.1, 0.1, 100.0};
    GLfloat lightAmbient[] = {0.3, 0.3, 0.9, 1.0};
    GLfloat lightDiffuse[] = {0.3, 0.3, 0.7, 1.0};
    GLfloat lightSpecular[] = {0.1, 0.7, 1.0};
    GLfloat lightPosition[] = {0.0, 0.0, 1.0, 1.0};
    GLfloat lightDirection[] = {0.0, 0.0, -1.0};

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, fl);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lightAng);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 15.0);
}

void disableLight() {
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

// главная функция отрисовки
void draw(float dx, float dy, float dz) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(0.7f, 0.7f, 0.7f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (lightsOn)
        enableLight();
    else disableLight();
    if (textureOn)
        texture();
    else glDisable(GL_TEXTURE_2D);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glTranslatef(dx, dy, dz);
    glScaled(figureScale, figureScale, figureScale);
    // инициализация вершин для конуса
    if (flag == 0) {
        figure = new Vertex *[step + 1];
        for (int i = 0; i < step + 1; i++)
            figure[i] = new Vertex[k];
        bottom = new Vertex *[step + 1];
        for (int i = 0; i < step + 1; i++)
            bottom[i] = new Vertex[k];

        redraw();
        flag = 1;
    } else {
        drawFigure();
    }
}

void timerFunction() {
    for (int i = 0; i < step; i++) {
        for (int j = 0; j < k; j++) {
            if (bottom[i][j].y + transY + vy > 0.8 || bottom[i][j].y + transY + vy < -0.8) {
                vy = -vy;
                break;
            }
            if (bottom[i][j].x + transX + vx > 0.8 || bottom[i][j].x + transX + vx < -0.8) {
                vx = -vx;
                break;
            }
            if (bottom[i][j].z + transZ + vz > 0.8 || bottom[i][j].z + transZ + vz < -0.8) {
                vz = -vz;
                break;
            }
            if (figure[i][j].x + transX + vx > 0.8 || figure[i][j].x + transX + vx < -0.8) {
                vx = -vx;
                break;
            }
            if (figure[i][j].y + transY + vy > 0.8 || figure[i][j].y + transY + vy < -0.8) {
                vy = -vy;
                break;
            }
            if (figure[i][j].x + transX + vx > 0.8 || figure[i][j].x + transX + vx < -0.8) {
                vx = -vx;
                break;
            }
            if (figure[i][j].z + transZ + vz > 0.8 || figure[i][j].z + transZ + vz < -0.8) {
                vz = -vz;
                break;
            }
        }
    }
    transX+=vx;
    transY+=vy;
    transZ+=vz;
    draw(transX, transY, transZ);
}

int main(int argc, char const *argv[]) {
    GLFWwindow *window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Khusnutdinov_lab#6", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwMakeContextCurrent(window);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetKeyCallback(window, keyboard_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);


    double time;
    int frames = 0;
    glfwSetTime(0);
    glClearColor(0, 0, 0, 0);
    while (!glfwWindowShouldClose(window)) {
        frames++;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (moveOn) {
            timerFunction();
        } else draw(transX, transY, transZ);
        glfwSwapBuffers(window);
        time = glfwGetTime();
        if(frames > 20){
            std::cout << frames / time << "\n";
            frames = 0;
            glfwSetTime(0);
        }
        glfwPollEvents();
    }
}


void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_L)
        load();

    if (key == GLFW_KEY_K && action == GLFW_PRESS)
        save();

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        lightsOn = !lightsOn;
    }
    if (key == GLFW_KEY_N && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lightAng == 0 ? lightAng : lightAng -= 0.5);
    }
    if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lightAng == 180 ? lightAng : lightAng += 0.5);
    }

    if (key == GLFW_KEY_UP) {
        rotX += 0.5;
    }
    if (key == GLFW_KEY_DOWN) {
        rotX -= 0.5;
    }
    if (key == GLFW_KEY_RIGHT) {
        rotY -= 0.5;
    }
    if (key == GLFW_KEY_LEFT) {
        rotY += 0.5;
    }
    if (key == '-') {
        figureScale -= 0.01;
        redraw();

    }
    if (key == '=') {
        figureScale += 0.01;
        redraw();
    }
    // управление полигональной сеткой
    if (key == GLFW_KEY_A)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (key == GLFW_KEY_Q)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        textureOn = !textureOn;
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        moveOn = !moveOn;
    }

    if (key == GLFW_KEY_3){
        transY += 0.02;
    }
    if (key == GLFW_KEY_4){
        transY -= 0.02;
    }
    if (key == GLFW_KEY_6){
        transX += 0.02;
    }
    if (key == GLFW_KEY_5){
        transX -= 0.02;
    }

    if (key == GLFW_KEY_LEFT_SHIFT) {
        wid += 0.1f;
        redraw();
    }
    if (key == GLFW_KEY_RIGHT_SHIFT) {
        wid -= 0.1f;
        redraw();
    }

    if (key == GLFW_KEY_0) {
        inc -= 0.1f;
        redraw();
    }
    if (key == GLFW_KEY_1) {
        inc += 0.1f;
        redraw();
    }

    if (key == GLFW_KEY_F4) {
        flag = 0;
        if (k < 30)
            k += 1;
    }
    if (key == GLFW_KEY_F5) {
        flag = 0;
        if (k > 2)
            k -= 1;
    }
    if (key == GLFW_KEY_F6) {
        flag = 0;
        if (step > 1)
            step -= 1;
    }
    if (key == GLFW_KEY_F7) {
        flag = 0;
        if (step < 180)
            step += 1;
    }
    if (key == GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window, true);
    }
}
