
#include "pipeline.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <cassert>
#ifdef _WIN32
#include <windows.h>
#endif

namespace the_engine
{
    Pipeline::Pipeline(EngineDevice &device, const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &configInfo) : device{device}
    {
        createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }
    Pipeline::~Pipeline()
    {
        vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
        vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
    }
    std::vector<char> Pipeline::readFile(const std::string &path)
    {
        std::filesystem::path fullPath;
#ifdef _WIN32
        char pathBuffer[MAX_PATH];
        DWORD length = GetModuleFileNameA(
            nullptr,
            pathBuffer,
            MAX_PATH);
        if (length == 0)
        {
            throw std::runtime_error(
                "Failed to get executable path");
        }

        fullPath = std::filesystem::path(
                       std::string(pathBuffer, length))
                       .parent_path() /
                   path;
#else
        fullPath = std::filesystem::current_path() / path;
#endif
        std::cout << "Resolved path: "
                  << fullPath.string()
                  << '\n';
        std::ifstream file{fullPath, std::ios::ate | std::ios::binary};

        if (!file.is_open())
        {
            throw std::runtime_error(
                "failed to open the file at path: " + path);
        }
        const auto file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);
        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath, const PipelineConfigInfo &configInfo)
    {
        auto vertCode = readFile(vertFilePath);
        auto fragCode = readFile(fragFilePath);
        assert(
            configInfo.pipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(
            configInfo.renderPass != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no renderPass provided in configInfo");
        std::cout << "Vertex Shader Code Size: " << vertCode.size() << "\n";
        std::cout << "Fragment Shader Code Size: " << fragCode.size() << "\n";
        createShaderModule(vertCode, &vertShaderModule);
        createShaderModule(fragCode, &fragShaderModule);
        VkPipelineShaderStageCreateInfo shaderStages[2];

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
                VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &configInfo.viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &configInfo.scissor;
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create graphics pipeline");
        }
    }
    void Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module");
        }
    }
    PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
    {
        PipelineConfigInfo pipelineConfigInfo{};
        pipelineConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        pipelineConfigInfo.viewport.x = 0.0f;
        pipelineConfigInfo.viewport.y = 0.0f;
        pipelineConfigInfo.viewport.width = static_cast<float>(width);
        pipelineConfigInfo.viewport.height = static_cast<float>(height);
        pipelineConfigInfo.viewport.minDepth = 0.0f;
        pipelineConfigInfo.viewport.maxDepth = 1.0f;

        pipelineConfigInfo.scissor.offset = {0, 0};
        pipelineConfigInfo.scissor.extent = {width, height};


        pipelineConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        pipelineConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        pipelineConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineConfigInfo.rasterizationInfo.lineWidth = 1.0f;
        pipelineConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        pipelineConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        pipelineConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        pipelineConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        pipelineConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
        pipelineConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        pipelineConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        pipelineConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineConfigInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
        pipelineConfigInfo.multisampleInfo.pSampleMask = nullptr;            // Optional
        pipelineConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        pipelineConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

        pipelineConfigInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        pipelineConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        pipelineConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        pipelineConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        pipelineConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        pipelineConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        pipelineConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        pipelineConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        pipelineConfigInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        pipelineConfigInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        pipelineConfigInfo.colorBlendInfo.attachmentCount = 1;
        pipelineConfigInfo.colorBlendInfo.pAttachments = &pipelineConfigInfo.colorBlendAttachment;
        pipelineConfigInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
        pipelineConfigInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
        pipelineConfigInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
        pipelineConfigInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

        pipelineConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        pipelineConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        pipelineConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        pipelineConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        pipelineConfigInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
        pipelineConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
        pipelineConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        pipelineConfigInfo.depthStencilInfo.front = {}; // Optional
        pipelineConfigInfo.depthStencilInfo.back = {};  // Optional

        return pipelineConfigInfo;
    }
}