#pragma once

#include "../Image.h"
#include "vulkan/vulkan.h"
#include "imgui.h"

#include <string>

#include "ComputeBase.h"

namespace Surge
{
    class TransformCompute : ComputeBase
    {
        const std::string TransformComputeShader = "Shaders/TransformCompute.comp";
    public:
    
        struct PushParams
        {
            ImVec2 scale;
            ImVec2 size;
            float rotation;
        };

        TransformCompute();
        ~TransformCompute();
        void Run( Image *input, Image *output, PushParams params );

    private:
        void Bind( Image *input, Image *output,  PushParams params );
        void UnBind();

        VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );
    
        Image *m_input = nullptr;
        Image *m_output = nullptr;
};
}