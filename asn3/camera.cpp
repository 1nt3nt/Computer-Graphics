#include <glm/vec4.hpp> // vec4
#include <glm/vec3.hpp> // vec3
#include <glm/gtc/matrix_transform.hpp> // translate, rotate
#include <glm/gtc/type_ptr.hpp> // value_ptr
#define NOMINMAX // avoid conflict with VC's built-in max in WinDef.h

#include <string>
#include <iostream>
#include <limits> // numeric_limits<float>min, max

#include "camera.hpp"
#include "gl_error.hpp"

extern void print_vec3(const glm::vec3& v);
extern void print_vec4(const glm::vec4& v);
extern void print_mat4(const glm::mat4& m);

Camera::Camera() {
    set_default_pose();
}

void Camera::set_default_pose() {
    eye_pt = glm::vec4(0, 0, 10, 1);
    ref_pt = glm::vec4(0, 0, 0, 1);
    v_up = glm::vec4(0, 1, 0, 0);
    clip_L = -1;
    clip_R = +1;
    clip_B = -1;
    clip_T = +1;
    clip_N = 2;
    clip_F = 20;

    vert_step = glm::radians<float>(2);
    horiz_step = vert_step;
}

Camera::Camera(int display_width, int display_height) {
    // On MacOS, are multiple display pixels per pixel (high resolution),
    // but mouse coords are reported in low resolution.
    // So we need two copies of the viewport dimensions,
    // one for mouse and for openGL drawing.
    mouse_viewport_W = display_width;
    mouse_viewport_H = display_height;
    resize_display(display_width, display_height);
    set_default_pose();
    glEnable(GL_DEPTH_TEST);
}

void Camera::init_2D(float world_BLx, float world_BLy,
                     float world_W, float world_H) {
    this->world_BLx = world_BLx;
    this->world_BLy = world_BLy;
    this->world_W = world_W;
    this->world_H = world_H;

    eye_pt = glm::vec4(world_BLx + world_W / 2,
                       world_BLy + world_H / 2,
                       10, 1);
    ref_pt = glm::vec4(world_BLx + world_W / 2,
                       world_BLy + world_H / 2,
                       0, 1);
    v_up = glm::vec4(0, 1, 0, 1);

    clip_L = -world_W / 4;
    clip_R = +world_W / 4;
    clip_B = -world_H / 4;
    clip_T = +world_H / 4;
    clip_N = 5;
    clip_F = 100;

    basis_X = glm::vec4(1, 0, 0, 0);
    basis_Y = glm::vec4(0, 1, 0, 0);
    basis_Z = glm::vec4(0, 0, 1, 0);

    set_matrices();
}

glm::vec3 Camera::viewport_to_world(int viewport_x, int viewport_y) const {

    // std::cout << "viewport to world. DCS x y = "
    //           << viewport_x << " " << viewport_y << "\n";

    float world_x = world_BLx
        + world_W * (viewport_x + 0.5f) / (float)mouse_viewport_W;
    float world_y = world_BLy
        + world_H * (viewport_y + 0.5f) / (float)mouse_viewport_H;
    return glm::vec3(world_x, world_y, 0);
}

// Convenience method: mouse to world coords (flips y axis)
glm::vec3 Camera::mouse_to_world(int mouse_x, int mouse_y) const {
    return viewport_to_world(mouse_x, mouse_viewport_H - mouse_y);
}

glm::vec3 Camera::world_to_viewport(float world_x, float world_y) const {
    int viewport_x = (int)(world_x / world_W * viewport_W);
    int viewport_y = (int)(world_y / world_H * viewport_H);
    return glm::vec3(viewport_x, viewport_y, 0);
}

void Camera::init(const std::vector<glm::vec3>& points,
                  const glm::vec3& prefered_v_up) {
    // Get bounding box
    float x_min = std::numeric_limits<float>::max();
    float y_min = std::numeric_limits<float>::max();
    float z_min = std::numeric_limits<float>::max();
    float x_max = std::numeric_limits<float>::lowest();
    float y_max = std::numeric_limits<float>::lowest();
    float z_max = std::numeric_limits<float>::lowest();
    for (auto p : points) {
        if (p.x > x_max) x_max = p.x;
        if (p.y > y_max) y_max = p.y;
        if (p.z > z_max) z_max = p.z;

        if (p.x < x_min) x_min = p.x;
        if (p.y < y_min) y_min = p.y;
        if (p.z < z_min) z_min = p.z;
    }

    // Get largest side of bbox
    float dx = x_max - x_min;
    float dy = y_max - y_min;
    float dz = z_max - z_min;
    bbox_width = glm::max(glm::max(dx, dy), dz);

    // look-at point is center of bbox
    float ref_x = (x_min + x_max) / 2;
    float ref_y = (y_min + y_max) / 2;
    float ref_z = (z_min + z_max) / 2;
    ref_pt = glm::vec4(ref_x, ref_y, ref_z, 1);

    // eye point is set back from look-at, along z axis
    float eye_x = ref_x;
    float eye_y = ref_y;
    float eye_z = ref_z + bbox_width * 2;
    eye_pt = glm::vec4(eye_x, eye_y, eye_z, 1);

    // set clipping frustum
    clip_L = bbox_width / 3.5f;
    clip_R = -bbox_width / 3.5f;
    clip_B = bbox_width / 3.5f;
    clip_T = -bbox_width / 3.5f;
    clip_N = bbox_width * 1;
    clip_F = bbox_width * 20;

    v_up = glm::vec4(prefered_v_up, 1);
    v_up.w = 1;

    // The camera's reference frame, initially.
    basis_Z = glm::vec4(0, 0, 1, 0);
    glm::vec3 x_vec = glm::normalize(glm::cross((glm::vec3)v_up,
                                                (glm::vec3)basis_Z));
    basis_X = glm::vec4(x_vec, 0);
    glm::vec3 y_vec = glm::cross((glm::vec3)basis_Z, (glm::vec3)basis_X);
    basis_Y = glm::vec4(y_vec, 0);

    set_matrices();

    // std::cout << "eye: "; print_vec3(eye_pt); std::cout << "\n";
    // std::cout << "ref: "; print_vec3(ref_pt); std::cout << "\n";
    // std::cout << "vup: "; print_vec3(v_up); std::cout << "\n";
    // std::cout << "basis_X: "; print_vec3(basis_X); std::cout << "\n";
    // std::cout << "basis_Y: "; print_vec3(basis_Y); std::cout << "\n";
    // std::cout << "basis_Z: "; print_vec3(basis_Z); std::cout << "\n";

}

void Camera::set_matrices() {
    glm::mat4 projection = glm::frustum(clip_L, clip_R, clip_B, clip_T,
                                        clip_N, clip_F);
    glm::mat4 view = glm::lookAt((glm::vec3)eye_pt,
                                 (glm::vec3)ref_pt,
                                 (glm::vec3)v_up);
    proj_view = projection * view;

    // std::cout << "Mproj:\n"; print_mat4(projection); std::cout << "\n";
    // std::cout << "Mview:\n"; print_mat4(view); std::cout << "\n";
}

void Camera::update_shader(GLuint shader_program_handle,
                           GLint proj_view_uniform_loc) const {
    glUseProgram(shader_program_handle);
    glUniformMatrix4fv(proj_view_uniform_loc,
                       1, GL_FALSE, glm::value_ptr(proj_view));
    GL_Error::check("update_shader_camera");
}

void Camera::resize_display(int new_width, int new_height) {
    viewport_W = new_width;
    viewport_H = new_height;
    glViewport(0, 0, new_width, new_height);
    GL_Error::check("glViewport");
    set_matrices();
}

void Camera::orbit(const glm::vec4& axis, float angle_step) {
    // Move eye point along a circle, centered at the look-at point.
    glm::vec4 ref_to_eye = eye_pt - ref_pt;
    glm::mat4 turn = glm::rotate(glm::mat4(1.0f), angle_step, (glm::vec3)axis);
    ref_to_eye = turn * ref_to_eye;
    eye_pt = ref_pt + ref_to_eye;
    basis_X = turn * basis_X;
    basis_Y = turn * basis_Y;
    basis_Z = turn * basis_Z;
    set_matrices();
}

void Camera::advance(float distance) {
    // Move the camera towards the look-at point.
    eye_pt += distance * glm::normalize(ref_pt - eye_pt);
    set_matrices();
}

void Camera::up() {
    // Orbit up, turning on camera's X axis
    orbit(basis_X, -vert_step);
}

void Camera::down() {
    // Orbit down, turning on camera's X axis
    orbit(basis_X, vert_step);
}

void Camera::right() {
    // Orbit right, turning on world up direction
    //orbit(v_up, vert_step);
    orbit(basis_Y, vert_step);
}

void Camera::left() {
    // Orbit left, turning on world up direction
    //orbit(v_up, -vert_step);
    orbit(basis_Y, -vert_step);
}

void Camera::forward() {
    advance(bbox_width / 50);
    set_matrices();
}

void Camera::back() {
    advance(-bbox_width / 50);
    set_matrices();
}

glm::vec4 Camera::get_eye() {
    return eye_pt;
}
