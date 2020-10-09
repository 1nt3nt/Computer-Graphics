// Draw an articulated figure.

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

#include "mesh3d.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "diffuse_lighter.hpp"
#include "shapes.hpp"
#include "matrix_stack.hpp"
#include "kbui.hpp"

#define EPSILON 0.0001f

extern void print_mat4(const glm::mat4& m);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods );
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

void camera_changed(float);
void angles_changed(float);
void reset_camera(float dummy);
void reset_angles(float dummy);

///////////////////////////////////////////
// These are the variables that the user controls.
//
// ***** NOTE!!!! ***********
//
// When the angles are all zero, the robot
// should be standing upright, with arms straight
// down by its sides, and legs straight,
// looking straight ahead.
//
// Since the robot is facing the viewer, its right side
// appears on the left side the window.  Similarly for left/right.
//
//////////////////////////////////////////

float rightShoulderAngle=0;   // angle under the right armpit
float leftShoulderAngle=0;    // angle under the left armpit
float rightElbowAngle=0;      // angle at the right elbow
float leftElbowAngle=0;       // angle at the left elbow
float headTurnAngle=0;        // left-right head angle
float headTiltAngle=0;        // forward-back head angle
float elevationAngle=0;       // body tilt angle above base (latitude)
float azimuthAngle=0;         // body rotation angle around base (longitude)
float rollAngle=0;            // body rotation around the cylinder

// These are the camera parameters.
float eye_X=2, eye_Y=4, eye_Z=10;
float ref_X=0, ref_Y=4, ref_Z=0;
float vup_X=0, vup_Y=1, vup_Z=0;

float leftX  = -1;
float rightX = +1;
float bottomY= -1;
float topY   = +1;
float nearZ  = 1.5;
float farZ   = 20;

KBUI the_ui;

Camera cam;
bool scene_changed = true;
Shapes shapes;
GLFWwindow *window;
Diffuse_Lighter lighter;

float world_BL_x = -6;
float world_BL_y = -2;
float world_width = 15;
float world_height = 15;

std::vector<Mesh3d*> box;
std::vector<Mesh3d*> ball;
std::vector<Mesh3d*> cyl;

float reset_angles_dummy;
float reset_camera_dummy;


void init_UI() {
    the_ui.add_variable("Head Left/Right",&headTurnAngle, -90,  90,  5,
                        angles_changed);
    the_ui.add_variable("Head Up/Down",   &headTiltAngle, -45,  45,  5,
                        angles_changed);

    the_ui.add_variable("Left Shoulder", &leftShoulderAngle,   0, 135, 5,
                        angles_changed);
    the_ui.add_variable("Left Elbow",    &leftElbowAngle,      0, 135, 5,
                        angles_changed);

    the_ui.add_variable("Right Shoulder", &rightShoulderAngle, 0, 135, 5,
                        angles_changed);
    the_ui.add_variable("Right Elbow",    &rightElbowAngle,    0, 135, 5,
                        angles_changed);

    the_ui.add_variable("Body Azimuth",   &azimuthAngle,   -180, 180, 10,
                        angles_changed);
    the_ui.add_variable("Body Elevation", &elevationAngle,  -90,  90, 10,
                        angles_changed);
    the_ui.add_variable("Body Roll",      &rollAngle,      -180, 180, 10,
                        angles_changed);

    // Unlike all the above variables, these ones will trigger
    // a call-back when they are changed.
    the_ui.add_variable("Eye X", &eye_X, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Eye Y", &eye_Y, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Eye Z", &eye_Z, -10, 10, 0.2f, camera_changed);

    the_ui.add_variable("Ref X", &ref_X, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Ref Y", &ref_Y, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Ref Z", &ref_Z, -10, 10, 0.2f, camera_changed);

    the_ui.add_variable("Vup X", &vup_X, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Vup Y", &vup_Y, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Vup Z", &vup_Z, -10, 10, 0.2f, camera_changed);

    the_ui.add_variable("Clip L", &leftX,   -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Clip R", &rightX,  -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Clip B", &bottomY, -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Clip T", &topY,    -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Clip N", &nearZ,   -10, 10, 0.2f, camera_changed);
    the_ui.add_variable("Clip F", &farZ,    -10, 10, 0.2f, camera_changed);

    the_ui.add_variable("Reset Angles", &reset_angles_dummy,0,100,
                        0.001f, reset_angles);
    the_ui.add_variable("Reset Camera", &reset_camera_dummy,0,100,
                        0.001f, reset_camera);

    the_ui.done_init();

}

void make_meshes() {
    box = shapes.cube();

    ball = shapes.sphere(12, 16);

    cyl = shapes.cylinder(16);
}

void draw_shape(const glm::mat4x4& M,
                const std::vector<Mesh3d*> shape,
                float r, float g, float b) {
    lighter.set_model_transform(M);
    lighter.set_color(glm::vec4(r,g,b,1));
    lighter.use();
    for (auto mesh: shape)
        mesh->draw();
}

void draw_scene() {
    // Draw all the boxes that make up an articulated figure.
    // ------
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lighter.use();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    lighter.set_camera(cam);

    Matrix_Stack stack;
    stack.translate(0, 3, 0);

    //ball
    stack.push();
    {
        stack.translate(0, -2.6f, 0);
        stack.scale(1, 1, 0.7f);
        stack.rotate(azimuthAngle, 0, 1, 0);
        stack.rotate(elevationAngle, 1, 0, 0);
        draw_shape(stack.top(), ball, 1, 0.4f, 1);


        // body
        stack.push();
        {
            stack.translate(0, 4.4f, 0);
            stack.scale(1, 2, 0.5f);
            stack.rotate(rollAngle, 0, 1, 0);
            draw_shape(stack.top(), box, 1, 0.6f, 0.3f);

            // head
            stack.push();
            {
                stack.translate(0, 1.5f, 0.25f);
                stack.rotate(headTurnAngle, 0, 1, 0);
                stack.rotate(headTiltAngle, 1, 0, 0);
                stack.push();
                {
                    stack.scale(0.75f, 0.5f, 2);
                    draw_shape(stack.top(), cyl, 1, 0.6f, 0.3f);

                    // right eye
                    stack.push();
                    {
                        stack.translate(0.5f, 0.25f, 1.2f);
                        stack.scale(0.2f, 0.2f, 0);
                        draw_shape(stack.top(), ball, 1, 0, 0);
                    }
                    stack.pop();

                    // left eye
                    stack.push();
                    {
                        stack.translate(-0.4f, 0.25f, 1.2f);
                        stack.scale(0.2f, 0.2f, 0);
                        draw_shape(stack.top(), ball, 0, 1, 0);
                    }
                    stack.pop();
                }
                stack.pop();
            }
            stack.pop();

            //right shoulder and elbow
            stack.push();
            {
                stack.translate(-1.5f, 0.9f, 0);

                stack.push();
                {
                    stack.scale(0.5f, 0.25f, 1);
                    draw_shape(stack.top(), cyl, 0, 1, 0);
                }
                stack.pop();

                stack.push();
                {
                    stack.translate(0, 0.75f, 0);
                    stack.rotate(rightShoulderAngle, 0, 0, -1);

                    stack.translate(0, -1.5f, 0);
                    stack.push();
                    {
                        stack.scale(0.5f, 0.5f, 1);
                        draw_shape(stack.top(), box, 1, 0.6f, 0.3f);
                    }
                    stack.pop();

                    stack.push();
                    {
                        stack.translate(0, -0.75f, 0);

                        stack.push();
                        {
                            stack.scale(0.5f, 0.25f, 1);
                            draw_shape(stack.top(), cyl, 0, 1, 0);
                        }
                        stack.pop();
                    }
                    stack.pop();

                    stack.push();
                    {
                        stack.translate(0, 0, 0);
                        stack.rotate(rightElbowAngle, 0, 0, -1);

                        stack.translate(0, -1.5f, 0);
                        stack.push();
                        {
                            stack.scale(0.5f, 0.5f, 1);
                            draw_shape(stack.top(), box, 1, 0.6f, 0.3f);
                        }
                        stack.pop();
                    }
                    stack.pop();
                }
                stack.pop();
            }
            stack.pop();

            // left shoulder, elbow and arm bones
            stack.push();
            {
                stack.translate(1.5f, 0.9f, 0);

                stack.push();
                {
                    stack.scale(0.5f, 0.25f, 1);
                    draw_shape(stack.top(), cyl, 0, 1, 0);
                }
                stack.pop();

                stack.push();
                {
                    stack.translate(0, 0.75f, 0);
                    stack.rotate(leftShoulderAngle, 0, 0, 1);

                    stack.translate(0, -1.5f, 0);
                    stack.push();
                    {
                        stack.scale(0.5f, 0.5f, 1);
                        draw_shape(stack.top(), box, 1, 0.6f, 0.3f);
                    }
                    stack.pop();

                    stack.push();
                    {
                        stack.translate(0, -0.75f, 0);

                        stack.push();
                        {
                            stack.scale(0.5f, 0.25f, 1);
                            draw_shape(stack.top(), cyl, 0, 1, 0);
                        }
                        stack.pop();
                    }
                    stack.pop();

                    stack.push();
                    {
                        stack.translate(0, 0, 0);
                        stack.rotate(leftElbowAngle, 0, 0, 1);

                        stack.translate(0, -1.5f, 0);
                        stack.push();
                        {
                            stack.scale(0.5f, 0.5f, 1);
                            draw_shape(stack.top(), box, 1, 0.6f, 0.3f);
                        }
                        stack.pop();
                    }
                    stack.pop();
                }
                stack.pop();
            }
            stack.pop();

            //roller
            stack.push();
            {
                stack.translate(0.02f, -1.38f, 1.5f);
                stack.rotate(90, 1, 0, 0);

                stack.translate(0, -1.5f, 0);
                stack.push();
                {
                    stack.scale(1, 1, 0.35f);
                    draw_shape(stack.top(), cyl, 0, 1, 0);
                }
                stack.pop();
            }
            stack.pop();
        }
        stack.pop();
    }
    stack.pop();

    //base
    stack.push();
    {
        stack.translate(0, -4, 0);
        stack.scale(1, 0.5f, 0.7f);
        draw_shape(stack.top(), box, 1, 0.6f, 0.3f);
    }
    stack.pop();


    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cerr << "Usage:\n";
        std::cerr << "  rasterizer\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }

    init_UI();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = (int)(display_width / world_width * world_height);

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "Robot 2D",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    cam = Camera(display_width, display_height);
    cam.init_2D(world_BL_x, world_BL_y, world_width, world_height);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);
    reset_camera(0);

    lighter.set_camera(cam);

    make_meshes();

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
    return glfwGetKey(window, (int)key_ID) == GLFW_PRESS;
}

bool shift_down(GLFWwindow *window) {
    // Is the SHIFT key pressed right now?
    return key_pressed(GLFW_KEY_LEFT_SHIFT, window) ||
           key_pressed(GLFW_KEY_RIGHT_SHIFT, window);
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame, and react accordingly
// --------------------------------------------------------------
void processInput(GLFWwindow *w)
{
    if (key_pressed(GLFW_KEY_ESCAPE, w) ||
        key_pressed(GLFW_KEY_Q, w))
        glfwSetWindowShouldClose(w, true);

    else if (key_pressed(GLFW_KEY_UP, w)) {
        the_ui.handle_key(GLFW_KEY_UP);
    }
    else if (key_pressed(GLFW_KEY_DOWN, w)) {
        the_ui.handle_key(GLFW_KEY_DOWN);
    }
    else if (key_pressed(GLFW_KEY_LEFT, w)) {
        the_ui.handle_key(GLFW_KEY_LEFT);
    }
    else if (key_pressed(GLFW_KEY_RIGHT, w)) {
        the_ui.handle_key(GLFW_KEY_RIGHT);
    }
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

void angles_changed(float dummy) {
    scene_changed = true;
}

void camera_changed(float dummy) {
    cam.update_params(eye_X, eye_Y, eye_Z,
                      ref_X, ref_Y, ref_Z,
                      vup_X, vup_Y, vup_Z,
                      leftX, rightX,
                      bottomY, topY,
                      nearZ, farZ);
    scene_changed = true;
}

void reset_angles(float dummy) {
    rightShoulderAngle = 0;
    leftShoulderAngle = 0;
    rightElbowAngle = 0;
    leftElbowAngle = 0;
    headTurnAngle = 0;
    headTiltAngle = 0;
    azimuthAngle = 0;
    elevationAngle = 0;
    rollAngle = 0;

    scene_changed = true;
}

void reset_camera(float dummy) {
    eye_X=2;
    eye_Y=4;
    eye_Z=10;

    ref_X=0;
    ref_Y=4;
    ref_Z=0;

    vup_X=0;
    vup_Y=1;
    vup_Z=0;

    leftX  = -1;
    rightX = +1;
    bottomY= -1;
    topY   = +1;
    nearZ  = 1.5;
    farZ   = 20;

    camera_changed(0);
    scene_changed = true;
}

