#include "window.h"
#include <GLFW/glfw3.h>


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
}