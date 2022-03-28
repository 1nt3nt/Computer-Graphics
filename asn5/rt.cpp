/////////////////////////////////////////////////////
//
// A simple ray tracer.
//
//////////////////////////////////////////////////////

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>

#include "camera.hpp"
#include "kbui.hpp"

#include "object.hpp"
#include "triangle.hpp"
#include "sphere.hpp"
#include "hit.hpp"
#include "hitlist.hpp"

using namespace std;

KBUI the_ui;
Camera cam; // the opengl camera.
GLFWwindow* window;

/////////////////////////////////////////////////////
// DECLARATIONS
////////////////////////////////////////////////////

// Forward declarations for functions in this file
void init_UI();
void setup_camera();
void check_for_resize();
void setRay(int xDCS, int yDCS, glm::vec3& start, glm::vec3& direction);
bool first_hit(const glm::vec3& start, const glm::vec3& direction, Hit& hit);
glm::vec3 rayColor(int xDCS, int yDCS);
void getMvcswcs();
void render();
void camera_changed();
void cam_param_changed(float);
void reset_camera(float);
void init_scene();

void key_callback(GLFWwindow* window, int key,
    int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button,
    int action, int mods);
void error_callback(int error, const char* description);

void display();
int main(int argc, char* argv[]);

// USEFUL Flag:
// When the user clicks on a pixel, the mouse_button_callback does two things:
//   1. sets this flag.
//   2. calls ray_color() on that pixel
// This lets you check all your intersection code, for ONE ray of your choosing.
bool debugOn = false;

// FRAME BUFFER Declarations.
// The initial image is 300 x 300 x 3 bytes (3 bytes per pixel)
int winWidth = 300;
int winHeight = 300;
GLubyte* img = NULL;   // image is allocated by check_for_resize(), not here.

// These are the camera parameters.
// The camera position and orientation:
glm::vec3 eye;
glm::vec3 lookat;
glm::vec3 vup;

// The camera's HOME parameters, used in reset_camera()
glm::vec3 eye_home(1.4, 1.6, 4.6);
glm::vec3 lookat_home(0, 0, 0);
glm::vec3 vup_home(0, 1, 0);

// The clipping frustum
float clipL = -1;
float clipR = +1;
float clipB = -1;
float clipT = +1;
float clipN = 2;

vector<Object*> sceneObjects; // list of objects in the scene

glm::mat4 Mvcswcs;  // the inverse of the view matrix.
vector<Hit> hits;     // list of hit records for current ray
Hit* hitPool;     // array of available hit records.

glm::vec3 start(0, 0, 0);
glm::vec3 direction(0, 0, 0);
Hit hit;

// Used to trigger render() when camera has changed.
bool frame_buffer_stale = true;

// Rays which miss all objects have this color.
glm::vec3 background_color(0.3, 0.4, 0.4); // dark blue

//*********************************************************************
//
// IMPLEMENT THE FUNCTIONS THAT ARE BELOW THIS LINE
//

//////////////////////////////////////////////////////////////////////
// Compute Mvcswcs.
// YOU MUST IMPLEMENT THIS FUNCTION.
//////////////////////////////////////////////////////////////////////
void getMvcswcs() {
    Mvcswcs = glm::inverse(glm::lookAt(eye, lookat, vup));
    //glm::vec3 x, y, z;
    //z = glm::normalize((eye - lookat));
    //x = glm::normalize(glm::cross(vup, z));
    //y = glm::normalize(glm::cross(z, x));
    //Mvcswcs = glm::mat4(x.operator[](0), x.operator[](1), z.operator[](2), 0,
    //    y.operator[](0), y.operator[](1), y.operator[](2), 0,
    //    z.operator[](0), z.operator[](1), z.operator[](2), 0,
    //    eye.operator[](0), eye.operator[](1), eye.operator[](2), 1);
}

void setup_camera() {

    // Leave this line in place.  It will trigger a re-allocation
    // of the image's pixels, when the user re-sizes the window.
    check_for_resize();

    getMvcswcs();


}

/////////////////////////////////////////////////////////
// Get color of a ray passing through (x,y)DCS.
// YOU MUST IMPLEMENT THIS FUNCTION.
/////////////////////////////////////////////////////////

glm::vec3 rayColor(int xDCS, int yDCS)
{
    glm::vec3 start;
    glm::vec3 direction;
    setRay(xDCS, yDCS, start, direction);
    Hit tempHit;
    bool fh = first_hit(start, direction, tempHit);
    if (fh)
    {
        return tempHit.getObject()->getColor();
    }

    return glm::vec3(0.3, 0.4, 0.4);
}


/////////////////////////////////////////////////////////
// Initialize a ray starting at (x y)DCS.
// YOU MUST IMPLEMENT THIS FUNCTION.
/////////////////////////////////////////////////////////

void setRay(int xDCS, int yDCS, glm::vec3& start, glm::vec3& direction) {
    // convert DCS to VCS
    double xVCS, yVCS, zVCS;
    double dX, dY;
    dX = (clipR - clipL) / winWidth;
    dY = (clipT - clipB) / winHeight;
    xVCS = clipL + (xDCS + 0.5) * dX;
    yVCS = clipB + (yDCS + 0.5) * dY;
    zVCS = -clipN;
    glm::vec4 pVCS(xVCS, yVCS, zVCS, 1);
    glm::vec4 pWCS = Mvcswcs * pVCS;
    start = pWCS;
    direction = glm::normalize(start - eye);
}

/////////////////////////////////////////////////////////
// Find the first object hit by the ray, if any
// YOU MUST IMPLEMENT THIS FUNCTION.
/////////////////////////////////////////////////////////

bool first_hit(const glm::vec3& start, const glm::vec3& direction, Hit& hit) {
    for (int i = 0; i < sceneObjects.size(); i++) 
    {
        if (sceneObjects[i]->intersects(start, direction, hit)) {
            hits.push_back(hit);
        }
    }

    if(!hits.empty())
    {
        hit = hits[0];
        //cout << hitList.size() << endl;
        for (int i = 1; i < hits.size(); i++)
        {
            if (hit.getDistance() > hits[i].getDistance())
            {
                hit = hits[i];
            }
        }
        hits.clear();
        return true;
    }

    return false;
}

//
// IMPLEMENT THE FUNCTIONS THAT ARE ABOVE THIS LINE
//
//*********************************************************************

//////////////////////////////////////////////////////////////////////
// If window size has changed, re-allocate the frame buffer
//////////////////////////////////////////////////////////////////////

void check_for_resize() {
    // Now, check if the frame buffer needs to be created,
    // or re-created.

    GLint width_now, height_now;
    glfwGetFramebufferSize(window, &width_now, &height_now);

    bool should_allocate = false;
    if (img == NULL) {
        // frame buffer not yet allocated.
        should_allocate = true;
    }
    else if (winWidth != width_now ||
        winHeight != height_now) {

        // frame buffer allocated, but has changed size.
        delete[] img;
        should_allocate = true;
        winWidth = width_now;
        winHeight = height_now;
    }

    if (should_allocate) {

        if (debugOn) {
            cout << "ALLOCATING: (W H)=(" << winWidth
                << " " << winHeight << ")\n";
        }

        img = new GLubyte[winWidth * winHeight * 3];
        camera_changed();
    }
}

//////////////////////////////////////////////////////
//
// Displays, on STDOUT, the colour of the pixel that
//  the user clicked on.
//
// THIS IS VERY USEFUL!  USE IT!
//
// You don't have to change this function.
//
//////////////////////////////////////////////////////

void mouse_button_callback(GLFWwindow* window, int button,
    int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        debugOn = true;

        // Get the mouse's position.

        double xpos, ypos;
        int W, H;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &W, &H);

        // mouse position, as a fraction of the window dimensions
        // The y mouse coord increases as you move down,
        // but our yDCS increases as you move up.
        double mouse_fx = xpos / float(W);
        double mouse_fy = (W - 1 - ypos) / float(W);

        int xDCS = (int)(mouse_fx * winWidth + 0.5);
        int yDCS = (int)(mouse_fy * winHeight + 0.5);

        glm::vec3 pixelColor = rayColor(xDCS, yDCS);

        if (debugOn) {
            cout << "cursorpos:" << xpos << " " << ypos << "\n";
            cout << "Width Height: " << winWidth << " " << winHeight << "\n";
            cout << "Window Size: " << W << " " << H << "\n";
            cout << "Pixel at (x y)=(" << xDCS << " " << yDCS << ")\n";
            cout << "Pixel Color = " << to_string(pixelColor) << endl;
        }
        debugOn = false;
    }
}

/////////////////////////////////////////////////////////
// This function actually generates the ray-traced image.
// You don't have to change this function.
/////////////////////////////////////////////////////////

void render() {
    int x, y;
    GLubyte r, g, b;
    int p;

    setup_camera();

    for (y = 0; y < winHeight; y++) {
        for (x = 0; x < winWidth; x++) {

            if (debugOn) {
                cout << "pixel (" << x << " " << y << ")\n";
                cout.flush();
            }

            glm::vec3 pixelColor = rayColor(x, y);

            r = (GLubyte)(pixelColor.r * 255.0);
            g = (GLubyte)(pixelColor.g * 255.0);
            b = (GLubyte)(pixelColor.b * 255.0);

            p = (y * winWidth + x) * 3;

            img[p] = r;
            img[p + 1] = g;
            img[p + 2] = b;
        }
    }
}

/////////////////////////////////////////////////////////
// Call this when scene has changed, and we need to re-run
// the ray tracer.
// You don't have to change this function.
/////////////////////////////////////////////////////////

void camera_changed() {
    float dummy = 0;
    cam_param_changed(dummy);
}

/////////////////////////////////////////////////////////
// Called when user modifies a camera parameter.
// You don't have to change this function.
/////////////////////////////////////////////////////////
void cam_param_changed(float param) {
    frame_buffer_stale = true;
}

//////////////////////////////////////////////////////
// This function sets up a simple scene.
// You don't have to change this function.
/////////////////////////////////////////////////////
void init_scene() {
    sceneObjects.push_back(new Sphere(glm::vec3(0, 1, 0), 0.5,
        glm::vec3(1, 0.5, 0.5)));
    sceneObjects.push_back(new Sphere(glm::vec3(0, 0, 0), 0.75,
        glm::vec3(0.5, 1, 0.5)));
    sceneObjects.push_back(new Sphere(glm::vec3(0, -1.5, 0), 1,
        glm::vec3(0.5, 0.5, 1)));

    sceneObjects.push_back(new Triangle(glm::vec3(4, -2.5, -4),
        glm::vec3(-4, -2.5, 4),
        glm::vec3(4, -2.5, 4),
        glm::vec3(1, 0.75, 0.5)));
    sceneObjects.push_back(new Triangle(glm::vec3(-4, -2.5, -4),
        glm::vec3(-4, -2.5, 4),
        glm::vec3(4, -2.5, -4),
        glm::vec3(1, 0.75, 0.5)));

    hitPool = new Hit[20];
}

///////////////////////////////////////////////////
// Resets the camera parameters.
// You don't have to change this function.
///////////////////////////////////////////////////

void reset_camera(float dummy) {
    eye = eye_home;
    lookat = lookat_home;
    vup = vup_home;

    clipL = -1;
    clipR = +1;
    clipB = -1;
    clipT = +1;
    clipN = 2;

    camera_changed();
}


/////////////////////////////////////////////////////////
// Called on a GLFW error.
// You don't have to change this function.
/////////////////////////////////////////////////////////

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//////////////////////////////////////////////////////
// Quit if the user hits "q" or "ESC".
// All other key presses are passed to the UI.
// You don't have to change this function.
//////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key,
    int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q ||
        key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (action == GLFW_RELEASE) {
        the_ui.handle_key(key);
    }
}


//////////////////////////////////////////////////////
// Show the image.
// You don't have to change this function.
//////////////////////////////////////////////////////

void display() {
    glClearColor(.1f, .1f, .1f, 1.f);   /* set the background colour */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam.begin_drawing();

    glRasterPos3d(0.0, 0.0, 0.0);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (frame_buffer_stale) {
        //
        // Don't re-render the scene EVERY time display() is called.
        // It might get called if the window is moved, or if it
        // is exposed.  Only re-render if the window is RESIZED.
        // This is detected in check_for_resize().
        //

        render();
        frame_buffer_stale = false;
    }

    //
    // This paints the current image buffer onto the screen.
    //
    glDrawPixels(winWidth, winHeight,
        GL_RGB, GL_UNSIGNED_BYTE, img);

    glFlush();
}

///////////////////////////////////////////////////////////////////
// Set up the keyboard UI.
// No need to change this.
//////////////////////////////////////////////////////////////////
void init_UI() {
    // These variables will trigger a call-back when they are changed.
    the_ui.add_variable("Eye X", &eye.x, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Eye Y", &eye.y, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Eye Z", &eye.z, -10, 10, 0.2, cam_param_changed);

    the_ui.add_variable("Ref X", &lookat.x, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Ref Y", &lookat.y, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Ref Z", &lookat.z, -10, 10, 0.2, cam_param_changed);

    the_ui.add_variable("Vup X", &vup.x, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Vup Y", &vup.y, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Vup Z", &vup.z, -10, 10, 0.2, cam_param_changed);

    the_ui.add_variable("Clip L", &clipL, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Clip R", &clipR, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Clip B", &clipB, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Clip T", &clipT, -10, 10, 0.2, cam_param_changed);
    the_ui.add_variable("Clip N", &clipN, -10, 10, 0.2, cam_param_changed);

    static float dummy2 = 0;
    the_ui.add_variable("Reset Camera", &dummy2, 0, 100, 0.001, reset_camera);

    the_ui.done_init();

}

//////////////////////////////////////////////////////
// Main program.
// You don't have to change this function.
//////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    init_UI();
    init_scene();


    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        cerr << "glfwInit failed!\n";
        cerr << "PRESS Control-C to quit\n";
        char line[100];
        cin >> line;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(winWidth, winHeight,
        "Ray Traced Scene", NULL, NULL);

    if (!window)
    {
        cerr << "glfwCreateWindow failed!\n";
        cerr << "PRESS Control-C to quit\n";
        char line[100];
        cin >> line;

        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    int w = winWidth;
    int h = winHeight;

    cam = Camera(0, 0, 10, 10, w, h, window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    float dummy = 0;
    reset_camera(dummy);

    while (!glfwWindowShouldClose(window))
    {
        cam.check_resize();
        setup_camera();

        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

