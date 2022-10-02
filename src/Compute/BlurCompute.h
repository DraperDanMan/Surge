#pragma once

#include <string>

#include "ComputeBase.h"
#include "../Image.h"
#include "imgui.h"
#include "vulkan/vulkan.h"

namespace Surge
{
    class BlurCompute : ComputeBase
    {
        const std::string BlurComputeShader = "Shaders/BlurCompute.comp";
    public:

        enum class BlurMode
        {
            GAUSSIAN,
            MOTION,
            RADIAL,
        };
        
        struct PushParams
        {
            ImVec2 center; //TODO draperdanman: support radial blur
            float angle;
            float sigma;
            float samples; //TODO draperdanman: support radial blur
            float useAlpha;
            BlurMode blurMode; //TODO draperdanman: support the different blur modes in compute
        };

        BlurCompute();
        ~BlurCompute();
        void Run( Image *input, Image *output, PushParams params );

    private:
        void Bind( Image *input, Image *output, PushParams params );
        void UnBind();
        
        VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );

        Image *m_input;
        Image *m_output;
    };
}