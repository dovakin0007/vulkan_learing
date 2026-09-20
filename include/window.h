#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace the_engine
{

    class EngineWindow
    {
    private:
        void initWindow();
        GLFWwindow *window;
        const int width;
        const int height;
        const std::string windowName;

    public:
        EngineWindow(int h, int w, std::string name);
        EngineWindow(const EngineWindow &) = delete;
        EngineWindow &operator=(const EngineWindow &) = delete;
        EngineWindow(EngineWindow &&other) noexcept;
        ~EngineWindow();
        inline bool shouldClose() { return glfwWindowShouldClose(this->window); }
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    };
}