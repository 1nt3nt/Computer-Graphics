#ifndef CAMERA_HPP
#define CAMERA_HPP

// This class implements a simple OpenGL camera with two specializations:
// 1. Given a list of 3D points, positions the camera to view them.
// 2. Given a 3D rectangle on the XY plane, positions the camera for it.


#include <glad/glad.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // mat4
#include <vector>

class Camera {
public:
    // Default constructor, to keep compiler happy.
    Camera();

    // Construct camera, given viewport dimensions.
    Camera(int display_width, int display_height);

    // Initializer, for the default constructor
    void set_default_pose();

    // Call this when the view window is resized.
    void resize_display(int new_width, int new_height);

    // Call this when the camera has changed, to communicate
    // the camera's projection-view matrix to a shader program.
    void update_shader(GLuint shader_program_handle,
                       GLint proj_view_loc) const;

    // Initializer for a list of 3D points.
    void init(const std::vector<glm::vec3>& points,
              const glm::vec3& preferred_v_up);

    // Initializer for a 3D world rectangle on the XY plane.
    // Use this if you just want to view a 2D scene.
    void init_2D(float world_BLx, float world_BLy,
                 float world_W, float world_H);

    // For 2D view, convert viewport X,Y to world X,Y
    glm::vec3 viewport_to_world(int viewport_x, int viewport_y) const;

    // For 2D view, project a world X,Y to the viewport X,Y
    glm::vec3 world_to_viewport(float world_x, float world_y) const;

    // Convenience method: mouse to world coords (flips y axis)
    glm::vec3 mouse_to_world(int mouse_x, int mouse_y) const;

    void up();
    void down();
    void right();
    void left();
    void forward();
    void back();
    glm::vec4 get_eye();

private:
    void set_matrices();
    void orbit(const glm::vec4& axis, float angle_step);
    void advance(float distance);

    float bbox_width;
    glm::vec4 basis_X, basis_Y, basis_Z;
    glm::vec4 eye_pt, ref_pt, v_up;
    glm::mat4 proj_view;
    float clip_L, clip_R, clip_B, clip_T, clip_N, clip_F;
    int mouse_viewport_W, mouse_viewport_H;
    int viewport_W, viewport_H;
    float world_BLx, world_BLy;
    float world_W, world_H;
    float vert_step, horiz_step;
};
#endif
