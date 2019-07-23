#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


using namespace std;

struct Vertex
{
    double x, y;
} *vertexs;

int my_width = 600, my_height = 600;
int vertexscount = 0, maxcount = 50;
bool my_flag = true;
bool triangle_flag = false;

class Node {  // вершина многоугольника
public:
    double x, y;
    Node *prev, *next;
    Node (double a = 0.0, double b = 0.0): x(a), y(b), prev(0), next(0) {}
};

//направление, в котором заданы координаты(по часовой стрелке, против часовой стрелки)
typedef enum { clockwise, count_clockwise, error } type;

class List {
public:
    int size; //количество вершин
    Node* cur; //указатель на текущую вершину
    List (int s = 0, Node* tmp = 0): size(s), cur(tmp) {}
    bool is_empty () {
        return ( size == 0 ? true : false );
    }
    void insert (double a, double b);
    void insert (Vertex *a, int n);
    Node* find (double a, double b);
    void delete_node (double a, double b);
    double cross_prod (Node *first, Node *second, Node *third);
    type direction ();
    bool is_in_triangle (Node *first, Node *second, Node *third);
    vector <List> triangulation (); //триангуляция
};



void List :: insert (double a, double b) {
    Node *tmp = new Node;
    tmp->x = a; tmp->y = b;
    if ( is_empty() ) {
        tmp->next = tmp;
        tmp->prev = tmp;
    }
    else {
        tmp->next = cur->next;
        cur->next = tmp;
        tmp->prev = cur;
        tmp->next->prev = tmp;
    }
    cur = tmp; //меняем указатель на текущую вершину(вставленная вершина)
    size++;
}

void List :: insert (Vertex *a, int n) {
    Node *tail = 0;
    /*добавлять массив из координат можно только в пустой список, иначе
     он будет замкнут неправильно*/
    if ( is_empty() ) {
        for ( int i = 0; i < n; i++ ) {
            insert (vertexs[i].x, vertexs[i].y);
            //сохраняем указатель на начало списка, чтобы потом его замкнуть
            if ( i == 0 )
                tail = cur;
        }
        //замыкаем список
        cur->next = tail;
        cur->next->prev = cur;
        cur = cur->next;
    }
}

Node* List :: find (double a, double b) {
    Node *tmp = cur;
    if ( !is_empty() ) {
        do {
            if ( a == tmp->x && b == tmp->y )
                return tmp;
            else
                tmp = tmp->next;
        } while ( tmp != cur );
    }
    return 0;
}
void List :: delete_node (double a, double b) {
    Node *tmp = find (a, b);
    if (tmp) { //вершина с такими координатами найдена
        //если удаляется текущая вершина, смещаем указатель на предыдущую
        if ( tmp == cur )
            cur = cur->prev;
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        delete tmp;
        size--;
    }
}

//векторное произведение векторов, заданных парами first, second и first, third
double List :: cross_prod (Node *first, Node *second, Node *third) {
    double x1 = second->x - first->x, x2 = third->x - first->x,
    y1 = second->y - first->y, y2 = third->y - first->y;
    return (x1 * y2 - x2 * y1);
}

type List :: direction () {
    Node *tmp = cur, *min = cur;
    double min_x = tmp->x, min_y = tmp->y;
    /*поиск самой левой точки(с минимальным значением абсциссы)
     если таких несколько, выберем нижнюю из них(с минимальным значением ординаты)*/
    if ( !is_empty() ) {
        do {
            cout << "\nX:" << min_x << " Y:" << min_y << endl;
            if ( tmp->x < min_x ) {
                min_x = tmp->x;
                min = tmp;
            }
            if ( tmp->x == min_x && tmp->y > min_y ) {
                min_y = tmp->y;
                min = tmp;
            }
            tmp = tmp->next;
        } while ( tmp != cur );
        /*направление обхода зависит от знака векторного произведения между векторами,
         задаваемыми "минимальной" вершиной и двумя соседними с ней*/
        return ( ( cross_prod ( min, min->next, min->prev ) < 0) ? clockwise : count_clockwise );
    }
    return error;
}

bool List :: is_in_triangle ( Node *first, Node *second, Node *third ) {
    double a, b, c;
    Node *tmp = cur;
    do {
        if ( tmp != first && tmp != second && tmp != third ) {
            a = cross_prod(tmp, first, second);
            b = cross_prod(tmp, second, third);
            c = cross_prod(tmp, third, first);
            //какая-либо из точек многоугольника попадает внутрь треугольника
            if ( ( a > 0 && b > 0 && c > 0 ) || ( a < 0 && b < 0 && c < 0 ) )
                return true;
        }
        tmp = tmp->next;
    } while ( tmp != cur );
    return false;
}
vector <List> List :: triangulation () {
    //массив, c координатами треугольников, образующими триангуляцию
    vector <List> triangles, empty(0, 0);
    List *l; //список координат треугольника
    //берем три последовательно расположенные вершины
    Node *first = cur, *second = cur->next, *third = cur->next->next;
    //определяем направление обхода(по часовой стрелке/против часовой стрелки)
    type cond = direction();
    double prod;
    if ( cond != error ) {
        while (size > 3) {
            prod = cross_prod(first, second, third);
            /*вторая вершина должна лежать левее прямой, соединяющей first и third, если вершины
             заданы по часовой стрелке и правее прямой, если против часовой стрелки*/
            if ( ( cond == clockwise && prod < 0 ) || ( cond == count_clockwise && prod > 0 ) ) {
                //внутри треугольника нет других вершин многоугольника
                if ( !is_in_triangle ( first, second, third ) ) {
                    l = new List;
                    l->insert(first->x, first->y);
                    l->insert(second->x, second->y);
                    l->insert(third->x, third->y);
                    triangles.push_back(*l);
                    //исключаем вершину second из рассмотрения
                    delete_node(second->x, second->y);
                    delete l;
                }
                second = third;
                third = third->next;
            }
            else {
                first = second;
                second = third;
                third = third->next;
            }
        }
        if (size == 3) { //добавляем последний треугольник
            l = new List;
            l->insert(first->x, first->y);
            l->insert(second->x, second->y);
            l->insert(third->x, third->y);
            triangles.push_back(*l);
            delete l;
        }
        return triangles;
    }
    return empty;
}


void resize_vertexs()
{
    struct Vertex* v = new Vertex[maxcount * 2];
    for (int i = 0; i < maxcount; i++) v[i] = vertexs[i];
    maxcount *= 2;
    delete(vertexs);
    vertexs = v;
}

void frame_buff_size_callback(GLFWwindow* window, int newwidth, int newheight) {
    my_width = newwidth;
    my_height = newheight;
    vertexscount = 0;
    glViewport(0, 0, my_width, my_height);
}

void draw_polygon() {
    glBegin(GL_LINE_STRIP);
    if (my_flag)
        glColor3f(0, 0, 1);
    else
        glColor3f(1, 0, 0);
    for (int i = 0; i < vertexscount; i++)
        glVertex2d(vertexs[i].x, vertexs[i].y);
    glVertex2d(vertexs[0].x, vertexs[0].y);
    glEnd();
}

void draw_triangle (double x1, double y1, double x2, double y2, double x3, double y3) {

    glBegin(GL_LINE_LOOP);
    //координаты вершин
    glVertex2d (x1, y1);
    glVertex2d (x2, y2);
    glVertex2d (x3, y3);
    glEnd();
}

void fill_triangle (double x1, double y1, double x2, double y2, double x3, double y3) {
    glColor3ub (rand () % 254, rand() % 254, rand() % 254);
    glBegin(GL_TRIANGLES);
    glVertex2d (x1, y1);
    glVertex2d (x2, y2);
    glVertex2d (x3, y3);
    glEnd();
}

void triangulate () {
    draw_polygon();
    List *polygon;
    polygon = new List;
    polygon->insert (vertexs, vertexscount); //инициализация многоугольника
    double x1, y1, x2, y2, x3, y3;
    srand (time(0));
    for (auto now : polygon->triangulation()) {
        x1 = now.cur->x;
        y1 = now.cur->y;
        x2 = now.cur->next->x;
        y2 = now.cur->next->y;
        x3 = now.cur->prev->x;
        y3 = now.cur->prev->y;
        // делаем их разноцветными
        fill_triangle(x1, y1, x2, y2, x3, y3);
        //прорисовываем границы
        draw_triangle(x1, y1, x2, y2, x3, y3);
    }
    delete polygon;
}

// знак
bool sign(double x){
    return (x < 0);
}

// проверка на выпуклость многоугольника
bool check_convex(){
        
        int i, j, k;
        int flag = 0;
        double prod;
        
        if (vertexscount < 3)
            return(true);
        for (i = 0; i < vertexscount; i++) {
            j = (i + 1) % vertexscount;
            k = (i + 2) % vertexscount;
            // проверяется по разности произведений векторов
            prod  = (vertexs[j].x - vertexs[i].x) * (vertexs[k].y - vertexs[j].y);
            prod -= (vertexs[j].y - vertexs[i].y) * (vertexs[k].x - vertexs[j].x);
            if (prod < 0)
                flag |= 1;
            else if (prod > 0)
                flag |= 2;
            if (flag == 3)
                return(false);
        }
    return(true);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // приведение координат
        double y = 2 * (my_height - ypos) / my_height - 1, x = 2 * (xpos) / my_width - 1;
        vertexs[vertexscount] = { x, y };
        cout << x << " " << y<< endl;
        vertexscount++;
        if (vertexscount == maxcount) resize_vertexs();
        my_flag = check_convex();
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
        frame_buff_size_callback(NULL, my_width, my_height);
    if (key == GLFW_KEY_Z && action == GLFW_PRESS){
        if (vertexscount > 0){
            vertexscount--;
            my_flag = check_convex();
        }
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS){
        if (!my_flag){
            triangle_flag = !triangle_flag;
        }
    }
}

int main(void)
{
    GLFWwindow* window;
    if (!glfwInit()) exit(EXIT_FAILURE);
    vertexs = new Vertex[100];
    window = glfwCreateWindow(my_width - 1, my_height, "Khusnutdinov_lab#5", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, frame_buff_size_callback);
    glfwSetCursorPos(window, my_width, my_height);
    glClearColor(0, 0, 0, 1.0);
    glViewport(0, 0, my_width, my_height);
    while (!glfwWindowShouldClose(window))
    {
        //glLineWidth (2);
        glClear(GL_COLOR_BUFFER_BIT);
        if(!triangle_flag)
            draw_polygon();
        else
            triangulate();
        glfwSwapBuffers(window);
        //glFlush();
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
