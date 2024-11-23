#include "context.h"

std::unique_ptr<Context> Context::create() {
    auto context = std::unique_ptr<Context>(new Context());
    if (!context->init())
        return nullptr;
    return std::move(context);
}

bool Context::init() {
    return true;
}

void Context::processInput(GLFWwindow* window) {
    return;
}

void Context::reshape(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
}

void Context::mouseMove(double x, double y) {
    return;
}

void Context::mouseButton(int button, int action, double x, double y) {
    return;
}

void Context::render() {
    return;
}
