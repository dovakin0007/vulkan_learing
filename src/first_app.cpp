#include "first_app.h"

namespace the_engine {
    void FirstApp::run() {
        while (!this->engineWindow.shouldClose() )
        {
            glfwPollEvents();
        }
        
    }
}