#pragma once

#include "../Image.h"
#include "vulkan/vulkan.h"

#include <string>

#include "ComputeBase.h"

namespace Surge
{
    class InvertCompute : ComputeBase
    {
        const std::string InvertComputeShader = "Shaders/InvertCompute.comp";
    public:
    
        struct PushParams
        {
            int channels;
        };

        InvertCompute();
        ~InvertCompute();
        void Run( Image *input, Image *output, PushParams params );

    private:
        void Bind( Image *input, Image *output, PushParams params );
        void UnBind();

        VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );
    
        Image *m_input;
        Image *m_output;
    };
}