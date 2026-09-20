#pragma once

#include "window.h"
#include "pipeline.h"
#include "engine_device.h"
#include "swap_chain.h"
#include <memory>
#include <vector>

namespace the_engine
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();
        FirstApp();
        ~FirstApp();
        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        EngineWindow engineWindow{WIDTH, HEIGHT, "Hello Vulkan"};
        EngineDevice device{engineWindow};
        EngineSwapChain engineSwapChain{device, engineWindow.getExtent()};

        std::unique_ptr<Pipeline> enginePipeline;
        VkPipelineLayout pipelineLayout;

        std::vector<VkCommandBuffer> commandBuffers;
        // Pipeline pipeline{device, "shader/simple_shader.vert.spv", "shader/simple_shader.frag.spv", Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}
