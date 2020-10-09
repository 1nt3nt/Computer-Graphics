// Splat-based point renderer.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, rotate
#include <glm/gtx/string_cast.hpp> // glm::to_string()

#include "mesh.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "grid.hpp"
#include "flat_lighter.hpp"
#include "mesh.hpp"

using namespace std;

#define EPSILON 0.0001
#define PI 2 * M_PI

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button,
    int action, int mods);
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void updateVertexPosition(int i, glm::vec3 cur);

Camera cam;
bool scene_changed = true;
GLFWwindow* window;
Grid grid;
Flat_Lighter lighter;
float vertex_size = 0.1;
Mesh* vertex_shape;
std::vector<int> vec; //store file data
std::vector<Mesh*> shapeGroup;
vector<glm::vec4> colorSet;
std::vector<glm::vec3> position;

//***************************
//Variables for Edges part. *
//***************************
int vertice_loc; // get total vertices
int max_edges; // get total edges
std::vector<int> pairEdge; // record which two vertecise are a pair.
std::vector<Mesh*> edge_shape;// store edge shapes.

//***************************
//Mouse event Variables.    *
//***************************
bool vertexActive = false;
int whichVertex;
glm::vec3 curPos;

//***************************
//Intersection Variables.   *
//***************************
std::vector<float> intersection_loc;
int num_intersection = 0;
vector<Mesh*> point_shape;

// intersection struct
struct crossPoint
{
    float a1, a2, b1, b2, c1, c2, d1, d2; // coords letter1 means x, letter 2 = y
    float dx, dy;
    float m1, m2, n1, n2; //for calculate the intersection coords
    float intersectionX, intersectionY;
    float getIntersectionX();
    float getIntersectionY();
    void firstLine(float x1, float y1, float x2, float y2);
    void secondLine(float x3, float y3, float x4, float y4);
} thePoint;

float crossPoint::getIntersectionX() {
    //line 1
    dx = b1 - a1;
    dy = b2 - a2;
    m1 = dy / dx;
    n1 = a2 - m1 * a1;

    //line 2
    dx = d1 - c1;
    dy = d2 - c2;
    m2 = dy / dx;
    n2 = d2 - m2 * d1;

    //if there is no intersection
    if ((m1 - m2) == 0)
    {
        return -1;
    }
    else
    {
        intersectionX = (n2 - n1) / (m1 - m2);
        intersectionY = m1 * intersectionX + n1;
    }

    return intersectionX;
}
float crossPoint::getIntersectionY() {
    return intersectionY;
}

void crossPoint::firstLine(float x1, float y1, float x2, float y2) {
    a1 = x1;
    a2 = y1;
    b1 = x2;
    b2 = y2;
}

void crossPoint::secondLine(float x3, float y3, float x4, float y4) {
    c1 = x3;
    c2 = y3;
    d1 = x4;
    d2 = y4;
}
//------------------------ end ----------------------------

// The data for render vertex, edges and intersection
int add_vertex(float x, float y, float z,
    float* coords, GLuint* indexes, int vert_index) {
    int i = vert_index / 4;
    indexes[i] = i;
    coords[vert_index++] = x;
    coords[vert_index++] = y;
    coords[vert_index++] = z;
    coords[vert_index++] = 1;
    return vert_index;
}

int add_edges(float x, float y, float z, float* vertex, GLuint* order, int index) {
    int i = index / 4;
    order[i] = i;
    vertex[index++] = x;
    vertex[index++] = y;
    vertex[index++] = z;
    vertex[index++] = 1;
    return index;
}

int add_cross(float x, float y, float z, float* vertex, GLuint* order, int index) {
    int i = index / 4;
    order[i] = i;
    vertex[index++] = x;
    vertex[index++] = y;
    vertex[index++] = z;
    vertex[index++] = 1;
    return index;
}
//------------------------ end -----------------------------

void make_scene() {
    int max_vertices = 100;
    float* coords = new float[max_vertices * 4];
    GLuint* indexes = new GLuint[max_vertices];
    GLuint program_handle = lighter.get_program_handle();

    int i = 0;
    float s = vertex_size / 2;

    //M_PI, radius 1, center(0,0)
    //x_i = cos(M_PI/6) [30 degree]

    i = add_vertex(-s, -s, 0, coords, indexes, i);
    i = add_vertex(+s, -s, 0, coords, indexes, i);
    i = add_vertex(+s, +s, 0, coords, indexes, i);
    i = add_vertex(-s, +s, 0, coords, indexes, i);
    vertex_shape = new Mesh(coords, 4, 4,
        indexes, 4,
        4,
        GL_TRIANGLE_FAN, program_handle,
        "Vertex");


    vertice_loc = vec[0];
    max_edges = vec[1];
    printf("max edges: " + max_edges);
    pairEdge = vec;
    pairEdge.erase(pairEdge.begin(), pairEdge.begin() + 2);

    for (int j = 0; j < vertice_loc; j++)
    {
        shapeGroup.push_back(vertex_shape);
    }

    // have a recangle for intersections
    
    delete[] coords;
    delete[] indexes;
}


void draw_scene() {
    // render
    // ------
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.draw();

    float* x_n = new float[shapeGroup.size()];
    float* y_n = new float[shapeGroup.size()];
    for (int i = 0; i <= shapeGroup.size(); i++)
    {
        x_n[i] = cos(i * PI / (shapeGroup.size()));
        y_n[i] = sin(i * PI / (shapeGroup.size()));
        position.push_back(glm::vec3(x_n[i], y_n[i], 0));
    }

    for (int i = 0; i < shapeGroup.size(); i++)
    {
        int rgb[4]; //store the bit of i
        int index = 0; // rbg index;
        int temp = i;
        do
        {
            rgb[index] = temp % 2;
            index++;
            temp /= 2;
        } while (temp > 0);
        glm::mat4 move = glm::translate(glm::mat4(1.0f), position[i]);
        lighter.set_model_transform(move);
        colorSet.push_back(glm::vec4(rgb[0], rgb[1], rgb[2], rgb[3]));
        lighter.set_color(colorSet[i]);
        shapeGroup[i]->draw();
    }


    //draw edges
    float* edgeGroup = new float[400];
    GLuint* order = new GLuint[100];
    for (int i = 0; i < 2 * max_edges; i++)
    {
        int edge_num = 0;
        
        edge_num = add_edges(position[pairEdge[i]].operator[](0), position[pairEdge[i]].operator[](1), 0, edgeGroup, order, edge_num);
        i++;
        edge_num = add_edges(position[pairEdge[i]].operator[](0), position[pairEdge[i]].operator[](1), 0, edgeGroup, order, edge_num);

        edge_shape.push_back(new Mesh(edgeGroup, 2, 4,
                order, 2,
                2,
                GL_LINE_STRIP, lighter.get_program_handle(),
                "Edge"));
        if (vertexActive) {
            edge_shape.erase(edge_shape.begin());
        }

    }

    for (int i = 0; i < edge_shape.size(); i++) {
        glm::vec3 pos(0, 0, 0);
        glm::mat4 move2 = glm::translate(glm::mat4(1.0f), pos);
        lighter.set_model_transform(move2);

        edge_shape[i]->draw();
    }

    //draw intersections
    //for (int i = 0; i < 2 * max_edges; i++)
    //{
    //    thePoint.firstLine(position[pairEdge[i]].operator[](0), position[pairEdge[i]].operator[](1), position[pairEdge[i+1]].operator[](0), position[pairEdge[i+1]].operator[](1));
    //    i+=3;
    //    thePoint.secondLine(position[pairEdge[i-1]].operator[](0), position[pairEdge[i-1]].operator[](1), position[pairEdge[i]].operator[](0), position[pairEdge[i]].operator[](1));
    //    intersection_loc.push_back(thePoint.getIntersectionX());
    //    intersection_loc.push_back(thePoint.getIntersectionY());
    //    //intersection_loc.insert(pair<float, float>(thePoint.getIntersectionX(), thePoint.getIntersectionY()));
    //}

    delete[]edgeGroup;
    delete[]order;
    glfwSwapBuffers(window);
    scene_changed = false;
}

void updateVertexPosition(int i, glm::vec3 cur) {
    position[i] = cur;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage:\n";
        std::cerr << "  uncross <graph-file.txt>\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = 800;

    GLFW_OpenGL_Window glfw_window(3, 3,
        display_width,
        display_height,
        "Uncross",
        &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    grid = Grid(-1, -1, 4, 4,
        1, 0.1, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(-1.5, -1.5, 3, 3);

    glfwGetFramebufferSize(glfw_window.get_window(),
        &display_width,
        &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    ifstream myfile;
    myfile.open(argv[1]);
    string tempVariable;
    if (myfile.is_open()) {
        char line;
        while (myfile.get(line))
        {
            if (myfile.eof())
            {
                break;
            }
            
            if (isspace(line)) {
                vec.push_back(stoi(tempVariable));
                tempVariable = "";
            }
            tempVariable = tempVariable + line;
        }
    }
    myfile.close();

    make_scene();
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_motion_callback);

    // render loop
    // -----------
    scene_changed = true;
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        if (scene_changed) {

            draw_scene();

        }
        glfwWaitEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool key_pressed(unsigned int key_ID, GLFWwindow* window) {
    return glfwGetKey(window, key_ID) == GLFW_PRESS;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame, and react accordingly
// --------------------------------------------------------------
void processInput(GLFWwindow* w)
{
    if (key_pressed(GLFW_KEY_ESCAPE, w) ||
        key_pressed(GLFW_KEY_Q, w))
        glfwSetWindowShouldClose(w, true);

}

// glfw: whenever the window size changed (by OS or user resize)
// this callback function executes
// ------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions;
    // note that width and height will be significantly larger
    // than specified on retina displays.
    cam.resize_display(width, height);
    lighter.set_camera(cam);
}

// glfw calls this every time the mouse moves
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos) {
    // Save the mouse position.
    // If a vertex is active, change its position.
    // Windows position is different from world coordinates.
    // first get windows coords, then convert it
    if (vertexActive)
    {
        curPos = cam.mouse_to_world(xpos, ypos);
        //draw_scene();
        updateVertexPosition(whichVertex, curPos);
        scene_changed = true;
    }
}

// glfw calls this every time the user presses/releases a mouse button
void mouse_button_callback(GLFWwindow* window, int button,
    int action, int mods) {
    glm::vec3 tmd(0, 0, 0);
    double last_x, last_y;
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // Left button was pressed:
        // 1. convert mouse coords to world.
        // 2. check each vertex, to see if the mouse
        //    is inside its square.
        // 3. if so, make the vertex active.
        glfwGetCursorPos(window, &last_x, &last_y);
        tmd = cam.mouse_to_world(last_x, last_y);
        //check if mouse is inside its square

        for (int i = 0; i < shapeGroup.size(); i++)
        {
            
            if ( (position[i].operator[](0) + 0.05) >= tmd.operator[](0) && tmd.operator[](0) >= (position[i].operator[](0) - 0.05) &&
                (position[i].operator[](1)+0.05) >= tmd.operator[](1) && tmd.operator[](1) >= (position[i].operator[](1) -0.05) )
            {
                vertexActive = true;
                whichVertex = i;
                break;
            }
            else
            {
                vertexActive = false;
            }
        }
    }
    else 
    {
        glfwSwapBuffers(window);
        vertexActive = false;
    }
}
