#pragma once

#include <string>
#include "../Image.h"
#include "imgui.h"
#include "vulkan/vulkan.h"

#include "ComputeBase.h"

namespace Surge
{
    class LevelsCompute : ComputeBase
    {
        const std::string LevelsComputeShader = "Shaders/LevelsCompute.comp";
    public:
    
        struct PushParams
        {
            ImVec2 inputRange;
            ImVec2 outputRange;
            float gamma;
            float luminanceOnly;
        };

        LevelsCompute();
        ~LevelsCompute();
        void Run( Image *input, Image *output, PushParams params );

    private:
        void Bind( Image *input, Image *output, PushParams params );
        void UnBind();
        
        VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );

        Image *m_input;
        Image *m_output;
    };
}