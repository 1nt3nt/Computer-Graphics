// Sutherland-Hodgman algorithm, demonstrated.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

#define EPSILON 0.0001

extern int add_vertex(float x, float y, float z,
                      float *coords, GLuint *indexes, int vert_index);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods );
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void updatePosition(int index, glm::vec3 a);
void draw_green_line();

Camera cam;
bool scene_changed = true;
GLFWwindow *window;
Grid grid;
Flat_Lighter lighter;

const float vertex_size = 0.10;
int active_vertex = -1;
Mesh *vertex_shape;
Mesh *edge_crossing_shape;
Mesh *line_segment_shape;

glm::vec3 mouse_pos;

std::vector<glm::vec3> vertexPositions; // vertetices positions
std::vector<glm::vec3> greenVexPos; //used for green vertex position
std::vector<std::pair<float, float>> yellow_lines;
std::vector < std::vector< std::pair<float, float> > > bounds;

//mouse event action variables
double _x, _y;
int theSpecialOne = 0; // tell which vertex is active
bool isOnItem = false; // check if its active.
bool addNew = false; // tell if add another vertex.
bool isOutofWindow = false; // check if the red vertex is out of yellow line
bool intersection = false; // tell if there is still a vertex out of window.
bool aloneVerx = false; // check if there is only one vertex left.
int aloneOne = 0;

//check intersection if exist
// (mouse_pos.operator[](0) > 1.05 || mouse_pos.operator[](0) < -1.05 || mouse_pos.operator[](1) > 0.55
// || mouse_pos.operator[](1) < -0.55
bool isThereIntersection() {
    int count = 0;

    for (int i = 0; i < vertexPositions.size(); i++)
    {
        if (vertexPositions[i].operator[](0) > 1 || vertexPositions[i].operator[](0) < -1 ||
            vertexPositions[i].operator[](1) > 0.5 || vertexPositions[i].operator[](1) < -0.5) 
        {
            count++;
        }
    }

    if (count == vertexPositions.size() - 1)
    {
        aloneVerx = true;
    }
    else
    {
        aloneVerx = false;
    }

    if (count == 0)
    {
        intersection = false;
    }
    else if (count == vertexPositions.size())
    {
        intersection = false;
    }
    else {
        intersection = true;
    }

    return intersection;
}

int aloneVertex() {
    int index = 0;
    for (int i = 0; i < vertexPositions.size(); i++)
    {
        if (vertexPositions[i].operator[](0) < 1.05 && vertexPositions[i].operator[](0) > -1.05 &&
            vertexPositions[i].operator[](1) < 0.55 && vertexPositions[i].operator[](1) > -0.55)
        {
            index++;
            if (index == 1)
            {
                aloneOne = i;
                break;
            }
        }
    }

    return aloneOne;
}

void make_meshes(GLuint program_handle) {
    float *coords = new float[4 * 4];
    GLuint *indexes = new GLuint[4];

    int i = 0;
    i = add_vertex(-vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(-vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);

    vertex_shape = new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_TRIANGLE_FAN, program_handle,
                            "Vertex");

    i = 0;
    float cross_size = vertex_size * 0.7;
    i = add_vertex(-cross_size / 2, -cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(-cross_size / 2, +cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(+cross_size / 2, +cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(+cross_size / 2, -cross_size / 2, 0, coords, indexes, i);

    edge_crossing_shape = new Mesh(coords, 4, 4,
                                   indexes, 4,
                                   4,
                                   GL_TRIANGLE_FAN, program_handle,
                                   "Crossing");

    i = 0;
    i = add_vertex(0, 0, 0, coords, indexes, i);
    i = add_vertex(0, 1, 0, coords, indexes, i);

    line_segment_shape = new Mesh(coords, 2, 4,
                                   indexes, 2,
                                   2,
                                   GL_LINE_STRIP, program_handle,
                                   "Edge");

    vertexPositions.push_back(glm::vec3(-0.2, 0.4, 0));
    vertexPositions.push_back(glm::vec3(0.2, 0.4, 0));
    vertexPositions.push_back(glm::vec3(0.2, -0.4, 0));
    vertexPositions.push_back(glm::vec3(-0.2, -0.4, 0));

    //vector ab - bc - cd -da;
    yellow_lines.push_back(std::make_pair(-1.5, 0.5));
    yellow_lines.push_back(std::make_pair(1.5, 0.5));//top line
    bounds.push_back(yellow_lines);
    yellow_lines.clear();

    yellow_lines.push_back(std::make_pair(1, 1.5));
    yellow_lines.push_back(std::make_pair(1, -1.5)); // right line
    bounds.push_back(yellow_lines);
    yellow_lines.clear();

    yellow_lines.push_back(std::make_pair(1.5, -0.5));
    yellow_lines.push_back(std::make_pair(-1.5, -0.5));// bottom line
    bounds.push_back(yellow_lines);
    yellow_lines.clear();

    yellow_lines.push_back(std::make_pair(-1, -1.5));
    yellow_lines.push_back(std::make_pair(-1, 1.5));//left line
    bounds.push_back(yellow_lines);
    yellow_lines.clear();


    delete[] coords;
    delete[] indexes;
}

void draw_scene() {
    // Draw the current scene

    // Erase the screen and depth buffer.
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the background grid.
    grid.draw();

    lighter.set_camera(cam);

    // Draw two line segments, both yellow.
    lighter.set_color(glm::vec4(1, 1, 0, 1));

    float *coords = new float[4];

    //top
    coords[0] = -1.5;
    coords[1] = 0.5;
    coords[2] = 0;
    coords[3] = 1;

    line_segment_shape->update_vertex(coords, 0);
    coords[0] = 1.5;
    coords[1] = 0.5;
    line_segment_shape->update_vertex(coords, 1);
    line_segment_shape->draw();

    //left
    coords[0] = -1;
    coords[1] = 1.5;
    line_segment_shape->update_vertex(coords, 0);
    coords[0] = -1;
    coords[1] = -1.5;
    line_segment_shape->update_vertex(coords, 1);
    line_segment_shape->draw();

    //right
    coords[0] = 1;
    coords[1] = 1.5;
    line_segment_shape->update_vertex(coords, 0);
    coords[0] = 1;
    coords[1] = -1.5;
    line_segment_shape->update_vertex(coords, 1);
    line_segment_shape->draw();

    //bot
    coords[0] = -1.5;
    coords[1] = -0.5;
    line_segment_shape->update_vertex(coords, 0);
    coords[0] = 1.5;
    coords[1] = -0.5;
    line_segment_shape->update_vertex(coords, 1);
    line_segment_shape->draw();

    delete[] coords;

    //Draw red line.
    float* rCoords = new float[4];
    //green line
    float* gCoords = new float[4];

    //check intersection exist.


    float lastx, lasty, curx, cury;
    for (int i = 0; i < vertexPositions.size(); i++)
    {
        lighter.set_color(glm::vec4(1, 0, 0, 1));
        glm::mat4 move = glm::translate(glm::mat4(1.0f), vertexPositions[i]);
        lighter.set_model_transform(move);
        vertex_shape->draw();

        // Red line
        rCoords[0] = vertexPositions[i].operator[](0);
        lastx = rCoords[0];
        rCoords[1] = vertexPositions[i].operator[](1);
        lasty = rCoords[1];
        rCoords[2] = 0;
        rCoords[3] = 1;
        line_segment_shape->update_vertex(rCoords, 0);
        if (i + 1 == vertexPositions.size())
        {
            rCoords[0] = vertexPositions[0].operator[](0);
            curx = rCoords[0];
            rCoords[1] = vertexPositions[0].operator[](1);
            cury = rCoords[1];
            line_segment_shape->update_vertex(rCoords, 1);
        }
        else
        {
            rCoords[0] = vertexPositions[i + 1].operator[](0);
            curx = rCoords[0];
            rCoords[1] = vertexPositions[i + 1].operator[](1);
            cury = rCoords[1];
            line_segment_shape->update_vertex(rCoords, 1);
        }
        lighter.set_color(glm::vec4(1, 0, 0, 1));
        glm::mat4 move2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        lighter.set_model_transform(move2);
        line_segment_shape->draw();

        // And a smaller box where the line segments cross, green.;
        // travel all yellow line to get all vertex of one line.
        // every two sets are one yellow line.
        //   ^  ->  | <- 
        //   |  <-  v    
        for (int i = 0; i < bounds.size(); i++)
        {   
            float x = 0.0;
            float y = 0.0;
            while (true)
            {
                float a = cury - lasty;
                float b = lastx - curx;
                float c1 = a * lastx + b * lasty;

                float a2 = bounds[i][1].second - bounds[i][0].second;
                float b2 = bounds[i][0].first - bounds[i][1].first;
                float c2 = a2 * bounds[i][0].first + b2 * bounds[i][0].second;
                float d = a * b2 - a2 * b;

                if (d != 0)
                {
                    x = (b2 * c1 - b * c2) / d;
                    y = (a * c2 - a2 * c1) / d;

                    //to avoid if one of invodes is segment instand of line.
                    if ( std::min(lastx, curx) <= x && x <= std::max(lastx, curx) 
                        &&std::min(lasty,cury) <= y && y <= std::max(lasty,cury))
                    {
                        greenVexPos.push_back(glm::vec3(x, y, 0));
                        // eliminate ghosting
                        if (isOnItem)
                        {
                            greenVexPos.erase(greenVexPos.begin());
                        }
                        // clear screen when vertex is inside of box
                        //if (!isThereIntersection())
                        //{
                        //    greenVexPos.clear();
                        //}

                    }
                }
                break;
            }
        }

        //if (isThereIntersection())
        //{
        draw_green_line();
        //}
    }

    // Bring the pixels to the front.
    glfwSwapBuffers(window);
    scene_changed = false;
    delete[] rCoords;
}

void draw_green_line(){
    float* gCoords = new float[4];

    for (int i = 0; i < greenVexPos.size(); i++)
    {
        lighter.set_color(glm::vec4(0, 1, 0, 1));
        glm::mat4 move2 = glm::translate(glm::mat4(1.0f), greenVexPos[i]);
        lighter.set_model_transform(move2);
        edge_crossing_shape->draw();

        // draw green line
        gCoords[0] = greenVexPos[i].operator[](0);
        gCoords[1] = greenVexPos[i].operator[](1);
        gCoords[2] = 0;
        gCoords[3] = 1;
        line_segment_shape->update_vertex(gCoords, 0);

        if (i + 1 == greenVexPos.size())
        {
            gCoords[0] = greenVexPos[i].operator[](0);
            gCoords[1] = greenVexPos[i].operator[](1);
            line_segment_shape->update_vertex(gCoords, 1);
        }
        else
        {
            gCoords[0] = greenVexPos[i + 1].operator[](0);
            gCoords[1] = greenVexPos[i + 1].operator[](1);
            line_segment_shape->update_vertex(gCoords, 1);
        }
        lighter.set_color(glm::vec4(0, 1, 0, 1));
        move2 = glm::translate(glm::mat4(0.5f), glm::vec3(0, 0, 0));
        lighter.set_model_transform(move2);
        line_segment_shape->draw();
    }

    delete[] gCoords;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cerr << "Usage:\n";
        std::cerr << "  clipper\n";
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


    std::string title(argv[0]);
    for (int i = 1; i < argc; i++) {
        title += std::string(" ") + argv[i];
    }

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   title.c_str(),
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    mouse_pos = glm::vec3(0, 0, 0);

    grid = Grid(-1, -1, 4, 4,
                1, 0.1, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(-1.5, -1.5, 3, 3);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    make_meshes(lighter.get_program_handle());

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

bool key_pressed(unsigned int key_ID, GLFWwindow *window) {
    return glfwGetKey(window, key_ID) == GLFW_PRESS;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame, and react accordingly
// --------------------------------------------------------------
void processInput(GLFWwindow *w)
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
    mouse_pos = cam.mouse_to_world(xpos, ypos);
    if (isOnItem)
    {
        updatePosition(theSpecialOne, mouse_pos);

        if (mouse_pos.operator[](0) > 1.05 || mouse_pos.operator[](0) < -1.05 || mouse_pos.operator[](1) > 0.55
            || mouse_pos.operator[](1) < -0.55)
        {
            isOutofWindow = true;
        }
        else
        {
            isOutofWindow = false;
        }

        scene_changed = true;
    }

}

void updatePosition(int index, glm::vec3 a) {
    vertexPositions.at(index) = a;
}

void updateGreenLine() {
    glm::vec3 temp2 = vertexPositions[aloneVertex()];
    greenVexPos.push_back(temp2);

}

// glfw calls this every time the user presses/releases a mouse button
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods ) {
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS )
    {
        // Check if mouse is on a polygon vertex.
        // If so, make the vertex active.
        // If not, add a vertex to 
        glfwGetCursorPos(window, &_x, &_y);
        glm::vec3 temp1 = cam.mouse_to_world(_x, _y);
        for (int i = 0; i < vertexPositions.size(); i++)
        {
            if ( (vertexPositions[i].operator[](0)+0.05 >= temp1.operator[](0) ) && (vertexPositions[i].operator[](0) - 0.05 <= temp1.operator[](0)) &&
                (vertexPositions[i].operator[](1) + 0.05 >= temp1.operator[](1) ) && (vertexPositions[i].operator[](1) - 0.05 <= temp1.operator[](1)) )
            {
                isOnItem = true;
                theSpecialOne = i; 
                addNew = false;
                break;
            }
            else
            {
                isOnItem = false;
                addNew = true;

                if (temp1.operator[](0) > 1.05 || temp1.operator[](0) < -1.05 || temp1.operator[](1) > 0.55 ||
                    temp1.operator[](1) < -0.55)
                {
                    isOutofWindow = true;
                    //printf("out");
                }

            }

        }
    }
    else {
        // mouse released.
        isOnItem = false;
        if (addNew)
        {
            vertexPositions.push_back(cam.mouse_to_world(_x, _y));
        }
    }

    glfwSwapBuffers(window);
    scene_changed = true;
}
