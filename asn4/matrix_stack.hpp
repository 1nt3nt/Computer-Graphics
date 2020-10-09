#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include <glm/mat4x4.hpp> // glm::mat4
#include <stack>

class Matrix_Stack {
public:
    Matrix_Stack();

    void push();
    void pop();

    glm::mat4x4& top();
    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);

private:
    std::stack<glm::mat4x4> the_stack;
};

#endif
