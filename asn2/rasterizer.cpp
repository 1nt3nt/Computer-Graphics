// Line rasterization, demonstrated.

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

Camera cam;
bool scene_changed = true;
GLFWwindow *window;
Flat_Lighter lighter;

float world_BL_x = 0;
float world_BL_y = 0;
float world_width = 1;
float world_height = 1;
int raster_width = 10;
int raster_height = 10;

float vertex_size;
Mesh *pixel_shape;
Mesh *line_shape;

glm::vec2 mouse_pos;
glm::vec2 start;
glm::vec2 end;

bool finished = false;
bool begin = false;

float step = world_width / raster_width;
// Start point and end point.
int s_x=-1, s_y=-1, e_x, e_y;

void make_meshes(GLuint program_handle) {
    float *coords = new float[4 * 4];
    GLuint *indexes = new GLuint[4];

    float vertex_size = world_width / raster_width * 0.9;

    int i = 0;
    i = add_vertex(-vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(-vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);

    pixel_shape = new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_TRIANGLE_FAN, program_handle,
                            "Pixel");
    i = 0;
    i = add_vertex(0, 0, 0, coords, indexes, i);
    i = add_vertex(0, 1, 0, coords, indexes, i);

    line_shape = new Mesh(coords, 2, 4,
                          indexes, 2,
                          2,
                          GL_LINE_STRIP, program_handle,
                          "Line Segment");

    delete[] coords;
    delete[] indexes;
}


void draw_scene() {
    // Draw the current scene.

    // Clear the image, and the depth buffer.

    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    lighter.set_camera(cam);

	// Draw a basic 10x10 graypixel
	lighter.set_color(glm::vec4(0.5, 0.5, 0.5, 1));
	float x = world_BL_x + world_width / raster_width / 2;
	float y = world_BL_y + world_height / raster_height / 2;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			glm::vec2 pixel_center = glm::vec2(x + i * step, y + j * step);
			glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(pixel_center, 0));
			lighter.set_model_transform(move);
			pixel_shape->draw();
		}
	}
	
    // Draw one pixel, at the bottom left, gray.
	/*
    lighter.set_color(glm::vec4(0.5, 0.5, 0.5, 1));
    float x = world_BL_x + world_width / raster_width / 2;
    float y = world_BL_y + world_height / raster_height / 2;
	
    glm::vec2 pixel_center = glm::vec2(x, y);
    //glm::vec3 position(pixel_center, 0);
    glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(pixel_center, 0));
    lighter.set_model_transform(move);
    pixel_shape->draw();
	*/

	// Draw the white pixels using DDA algorithm
	if (begin) {
		lighter.set_color(glm::vec4(1, 1, 1, 1));
		float dx = 0.0, dy = 0.0;
		int dm;
		if (fabs(e_x-s_x) >= fabs(e_y-s_y))
			dm = fabs(e_x - s_x);
		else
			dm = fabs(e_y - s_y);
		dx = (float)(e_x - s_x) / dm;
		dy = (float)(e_y - s_y) / dm;

		float t_x = s_x, t_y = s_y;

		for (int i = 0; i <= dm; i++) {
			glm::vec2 pixel_center = glm::vec2(x + (int)round(t_x) * step, y + (int)round(t_y) * step);
			glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(pixel_center, 0));
			lighter.set_model_transform(move);
			pixel_shape->draw();

			t_x = t_x + dx;
			t_y = t_y + dy;
		}

		lighter.set_color(glm::vec4(0, 0, 0, 1));
		glm::vec2 pixel_center = glm::vec2(x+s_x*step, y+s_y*step);
		glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3(pixel_center, 0));
		lighter.set_model_transform(move);
		float *coords = new float[4];
		coords[0] = world_BL_x;
		coords[1] = world_BL_y;
		coords[2] = 0;
		coords[3] = 1;
		line_shape->update_vertex(coords, 0);
		coords[0] = (e_x-s_x)*step;
		coords[1] = (e_y-s_y)*step;
		coords[2] = 0;
		coords[3] = 1;
		line_shape->update_vertex(coords, 1);
		line_shape->draw();

		// And draw a black line
		/*
		lighter.set_color(glm::vec4(0, 0, 0, 1));
		float *coords = new float[4];
		coords[0] = world_BL_x;
		coords[1] = world_BL_y + world_height / 2;
		coords[2] = 0;
		coords[3] = 1;
		line_shape->update_vertex(coords, 0);
		coords[0] = world_BL_x + world_width / 2;
		coords[1] = world_BL_y + world_height / 2;
		coords[2] = 0;
		coords[3] = 1;
		line_shape->update_vertex(coords, 1);
		line_shape->draw();
		*/
	}
	
    // Bring the pixels to the front
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

    //test_pixel_math();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = 800;

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "Rasterizer",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    cam = Camera(display_width, display_height);
    cam.init_2D(world_BL_x, world_BL_y, world_width, world_height);

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
    // Rasterize the line, and set scene_changed to true
	if (!finished) {
		e_x = (int)(mouse_pos.x / step);
		e_y = (int)(mouse_pos.y / step);
	}
	scene_changed = true;
}

// glfw calls this every time the user presses/releases a mouse button
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods ) {
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        // Mouse pressed. Save the start of the line.
		begin = true;
		if (!finished) {
			s_x = (int)(mouse_pos.x / step);
			s_y = (int)(mouse_pos.y / step);
		}
        // Set scene_changed to true
        scene_changed = true;
    }
    else {
        // Mouse released.  Stop making pixels.
		finished = true;
        // Set scene_changed to true
        scene_changed = true;
    }
}
