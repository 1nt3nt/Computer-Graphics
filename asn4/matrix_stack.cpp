#include "matrix_stack.hpp"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, rotate
#include <iostream>

extern void print_mat4(const glm::mat4& m);

Matrix_Stack::Matrix_Stack() {
    the_stack.push(glm::mat4(1.0f));
}

void Matrix_Stack::push() {
    the_stack.push(the_stack.top());
}

void Matrix_Stack::pop() {
    the_stack.pop();
}

glm::mat4x4& Matrix_Stack::top() {
    return the_stack.top();
}

void Matrix_Stack::translate(float x, float y, float z) {
    the_stack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(x,y,z));
}

void Matrix_Stack::scale(float x, float y, float z) {
    the_stack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(x,y,z));
}

void Matrix_Stack::rotate(float angle, float x, float y, float z) {
    angle *= 3.1416f / 180;

    the_stack.top() *= glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x,y,z));
}



