#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>


namespace the_engine {
    EngineWindow::EngineWindow(int h, int w, std::string name):width{w}, height{h},windowName{name}{
        initWindow();
    }

    void EngineWindow::initWindow() {
        if(glfwInit()){
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            this->window = glfwCreateWindow(this->width, this->height, this->windowName.c_str(), nullptr, nullptr);
        }
    }

    EngineWindow::~EngineWindow() {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }
    void EngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, this->window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a window surface");
        }
    }
}