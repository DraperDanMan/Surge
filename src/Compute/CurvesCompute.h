#pragma once


#include <string>
#include "../Image.h"
#include "vulkan/vulkan.h"

#include "ComputeBase.h"

namespace Surge
{
    class CurvesCompute : ComputeBase
    {
        const std::string CurvesComputeShader = "Shaders/CurvesCompute.comp";
    public:
    
        struct PushParams
        {
            int widthLUT;
        };

        CurvesCompute();
        ~CurvesCompute();
        void Run( Image *input, Image *curvesLUT, Image *output, PushParams params );

    private:
        void Bind( Image *input, Image *curvesLUT, Image *output, PushParams params );
        void UnBind();
        
        VkCommandBuffer CreateCommandBuffer( VkDevice device, VkCommandPool pool, VkPipeline pipeline, VkPipelineLayout layout, VkDescriptorSet dscSet, PushParams p );

        Image *m_input;
        Image *m_curvesLUT;
        Image *m_output;
    };
}