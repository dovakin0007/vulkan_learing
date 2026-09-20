#include "first_app.h"
#include "pipeline.h"
#include <stdexcept>
#include <iostream>

namespace the_engine {
    FirstApp::FirstApp() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }
    void FirstApp::run() {
        while (!this->engineWindow.shouldClose() )
        {
            glfwPollEvents();
        }
        
    }

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }
    void FirstApp::createPipeline() {
        auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(engineSwapChain.width(), engineSwapChain.height());
        pipelineConfig.renderPass = engineSwapChain.getRenderPass();
        std::cout << "does this get called \n";
        pipelineConfig.pipelineLayout = pipelineLayout;
        enginePipeline = std::make_unique<Pipeline>(device, "shader/simple_shader.vert.spv", "shader/simple_shader.frag.spv", Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT));
    }

    void FirstApp::createCommandBuffers() {

    }
    void FirstApp::drawFrame(){

    }
}